#include "axi_dsp.h"
#include "axi_multiplier.h"
#include "pl_dma.hpp"

#include <cstdint>
#include <fcntl.h>
#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define RX_DEV          "/dev/dma_proxy_rx"
#define TX_DEV_CH0      "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1      "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH2      "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH3      "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH4      "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH5      "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH6      "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH7      "/dev/dma_proxy_tx_ch7"

// #define AXI_MULTIPLIER
#define NUM_CHANNELS_RX 1
#define NUM_CHANNELS_TX 8


/**
 * Parse a 4-character hex string to int16_t (signed).
 * Example: "FFD6" -> -42 (0xFFD6 as signed 16-bit)
 */
static int16_t hex_to_int16(const char * hex_str) {
	uint16_t val = (uint16_t)strtoul(hex_str, NULL, 16);
	return (int16_t)val;
}

/**
 * Fill a uint8_t buffer from a file containing int16_t values.
 *
 * Assumptions:
 * - buffer_size is large enough for the file data (or file is truncated to buffer size).
 * - buffer is large enough for the data read.
 * - File contains int16_t values in binary format.
 */
int fill_buffer_from_file(uint8_t * buffer, size_t buffer_size_bytes, const char * filename) {
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("Failed to open file %s: %s\n", filename, strerror(errno));
		return -1;
	}

	size_t bytes_to_read = buffer_size_bytes;
	ssize_t bytes_read   = read(fd, buffer, bytes_to_read);

	if (bytes_read < 0) {
		printf("Failed to read from file %s: %s\n", filename, strerror(errno));
		close(fd);
		return -1;
	}

	if (bytes_read < bytes_to_read) {
		printf("File %s is smaller than buffer (%zd bytes vs %zd bytes requested)\n", filename, bytes_read, bytes_to_read);
		// Optionally pad remaining buffer with zeros
		memset(buffer + bytes_read, 0, bytes_to_read - bytes_read);
	}

	close(fd);
	printf("Successfully loaded %zd bytes from %s into buffer\n", bytes_read, filename);
	return 0;
}

/**
 * Read a hex file once and extract data for all 8 channels.
 * File format: ch0_re ch0_im ch1_re ch1_im ... ch7_re ch7_im
 * Each value is 4 hex chars (16‑bit signed).
 *
 * For each channel, the buffer is filled with:
 *   re0, im0, re1, im1, re2, im2, ...
 *
 * @param filename        Path to the hex file
 * @param buffers[8]      Array of 8 output buffers (uint8_t*), each holds int16_t data
 * @param buffer_size_bytes  Size of each buffer in bytes (must be same for all)
 * @return 0 on success, -1 on error
 */
int read_all_channels_from_file(const char * filename, uint8_t * buffers[8], size_t buffer_size_bytes) {
	FILE * fp = fopen(filename, "r");
	if (!fp) {
		printf("Failed to open file %s: %s\n", filename, strerror(errno));
		return -1;
	}

	size_t max_int16_elements = buffer_size_bytes / sizeof(int16_t);
	size_t max_samples        = max_int16_elements / 2; // each sample = re+im pair per channel

	int16_t * buf16[8];
	for (int ch = 0; ch < 8; ch++) {
		buf16[ch] = (int16_t *)buffers[ch];
	}

	size_t sample_count = 0;
	char line[256];

	while (sample_count < max_samples && fgets(line, sizeof(line), fp)) {
		// Remove whitespace and newlines
		char clean[256];
		size_t j = 0;
		for (size_t i = 0; i < strlen(line) && j < sizeof(clean) - 1; i++) {
			if (line[i] != ' ' && line[i] != '\n' && line[i] != '\r' && line[i] != '\t') {
				clean[j++] = line[i];
			}
		}
		clean[j] = '\0';
		if (j == 0) continue; // skip empty lines

		size_t line_len = strlen(clean);
		// Each sample block is 8 channels * (re+im) * 4 hex chars = 64 hex chars
		for (size_t pos = 0; pos + 64 <= line_len && sample_count < max_samples; pos += 64) {
			// Extract all 8 channels from this sample block
			for (int ch = 0; ch < 8; ch++) {
				int re_offset = (ch * 2) * 4; // channel * 2 values * 4 chars
				int im_offset = (ch * 2 + 1) * 4;

				char re_hex[5], im_hex[5];
				memcpy(re_hex, clean + pos + re_offset, 4);
				memcpy(im_hex, clean + pos + im_offset, 4);
				re_hex[4]                       = '\0';
				im_hex[4]                       = '\0';

				int16_t re_val                  = (int16_t)strtoul(re_hex, NULL, 16);
				int16_t im_val                  = (int16_t)strtoul(im_hex, NULL, 16);

				// Store re, im consecutively
				buf16[ch][sample_count * 2]     = re_val;
				buf16[ch][sample_count * 2 + 1] = im_val;
			}
			sample_count++;
		}
	}

	fclose(fp);

	// Pad remaining space in each buffer with zeros (if file was shorter)
	for (int ch = 0; ch < 8; ch++) {
		for (size_t i = sample_count * 2; i < max_int16_elements; i++) {
			buf16[ch][i] = 0;
		}
	}

	printf("Loaded %zu samples (all channels) from %s\n", sample_count, filename);
	return 0;
}


