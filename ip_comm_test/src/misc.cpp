#include "misc.h"
#include <chrono>
#include <iomanip>

#include <unistd.h>
#include <sys/select.h>
#include <cstdio>

#include <atomic>
#include <thread>
#include <iostream>

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
