// Created with Corsair v1.0.4
#ifndef __REGS_H
#define __REGS_H

#define __I  volatile const // 'read only' permissions
#define __O  volatile       // 'write only' permissions
#define __IO volatile       // 'read / write' permissions


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

#define CSR_BASE_ADDR 0x0

// ip_ver - IP version
#define CSR_IP_VER_ADDR 0x0
#define CSR_IP_VER_RESET 0x0
typedef struct {
    uint32_t MIN_VER : 16; // Minor IP version
    uint32_t MAJ_VER : 16; // Major IP version
} csr_ip_ver_t;

// ip_ver.min_ver - Minor IP version
#define CSR_IP_VER_MIN_VER_WIDTH 16
#define CSR_IP_VER_MIN_VER_LSB 0
#define CSR_IP_VER_MIN_VER_MASK 0xffff
#define CSR_IP_VER_MIN_VER_RESET 0x0

// ip_ver.maj_ver - Major IP version
#define CSR_IP_VER_MAJ_VER_WIDTH 16
#define CSR_IP_VER_MAJ_VER_LSB 16
#define CSR_IP_VER_MAJ_VER_MASK 0xffff0000
#define CSR_IP_VER_MAJ_VER_RESET 0x0

// kill - Synchronous reset register
#define CSR_KILL_ADDR 0x4
#define CSR_KILL_RESET 0x0
typedef struct {
    uint32_t KILL : 1; // Kill
    uint32_t : 31; // reserved
} csr_kill_t;

// kill.kill - Kill
#define CSR_KILL_KILL_WIDTH 1
#define CSR_KILL_KILL_LSB 0
#define CSR_KILL_KILL_MASK 0x1
#define CSR_KILL_KILL_RESET 0x0

// test_point - Test point control register
#define CSR_TEST_POINT_ADDR 0x8
#define CSR_TEST_POINT_RESET 0x0
typedef struct {
    uint32_t TEST_POINT : 3; // Test point
    uint32_t : 29; // reserved
} csr_test_point_t;

// test_point.test_point - Test point
#define CSR_TEST_POINT_TEST_POINT_WIDTH 3
#define CSR_TEST_POINT_TEST_POINT_LSB 0
#define CSR_TEST_POINT_TEST_POINT_MASK 0x7
#define CSR_TEST_POINT_TEST_POINT_RESET 0x0

// channel - Output channel control register
#define CSR_CHANNEL_ADDR 0xc
#define CSR_CHANNEL_RESET 0x0
typedef struct {
    uint32_t TEST_POINT : 3; // Test point
    uint32_t : 29; // reserved
} csr_channel_t;

// channel.test_point - Test point
#define CSR_CHANNEL_TEST_POINT_WIDTH 3
#define CSR_CHANNEL_TEST_POINT_LSB 0
#define CSR_CHANNEL_TEST_POINT_MASK 0x7
#define CSR_CHANNEL_TEST_POINT_RESET 0x0

// mult0 - Multiplication value for ch0
#define CSR_MULT0_ADDR 0x10
#define CSR_MULT0_RESET 0x0
typedef struct {
    uint32_t MULT0 : 16; // Multiplication value for ch0
    uint32_t : 16; // reserved
} csr_mult0_t;

// mult0.mult0 - Multiplication value for ch0
#define CSR_MULT0_MULT0_WIDTH 16
#define CSR_MULT0_MULT0_LSB 0
#define CSR_MULT0_MULT0_MASK 0xffff
#define CSR_MULT0_MULT0_RESET 0x0

// mult1 - Multiplication value for ch1
#define CSR_MULT1_ADDR 0x14
#define CSR_MULT1_RESET 0x0
typedef struct {
    uint32_t MULT1 : 16; // Multiplication value for ch1
    uint32_t : 16; // reserved
} csr_mult1_t;

// mult1.mult1 - Multiplication value for ch1
#define CSR_MULT1_MULT1_WIDTH 16
#define CSR_MULT1_MULT1_LSB 0
#define CSR_MULT1_MULT1_MASK 0xffff
#define CSR_MULT1_MULT1_RESET 0x0

// mult2 - Multiplication value for ch2
#define CSR_MULT2_ADDR 0x18
#define CSR_MULT2_RESET 0x0
typedef struct {
    uint32_t MULT2 : 16; // Multiplication value for ch2
    uint32_t : 16; // reserved
} csr_mult2_t;

