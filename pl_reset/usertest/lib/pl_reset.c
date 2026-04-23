#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "pl_reset.h"

uint32_t reset_pl()
{
    ssize_t n;
    const char *pl_reset_path = "/sys/bus/platform/devices/amba_pl@0:pl_reset/reset";

    int fd = open(pl_reset_path, O_WRONLY);
    if (fd < 0)
    {
        perror("open");
        return PL_RESET_ERR_NO_DEVICE;
    }

    n = write(fd, "1", 1);
    if (n < 0)
    {
        perror("write");
        close(fd);
        return PL_RESET_ERR_WRITE;
    }

    close(fd);
    printf("pl_reset triggered\n");
    return PL_RESET_ERR_NONE;
}