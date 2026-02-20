/* Source du noyau de Zephyr */
#include <zephyr/kernel.h>
/* Source des fonctions d'accès au GPIO */
#include <zephyr/drivers/gpio.h>
/* Source des fonctions d'acces aux ports d'entree/sortie */
#include "ports.c"

/* Affiche un caractère (0-9, A-F) sur un afficheur 7 segments */
static inline void sevenseg_show_char(struct gpio_dt_spec *pins, size_t count, char c)
{
    uint8_t mask = 0;

    if (c >= '0' && c <= '9') {
        mask = sevensegLUT[c - '0'];
    } 
    else if (c >= 'A' && c <= 'Z') {
        mask = sevensegLUT[10 + (c - 'A')];
    }
    else if (c >= 'a' && c <= 'f') {
        mask = sevensegLUT[10 + (c - 'a')];
    }
    else {
        // caractère non reconnu → éteindre tous les segments
        mask = 0x00;
    }

    // Appliquer l’état de chaque segment
    for (size_t i = 0; i < count; i++) {
        gpio_pin_set_dt(&pins[i], (mask >> i) & 0x01);
    }
}

/* Eteint un bloc 7-seg (tous segments off) */
static inline void sevenseg_clear(struct gpio_dt_spec *pins, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        gpio_pin_set_dt(&pins[i], 0);
    }
}

/* Eteint tous les 7-seg (0..3) */
static inline void sevenseg_clear_all(void)
{
    sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);
    sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
    sevenseg_clear(sevensegment2, SEVENSEGMENT2_PINS_COUNT);
    sevenseg_clear(sevensegment3, SEVENSEGMENT3_PINS_COUNT);
}
/* ==================== FIN AJOUT utilitaires 7-seg ====================== */


/*Declaration des semaphores*/
struct k_mutex mutex;
struct k_sem feu1;
struct k_sem feu2;

/*Creation et initialisation des sempahores*/
K_MUTEX_DEFINE(mutex);
K_SEM_DEFINE(feu1, 0, 1);
K_SEM_DEFINE(feu2, 1, 1);

/*------------------------------------------------VEHICULE---------------------------------------------------------------*/
/* Declaration d'une fonction */
static void vehicule4(void *p1, void *p2, void *p3){
    k_sem_take(&feu1, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);
    printk("Traversee du vehicue 4 sur la voie 1 \n");

    /* === AJOUT : afficher '4' pendant la traversee === */
    sevenseg_show_char(sevensegment1, SEVENSEGMENT1_PINS_COUNT, 'C');
	sevenseg_show_char(sevensegment0, SEVENSEGMENT0_PINS_COUNT, '4');

    k_msleep(2000);

    /* === AJOUT : eteindre apres la traversee (optionnel) === */
    sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
	sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);

    k_mutex_unlock(&mutex);
    k_sem_give(&feu1);
}

static void vehicule1(void *p1, void *p2, void *p3) {
    k_sem_take(&feu2, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);
    printk("Traversee du vehicue 1 sur la voie 2 \n");

    /* === AJOUT : afficher '1' pendant la traversee === */

	sevenseg_show_char(sevensegment1, SEVENSEGMENT1_PINS_COUNT, 'C');
    sevenseg_show_char(sevensegment0, SEVENSEGMENT0_PINS_COUNT, '1');

    k_msleep(2000);

    /* === AJOUT : eteindre apres la traversee (optionnel) === */
    sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
	sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);

    k_mutex_unlock(&mutex);
    k_sem_give(&feu2);
}

static void vehicule5(void *p1, void *p2, void *p3) {
    k_sem_take(&feu1, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);
    printk("Traversee du vehicue 5 sur la voie 1 \n");

    /* === AJOUT : afficher '5' pendant la traversee === */
	sevenseg_show_char(sevensegment1, SEVENSEGMENT1_PINS_COUNT, 'C');
    sevenseg_show_char(sevensegment0, SEVENSEGMENT0_PINS_COUNT, '5');

    k_msleep(2000);

    /* === AJOUT : eteindre apres la traversee (optionnel) === */
    sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
	sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);

    k_mutex_unlock(&mutex);
    k_sem_give(&feu1);
}

static void vehicule2(void *p1, void *p2, void *p3) {
    k_sem_take(&feu2, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);
    printk("Traversee du vehicue 2 sur la voie 2 \n");

    /* === AJOUT : afficher '2' pendant la traversee === */
	sevenseg_show_char(sevensegment1, SEVENSEGMENT1_PINS_COUNT, 'C');
    sevenseg_show_char(sevensegment0, SEVENSEGMENT0_PINS_COUNT, '2');

    k_msleep(2000);

    /* === AJOUT : eteindre apres la traversee (optionnel)	k_mutex_unlock(&mutex1); === */
    sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
	sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);

    k_mutex_unlock(&mutex);
    k_sem_give(&feu2);
}

static void vehicule3(void *p1, void *p2, void *p3) {
    k_sem_take(&feu2, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);
    printk("Traversee du vehicue 3 sur la voie 2 \n");

    /* === AJOUT : afficher '3' pendant la traversee === */
	sevenseg_show_char(sevensegment1, SEVENSEGMENT1_PINS_COUNT, 'C');
    sevenseg_show_char(sevensegment0, SEVENSEGMENT0_PINS_COUNT, '3');

    k_msleep(2000);

    /* === AJOUT : eteindre apres la traversee (optionnel) === */
    sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
	sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);

    k_mutex_unlock(&mutex);
    k_sem_give(&feu2);
}


