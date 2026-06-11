#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "axi_multiplier.h"
#include "pl_dma.hpp" 

#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>

#define RX_DEV "/dev/dma_proxy_rx"
#define TX_DEV_CH0 "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch7"

/**
 * Parse a 4-character hex string to int16_t (signed).
 * Example: "FFD6" -> -42 (0xFFD6 as signed 16-bit)
 */
static int16_t hex_to_int16(const char* hex_str)
{
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
int read_channel_from_file(
    const char* filename,
    int channel,
    uint8_t* buffer,
    size_t buffer_size_bytes
)
{
    if (channel < 0 || channel > 7) {
        printf("Invalid channel: %d (must be 0-7)\n", channel);
        return -1;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open file %s: %s\n", filename, strerror(errno));
        return -1;
    }

    // Each sample has 8 channels * 4 hex chars = 32 hex chars per sample block
    // Channel offset within each block: channel * 2 (each channel has re + im = 2 values)
    int channel_offset = channel * 2;  // 0, 2, 4, 6, 8, 10, 12, 14

    size_t max_int16_elements = buffer_size_bytes / sizeof(int16_t);
    size_t int16_count = 0;

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

        if (j == 0) continue;  // Empty line

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
            re_hex[4] = '\0';
            im_hex[4] = '\0';

            int16_t re_val = hex_to_int16(re_hex);
            int16_t im_val = hex_to_int16(im_hex);

            // Fill buffer: re, im, re, im, ...
            if (int16_count + 2 <= max_int16_elements) {
                int16_t* buf16 = reinterpret_cast<int16_t*>(buffer);
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
    int16_t* buf16 = reinterpret_cast<int16_t*>(buffer);
    for (size_t i = int16_count; i < max_int16_elements; i++) {
        buf16[i] = 0;
    }

    printf("Loaded %zu int16 values (%zu real+imag pairs) for channel %d from %s\n",
           int16_count, int16_count / 2, channel, filename);
    
    return 0;
}

/**/**
 * Fill a uint8_t buffer from a file containing int16_t values.
 *
 * Assumptions:
 * - buffer_size is large enough for the file data (or file is truncated to buffer size).
 * - buffer is large enough for the data read.
 * - File contains int16_t values in binary format.
 */
int fill_buffer_from_file(
    uint8_t* buffer,
    size_t buffer_size_bytes,
    const char* filename
)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open file %s: %s\n", filename, strerror(errno));
        return -1;
    }

    size_t bytes_to_read = buffer_size_bytes;
    ssize_t bytes_read = read(fd, buffer, bytes_to_read);
    
    if (bytes_read < 0) {
        printf("Failed to read from file %s: %s\n", filename, strerror(errno));
        close(fd);
        return -1;
    }

    if (bytes_read < bytes_to_read) {
        printf("File %s is smaller than buffer (%zd bytes vs %zd bytes requested)\n", 
               filename, bytes_read, bytes_to_read);
        // Optionally pad remaining buffer with zeros
        memset(buffer + bytes_read, 0, bytes_to_read - bytes_read);
    }

    close(fd);
    printf("Successfully loaded %zd bytes from %s into buffer\n", bytes_read, filename);
    return 0;
}

 * Fill a uint8_t buffer with int16_t values starting from `start_cnt`.
 *
 * Assumptions:
 * - buffer_size is at least even (multiple of 2) in bytes.
 * - buffer is large enough for `(count) int16_t` values.
 */
void fill_int16_buffer(
    uint8_t* buffer,
    size_t buffer_size_bytes,
    int16_t start_cnt,
    int16_t step = 1
)
{
    int16_t* buf16 = reinterpret_cast<int16_t*>(buffer);
    size_t max_elements = buffer_size_bytes / sizeof(int16_t);

    for (size_t i = 0; i < max_elements; ++i) {
        buf16[i] = start_cnt + static_cast<int16_t>(i * step);
    }
}


int main(int argc, char *argv[])
{
    if (argc < 6) {
        printf("usage: %s <mult_ch0> <mult_ch1> <num_transfers> <input_file> <channel>\n", argv[0]);
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


    int16_t mult0 = (int16_t)strtol(argv[1], NULL, 0);
    int16_t mult1 = (int16_t)strtol(argv[2], NULL, 0);
    int16_t num_transfers = (int16_t)strtol(argv[3], NULL, 0);
    const char* input_file = argv[4];
    int channel = atoi(argv[5]);  // Channel to extract (0-7)
    
    axi_multiplier_init();
    axi_multiplier_set_mult(mult0, 0);
    axi_multiplier_set_mult(mult1, 1);


    pl_dma dma_ch0;
    pl_dma dma_ch1;


    std::vector<pl_dma::ch_config> tx_ch0 = {{
        .devnode = TX_DEV_CH0,
        .buffer_size = 4 * 1024,
    }};


    std::vector<pl_dma::ch_config> rx_ch0 = {{
        .devnode = RX_DEV,
        .buffer_size = 4 * 1024,
    }};



    std::vector<pl_dma::ch_config> tx_ch1 = {{
        .devnode = TX_DEV_CH1,
        .buffer_size = 4 * 1024,
    }};



    if (dma_ch0.init(tx_ch0, rx_ch0) != 0) {
        printf("Init dma for ch0 failed\n");
        return 1;
    }
    dma_ch0.set_num_transfers(num_transfers);


    if (dma_ch1.init(tx_ch1) != 0) {
        printf("Init dma for ch1 failed\n");
        return 1;
    }
    dma_ch1.set_num_transfers(num_transfers);


    uint8_t *rx_buf =  (uint8_t *)dma_ch0.get_rx_buffer(0);
    uint8_t *tx_buf0 = (uint8_t *)dma_ch0.get_tx_buffer(0);
    uint8_t *tx_buf1 = (uint8_t *)dma_ch1.get_tx_buffer(0);


    // Fill tx_buf0 from file for the specified channel
    if (read_channel_from_file(input_file, channel, tx_buf0, 4 * 1024) != 0) {
        printf("Failed to read channel %d from file\n", channel);
        return 1;
    }
    
    // For tx_buf1, you can use a different channel or the same
    // Example: use channel+1 for ch1 (if channel < 7)
    int channel1 = (channel + 1) % 8;
    if (read_channel_from_file(input_file, channel1, tx_buf1, 4 * 1024) != 0) {
        printf("Failed to read channel %d from file for ch1\n", channel1);
        return 1;
    }
    
    axi_multiplier_set_ch(0);
    dma_ch0.start();
    dma_ch1.start();
    WAIT_FOR_EXIT;
    dma_ch0.stop_transfer();
    dma_ch0.stop_receive();
    dma_ch1.stop_transfer();
    dma_ch0.stop();
    dma_ch1.stop();


    auto stats = dma_ch0.get_stats();
    printf("Throughput: %d MB/s\n", stats.mb_per_sec);


    dma_ch0.cleanup();
    dma_ch1.cleanup();
    axi_multiplier_deinit();
    piDeleteSafety(kbd);


    return 0;
}