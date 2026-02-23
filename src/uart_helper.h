#ifndef UART_HELPER_H
#define UART_HELPER_H

#include <stdint.h>

#define UART_NUM UART_NUM_0          // Use UART0 (USB)
#define UART_BAUD_RATE 115200        // Standard baud rate
#define UART_BUF_SIZE 1024           // RX/TX buffer size

// UART0 on ESP32 DevKit: GPIO1 (TX), GPIO3 (RX)
#define UART_TX_PIN GPIO_NUM_1       // TX pin (USB converter)
#define UART_RX_PIN GPIO_NUM_3       // RX pin (USB converter)

/**
 * Initialize UART0 with USB-to-UART converter.
 * Call this function once during startup.
 */
void uart_init(void);

/**
 * Write a null-terminated string via UART.
 * @param src: Pointer to null-terminated string.
 */
void uart_write(const char* src);

/**
 * Write raw bytes via UART.
 * @param data: Pointer to data buffer.
 * @param len: Number of bytes to write.
 */
void uart_write_bytes_helper(const char* data, uint32_t len);

/**
 * Read data from UART.
 * @param buf: Pointer to buffer for received data.
 * @param max_len: Maximum number of bytes to read.
 * @return: Number of bytes actually read.
 */
uint32_t uart_read(char* buf, uint32_t max_len);

/**
 * Check if data is available in UART RX buffer.
 * @return: Number of bytes available to read.
 */
uint32_t uart_available(void);

#endif // UART_HELPER_H
