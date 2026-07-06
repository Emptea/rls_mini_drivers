#include "axi_multiplier.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define RESET "\033[0m"
#define RED "\033[31m" /* Red */

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("usage: %s <mult_val> ><ch_num>\n", argv[0]);
        return 1;
    }
    int16_t mult = (int16_t)strtol(argv[1], NULL, 0);
    uint32_t ch = (uint32_t)strtol(argv[2], NULL, 0);

    if (axi_multiplier_init())
    {
        return 1;
    }
    uint16_t ver_maj, ver_min;
    axi_multiplier_get_ip_ver(&ver_maj, &ver_min);
    printf("AXI Multiplier v%d.%d\n", ver_maj, ver_min);

    uint32_t tp = 0;
    uint32_t tp_get;
    axi_multiplier_set_tp(tp);
    tp_get = axi_multiplier_get_tp();
    printf("Set test point %d, returned %d\n", tp, tp_get);
    if (tp != tp_get)
    {
        printf(RED "Test point check failed" RESET "\n");
    }

    // uint32_t ch = 2;
    uint32_t ch_get;
    axi_multiplier_set_ch(ch);
    ch_get = axi_multiplier_get_ch();
    printf("Set channel %d, returned %d\n", ch, ch_get);
    if (ch != ch_get)
    {
        printf(RED "Channel check failed" RESET "\n");
    }

    for (size_t ch_num = 0; ch_num < 8; ch_num++){
        axi_multiplier_set_mult(mult, ch_num);
        int16_t mult_get = axi_multiplier_get_mult(ch_num);
        printf("Set multiplier value %d for ch %ld, returned %d\n", mult, ch_num, mult_get);
        if (mult != mult_get)
        {
            printf(RED "Value is not set into multiplier ch %ld" RESET "\n", ch_num);
        }
    }

    axi_multiplier_deinit();
    return 0;
}