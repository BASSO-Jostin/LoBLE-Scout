
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/sys/util.h>
#include <stdio.h>
#include <math.h>
#include <zephyr/sys/ring_buffer.h>
#include "ble_uart.h"

#define GPS_UART_LABEL DT_LABEL(DT_NODELABEL(uart0))
#define LORA_UART_LABEL DT_LABEL(DT_NODELABEL(uart1))

#define GPS_STACK_SIZE 1024
#define BLE_STACK_SIZE 1024

#define GPS_PRIORITY 5
#define BLE_PRIORITY 10
#define LORA_PRIORITY 20
#define GPS_BUF_SIZE 128

#define PI 3.14159265358979323846

static const int32_t sleep_time_ms = 1000;
static const int64_t SEND_INTERVAL_MS = 125000;
static const int64_t SEND_INTERVAL_LORA_MS = 247000; //L'envoie vers le LoRa chaque ~4 minutes

static const struct device *gps_uart;
static const struct device *lora_uart;

static uint8_t gps_buf[GPS_BUF_SIZE];

/* Shared GPS state */ 
static struct k_mutex gps_mutex;
static struct k_mutex lora_mutex_uart1;
static double latest_lat = 0.0; 
static double latest_lon = 0.0; 
static bool have_fix = false; 
static bool init_point_set = false; 
static double init_lat = 0.0; 
static double init_lon = 0.0;


struct k_sem radio_sem;
K_SEM_DEFINE(radio_sem, 1, 1);


#define GPS_RING_SIZE 512
RING_BUF_DECLARE(gps_ring, GPS_RING_SIZE);

static void gps_uart_isr(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) {
        while (uart_fifo_read(dev, &c, 1)) {
            ring_buf_put(&gps_ring, &c, 1);
        }
    }
}


double nmea_to_decimal(const char *nmea, char hemi)
{
    if (!nmea || strlen(nmea) < 4) {
        return 0.0;
    }

    double val = atof(nmea);

    int degrees = (int)(val / 100);
    double minutes = val - (degrees * 100);

    double decimal = degrees + (minutes / 60.0);

    if (hemi == 'S' || hemi == 'W') {
        decimal = -decimal;
    }

    return decimal;
}



// Fonction simple pour extraire latitude et longitude de la trame GPRMC
void process_nmea(char *s)
{
    if (strncmp(s, "$GPGGA", 6) && strncmp(s, "$GNGGA", 6))
        return;

    char *f[15];
    int n = 0;

    char *p = strtok(s, ",");
    while (p && n < 15) {
        f[n++] = p;
        p = strtok(NULL, ",");
    }

    if (n < 7) return;
    if (f[6][0] == '0') return; // no fix

    printk("LAT=%s %s  LON=%s %s\n",
           f[2], f[3],
           f[4], f[5]);

    /* Envoi via BLE UART */
    char ble_msg[64];
    int len = snprintf(ble_msg, sizeof(ble_msg),
                       "%s\n",
                       f[2]);
    //ble_uart_send((uint8_t *)ble_msg, len);

    double lat = nmea_to_decimal(f[2], f[3][0]);
    double lon = nmea_to_decimal(f[4], f[5][0]);

    printk("\nLAT=%.6f LON=%.6f\n", lat, lon);

    //In the other code, there is a mutex lock here to protect shared state below
    k_mutex_lock(&gps_mutex, K_FOREVER);
    latest_lat = lat;
    latest_lon = lon;
    have_fix = true;
    if (!init_point_set) {
        init_lat = lat;
        init_lon = lon;
        init_point_set = true;
    }
    k_mutex_unlock(&gps_mutex);
}

static double distance_km(double lat1, double lon1, double lat2, double lon2)
{
    double dlat = (lat2 - lat1) * (PI / 180.0);
    double dlon = (lon2 - lon1) * (PI / 180.0);

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1 * (PI / 180.0)) * cos(lat2 * (PI / 180.0)) *
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double R = 6371.0; // Rayon de la Terre en kilomètres
    return R * c;
}




// Thread GPS
void gps_thread(void)
{
    gps_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
    if (!device_is_ready(gps_uart)) {
        printk("UART device not ready!\n");
        return;
    }
    uart_irq_callback_set(gps_uart, gps_uart_isr);
    uart_irq_rx_enable(gps_uart);

    static char line[128];
    int pos = 0;
    uint8_t c;

    while (1) {
        if (ring_buf_get(&gps_ring, &c, 1) == 0) {
            k_sleep(K_MSEC(5));
            continue;
        }

        if (c == '$') {
            pos = 0;
            line[pos++] = c;
            continue;
        }

        if (pos == 0)
            continue;

        if (c == '\n') {
            line[pos] = '\0';
            process_nmea(line);
            pos = 0;
        } else if (pos < sizeof(line) - 1) {
            line[pos++] = c;
        }
    }
}


