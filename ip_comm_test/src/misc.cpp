#include "misc.h"
#include <chrono>
#include <iomanip>

#include <unistd.h>
#include <sys/select.h>
#include <cstdio>
#include <errno.h>
#include <string.h>

#include <atomic>
#include <thread>
#include <iostream>


/**
 * Parse a 4-character hex string to int16_t (signed).
 * Example: "FFD6" -> -42 (0xFFD6 as signed 16-bit)
 */
static int16_t hex_to_int16(const char * hex_str) {
	uint16_t val = (uint16_t)strtoul(hex_str, NULL, 16);
	return (int16_t)val;
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
 * @param offset_samples  Number of samples to skip from the beginning of the file
 * @return 0 on success, -1 on error
 */
int misc_read_8chs_from_file(const char * filename, uint8_t * buffers[8], size_t buffer_size_bytes, size_t sample_offset) {
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
    size_t samples_skipped = 0;
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
            // Skip samples based on offset
            if (samples_skipped < sample_offset) {
                samples_skipped++;
                continue;
            }
            
            // Extract all 8 channels from this sample block
            for (int ch = 0; ch < 8; ch++) {
                int re_offset = (ch * 2) * 4; // channel * 2 values * 4 chars
                int im_offset = (ch * 2 + 1) * 4;

                char re_hex[5], im_hex[5];
                memcpy(re_hex, clean + pos + re_offset, 4);
                memcpy(im_hex, clean + pos + im_offset, 4);

				// if ((ch == 1) && sample_count < 10){
				// 	printf("Sample for ch %d, count %ld, is re = %s, im = %s\n", ch, sample_count, re_hex, im_hex);
				// }
                re_hex[4] = '\0';
                im_hex[4] = '\0';

                int16_t re_val = (int16_t)strtoul(re_hex, NULL, 16);
                int16_t im_val = (int16_t)strtoul(im_hex, NULL, 16);

                // Store re, im consecutively
                buf16[ch][sample_count * 2] = re_val;
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

    printf("Loaded %zu samples (skipped %zu) from %s\n", sample_count, samples_skipped, filename);
    return 0;
}

std::string misc_get_date()
{
    time_t now = time(nullptr);
    tm *local_time = localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d");
    return oss.str();
}

std::string misc_get_datetime()
{
    time_t now = time(nullptr);
    tm *local_time = localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

std::string misc_get_timestamp_ms()
{
    using namespace std::chrono;
    static high_resolution_clock::time_point start_time = high_resolution_clock::now();
    high_resolution_clock::time_point now = high_resolution_clock::now();
    duration<long long, std::milli> diff = duration_cast<milliseconds>(now - start_time);
    return std::to_string(diff.count());
}

bool misc_stdin_has_data()
{
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 0; // do not wait

    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout) > 0;
}

std::atomic<bool> has_input(false);

static volatile void input_tread_work()
{
    std::string input;
    while (true) {
        getline(std::cin, input);
        has_input = true;
    }
}

void misc_init_input_tread()
{
    std::thread t(input_tread_work);
    t.detach();
}

bool misc_has_input()
{
    return has_input;
}

void misc_to_binstr(const int a[], int len, char result[])
{
    for (int i = 0; i < len; i++) {
        result[len - 1 - i] = a[i] + '0'; // Store with MSB first
    }
    result[len] = '\0';
}

bool misc_arr_bin2uint16(const uint bits[], int len, uint16_t result[], int res_len)
{
    if (res_len * 16 < len) {
        return false;
    }

    for (int i = 0; i < res_len; i++) {
        result[i] = 0;
    }

    for (int i = 0; i < len; i++) {
        if (bits[i]) {
            int word_index = i / 16;
            int bit_position = i % 16;
            result[word_index] |= (bits[i] << bit_position);
        }
    }
    return true;
}
