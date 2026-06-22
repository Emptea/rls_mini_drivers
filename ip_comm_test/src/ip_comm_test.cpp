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

#define RX_DEV     "/dev/dma_proxy_rx"
#define TX_DEV_CH0 "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH2 "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH3 "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH4 "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH5 "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH6 "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH7 "/dev/dma_proxy_tx_ch7"

// #define AXI_MULTIPLIER


/**
 * Parse a 4-character hex string to int16_t (signed).
 * Example: "FFD6" -> -42 (0xFFD6 as signed 16-bit)
 */
static int16_t hex_to_int16(const char * hex_str) {
	uint16_t val = (uint16_t)strtoul(hex_str, NULL, 16);
	return (int16_t)val;
}


/**
 * Read a hex file with format: ch0_re ch0_im ch1_re ch1_im ... ch7_re ch7_im
 * Each value is 4 hex chars (16 bits).
 *
 * Extracts data for the given channel and fills buffer with:
 * re0_chN, im0_chN, re1_chN, im1_chN, re2_chN, im2_chN, ...
 *
 * @param filename Path to the hex file
 * @param channel Channel number (0-7)
 * @param buffer Output buffer (uint8_t*) to fill with int16_t values
 * @param buffer_size_bytes Size of buffer in bytes
 * @return 0 on success, -1 on error
 */
int read_channel_from_file(const char * filename, int channel, uint8_t * buffer, size_t buffer_size_bytes) {
	if (channel < 0 || channel > 7) {
		printf("Invalid channel: %d (must be 0-7)\n", channel);
		return -1;
	}

	FILE * fp = fopen(filename, "r");
	if (!fp) {
		printf("Failed to open file %s: %s\n", filename, strerror(errno));
		return -1;
	}

	// Each sample has 8 channels * 4 hex chars = 32 hex chars per sample block
	// Channel offset within each block: channel * 2 (each channel has re + im = 2 values)
	int channel_offset        = channel * 2; // 0, 2, 4, 6, 8, 10, 12, 14

	size_t max_int16_elements = buffer_size_bytes / sizeof(int16_t);
	size_t int16_count        = 0;

	char line[256];
	size_t samples_read = 0;

	while (int16_count < max_int16_elements && fgets(line, sizeof(line), fp)) {
		// Remove whitespace/newlines
		char clean_line[256];
		size_t j = 0;
		for (size_t i = 0; i < strlen(line) && j < sizeof(clean_line) - 1; i++) {
			if (line[i] != ' ' && line[i] != '\n' && line[i] != '\r' && line[i] != '\t') {
				clean_line[j++] = line[i];
			}
		}
		clean_line[j] = '\0';

		if (j == 0) continue; // Empty line

		// Each sample block is 8 channels * 4 chars each = 32 hex chars
		// But the file might have multiple samples per line
		size_t line_len = strlen(clean_line);

		for (size_t pos = 0; pos + 32 <= line_len; pos += 32) {
			// Extract real and imaginary for the target channel
			// Real is at offset: channel_offset * 4
			// Imag is at offset: (channel_offset + 1) * 4
			int re_offset = channel_offset * 4;
			int im_offset = (channel_offset + 1) * 4;

			char re_hex[5];
			char im_hex[5];

			memcpy(re_hex, clean_line + pos + re_offset, 4);
			memcpy(im_hex, clean_line + pos + im_offset, 4);
			re_hex[4]      = '\0';
			im_hex[4]      = '\0';

			int16_t re_val = hex_to_int16(re_hex);
			int16_t im_val = hex_to_int16(im_hex);

			// Fill buffer: re, im, re, im, ...
			if (int16_count + 2 <= max_int16_elements) {
				int16_t * buf16      = reinterpret_cast<int16_t *>(buffer);
				buf16[int16_count++] = re_val;
				buf16[int16_count++] = im_val;
			}
		}
		samples_read++;
	}

	fclose(fp);

	if (int16_count == 0) {
		printf("No data found for channel %d in file %s\n", channel, filename);
		return -1;
	}

	// Pad remaining buffer with zeros if file was smaller than buffer
	int16_t * buf16 = reinterpret_cast<int16_t *>(buffer);
	for (size_t i = int16_count; i < max_int16_elements; i++) {
		buf16[i] = 0;
	}

	printf("Loaded %zu int16 values (%zu real+imag pairs) for channel %d from %s\n", int16_count, int16_count / 2, channel, filename);

	return 0;
}

