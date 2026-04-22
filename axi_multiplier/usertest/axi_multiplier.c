#include "axi_multiplier.h"

int fd;

uint32_t axi_multiplier_init()
{
    fd = open("/dev/axi_multiplier", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return FD_ERR_NO_DEVICE;
    }
    return FD_ERR_NONE;
}

void axi_multiplier_deinit()
{
    close(fd);
}

uint32_t axi_multiplier_write(int16_t val)
{
    uint32_t val_u32 = std::bit_cast<uint32_t>(val);

    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        return FD_ERR_READ;
    }

    if (write(fd, &val_u32, sizeof(val_u32)) != sizeof(val_u32))
    {
        perror("write");
        axi_multiplier_deinit();
        return FD_ERR_WRITE;
    }

    return FD_ERR_NONE;
}

uint32_t axi_multiplier_read(int16_t *val)
{
    uint32_t rd;
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek");
        return FD_ERR_READ;
    }

    if (read(fd, &rd, sizeof(rd)) != sizeof(rd))
    {
        perror("read");
        axi_multiplier_deinit();
        return FD_ERR_READ;
    }

    *val = std::bit_cast<int16_t>(rd);
    return FD_ERR_NONE;
}