#pragma once
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/bluetooth.h>

/* UUIDs UART */
#define BT_UUID_UART_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x6e400001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_UART_RX_VAL \
    BT_UUID_128_ENCODE(0x6e400002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_UART_TX_VAL \
    BT_UUID_128_ENCODE(0x6e400003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

#define BT_UUID_UART_SERVICE BT_UUID_DECLARE_128(BT_UUID_UART_SERVICE_VAL)
#define BT_UUID_UART_RX      BT_UUID_DECLARE_128(BT_UUID_UART_RX_VAL)
#define BT_UUID_UART_TX      BT_UUID_DECLARE_128(BT_UUID_UART_TX_VAL)

/* Fonction pour envoyer des données au téléphone */
void ble_uart_send(const uint8_t *data, uint16_t len);