/*----------------------------------------------------THREAD POUR LES FEU------------------------------*/
bool a=true;
static void changement(void *p1, void *p2, void *p3) {

    while(1){
        a=!a;

        if(a==true){
            k_sem_take(&feu1, K_FOREVER);
            k_sem_give(&feu2);

            printk("FEU1 ROUGE\n");
            printk("FEU2 VERT\n");

			sevenseg_show_char(sevensegment3, SEVENSEGMENT3_PINS_COUNT, 'F');
			sevenseg_show_char(sevensegment2, SEVENSEGMENT2_PINS_COUNT, '2');

        }
        else{
            k_sem_take(&feu2, K_FOREVER);
            k_sem_give(&feu1);

            printk("FEU1 VERT\n");
            printk("FEU2 ROUGE\n");

			sevenseg_show_char(sevensegment3, SEVENSEGMENT3_PINS_COUNT, 'F');
			sevenseg_show_char(sevensegment2, SEVENSEGMENT2_PINS_COUNT, '1');
        }
        k_msleep(10000);
		sevenseg_clear_all();
    }

}



/* Gestion des demandes de traversée piétonne - voie 1 */
static void demande_pieton1(void *p1, void *p2, void *p3)
{
    while (1) {
        gpio_port_value_t bouton1;
        gpio_port_get(port_swbtn, &bouton1);
		
        if (bouton1 == 256) {
            k_sem_take(&feu1, K_FOREVER);

			sevenseg_show_char(sevensegment1, SEVENSEGMENT1_PINS_COUNT, '6');
    		sevenseg_show_char(sevensegment0, SEVENSEGMENT0_PINS_COUNT, '0');
			
			printk("\nFEU1 ROUGE\n");
            printk("FEU2 VERT\n");
            printk("Traversée piéton voie 1\n\n");

			k_msleep(1000);

			sevenseg_clear(sevensegment1, SEVENSEGMENT1_PINS_COUNT);
			sevenseg_clear(sevensegment0, SEVENSEGMENT0_PINS_COUNT);

            k_sem_give(&feu1);
			

			

			
	}

		k_msleep(1000);

		

    }
}

K_THREAD_DEFINE(process2, 256, changement, NULL, NULL, NULL, 9, 0, 0);
K_THREAD_DEFINE(process1, 256, vehicule4, NULL, NULL, NULL, 10, 0, 0);

K_THREAD_DEFINE(process3, 256, vehicule1, NULL, NULL, NULL, 12, 0, 0);
K_THREAD_DEFINE(process4, 256, vehicule5, NULL, NULL, NULL, 15, 0, 0);
K_THREAD_DEFINE(process5, 256, vehicule2, NULL, NULL, NULL, 20, 0, 0);
K_THREAD_DEFINE(process6, 256, vehicule3, NULL, NULL, NULL, 20, 0, 0);
K_THREAD_DEFINE(process7, 256, demande_pieton1, NULL, NULL, NULL, 2, 0, 0);



int main(void) {
    int ret = 0;
    /* Tests sur la disponibilité des entrées et sorties */
    if (!device_is_ready(port_io)
        || !device_is_ready(port_led)
        || !device_is_ready(port_swbtn)
        || !device_is_ready(port_7seg01)
        || !device_is_ready(port_7seg23)) {
        return 1;
    }

    /* Configuration des ports d'entrée ou de sortie */
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        ret += gpio_pin_configure_dt(leds + i, GPIO_OUTPUT_ACTIVE);
    }   
    for (size_t i = 0; i < KEYS_COUNT; i++) {
        ret += gpio_pin_configure_dt(keys + i, GPIO_INPUT);
    }
    for (size_t i = 0; i < SEVENSEGMENT0_PINS_COUNT; i++) {
        ret += gpio_pin_configure_dt(sevensegment0 + i, GPIO_OUTPUT_ACTIVE);
    }
    for (size_t i = 0; i < SEVENSEGMENT1_PINS_COUNT; i++) {
        ret += gpio_pin_configure_dt(sevensegment1 + i, GPIO_OUTPUT_ACTIVE);
    }
    for (size_t i = 0; i < SEVENSEGMENT2_PINS_COUNT; i++) {
        ret += gpio_pin_configure_dt(sevensegment2 + i, GPIO_OUTPUT_ACTIVE);
    }
    for (size_t i = 0; i < SEVENSEGMENT3_PINS_COUNT; i++) {
        ret += gpio_pin_configure_dt(sevensegment3 + i, GPIO_OUTPUT_ACTIVE);
    }

    if (ret < 0) {
        return -1;
    }

    /* ====================== AJOUT : clear des 7-seg au demarrage ====================== */
    sevenseg_clear_all();
    /* (option) afficher 0 au repos sur le premier afficheur */
    // sevenseg_show_digit(sevensegment0, SEVENSEGMENT0_PINS_COUNT, 0);
    /* ================================================================================== */

    /* Boucle infinie */
    while (1) {
        gpio_port_value_t gpioc_value;
        gpio_port_get(port_swbtn, &gpioc_value);

        ret += gpio_port_set_masked(port_led, 0xFFFF, gpioc_value);

        if (ret < 0) {
            return -1;
        } 

        k_msleep(500);
    }
    return 1;
}