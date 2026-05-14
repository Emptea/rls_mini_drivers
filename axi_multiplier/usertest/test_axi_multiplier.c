#include "axi_multiplier.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define RESET "\033[0m"
#define RED "\033[31m" /* Red */

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <value>\n", argv[0]);
        return 1;
    }

    int16_t wr = (int16_t)strtol(argv[1], NULL, 0);
    int16_t rd;

    if (axi_multiplier_init())
    {
        return 1;
    }
    uint16_t ver_maj, ver_min;
    axi_multiplier_get_ip_ver(&ver_maj, &ver_min);
    printf("AXI Multiplier v%d.%d\n", ver_maj, ver_min);

    uint32_t tp = 2;
    uint32_t tp_get;
    axi_multiplier_set_tp(tp);
    tp_get = axi_multiplier_get_tp();
    printf("Set test point %d, returned %d", tp, tp_get);
    if (tp != tp_get)
    {
        printf(RED "Test point check failed" RESET);
    }

    uint32_t ch = 1;
    uint32_t ch_get;
    axi_multiplier_set_tp(ch);
    ch_get = axi_multiplier_get_ch();
    printf("Set channel %d, returned %d", ch, ch_get);
    if (ch != ch_get)
    {
        printf(RED "Channel check failed" RESET);
    }

    int16_t mult0 = 2;
    int16_t mult1 = -2;
    int16_t mult0_get, mult1_get;
    axi_multiplier_set_mult(mult0, 0);
    mult0_get = axi_multiplier_get_mult(0);
    axi_multiplier_set_mult(mult1, 1);
    mult1_get = axi_multiplier_get_mult(1);
    printf("Set multiplier value %d for ch %d, returned %d", mult0, 0, mult0_get);
    if (mult0 != mult0_get)
    {
        printf(RED "Value is not set into multiplier ch 0" RESET);
    }
    printf("Set multiplier value %d for ch %d, returned %d", mult1, 1, mult1_get);
    if (mult1 != mult1_get)
    {
        printf(RED "Value is not set into multiplier ch 1" RESET);
    }

    axi_multiplier_deinit();
    return 0;
}