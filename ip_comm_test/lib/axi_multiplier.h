#if !defined(AXI_MULTIPLIER_H)
#define AXI_MULTIPLIER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum fd_err
{
   FD_ERR_NONE,
   FD_ERR_WRITE,
   FD_ERR_READ,
   FD_ERR_NO_DEVICE,
};

uint32_t axi_multiplier_init();
void axi_multiplier_deinit();
uint32_t axi_multiplier_write(uint32_t val, uint32_t regmap_offset);
uint32_t axi_multiplier_read(uint32_t *val, uint32_t regmap_offset);

void axi_multiplier_reset();

void axi_multiplier_get_ip_ver(uint16_t *ver_maj, uint16_t *ver_min);
uint32_t axi_multiplier_get_kill();
uint32_t axi_multiplier_get_tp();
uint32_t axi_multiplier_get_ch();
int16_t axi_multiplier_get_mult(uint32_t ch_num);

void axi_multiplier_set_kill();
void axi_multiplier_reset_kill();
void axi_multiplier_set_tp(uint32_t tp);
void axi_multiplier_set_ch(uint32_t ch);
void axi_multiplier_set_mult(int16_t val, uint32_t ch_num);

#ifdef __cplusplus
}
#endif

#endif // AXI_MULTIPLIER_H
