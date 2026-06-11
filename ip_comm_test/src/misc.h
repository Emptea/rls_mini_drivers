#ifndef MISC_H
#define MISC_H

#include <string>
#include <math.h>
#include <stdint.h>

inline static uint32_t rad_to_fix(float ph, uint32_t ord)
{
    return static_cast<int32_t>(roundf(ph / (2.0f * M_PI) * (1UL << ord)));
}

inline static float fix_to_rad(uint32_t num, uint32_t ord)
{
    return (float)num / (1UL << ord) * (2.0f * M_PI);
}

inline static uint32_t deg_to_fix(float ph, uint32_t ord)
{
    return static_cast<int32_t>(roundf(ph / 360.0f * (1UL << ord)));
}

inline static float fix_to_deg(uint32_t num, uint32_t ord)
{
    return (float)num / (1UL << ord) * 360.0f;
}

inline static int32_t float_to_fix(float flt, uint32_t ord)
{
    return static_cast<int32_t>(roundf(flt * (1UL << ord)));
}

inline static int32_t double_to_fix(double d, uint32_t ord)
{
    return static_cast<int32_t>(roundf64(d * (1UL << ord)));
}

inline static float fix_to_float(int32_t fix, uint32_t ord)
{
    return (float)fix / (1UL << ord);
}

inline static double fix_to_double(int32_t fix, uint32_t ord)
{
    return (double)fix / (1UL << ord);
}

std::string misc_get_date();
std::string misc_get_datetime();
std::string misc_get_timestamp_ms();
bool misc_stdin_has_data();
bool misc_has_input();
void misc_init_input_tread();
void misc_to_binstr(const int a[], int len, char result[]);
bool misc_arr_bin2uint16(const uint bits[], int len, uint16_t result[], int res_len);
#endif /* MISC_H */