// mult2.mult2 - Multiplication value for ch2
#define CSR_MULT2_MULT2_WIDTH 16
#define CSR_MULT2_MULT2_LSB 0
#define CSR_MULT2_MULT2_MASK 0xffff
#define CSR_MULT2_MULT2_RESET 0x0

// mult4 - Multiplication value for ch4
#define CSR_MULT4_ADDR 0x20
#define CSR_MULT4_RESET 0x0
typedef struct {
    uint32_t MULT4 : 16; // Multiplication value for ch4
    uint32_t : 16; // reserved
} csr_mult4_t;

// mult4.mult4 - Multiplication value for ch4
#define CSR_MULT4_MULT4_WIDTH 16
#define CSR_MULT4_MULT4_LSB 0
#define CSR_MULT4_MULT4_MASK 0xffff
#define CSR_MULT4_MULT4_RESET 0x0

// mult5 - Multiplication value for ch5
#define CSR_MULT5_ADDR 0x24
#define CSR_MULT5_RESET 0x0
typedef struct {
    uint32_t MULT5 : 16; // Multiplication value for ch5
    uint32_t : 16; // reserved
} csr_mult5_t;

// mult5.mult5 - Multiplication value for ch5
#define CSR_MULT5_MULT5_WIDTH 16
#define CSR_MULT5_MULT5_LSB 0
#define CSR_MULT5_MULT5_MASK 0xffff
#define CSR_MULT5_MULT5_RESET 0x0

// mult6 - Multiplication value for ch6
#define CSR_MULT6_ADDR 0x28
#define CSR_MULT6_RESET 0x0
typedef struct {
    uint32_t MULT6 : 16; // Multiplication value for ch6
    uint32_t : 16; // reserved
} csr_mult6_t;

// mult6.mult6 - Multiplication value for ch6
#define CSR_MULT6_MULT6_WIDTH 16
#define CSR_MULT6_MULT6_LSB 0
#define CSR_MULT6_MULT6_MASK 0xffff
#define CSR_MULT6_MULT6_RESET 0x0

// mult7 - Multiplication value for ch7
#define CSR_MULT7_ADDR 0x2c
#define CSR_MULT7_RESET 0x0
typedef struct {
    uint32_t MULT7 : 16; // Multiplication value for ch7
    uint32_t : 16; // reserved
} csr_mult7_t;

// mult7.mult7 - Multiplication value for ch7
#define CSR_MULT7_MULT7_WIDTH 16
#define CSR_MULT7_MULT7_LSB 0
#define CSR_MULT7_MULT7_MASK 0xffff
#define CSR_MULT7_MULT7_RESET 0x0


// Register map structure
typedef struct {
    union {
        __I uint32_t IP_VER; // IP version
        __I csr_ip_ver_t IP_VER_bf; // Bit access for IP_VER register
    };
    union {
        __IO uint32_t KILL; // Synchronous reset register
        __IO csr_kill_t KILL_bf; // Bit access for KILL register
    };
    union {
        __IO uint32_t TEST_POINT; // Test point control register
        __IO csr_test_point_t TEST_POINT_bf; // Bit access for TEST_POINT register
    };
    union {
        __IO uint32_t CHANNEL; // Output channel control register
        __IO csr_channel_t CHANNEL_bf; // Bit access for CHANNEL register
    };
    union {
        __IO uint32_t MULT0; // Multiplication value for ch0
        __IO csr_mult0_t MULT0_bf; // Bit access for MULT0 register
    };
    union {
        __IO uint32_t MULT1; // Multiplication value for ch1
        __IO csr_mult1_t MULT1_bf; // Bit access for MULT1 register
    };
    union {
        __IO uint32_t MULT2; // Multiplication value for ch2
        __IO csr_mult2_t MULT2_bf; // Bit access for MULT2 register
    };
    __IO uint32_t RESERVED0[1];
    union {
        __IO uint32_t MULT4; // Multiplication value for ch4
        __IO csr_mult4_t MULT4_bf; // Bit access for MULT4 register
    };
    union {
        __IO uint32_t MULT5; // Multiplication value for ch5
        __IO csr_mult5_t MULT5_bf; // Bit access for MULT5 register
    };
    union {
        __IO uint32_t MULT6; // Multiplication value for ch6
        __IO csr_mult6_t MULT6_bf; // Bit access for MULT6 register
    };
    union {
        __IO uint32_t MULT7; // Multiplication value for ch7
        __IO csr_mult7_t MULT7_bf; // Bit access for MULT7 register
    };
} csr_t;

#define CSR ((csr_t*)(CSR_BASE_ADDR))

#ifdef __cplusplus
}
#endif

#endif /* __REGS_H */