//Thread BLE
void ble_thread(void)
{

    while (1) {
        k_sleep(K_MSEC(SEND_INTERVAL_MS));

        k_mutex_lock(&gps_mutex, K_FOREVER);
        bool have = have_fix && init_point_set;
        double lat = latest_lat;
        double lon = latest_lon;
        double init_lat_copy = init_lat;
        double init_lon_copy = init_lon;
        k_mutex_unlock(&gps_mutex);

        if (!have) {
            printk("No GPS fix available yet.\n");
            continue;
        }

        double dist = distance_km(init_lat_copy, init_lon_copy, lat, lon);
        char msg[64];
        int len;
                
        if(dist >= 10000){
            len = snprintf(msg, sizeof(msg), "Plus de 10 Km parcouru !!!\r\n");
        }
        else{
            int km = (int)(dist/1000);
            if(km < 1) km = 0;

            len = snprintf(msg, sizeof(msg), "%d Km parcouru !!!\r\n", km);
        }
        
        if (k_sem_take(&radio_sem, K_MSEC(500)) == 0) {
            ble_uart_send((uint8_t *)msg, len);
            // On laisse un petit temps pour que la pile BLE vide son buffer
            k_sleep(K_MSEC(50));
            k_sem_give(&radio_sem);
        } else {
            printk("Radio busy, skipping this BLE update...\n");
        }

        
        printk("BLE Sent -> %s", msg);

    }
}


//Thread LoRa
void lora_thread(void){


    lora_uart = DEVICE_DT_GET(DT_NODELABEL(uart1));

    uart1_print("AT+JOIN\r\n");

    k_sleep(K_SECONDS(10));


    while (!device_is_ready(lora_uart)) {
        printk("LoRa UART device not ready, waiting...\n");
        k_sleep(K_SECONDS(1));
    }

    printk("LoRa UART ready\n");

    

    while(1){
        //uart1_print("Here we go LoRa...\r\n");

        //bt_disable();   // stop BLE

        k_sleep(K_MSEC(SEND_INTERVAL_LORA_MS));

        k_mutex_lock(&gps_mutex, K_FOREVER);
        bool have = have_fix;
        double lat = latest_lat;
        double lon = latest_lon;   
        k_mutex_unlock(&gps_mutex);

        if(!have){
            printk("No GPS fix available yet for LoRa.\n");
            continue;
        }

        int32_t lat_enc = (int32_t)(lat * 1e6);
        int32_t lon_enc = (int32_t)(lon * 1e6);

        //Cela peut être aussi fait d'une autre manière comme dans le code Arduino

        char hexFrameLon[9];
        char hexFrameLat[9];

        snprintf(hexFrameLon, sizeof(hexFrameLon), "%08X", (uint32_t)lon_enc);
        snprintf(hexFrameLat, sizeof(hexFrameLat), "%08X", (uint32_t)lat_enc);

        char packet[64];

        snprintf(packet, sizeof(packet), "AT+SEND=1,0,8,1,%s%s\r\n", hexFrameLon, hexFrameLat);

        if (k_sem_take(&radio_sem, K_FOREVER) == 0) {
            // Send the packet via LoRa UART
            uart1_print(packet);
            /* TRÈS IMPORTANT : Le LoRa est LENT. 
               Une commande AT+SEND peut prendre du temps à être traitée 
               par le module interne. On bloque les autres pendant 2 secondes. */
            k_sleep(K_MSEC(2000));

            k_sem_give(&radio_sem);
        } else {
            printk("Failed to take radio semaphore for LoRa send.\n");
        }
        

        printk("LoRa TX: %s", packet);

        k_sleep(K_SECONDS(2));
        //bt_enable(NULL); // relance BLE
    
    }

}
/*
void uart1_send_async(const char *msg) {
    int len = strlen(msg);
    // On remplit le buffer de transmission matériel
    int sent = uart_fifo_fill(lora_uart, msg, len);
    
    if (sent < len) {
        // Optionnel : gérer le reste si le buffer est plein
    }
}*/



void uart1_print(const char *s)
{
    printk("UART1 ready = %d\n", device_is_ready(lora_uart));

    k_mutex_lock(&lora_mutex_uart1, K_FOREVER);

    while (*s) {
        uart_poll_out(lora_uart, *s++);
    }

    k_mutex_unlock(&lora_mutex_uart1);
}

K_THREAD_DEFINE(gps_thread_id, GPS_STACK_SIZE, gps_thread, NULL, NULL, NULL,
                GPS_PRIORITY, 0, 0);
K_THREAD_DEFINE(ble_stack, GPS_STACK_SIZE, ble_thread, NULL, NULL, NULL,
                BLE_PRIORITY, 0, 0);
K_THREAD_DEFINE(lora_stack, GPS_STACK_SIZE, lora_thread, NULL, NULL, NULL,
                LORA_PRIORITY, 0, 0);

void main(void)
{
   

    int err;

    k_mutex_init(&gps_mutex);
    k_mutex_init(&lora_mutex_uart1);
    
    
    printk("Initializing GPS UART...\n");
    gps_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
    if (!gps_uart) {
        printk("Cannot find UART device\n");
        return;
    }

    printk("Starting BLE UART service...\n");

    err = bt_enable(NULL);
    if (err) {
        printk("BLE init failed (%d)\n", err);
        return;
    }

    printk("BLE initialized\n");

    /* Advertising */
    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, NULL, 0, NULL, 0);
    if (err) {
        printk("Advertising failed (%d)\n", err);
        return;
    }

    printk("BLE advertising started\n");

    /*while (1) {
        k_sleep(K_SECONDS(1));
    }*/
   return;
}
