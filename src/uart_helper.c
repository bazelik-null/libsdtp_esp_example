#include <string.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include "uart_helper.h"

void uart_init(void) {
    // Configure UART0 parameters
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {
            .allow_pd = 0,
            .backup_before_sleep = 0
        }
    };

    // Configure UART parameters
    esp_err_t ret = uart_param_config(UART_NUM, &uart_config);
    if (ret != ESP_OK) {
        return;
    }

    // Install UART driver
    ret = uart_driver_install(
        UART_NUM,
        UART_BUF_SIZE * 2,  // RX buffer size
        UART_BUF_SIZE * 2,  // TX buffer size
        0,                  // Queue handle
        NULL,               // Queue
        0                   // Flags
    );
    if (ret != ESP_OK) {
        return;
    }

    // Set UART pins THIRD
    ret = uart_set_pin(
        UART_NUM,
        UART_TX_PIN,                    // TX pin (GPIO1)
        UART_RX_PIN,                    // RX pin (GPIO3)
        UART_PIN_NO_CHANGE,             // RTS (not used)
        UART_PIN_NO_CHANGE              // CTS (not used)
    );
    if (ret != ESP_OK) {
        return;
    }
}

void uart_write(const char* src) {
    if (src == NULL) return;

    uart_write_bytes(UART_NUM, (const uint8_t *)src, strlen(src));
}

void uart_write_bytes_helper(const char* data, const uint32_t len) {
    if (data == NULL || len == 0) return;

    uart_write_bytes(UART_NUM, (const uint8_t *)data, len);
}

uint32_t uart_read(char* buf, const uint32_t max_len) {
    if (buf == NULL || max_len == 0) return 0;

    return uart_read_bytes(UART_NUM, (uint8_t *)buf, max_len, 0);
}

uint32_t uart_available(void) {
    size_t available = 0;
    uart_get_buffered_data_len(UART_NUM, &available);

    return (uint32_t)available;
}
