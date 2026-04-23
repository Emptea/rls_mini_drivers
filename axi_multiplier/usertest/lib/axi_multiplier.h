#if !defined(AXI_MULTIPLIER_H)
#define AXI_MULTIPLIER_H
#include <stdint.h>

enum fd_err {
   FD_ERR_NONE,
   FD_ERR_WRITE,
   FD_ERR_READ,
   FD_ERR_NO_DEVICE,
}; 
uint32_t axi_multiplier_init();
void axi_multiplier_deinit();
uint32_t axi_multiplier_write(int16_t val);
uint32_t axi_multiplier_read(int16_t *val);


#endif // AXI_MULTIPLIER_H