/**
 * Dump int16 buffer to hex file.
 * Buffer contains interleaved re/im for a single channel: re0, im0, re1, im1, re2, im2, ...
 *
 * Output format: re0_im0 (one pair per line, 8 hex chars)
 *
 * @param buffer Input buffer containing int16_t values (re/im interleaved)
 * @param buffer_size_bytes Size of buffer in bytes
 * @param output_filename Path to output hex file
 * @return 0 on success, -1 on error
 */
int dump_buffer_to_file(const void * buffer, size_t buffer_size_bytes, const char * output_filename) {
	FILE * fp = fopen(output_filename, "w");
	if (!fp) {
		printf("Failed to open output file %s: %s\n", output_filename, strerror(errno));
		return -1;
	}

	size_t num_int16      = buffer_size_bytes / sizeof(int16_t);
	const int16_t * buf16 = reinterpret_cast<const int16_t *>(buffer);

	// Write one re/im pair per line
	for (size_t i = 0; i < num_int16; i += 2) {
		if (i + 1 >= num_int16) break; // Need pairs

		int16_t re_val = buf16[i];
		int16_t im_val = buf16[i + 1];

		// Convert to 4-char hex (uppercase, zero-padded)
		fprintf(fp, "%04X%04X\n", (uint16_t)re_val, (uint16_t)im_val);
	}

	fclose(fp);

	size_t total_samples = num_int16 / 2;
	printf("Dumped %zu int16 values (%zu re/im pairs) to %s\n", num_int16, total_samples, output_filename);

	return 0;
}

/**
 * Dump N bytes from each RX buffer to a hex file.
 * Each buffer's data is written sequentially with a header indicating the buffer index.
 *
 * @param rx_buffers Array of pointers to RX buffers
 * @param num_buffers Number of buffers in the array
 * @param bytes_per_buffer Number of bytes to read from each buffer
 * @param output_filename Path to output hex file
 * @return 0 on success, -1 on error
 */
int dump_rx_buffers_to_file(void ** rx_buffers, size_t num_buffers, size_t bytes_per_buffer, const char * output_filename) {
	FILE * fp = fopen(output_filename, "w");
	if (!fp) {
		printf("Failed to open output file %s: %s\n", output_filename, strerror(errno));
		return -1;
	}

	size_t num_int16 = bytes_per_buffer / sizeof(int16_t);

	for (size_t buf_idx = 0; buf_idx < num_buffers; buf_idx++) {
		// if (!rx_buffers[buf_idx]) {
		// 	fprintf(fp, "# Buffer %zu: NULL\n", buf_idx);
		// 	continue;
		// }

		// fprintf(fp, "# Buffer %zu (address: %p)\n", buf_idx, rx_buffers[buf_idx]);

		const int16_t * buf16 = reinterpret_cast<const int16_t *>(rx_buffers[buf_idx]);

		// Write one re/im pair per line
		for (size_t i = 0; i < num_int16; i += 2) {
			if (i + 1 >= num_int16) break; // Need pairs

			int16_t re_val = buf16[i];
			int16_t im_val = buf16[i + 1];

			fprintf(fp, "%04X%04X\n", (uint16_t)re_val, (uint16_t)im_val);
		}

		// fprintf(fp, "\n"); // Empty line between buffers
	}

	fclose(fp);

	printf("Dumped %zu buffers (%zu bytes each) to %s\n", num_buffers, bytes_per_buffer, output_filename);
	return 0;
}


