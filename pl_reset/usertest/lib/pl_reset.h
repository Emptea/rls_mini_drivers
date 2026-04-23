#if !defined(PL_RESET_H)
#define PL_RESET_H

#include <stdint.h>

enum pl_reset_err {
    PL_RESET_ERR_NONE,
    PL_RESET_ERR_WRITE,
    PL_RESET_ERR_NO_DEVICE
}; 
uint32_t reset_pl();

#endif // PL_RESET_H
