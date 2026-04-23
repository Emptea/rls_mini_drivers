#include "axi_multiplier.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s <value>\n", argv[0]);
        return 1;
    }

    int16_t wr = (int16_t)strtol(argv[1], NULL, 0);
    int16_t rd;

    if (axi_multiplier_init()) {
        return 1;
    }

    if (axi_multiplier_write(wr)) {
        axi_multiplier_deinit();
        return 1;
    }
    printf("axi_multiplier: wrote 0x%04hx\n", (uint16_t)wr);

    if (axi_multiplier_read(&rd)) {
        axi_multiplier_deinit();
        return 1;
    }
    printf("axi_multiplier: read 0x%04hx\n", (uint16_t)rd);

    if (wr != rd) {
        printf("wr != rd\n");
        axi_multiplier_deinit();
        return 1;
    }

    axi_multiplier_deinit();
    return 0;
}