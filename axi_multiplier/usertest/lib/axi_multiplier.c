#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "axi_multiplier.h"
#include "regs.h"

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

uint32_t axi_multiplier_write(uint32_t val, uint32_t regmap_offset)
{
    lseek(fd, regmap_offset, SEEK_SET);
    if (write(fd, &val, sizeof(val)) != sizeof(val))
    {
        perror("write");
        printf("Write error\n");
        axi_multiplier_deinit();
        return FD_ERR_WRITE;
    }

    return FD_ERR_NONE;
}

uint32_t axi_multiplier_read(uint32_t *val, uint32_t regmap_offset)
{
    uint32_t rd;

    lseek(fd, regmap_offset, SEEK_SET);
    if (read(fd, &rd, sizeof(rd)) != sizeof(rd))
    {
        perror("read");
        printf("Read error\n");
        axi_multiplier_deinit();
        return FD_ERR_READ;
    }

    *val = rd;
    return FD_ERR_NONE;
}

void axi_multiplier_get_ip_ver(uint16_t *ver_maj, uint16_t *ver_min)
{
    uint32_t ver;
    axi_multiplier_read(&ver, CSR_IP_VER_ADDR);
    *ver_maj = (uint16_t)(0xFFFF & (ver >> 16));
    *ver_min = (uint16_t)(0xFFFF & ver);
}

uint32_t axi_multiplier_get_kill()
{
    uint32_t kill;
    axi_multiplier_read(&kill, CSR_KILL_ADDR);
    return kill;
}

uint32_t axi_multiplier_get_tp()
{
    uint32_t tp;
    axi_multiplier_read(&tp, CSR_TEST_POINT_ADDR);
    return tp;
}

uint32_t axi_multiplier_get_ch()
{
    uint32_t ch;
    axi_multiplier_read(&ch, CSR_CHANNEL_ADDR);
    return ch;
}

int16_t axi_multiplier_get_mult(uint32_t ch_num)
{
    uint32_t data, regmap_offset;
    int16_t mult_val;
    switch (ch_num)
    {
    case 0:
        regmap_offset = CSR_MULT0_ADDR;
        break;
    case 1:
        regmap_offset = CSR_MULT1_ADDR;
        break;
    case 2:
        regmap_offset = CSR_MULT2_ADDR;
        break;
    case 4:
        regmap_offset = CSR_MULT4_ADDR;
        break;
    case 5:
        regmap_offset = CSR_MULT5_ADDR;
        break;
    case 6:
        regmap_offset = CSR_MULT6_ADDR;
        break;
    case 7:
        regmap_offset = CSR_MULT7_ADDR;
        break;
    }
    // regmap_offset = CSR_MULT0_ADDR + 4 * ch_num;
    axi_multiplier_read(&data, regmap_offset);
    mult_val = (int16_t)data;
    return mult_val;
}

void axi_multiplier_set_kill()
{
    axi_multiplier_write(1, CSR_KILL_ADDR);
}

void axi_multiplier_reset_kill()
{
    axi_multiplier_write(0, CSR_KILL_ADDR);
}

void axi_multiplier_reset()
{
    axi_multiplier_set_kill();
    axi_multiplier_reset_kill();
}

void axi_multiplier_set_tp(uint32_t tp)
{
    axi_multiplier_write(tp, CSR_TEST_POINT_ADDR);
}

void axi_multiplier_set_ch(uint32_t ch)
{
    axi_multiplier_write(ch, CSR_CHANNEL_ADDR);
}

void axi_multiplier_set_mult(int16_t val, uint32_t ch_num)
{
    uint32_t val_u32 = (uint32_t)(int32_t)val;
    uint32_t regmap_offset;
    switch (ch_num)
    {
    case 0:
        regmap_offset = CSR_MULT0_ADDR;
        break;
    case 1:
        regmap_offset = CSR_MULT1_ADDR;
        break;
    case 2:
        regmap_offset = CSR_MULT2_ADDR;
        break;
    case 4:
        regmap_offset = CSR_MULT4_ADDR;
        break;
    case 5:
        regmap_offset = CSR_MULT5_ADDR;
        break;
    case 6:
        regmap_offset = CSR_MULT6_ADDR;
        break;
    case 7:
        regmap_offset = CSR_MULT7_ADDR;
        break;
    }
    // regmap_offset = CSR_MULT0_ADDR + 4 * ch_num;
    axi_multiplier_write(val_u32, regmap_offset);
}