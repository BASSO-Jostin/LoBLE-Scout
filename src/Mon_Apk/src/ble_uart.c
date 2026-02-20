#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/printk.h>
#include "ble_uart.h"

/* ⚡ Variables globales */
static struct bt_conn *current_conn;
static bool notify_enabled;

/* 🔹 Déclarations forward */
static ssize_t uart_rx_cb(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          const void *buf, uint16_t len,
                          uint16_t offset, uint8_t flags);

static void uart_tx_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value);

/* 🔔 Service BLE UART */
BT_GATT_SERVICE_DEFINE(uart_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_UART_SERVICE),

    BT_GATT_CHARACTERISTIC(
        BT_UUID_UART_RX,
        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
        BT_GATT_PERM_WRITE,
        NULL,
        uart_rx_cb,
        NULL
    ),

    BT_GATT_CHARACTERISTIC(
        BT_UUID_UART_TX,
        BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_NONE,
        NULL,
        NULL,
        NULL
    ),

    BT_GATT_CCC(uart_tx_ccc_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

/* 📡 RX callback : données reçues du téléphone */
static ssize_t uart_rx_cb(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          const void *buf, uint16_t len,
                          uint16_t offset, uint8_t flags)
{
    printk("BLE RX (%d bytes): %.*s\n", len, len, (char *)buf);
    return len;
}

/* 🔔 TX CCC callback : notifications activées/désactivées */
static void uart_tx_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    printk("Notifications %s\n", notify_enabled ? "ON" : "OFF");

    /* Dès que notifications activées, envoie "hello" */
    if (notify_enabled && current_conn) {
        const char msg[] = "hello";
        bt_gatt_notify(current_conn, &uart_svc.attrs[4], msg, sizeof(msg)-1);
        printk("Sent 'hello' to phone\n");
    }
}

static void ble_notify_cb(struct bt_conn *conn, void *user_data)
{
    // notification envoyée, tu peux envoyer la suivante
    *(bool *)user_data = false;
}

void ble_uart_send(const uint8_t *data, uint16_t len)
{
    static int last_send_ms = 0;
    int now = k_uptime_get_32();

    // envoyer au max toutes les 200 ms
    if (!current_conn || !notify_enabled || (now - last_send_ms < 200)) {
        return;
    }

    bt_gatt_notify(current_conn, &uart_svc.attrs[4], data, len);
    last_send_ms = now;
}


/* 🔗 Callbacks connexion */
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (!err) {
        current_conn = bt_conn_ref(conn);
        printk("BLE connected\n");

        /* Dès la connexion, on envoie "hello" */
        const char msg[] = "hello";
        ble_uart_send((const uint8_t *)msg, sizeof(msg)-1);
    } else {
        printk("BLE connection failed (err %u)\n", err);
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("BLE disconnected (reason %u)\n", reason);
    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }
}

BT_CONN_CB_DEFINE(conn_cb) = {
    .connected = connected,
    .disconnected = disconnected,
};
