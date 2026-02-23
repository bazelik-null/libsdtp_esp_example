//
// Created by niko on 18.10.2025.
//

#include "uart_helper.h"
#include <api/libsdtp.h>

#include <driver/gpio.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>

#include <string.h>

// Pin for output buffer
#define OUTPUT_PIN GPIO_NUM_2
// Pin for input buffer
#define INPUT_PIN GPIO_NUM_4

#define IS_DEBUG true

sdtp_instance_t* instance = NULL;

void debug_write(const char* prefix, const char* msg) {
	if (!IS_DEBUG) return;

	uart_write(prefix);
	uart_write(msg);
	uart_write("\r\n");
}

void send_packet() {
	// Data to send
	const char payload[] = "Hello SDTP";

	// Construct packet
	sdtp_packet_t* packet = sdtp_construct_packet(payload, SDTP_DATA_PACKET);
	if (!packet) {
		debug_write("[ERROR]: ", "Packet construction failed");
		return;
	}

	// Write packet to the output buffer
	const sdtp_status_code_t res_code = sdtp_write_packet(instance, packet);
	if (res_code != SDTP_OK) {
		if (IS_DEBUG) {
			char error_msg_buffer[100];
			sprintf(error_msg_buffer, "Packet write failed: %d", res_code);
			debug_write("[ERROR]: ", error_msg_buffer);
		}
		sdtp_packet_free(packet);
		return;
	}

	// Transfer data from output buffer to input buffer (temporary, will be removed with driver support)
	const size_t output_buffer_used = sdtp_buffer_get_used_space(instance->output_buffer);
	if (output_buffer_used > 0) {
		uint8_t* tmp_buffer = malloc(output_buffer_used);
		if (tmp_buffer) {
			const size_t output_read = sdtp_buffer_read(instance, SDTP_OUTPUT_BUFFER, tmp_buffer, output_buffer_used, SDTP_READ_PARTIAL);
			const size_t written_bytes = sdtp_buffer_write(instance, SDTP_INPUT_BUFFER, tmp_buffer, output_read);
			free(tmp_buffer);

			if (written_bytes == 0 && IS_DEBUG) {
				debug_write("[ERROR]: ", "Failed to transfer packet to input buffer");
			}
		}
	}

	// Free memory
	sdtp_packet_free(packet);

	debug_write("[INFO]: ", "Packet was send");
}

void read_packet() {
	// Check is buffer empty
	if (sdtp_buffer_get_used_space(instance->input_buffer) == 0) return;

	// Read packet from the input buffer.
	sdtp_packet_t* packet = sdtp_read_packet(instance, SDTP_READ_PARTIAL);

	// If packet reading failed
	if (!packet) {
		debug_write("[ERROR]: ", "Packet read failed");
		return;
	}

	debug_write("[INFO]: ", "Packet read complete");

	// Print packet contents
	char* msg_buffer = sdtp_get_char_data(packet);
	debug_write("[DEBUG]: Contents: ", msg_buffer);
	debug_write("", "");

	// Free memory
	sdtp_packet_free(packet);
	free(msg_buffer);
}

// Loop
void app_loop() {
	// Send hello message
	send_packet();
	// Read incoming messages
	read_packet();

	// Delay for 1 second
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Entry point
[[noreturn]]
void app_main() {
	// Init debugging UART connection
	if (IS_DEBUG) uart_init();

	debug_write("", "");
	debug_write("", "=== libsdtp ===");
	debug_write("[INFO]: ", "Initializing libsdtp configuration...");

	// Setup pins
	gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
	gpio_set_direction(OUTPUT_PIN, GPIO_MODE_OUTPUT);

	const sdtp_config_t config = {
		.input_bus_pin = INPUT_PIN,
		.output_bus_pin = OUTPUT_PIN,
		.buffer_size = 4096,
		.baud_rate = 115200,
		.device_id = esp_random(),
		.device_type = SDTP_CONTROLLER
	};

	// Init SDTP
	instance = sdtp_instance_create(&config);

	// If instance was not created
	if (!instance) {
		debug_write("[ERROR]: ", "Setup failed");
		sdtp_instance_close(instance);
		while (1) {} // Hang up
	}

	debug_write("[INFO]: ", "Setup complete");

	// Finish setup and start main loop
	while (true) {
		app_loop();
	}
}
