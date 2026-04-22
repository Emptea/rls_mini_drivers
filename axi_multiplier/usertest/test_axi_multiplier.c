#include "axi_multiplier.h"

int main()
{
    int fd = open("/dev/axi_multiplier", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    if (axi_multiplier_init())
    {
        return 1;
    }

    int16_t wr = 0x1234;
    if (axi_multiplier_write(wr))
    {
        return 1;
    }

    int16_t rd;
    if (axi_multiplier_read(&rd))
    {
        return 1;
    }

    if (wr != rd)
    {
        fprintf("wr != rd");
        return 1;
    }

    // uint32_t val = 0x00001234;
    // if (write(fd, &val, sizeof(val)) != sizeof(val)) {
    //     perror("write");
    //     close(fd);
    //     return 1;
    // }

    // lseek(fd, 0, SEEK_SET);

    // uint32_t rd = 0;
    // if (read(fd, &rd, sizeof(rd)) != sizeof(rd)) {
    //     perror("read");
    //     close(fd);
    //     return 1;
    // }

    // printf("read back: 0x%08x\n", rd);

    // close(fd);

    axi_multiplier_deinit();
    return 0;
}