int main(int argc, char * argv[]) {
	if (argc < 5) {
		printf("usage: %s <test_point> <channel> <input_file> <output_file>\n", argv[0]);
		return 1;
	}


	PISignals::setSlot([](PISignals::Signal s) {
		piCout << "Signal" << s;
		PIKbdListener::exiting = true;
		PISignals::releaseSignals(s);
	});
	PISignals::grabSignals(PISignals::Interrupt | PISignals::Termination);


	PIKbdListener * kbd = nullptr;


	kbd                 = new PIKbdListener(nullptr, nullptr, false);
	kbd->enableExitCapture(PIKbdListener::F10);


	uint32_t test_point      = (uint32_t)strtol(argv[1], NULL, 0);
	uint32_t channel         = (uint32_t)strtol(argv[2], NULL, 0);
	const char * input_file  = argv[3];
	const char * output_file = argv[4]; // File to dump RX data (optional, can be empty string)

	int buf_size             = BUFFER_SIZE;
	uint32_t num_transfers   = 0;
	uint32_t n_samps_per_buf = 232;

	PIVector<dma_channel> dma_channels;
	void * rx_buffers[RX_BUFFER_COUNT];
	void * tx_buffers[NUM_CHANNELS_TX][TX_BUFFER_COUNT];

	dma_channels.resize(NUM_CHANNELS_TX + NUM_CHANNELS_RX);

	PIString tx_devnodes[NUM_CHANNELS_TX] =
		{TX_DEV_CH0, TX_DEV_CH1, TX_DEV_CH2, TX_DEV_CH3, TX_DEV_CH4, TX_DEV_CH5, TX_DEV_CH6, TX_DEV_CH7};

	ch_config rx_config = {.devnode = RX_DEV, .buffer_size = BUFFER_SIZE, .buffer_count = RX_BUFFER_COUNT};
	ch_config tx_config = {.buffer_size = BUFFER_SIZE, .buffer_count = TX_BUFFER_COUNT};
	dma_channels[0].init(rx_config);
	dma_channels[0].set_save_to_file(output_file, n_samps_per_buf) dma_channels[0].set_num_transfers(num_transfers);
	dma_channels[0].get_all_buffers(rx_buffers);
	piCout << "Rx buffers adresses are:";
	for (size_t i = 0; i < RX_BUFFER_COUNT; i++) {
		piCout << "num " << i << " " << PICoutManipulators::PICoutFormat::Hex << rx_buffers[i];
	}


	piCout << "Tx buffer adresses are:";
	for (size_t i = 0; i < NUM_CHANNELS_TX; i++) {
		tx_config.devnode = tx_devnodes[i] dma_channels[i + 1].init(tx_config);
		dma_channels[i + 1].set_num_transfers(num_transfers);
		dma_channels[i + 1].get_all_buffers(tx_buffers[i]);
		for (size_t k; k < TX_BUFFER_COUNT; k++) {
			piCout << "ch" << k << " " << PICoutManipulators::PICoutFormat::Hex << tx_buffers[i][k];
		}
	}

#ifdef AXI_MULTIPLIER
	axi_multiplier_init();
	axi_multiplier_set_mult(1, 0);
	axi_multiplier_set_mult(2, 1);
	axi_multiplier_set_mult(3, 2);
	axi_multiplier_set_mult(4, 3);
	axi_multiplier_set_mult(5, 4);
	axi_multiplier_set_mult(6, 5);
	axi_multiplier_set_mult(7, 6);
	axi_multiplier_set_mult(8, 7);
#else
	axi_dsp_init();
	axi_dsp_set_output_source(test_point, channel);
	auto v = axi_dsp_get_output_source();
	piCout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << "\n";
	axi_dsp_apply();
#endif


	if (read_all_channels_from_file(input_file, tx_buffers, buf_size) != 0) {
		printf("Failed to read channels from file\n");
		return 1;
	}

	int buff_id = 0;
	for (size_t i = 0; i < num_transfers; i++) {
		uint8_t * current_buffers[TX_BUFFER_COUNT];
		for (size_t k = 0; k < TX_BUFFER_COUNT; k++) {
			current_buffers[k] = tx_buffers[buff_id][k]
		}
		read_all_channels_from_file(input_file, tx_buffers, buf_size);
		for (size_t k = TX_BUFFER_COUNT + RX_BUFFER_COUNT - 1; k >= 0; k--) {
			dma_channels[k].single_transfer();
		}
		buff_id = (buff_id + 1) % TX_BUFFER_COUNT;
	}

	WAIT_FOR_EXIT;

#ifdef AXI_MULTIPLIER
	axi_multiplier_deinit();
#else
	axi_dsp_deinit();
#endif

	piDeleteSafety(kbd);

	return 0;
}