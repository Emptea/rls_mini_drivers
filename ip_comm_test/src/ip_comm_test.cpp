#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "axi_dsp.h"
#include "pl_dma.hpp" 

#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>
#include <string.h>

#define RX_DEV "/dev/dma_proxy_rx"
#define TX_DEV_CH0 "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH2 "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH3 "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH4 "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH5 "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH6 "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH7 "/dev/dma_proxy_tx_ch7"

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

/**
 * Dump int16 buffer to hex file.
 * Buffer contains interleaved re/im for a single channel: re0, im0, re1, im1, re2, im2, ...
 * 
 * Output format: re0_im0 re1_im1 re2_im2 ... (64 hex chars = 16 int16 per line)
 * 
 * @param buffer Input buffer containing int16_t values (re/im interleaved)
 * @param buffer_size_bytes Size of buffer in bytes
 * @param output_filename Path to output hex file
 * @return 0 on success, -1 on error
 */
int dump_buffer_to_file(
    const uint8_t* buffer,
    size_t buffer_size_bytes,
    const char* output_filename
)
{
    FILE* fp = fopen(output_filename, "w");
    if (!fp) {
        printf("Failed to open output file %s: %s\n", output_filename, strerror(errno));
        return -1;
    }

    size_t num_int16 = buffer_size_bytes / sizeof(int16_t);
    const int16_t* buf16 = reinterpret_cast<const int16_t*>(buffer);

    // Group into lines of 16 int16 values (8 re/im pairs = 64 hex chars)
    const size_t int16_per_line = 16;
    char line_buffer[256];  // 16 int16 * 4 hex chars = 64 chars, plus newline
    size_t line_pos = 0;

    for (size_t i = 0; i < num_int16; i += 2) {
        if (i + 1 >= num_int16) break;  // Need pairs

        int16_t re_val = buf16[i];
        int16_t im_val = buf16[i + 1];

        // Convert to 4-char hex (uppercase, zero-padded)
        char re_hex[5];
        char im_hex[5];
        
        snprintf(re_hex, 5, "%04X", (uint16_t)re_val);
        snprintf(im_hex, 5, "%04X", (uint16_t)im_val);

        // Add re_im to line buffer (8 hex chars per pair)
        if (line_pos + 8 >= sizeof(line_buffer)) {
            // Write line and reset
            line_buffer[line_pos] = '\0';
            fprintf(fp, "%s\n", line_buffer);
            line_pos = 0;
        }

        snprintf(line_buffer + line_pos, 9, "%s%s", re_hex, im_hex);
        line_pos += 8;
    }

    // Write final line if not empty
    if (line_pos > 0) {
        line_buffer[line_pos] = '\0';
        fprintf(fp, "%s\n", line_buffer);
    }

    fclose(fp);

    size_t total_samples = num_int16 / 2;
    printf("Dumped %zu int16 values (%zu re/im pairs) to %s\n",
           num_int16, total_samples, output_filename);

    return 0;
}


/**
 * Dump RX buffer from DMA to hex file.
 * Reads the RX buffer and writes it as re/im pairs.
 * 
 * @param rx_buf RX buffer from DMA (uint8_t*)
 * @param buffer_size_bytes Size of RX buffer in bytes
 * @param output_filename Path to output hex file
 * @return 0 on success, -1 on error
 */