/**
 * Dump RX buffer from DMA to hex file.
 * Reads the RX buffer and writes it as re/im pairs.
 *
 * @param rx_buf RX buffer from DMA
 * @param buffer_size_bytes Size of RX buffer in bytes
 * @param output_filename Path to output hex file
 * @return 0 on success, -1 on error
 */
int dump_dma_rx_to_file(const void * rx_buf, size_t buffer_size_bytes, const char * output_filename) {
	return dump_buffer_to_file(rx_buf, buffer_size_bytes, output_filename);
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

/* Fill a uint8_t buffer with int16_t values starting from `start_cnt`.
 *
 * Assumptions:
 * - buffer_size is at least even (multiple of 2) in bytes.
 * - buffer is large enough for `(count) int16_t` values.
 */
void fill_int16_buffer(uint8_t * buffer, size_t buffer_size_bytes, int16_t start_cnt, int16_t step = 1) {
	int16_t * buf16     = reinterpret_cast<int16_t *>(buffer);
	size_t max_elements = buffer_size_bytes / sizeof(int16_t);

	for (size_t i = 0; i < max_elements; ++i) {
		buf16[i] = start_cnt + static_cast<int16_t>(i * step);
	}
}

static int init_dma_tx(pl_dma & dma, std::vector<pl_dma::ch_config> tx_dma, uint32_t num_transfers) {
	if (dma.init(tx_dma) != 0) {
		printf("Init dma for ch0 failed\n");
		return 1;
	}
	dma.set_num_transfers(num_transfers);
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
	// axi_dsp_set_test_point(test_point);
	// axi_dsp_set_channel(channel);
	axi_dsp_set_output_source(test_point, channel);
	auto v = axi_dsp_get_output_source();
	piCout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << "\n";
	axi_dsp_apply();
#endif

	pl_dma dma_ch0;
	pl_dma dma_ch1, dma_ch2, dma_ch3, dma_ch4, dma_ch5, dma_ch6, dma_ch7;


	std::vector<pl_dma::ch_config> tx_ch0 = {
		{
         .devnode     = TX_DEV_CH0,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> rx_ch0 = {
		{
         .devnode     = RX_DEV,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch1 = {
		{
         .devnode     = TX_DEV_CH1,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch2 = {
		{
         .devnode     = TX_DEV_CH2,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch3 = {
		{
         .devnode     = TX_DEV_CH3,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch4 = {
		{
         .devnode     = TX_DEV_CH4,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch5 = {
		{
         .devnode     = TX_DEV_CH5,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch6 = {
		{
         .devnode     = TX_DEV_CH6,
         .buffer_size = buf_size,
		 }
    };

	std::vector<pl_dma::ch_config> tx_ch7 = {
		{
         .devnode     = TX_DEV_CH7,
         .buffer_size = buf_size,
		 }
    };

	if (dma_ch0.init(tx_ch0, rx_ch0) != 0) {
		printf("Init dma for ch0 failed\n");
		return 1;
	}
	dma_ch0.set_num_transfers(num_transfers);

	init_dma_tx(dma_ch1, tx_ch1, num_transfers);
	init_dma_tx(dma_ch2, tx_ch2, num_transfers);
	init_dma_tx(dma_ch3, tx_ch3, num_transfers);
	init_dma_tx(dma_ch4, tx_ch4, num_transfers);
	init_dma_tx(dma_ch5, tx_ch5, num_transfers);
	init_dma_tx(dma_ch6, tx_ch6, num_transfers);
	init_dma_tx(dma_ch7, tx_ch7, num_transfers);

	void * rx_buffers[RX_BUFFER_COUNT];
	dma_ch0.get_all_rx_buffers(rx_buffers);

	piCout << "Rx buffers adresses are:";
	for (size_t i = 0; i < RX_BUFFER_COUNT; i++) {
		piCout << "num " << i << " " << PICoutManipulators::PICoutFormat::Hex << rx_buffers[i];
	}
	uint8_t * tx_buf0        = (uint8_t *)dma_ch0.get_tx_buffer(0);
	uint8_t * tx_buf1        = (uint8_t *)dma_ch1.get_tx_buffer(0);
	uint8_t * tx_buf2        = (uint8_t *)dma_ch2.get_tx_buffer(0);
	uint8_t * tx_buf3        = (uint8_t *)dma_ch3.get_tx_buffer(0);
	uint8_t * tx_buf4        = (uint8_t *)dma_ch4.get_tx_buffer(0);
	uint8_t * tx_buf5        = (uint8_t *)dma_ch5.get_tx_buffer(0);
	uint8_t * tx_buf6        = (uint8_t *)dma_ch6.get_tx_buffer(0);
	uint8_t * tx_buf7        = (uint8_t *)dma_ch7.get_tx_buffer(0);


	uint8_t * all_buffers[8] = {tx_buf0, tx_buf1, tx_buf2, tx_buf3, tx_buf4, tx_buf5, tx_buf6, tx_buf7};
	piCout << "Buffer adresses are:";
	for (size_t i = 0; i < 8; i++) {
		piCout << "ch" << i << " " << PICoutManipulators::PICoutFormat::Hex << all_buffers[i];
	}

	if (read_all_channels_from_file(input_file, all_buffers, buf_size) != 0) {
		printf("Failed to read channels from file\n");
		return 1;
	}

	dma_ch0.start();
	dma_ch1.start();
	dma_ch2.start();
	dma_ch3.start();
	dma_ch4.start();
	dma_ch5.start();
	dma_ch6.start();
	dma_ch7.start();

	WAIT_FOR_EXIT;

	dma_ch0.stop_transfer();
	dma_ch0.stop_receive();
	dma_ch1.stop_transfer();
	dma_ch2.stop_transfer();
	dma_ch3.stop_transfer();
	dma_ch4.stop_transfer();
	dma_ch5.stop_transfer();
	dma_ch6.stop_transfer();
	dma_ch7.stop_transfer();

	dma_ch0.stop();
	dma_ch1.stop();
	dma_ch2.stop();
	dma_ch3.stop();
	dma_ch4.stop();
	dma_ch5.stop();
	dma_ch6.stop();
	dma_ch7.stop();

	auto stats = dma_ch0.get_stats();
	printf("Throughput: %d MB/s\n", stats.mb_per_sec);

	// Dump RX buffer to hex file (only re/im pairs for one channel)
	if (output_file && strlen(output_file) > 0) {
		// Print memory addresses and sizes for debugging
		printf("rx_buffers[0] = %p\n", rx_buffers[0]);
		printf("rx_buffers[1] = %p\n", rx_buffers[1]);
		printf("Difference = %td bytes\n", (char *)rx_buffers[1] - (char *)rx_buffers[0]);
		printf("BUFFER_SIZE = %ld\n", BUFFER_SIZE);
		printf("BUFFER_SIZE + 64 = %ld\n", BUFFER_SIZE + 64);
		printf("Total size to dump = %zu bytes\n", (size_t)(BUFFER_SIZE + 64) * RX_BUFFER_COUNT);


		
		if (dump_dma_rx_to_file(rx_buffers[0], (BUFFER_SIZE + 64) * RX_BUFFER_COUNT, "dump_all.hex") != 0) {
			printf("Failed to dump RX buffer to file\n");
			return 1;
		}

		if (output_file && strlen(output_file) > 0) {
			// Dump all buffers with N bytes each
			if (dump_rx_buffers_to_file(rx_buffers, RX_BUFFER_COUNT, 232*4, output_file) != 0) {
				printf("Failed to dump RX buffers to file\n");
				return 1;
			}
		}
	}

	dma_ch0.cleanup();
	dma_ch1.cleanup();
	dma_ch2.cleanup();
	dma_ch3.cleanup();
	dma_ch4.cleanup();
	dma_ch5.cleanup();
	dma_ch6.cleanup();
	dma_ch7.cleanup();

#ifdef AXI_MULTIPLIER
	axi_multiplier_deinit();
#else
	axi_dsp_deinit();
#endif

	piDeleteSafety(kbd);

	return 0;
}