int dump_dma_rx_to_file(
    const uint8_t* rx_buf,
    size_t buffer_size_bytes,
    const char* output_filename
)
{
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

 /* Fill a uint8_t buffer with int16_t values starting from `start_cnt`.
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

static int init_dma_tx (pl_dma dma, std::vector<pl_dma::ch_config> tx_dma, uint32_t num_transfers){
    if (dma.init(tx_dma) != 0) {
        printf("Init dma for ch0 failed\n");
        return 1;
    }
    dma.set_num_transfers(num_transfers);
}



int main(int argc, char *argv[])
{
    if (argc < 5) {
        printf("usage: %s <test_point> <channel> <num_transfers> <input_file> <output_file>\n", argv[0]);
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


    uint32_t test_point = (uint32_t)strtol(argv[1], NULL, 0);
    uint32_t channel = (uint32_t)strtol(argv[2], NULL, 0);
    uint32_t num_transfers = (uint32_t)strtol(argv[3], NULL, 0);
    const char* input_file = argv[4];
    const char* output_file = argv[5];  // File to dump RX data (optional, can be empty string)
    
    axi_dsp_init();
    axi_dsp_set_test_point(test_point);
    axi_dsp_set_channel(channel);
    axi_dsp_apply();


    pl_dma dma_ch0;
    pl_dma dma_ch1, dma_ch2, dma_ch3, dma_ch4, dma_ch5, dma_ch6, dma_ch7;


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

    std::vector<pl_dma::ch_config> tx_ch2 = {{
        .devnode = TX_DEV_CH2,
        .buffer_size = 4 * 1024,
    }};

    std::vector<pl_dma::ch_config> tx_ch3 = {{
        .devnode = TX_DEV_CH3,
        .buffer_size = 4 * 1024,
    }};

    std::vector<pl_dma::ch_config> tx_ch4 = {{
        .devnode = TX_DEV_CH4,
        .buffer_size = 4 * 1024,
    }};

    std::vector<pl_dma::ch_config> tx_ch5 = {{
        .devnode = TX_DEV_CH5,
        .buffer_size = 4 * 1024,
    }};

    std::vector<pl_dma::ch_config> tx_ch6 = {{
        .devnode = TX_DEV_CH6,
        .buffer_size = 4 * 1024,
    }};

    std::vector<pl_dma::ch_config> tx_ch7 = {{
        .devnode = TX_DEV_CH7,
        .buffer_size = 4 * 1024,
    }};

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

    uint8_t *rx_buf =  (uint8_t *)dma_ch0.get_rx_buffer(0);
    uint8_t *tx_buf0 = (uint8_t *)dma_ch0.get_tx_buffer(0);
    uint8_t *tx_buf1 = (uint8_t *)dma_ch1.get_tx_buffer(0);
    uint8_t *tx_buf2 = (uint8_t *)dma_ch2.get_tx_buffer(0);
    uint8_t *tx_buf3 = (uint8_t *)dma_ch3.get_tx_buffer(0);
    uint8_t *tx_buf4 = (uint8_t *)dma_ch4.get_tx_buffer(0);
    uint8_t *tx_buf5 = (uint8_t *)dma_ch5.get_tx_buffer(0);
    uint8_t *tx_buf6 = (uint8_t *)dma_ch6.get_tx_buffer(0);
    uint8_t *tx_buf7 = (uint8_t *)dma_ch7.get_tx_buffer(0);


    // Fill tx_buf0 from file for the specified channel
    if (read_channel_from_file(input_file, 0, tx_buf0, 4 * 1024) != 0) {
        printf("Failed to read channel %d from file\n", 0);
        return 1;
    }
    
    if (read_channel_from_file(input_file, 1, tx_buf1, 4 * 1024) != 0) {
        printf("Failed to read channel %d from file for ch1\n", 1);
        return 1;
    }

    
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

   // Dump RX buffer to hex file (only re/im pairs for one channel)
    if (output_file && strlen(output_file) > 0) {
        if (dump_dma_rx_to_file(rx_buf, 4 * 1024, output_file) != 0) {
            printf("Failed to dump RX buffer to file\n");
            return 1;
        }
    }

    // Also dump to binary file for raw data
    {
        const char* binary_output = "rx_dump_binary.bin";
        FILE* bin_fp = fopen(binary_output, "wb");
        if (bin_fp) {
            fwrite(rx_buf, 1, 4 * 1024, bin_fp);
            fclose(bin_fp);
            printf("Dumped RX buffer to binary file: %s\n", binary_output);
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
    axi_dsp_deinit();
    piDeleteSafety(kbd);


    return 0;
}