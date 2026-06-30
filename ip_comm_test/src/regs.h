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
#define CSR_IP_VER_RESET 0x20000
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
#define CSR_IP_VER_MAJ_VER_RESET 0x2

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

// compensation_mode - 
#define CSR_COMPENSATION_MODE_ADDR 0x10
#define CSR_COMPENSATION_MODE_RESET 0x0
typedef struct {
    uint32_t MODE : 32; // Compensation mode
} csr_compensation_mode_t;

// compensation_mode.mode - Compensation mode
#define CSR_COMPENSATION_MODE_MODE_WIDTH 32
#define CSR_COMPENSATION_MODE_MODE_LSB 0
#define CSR_COMPENSATION_MODE_MODE_MASK 0xffffffff
#define CSR_COMPENSATION_MODE_MODE_RESET 0x0

// manual_compensation_0 - 
#define CSR_MANUAL_COMPENSATION_0_ADDR 0x14
#define CSR_MANUAL_COMPENSATION_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_0_t;

// manual_compensation_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_0_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_0_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_0_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_0_REAL_RESET 0x0

// manual_compensation_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_0_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_0_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_0_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_0_IMAG_RESET 0x0

// manual_compensation_1 - 
#define CSR_MANUAL_COMPENSATION_1_ADDR 0x18
#define CSR_MANUAL_COMPENSATION_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_1_t;

// manual_compensation_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_1_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_1_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_1_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_1_REAL_RESET 0x0

// manual_compensation_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_1_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_1_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_1_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_1_IMAG_RESET 0x0

// manual_compensation_2 - 
#define CSR_MANUAL_COMPENSATION_2_ADDR 0x1c
#define CSR_MANUAL_COMPENSATION_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_2_t;

// manual_compensation_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_2_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_2_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_2_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_2_REAL_RESET 0x0

// manual_compensation_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_2_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_2_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_2_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_2_IMAG_RESET 0x0

// manual_compensation_3 - 
#define CSR_MANUAL_COMPENSATION_3_ADDR 0x20
#define CSR_MANUAL_COMPENSATION_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_3_t;

// manual_compensation_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_3_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_3_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_3_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_3_REAL_RESET 0x0

// manual_compensation_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_3_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_3_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_3_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_3_IMAG_RESET 0x0

// manual_compensation_4 - 
#define CSR_MANUAL_COMPENSATION_4_ADDR 0x24
#define CSR_MANUAL_COMPENSATION_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_4_t;

// manual_compensation_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_4_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_4_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_4_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_4_REAL_RESET 0x0

// manual_compensation_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_4_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_4_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_4_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_4_IMAG_RESET 0x0

// manual_compensation_5 - 
#define CSR_MANUAL_COMPENSATION_5_ADDR 0x28
#define CSR_MANUAL_COMPENSATION_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_5_t;

// manual_compensation_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_5_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_5_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_5_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_5_REAL_RESET 0x0

// manual_compensation_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_5_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_5_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_5_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_5_IMAG_RESET 0x0

// manual_compensation_6 - 
#define CSR_MANUAL_COMPENSATION_6_ADDR 0x2c
#define CSR_MANUAL_COMPENSATION_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_6_t;

// manual_compensation_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_6_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_6_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_6_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_6_REAL_RESET 0x0

// manual_compensation_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_6_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_6_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_6_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_6_IMAG_RESET 0x0

// manual_compensation_7 - 
#define CSR_MANUAL_COMPENSATION_7_ADDR 0x30
#define CSR_MANUAL_COMPENSATION_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_manual_compensation_7_t;

// manual_compensation_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_7_REAL_WIDTH 16
#define CSR_MANUAL_COMPENSATION_7_REAL_LSB 0
#define CSR_MANUAL_COMPENSATION_7_REAL_MASK 0xffff
#define CSR_MANUAL_COMPENSATION_7_REAL_RESET 0x0

// manual_compensation_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_MANUAL_COMPENSATION_7_IMAG_WIDTH 16
#define CSR_MANUAL_COMPENSATION_7_IMAG_LSB 16
#define CSR_MANUAL_COMPENSATION_7_IMAG_MASK 0xffff0000
#define CSR_MANUAL_COMPENSATION_7_IMAG_RESET 0x0

// diagram_0_0 - 
#define CSR_DIAGRAM_0_0_ADDR 0x34
#define CSR_DIAGRAM_0_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_0_t;

// diagram_0_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_0_REAL_WIDTH 16
#define CSR_DIAGRAM_0_0_REAL_LSB 0
#define CSR_DIAGRAM_0_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_0_REAL_RESET 0x0

// diagram_0_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_0_IMAG_LSB 16
#define CSR_DIAGRAM_0_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_0_IMAG_RESET 0x0

// diagram_0_1 - 
#define CSR_DIAGRAM_0_1_ADDR 0x38
#define CSR_DIAGRAM_0_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_1_t;

// diagram_0_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_1_REAL_WIDTH 16
#define CSR_DIAGRAM_0_1_REAL_LSB 0
#define CSR_DIAGRAM_0_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_1_REAL_RESET 0x0

// diagram_0_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_1_IMAG_LSB 16
#define CSR_DIAGRAM_0_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_1_IMAG_RESET 0x0

// diagram_0_2 - 
#define CSR_DIAGRAM_0_2_ADDR 0x3c
#define CSR_DIAGRAM_0_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_2_t;

// diagram_0_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_2_REAL_WIDTH 16
#define CSR_DIAGRAM_0_2_REAL_LSB 0
#define CSR_DIAGRAM_0_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_2_REAL_RESET 0x0

// diagram_0_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_2_IMAG_LSB 16
#define CSR_DIAGRAM_0_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_2_IMAG_RESET 0x0

// diagram_0_3 - 
#define CSR_DIAGRAM_0_3_ADDR 0x40
#define CSR_DIAGRAM_0_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_3_t;

// diagram_0_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_3_REAL_WIDTH 16
#define CSR_DIAGRAM_0_3_REAL_LSB 0
#define CSR_DIAGRAM_0_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_3_REAL_RESET 0x0

// diagram_0_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_3_IMAG_LSB 16
#define CSR_DIAGRAM_0_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_3_IMAG_RESET 0x0

// diagram_0_4 - 
#define CSR_DIAGRAM_0_4_ADDR 0x44
#define CSR_DIAGRAM_0_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_4_t;

// diagram_0_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_4_REAL_WIDTH 16
#define CSR_DIAGRAM_0_4_REAL_LSB 0
#define CSR_DIAGRAM_0_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_4_REAL_RESET 0x0

// diagram_0_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_4_IMAG_LSB 16
#define CSR_DIAGRAM_0_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_4_IMAG_RESET 0x0

// diagram_0_5 - 
#define CSR_DIAGRAM_0_5_ADDR 0x48
#define CSR_DIAGRAM_0_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_5_t;

// diagram_0_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_5_REAL_WIDTH 16
#define CSR_DIAGRAM_0_5_REAL_LSB 0
#define CSR_DIAGRAM_0_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_5_REAL_RESET 0x0

// diagram_0_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_5_IMAG_LSB 16
#define CSR_DIAGRAM_0_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_5_IMAG_RESET 0x0

// diagram_0_6 - 
#define CSR_DIAGRAM_0_6_ADDR 0x4c
#define CSR_DIAGRAM_0_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_6_t;

// diagram_0_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_6_REAL_WIDTH 16
#define CSR_DIAGRAM_0_6_REAL_LSB 0
#define CSR_DIAGRAM_0_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_6_REAL_RESET 0x0

// diagram_0_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_6_IMAG_LSB 16
#define CSR_DIAGRAM_0_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_6_IMAG_RESET 0x0

// diagram_0_7 - 
#define CSR_DIAGRAM_0_7_ADDR 0x50
#define CSR_DIAGRAM_0_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_0_7_t;

// diagram_0_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_7_REAL_WIDTH 16
#define CSR_DIAGRAM_0_7_REAL_LSB 0
#define CSR_DIAGRAM_0_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_0_7_REAL_RESET 0x0

// diagram_0_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_0_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_0_7_IMAG_LSB 16
#define CSR_DIAGRAM_0_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_0_7_IMAG_RESET 0x0

// diagram_1_0 - 
#define CSR_DIAGRAM_1_0_ADDR 0x54
#define CSR_DIAGRAM_1_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_0_t;

// diagram_1_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_0_REAL_WIDTH 16
#define CSR_DIAGRAM_1_0_REAL_LSB 0
#define CSR_DIAGRAM_1_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_0_REAL_RESET 0x0

// diagram_1_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_0_IMAG_LSB 16
#define CSR_DIAGRAM_1_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_0_IMAG_RESET 0x0

// diagram_1_1 - 
#define CSR_DIAGRAM_1_1_ADDR 0x58
#define CSR_DIAGRAM_1_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_1_t;

// diagram_1_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_1_REAL_WIDTH 16
#define CSR_DIAGRAM_1_1_REAL_LSB 0
#define CSR_DIAGRAM_1_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_1_REAL_RESET 0x0

// diagram_1_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_1_IMAG_LSB 16
#define CSR_DIAGRAM_1_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_1_IMAG_RESET 0x0

// diagram_1_2 - 
#define CSR_DIAGRAM_1_2_ADDR 0x5c
#define CSR_DIAGRAM_1_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_2_t;

// diagram_1_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_2_REAL_WIDTH 16
#define CSR_DIAGRAM_1_2_REAL_LSB 0
#define CSR_DIAGRAM_1_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_2_REAL_RESET 0x0

// diagram_1_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_2_IMAG_LSB 16
#define CSR_DIAGRAM_1_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_2_IMAG_RESET 0x0

// diagram_1_3 - 
#define CSR_DIAGRAM_1_3_ADDR 0x60
#define CSR_DIAGRAM_1_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_3_t;

// diagram_1_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_3_REAL_WIDTH 16
#define CSR_DIAGRAM_1_3_REAL_LSB 0
#define CSR_DIAGRAM_1_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_3_REAL_RESET 0x0

// diagram_1_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_3_IMAG_LSB 16
#define CSR_DIAGRAM_1_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_3_IMAG_RESET 0x0

// diagram_1_4 - 
#define CSR_DIAGRAM_1_4_ADDR 0x64
#define CSR_DIAGRAM_1_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_4_t;

// diagram_1_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_4_REAL_WIDTH 16
#define CSR_DIAGRAM_1_4_REAL_LSB 0
#define CSR_DIAGRAM_1_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_4_REAL_RESET 0x0

// diagram_1_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_4_IMAG_LSB 16
#define CSR_DIAGRAM_1_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_4_IMAG_RESET 0x0

// diagram_1_5 - 
#define CSR_DIAGRAM_1_5_ADDR 0x68
#define CSR_DIAGRAM_1_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_5_t;

// diagram_1_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_5_REAL_WIDTH 16
#define CSR_DIAGRAM_1_5_REAL_LSB 0
#define CSR_DIAGRAM_1_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_5_REAL_RESET 0x0

// diagram_1_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_5_IMAG_LSB 16
#define CSR_DIAGRAM_1_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_5_IMAG_RESET 0x0

// diagram_1_6 - 
#define CSR_DIAGRAM_1_6_ADDR 0x6c
#define CSR_DIAGRAM_1_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_6_t;

// diagram_1_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_6_REAL_WIDTH 16
#define CSR_DIAGRAM_1_6_REAL_LSB 0
#define CSR_DIAGRAM_1_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_6_REAL_RESET 0x0

// diagram_1_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_6_IMAG_LSB 16
#define CSR_DIAGRAM_1_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_6_IMAG_RESET 0x0

// diagram_1_7 - 
#define CSR_DIAGRAM_1_7_ADDR 0x70
#define CSR_DIAGRAM_1_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_1_7_t;

// diagram_1_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_7_REAL_WIDTH 16
#define CSR_DIAGRAM_1_7_REAL_LSB 0
#define CSR_DIAGRAM_1_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_1_7_REAL_RESET 0x0

// diagram_1_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_1_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_1_7_IMAG_LSB 16
#define CSR_DIAGRAM_1_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_1_7_IMAG_RESET 0x0

// diagram_2_0 - 
#define CSR_DIAGRAM_2_0_ADDR 0x74
#define CSR_DIAGRAM_2_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_0_t;

// diagram_2_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_0_REAL_WIDTH 16
#define CSR_DIAGRAM_2_0_REAL_LSB 0
#define CSR_DIAGRAM_2_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_0_REAL_RESET 0x0

// diagram_2_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_0_IMAG_LSB 16
#define CSR_DIAGRAM_2_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_0_IMAG_RESET 0x0

// diagram_2_1 - 
#define CSR_DIAGRAM_2_1_ADDR 0x78
#define CSR_DIAGRAM_2_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_1_t;

// diagram_2_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_1_REAL_WIDTH 16
#define CSR_DIAGRAM_2_1_REAL_LSB 0
#define CSR_DIAGRAM_2_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_1_REAL_RESET 0x0

// diagram_2_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_1_IMAG_LSB 16
#define CSR_DIAGRAM_2_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_1_IMAG_RESET 0x0

// diagram_2_2 - 
#define CSR_DIAGRAM_2_2_ADDR 0x7c
#define CSR_DIAGRAM_2_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_2_t;

// diagram_2_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_2_REAL_WIDTH 16
#define CSR_DIAGRAM_2_2_REAL_LSB 0
#define CSR_DIAGRAM_2_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_2_REAL_RESET 0x0

// diagram_2_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_2_IMAG_LSB 16
#define CSR_DIAGRAM_2_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_2_IMAG_RESET 0x0

// diagram_2_3 - 
#define CSR_DIAGRAM_2_3_ADDR 0x80
#define CSR_DIAGRAM_2_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_3_t;

// diagram_2_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_3_REAL_WIDTH 16
#define CSR_DIAGRAM_2_3_REAL_LSB 0
#define CSR_DIAGRAM_2_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_3_REAL_RESET 0x0

// diagram_2_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_3_IMAG_LSB 16
#define CSR_DIAGRAM_2_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_3_IMAG_RESET 0x0

// diagram_2_4 - 
#define CSR_DIAGRAM_2_4_ADDR 0x84
#define CSR_DIAGRAM_2_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_4_t;

// diagram_2_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_4_REAL_WIDTH 16
#define CSR_DIAGRAM_2_4_REAL_LSB 0
#define CSR_DIAGRAM_2_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_4_REAL_RESET 0x0

// diagram_2_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_4_IMAG_LSB 16
#define CSR_DIAGRAM_2_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_4_IMAG_RESET 0x0

// diagram_2_5 - 
#define CSR_DIAGRAM_2_5_ADDR 0x88
#define CSR_DIAGRAM_2_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_5_t;

// diagram_2_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_5_REAL_WIDTH 16
#define CSR_DIAGRAM_2_5_REAL_LSB 0
#define CSR_DIAGRAM_2_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_5_REAL_RESET 0x0

// diagram_2_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_5_IMAG_LSB 16
#define CSR_DIAGRAM_2_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_5_IMAG_RESET 0x0

// diagram_2_6 - 
#define CSR_DIAGRAM_2_6_ADDR 0x8c
#define CSR_DIAGRAM_2_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_6_t;

// diagram_2_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_6_REAL_WIDTH 16
#define CSR_DIAGRAM_2_6_REAL_LSB 0
#define CSR_DIAGRAM_2_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_6_REAL_RESET 0x0

// diagram_2_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_6_IMAG_LSB 16
#define CSR_DIAGRAM_2_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_6_IMAG_RESET 0x0

// diagram_2_7 - 
#define CSR_DIAGRAM_2_7_ADDR 0x90
#define CSR_DIAGRAM_2_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_2_7_t;

// diagram_2_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_7_REAL_WIDTH 16
#define CSR_DIAGRAM_2_7_REAL_LSB 0
#define CSR_DIAGRAM_2_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_2_7_REAL_RESET 0x0

// diagram_2_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_2_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_2_7_IMAG_LSB 16
#define CSR_DIAGRAM_2_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_2_7_IMAG_RESET 0x0

// diagram_3_0 - 
#define CSR_DIAGRAM_3_0_ADDR 0x94
#define CSR_DIAGRAM_3_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_0_t;

// diagram_3_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_0_REAL_WIDTH 16
#define CSR_DIAGRAM_3_0_REAL_LSB 0
#define CSR_DIAGRAM_3_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_0_REAL_RESET 0x0

// diagram_3_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_0_IMAG_LSB 16
#define CSR_DIAGRAM_3_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_0_IMAG_RESET 0x0

// diagram_3_1 - 
#define CSR_DIAGRAM_3_1_ADDR 0x98
#define CSR_DIAGRAM_3_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_1_t;

// diagram_3_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_1_REAL_WIDTH 16
#define CSR_DIAGRAM_3_1_REAL_LSB 0
#define CSR_DIAGRAM_3_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_1_REAL_RESET 0x0

// diagram_3_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_1_IMAG_LSB 16
#define CSR_DIAGRAM_3_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_1_IMAG_RESET 0x0

// diagram_3_2 - 
#define CSR_DIAGRAM_3_2_ADDR 0x9c
#define CSR_DIAGRAM_3_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_2_t;

// diagram_3_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_2_REAL_WIDTH 16
#define CSR_DIAGRAM_3_2_REAL_LSB 0
#define CSR_DIAGRAM_3_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_2_REAL_RESET 0x0

// diagram_3_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_2_IMAG_LSB 16
#define CSR_DIAGRAM_3_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_2_IMAG_RESET 0x0

// diagram_3_3 - 
#define CSR_DIAGRAM_3_3_ADDR 0xa0
#define CSR_DIAGRAM_3_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_3_t;

// diagram_3_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_3_REAL_WIDTH 16
#define CSR_DIAGRAM_3_3_REAL_LSB 0
#define CSR_DIAGRAM_3_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_3_REAL_RESET 0x0

// diagram_3_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_3_IMAG_LSB 16
#define CSR_DIAGRAM_3_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_3_IMAG_RESET 0x0

// diagram_3_4 - 
#define CSR_DIAGRAM_3_4_ADDR 0xa4
#define CSR_DIAGRAM_3_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_4_t;

// diagram_3_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_4_REAL_WIDTH 16
#define CSR_DIAGRAM_3_4_REAL_LSB 0
#define CSR_DIAGRAM_3_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_4_REAL_RESET 0x0

// diagram_3_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_4_IMAG_LSB 16
#define CSR_DIAGRAM_3_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_4_IMAG_RESET 0x0

// diagram_3_5 - 
#define CSR_DIAGRAM_3_5_ADDR 0xa8
#define CSR_DIAGRAM_3_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_5_t;

// diagram_3_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_5_REAL_WIDTH 16
#define CSR_DIAGRAM_3_5_REAL_LSB 0
#define CSR_DIAGRAM_3_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_5_REAL_RESET 0x0

// diagram_3_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_5_IMAG_LSB 16
#define CSR_DIAGRAM_3_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_5_IMAG_RESET 0x0

// diagram_3_6 - 
#define CSR_DIAGRAM_3_6_ADDR 0xac
#define CSR_DIAGRAM_3_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_6_t;

// diagram_3_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_6_REAL_WIDTH 16
#define CSR_DIAGRAM_3_6_REAL_LSB 0
#define CSR_DIAGRAM_3_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_6_REAL_RESET 0x0

// diagram_3_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_6_IMAG_LSB 16
#define CSR_DIAGRAM_3_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_6_IMAG_RESET 0x0

// diagram_3_7 - 
#define CSR_DIAGRAM_3_7_ADDR 0xb0
#define CSR_DIAGRAM_3_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_3_7_t;

// diagram_3_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_7_REAL_WIDTH 16
#define CSR_DIAGRAM_3_7_REAL_LSB 0
#define CSR_DIAGRAM_3_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_3_7_REAL_RESET 0x0

// diagram_3_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_3_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_3_7_IMAG_LSB 16
#define CSR_DIAGRAM_3_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_3_7_IMAG_RESET 0x0

// diagram_4_0 - 
#define CSR_DIAGRAM_4_0_ADDR 0xb4
#define CSR_DIAGRAM_4_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_0_t;

// diagram_4_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_0_REAL_WIDTH 16
#define CSR_DIAGRAM_4_0_REAL_LSB 0
#define CSR_DIAGRAM_4_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_0_REAL_RESET 0x0

// diagram_4_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_0_IMAG_LSB 16
#define CSR_DIAGRAM_4_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_0_IMAG_RESET 0x0

// diagram_4_1 - 
#define CSR_DIAGRAM_4_1_ADDR 0xb8
#define CSR_DIAGRAM_4_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_1_t;

// diagram_4_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_1_REAL_WIDTH 16
#define CSR_DIAGRAM_4_1_REAL_LSB 0
#define CSR_DIAGRAM_4_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_1_REAL_RESET 0x0

// diagram_4_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_1_IMAG_LSB 16
#define CSR_DIAGRAM_4_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_1_IMAG_RESET 0x0

// diagram_4_2 - 
#define CSR_DIAGRAM_4_2_ADDR 0xbc
#define CSR_DIAGRAM_4_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_2_t;

// diagram_4_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_2_REAL_WIDTH 16
#define CSR_DIAGRAM_4_2_REAL_LSB 0
#define CSR_DIAGRAM_4_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_2_REAL_RESET 0x0

// diagram_4_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_2_IMAG_LSB 16
#define CSR_DIAGRAM_4_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_2_IMAG_RESET 0x0

// diagram_4_3 - 
#define CSR_DIAGRAM_4_3_ADDR 0xc0
#define CSR_DIAGRAM_4_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_3_t;

// diagram_4_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_3_REAL_WIDTH 16
#define CSR_DIAGRAM_4_3_REAL_LSB 0
#define CSR_DIAGRAM_4_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_3_REAL_RESET 0x0

// diagram_4_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_3_IMAG_LSB 16
#define CSR_DIAGRAM_4_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_3_IMAG_RESET 0x0

// diagram_4_4 - 
#define CSR_DIAGRAM_4_4_ADDR 0xc4
#define CSR_DIAGRAM_4_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_4_t;

// diagram_4_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_4_REAL_WIDTH 16
#define CSR_DIAGRAM_4_4_REAL_LSB 0
#define CSR_DIAGRAM_4_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_4_REAL_RESET 0x0

// diagram_4_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_4_IMAG_LSB 16
#define CSR_DIAGRAM_4_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_4_IMAG_RESET 0x0

// diagram_4_5 - 
#define CSR_DIAGRAM_4_5_ADDR 0xc8
#define CSR_DIAGRAM_4_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_5_t;

// diagram_4_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_5_REAL_WIDTH 16
#define CSR_DIAGRAM_4_5_REAL_LSB 0
#define CSR_DIAGRAM_4_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_5_REAL_RESET 0x0

// diagram_4_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_5_IMAG_LSB 16
#define CSR_DIAGRAM_4_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_5_IMAG_RESET 0x0

// diagram_4_6 - 
#define CSR_DIAGRAM_4_6_ADDR 0xcc
#define CSR_DIAGRAM_4_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_6_t;

// diagram_4_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_6_REAL_WIDTH 16
#define CSR_DIAGRAM_4_6_REAL_LSB 0
#define CSR_DIAGRAM_4_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_6_REAL_RESET 0x0

// diagram_4_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_6_IMAG_LSB 16
#define CSR_DIAGRAM_4_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_6_IMAG_RESET 0x0

// diagram_4_7 - 
#define CSR_DIAGRAM_4_7_ADDR 0xd0
#define CSR_DIAGRAM_4_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_4_7_t;

// diagram_4_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_7_REAL_WIDTH 16
#define CSR_DIAGRAM_4_7_REAL_LSB 0
#define CSR_DIAGRAM_4_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_4_7_REAL_RESET 0x0

// diagram_4_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_4_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_4_7_IMAG_LSB 16
#define CSR_DIAGRAM_4_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_4_7_IMAG_RESET 0x0

// diagram_5_0 - 
#define CSR_DIAGRAM_5_0_ADDR 0xd4
#define CSR_DIAGRAM_5_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_0_t;

// diagram_5_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_0_REAL_WIDTH 16
#define CSR_DIAGRAM_5_0_REAL_LSB 0
#define CSR_DIAGRAM_5_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_0_REAL_RESET 0x0

// diagram_5_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_0_IMAG_LSB 16
#define CSR_DIAGRAM_5_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_0_IMAG_RESET 0x0

// diagram_5_1 - 
#define CSR_DIAGRAM_5_1_ADDR 0xd8
#define CSR_DIAGRAM_5_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_1_t;

// diagram_5_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_1_REAL_WIDTH 16
#define CSR_DIAGRAM_5_1_REAL_LSB 0
#define CSR_DIAGRAM_5_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_1_REAL_RESET 0x0

// diagram_5_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_1_IMAG_LSB 16
#define CSR_DIAGRAM_5_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_1_IMAG_RESET 0x0

// diagram_5_2 - 
#define CSR_DIAGRAM_5_2_ADDR 0xdc
#define CSR_DIAGRAM_5_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_2_t;

// diagram_5_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_2_REAL_WIDTH 16
#define CSR_DIAGRAM_5_2_REAL_LSB 0
#define CSR_DIAGRAM_5_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_2_REAL_RESET 0x0

// diagram_5_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_2_IMAG_LSB 16
#define CSR_DIAGRAM_5_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_2_IMAG_RESET 0x0

// diagram_5_3 - 
#define CSR_DIAGRAM_5_3_ADDR 0xe0
#define CSR_DIAGRAM_5_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_3_t;

// diagram_5_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_3_REAL_WIDTH 16
#define CSR_DIAGRAM_5_3_REAL_LSB 0
#define CSR_DIAGRAM_5_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_3_REAL_RESET 0x0

// diagram_5_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_3_IMAG_LSB 16
#define CSR_DIAGRAM_5_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_3_IMAG_RESET 0x0

// diagram_5_4 - 
#define CSR_DIAGRAM_5_4_ADDR 0xe4
#define CSR_DIAGRAM_5_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_4_t;

// diagram_5_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_4_REAL_WIDTH 16
#define CSR_DIAGRAM_5_4_REAL_LSB 0
#define CSR_DIAGRAM_5_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_4_REAL_RESET 0x0

// diagram_5_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_4_IMAG_LSB 16
#define CSR_DIAGRAM_5_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_4_IMAG_RESET 0x0

// diagram_5_5 - 
#define CSR_DIAGRAM_5_5_ADDR 0xe8
#define CSR_DIAGRAM_5_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_5_t;

// diagram_5_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_5_REAL_WIDTH 16
#define CSR_DIAGRAM_5_5_REAL_LSB 0
#define CSR_DIAGRAM_5_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_5_REAL_RESET 0x0

// diagram_5_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_5_IMAG_LSB 16
#define CSR_DIAGRAM_5_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_5_IMAG_RESET 0x0

// diagram_5_6 - 
#define CSR_DIAGRAM_5_6_ADDR 0xec
#define CSR_DIAGRAM_5_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_6_t;

// diagram_5_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_6_REAL_WIDTH 16
#define CSR_DIAGRAM_5_6_REAL_LSB 0
#define CSR_DIAGRAM_5_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_6_REAL_RESET 0x0

// diagram_5_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_6_IMAG_LSB 16
#define CSR_DIAGRAM_5_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_6_IMAG_RESET 0x0

// diagram_5_7 - 
#define CSR_DIAGRAM_5_7_ADDR 0xf0
#define CSR_DIAGRAM_5_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_5_7_t;

// diagram_5_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_7_REAL_WIDTH 16
#define CSR_DIAGRAM_5_7_REAL_LSB 0
#define CSR_DIAGRAM_5_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_5_7_REAL_RESET 0x0

// diagram_5_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_5_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_5_7_IMAG_LSB 16
#define CSR_DIAGRAM_5_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_5_7_IMAG_RESET 0x0

// diagram_6_0 - 
#define CSR_DIAGRAM_6_0_ADDR 0xf4
#define CSR_DIAGRAM_6_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_0_t;

// diagram_6_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_0_REAL_WIDTH 16
#define CSR_DIAGRAM_6_0_REAL_LSB 0
#define CSR_DIAGRAM_6_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_0_REAL_RESET 0x0

// diagram_6_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_0_IMAG_LSB 16
#define CSR_DIAGRAM_6_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_0_IMAG_RESET 0x0

// diagram_6_1 - 
#define CSR_DIAGRAM_6_1_ADDR 0xf8
#define CSR_DIAGRAM_6_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_1_t;

// diagram_6_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_1_REAL_WIDTH 16
#define CSR_DIAGRAM_6_1_REAL_LSB 0
#define CSR_DIAGRAM_6_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_1_REAL_RESET 0x0

// diagram_6_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_1_IMAG_LSB 16
#define CSR_DIAGRAM_6_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_1_IMAG_RESET 0x0

// diagram_6_2 - 
#define CSR_DIAGRAM_6_2_ADDR 0xfc
#define CSR_DIAGRAM_6_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_2_t;

// diagram_6_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_2_REAL_WIDTH 16
#define CSR_DIAGRAM_6_2_REAL_LSB 0
#define CSR_DIAGRAM_6_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_2_REAL_RESET 0x0

// diagram_6_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_2_IMAG_LSB 16
#define CSR_DIAGRAM_6_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_2_IMAG_RESET 0x0

// diagram_6_3 - 
#define CSR_DIAGRAM_6_3_ADDR 0x100
#define CSR_DIAGRAM_6_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_3_t;

// diagram_6_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_3_REAL_WIDTH 16
#define CSR_DIAGRAM_6_3_REAL_LSB 0
#define CSR_DIAGRAM_6_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_3_REAL_RESET 0x0

// diagram_6_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_3_IMAG_LSB 16
#define CSR_DIAGRAM_6_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_3_IMAG_RESET 0x0

// diagram_6_4 - 
#define CSR_DIAGRAM_6_4_ADDR 0x104
#define CSR_DIAGRAM_6_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_4_t;

// diagram_6_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_4_REAL_WIDTH 16
#define CSR_DIAGRAM_6_4_REAL_LSB 0
#define CSR_DIAGRAM_6_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_4_REAL_RESET 0x0

// diagram_6_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_4_IMAG_LSB 16
#define CSR_DIAGRAM_6_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_4_IMAG_RESET 0x0

// diagram_6_5 - 
#define CSR_DIAGRAM_6_5_ADDR 0x108
#define CSR_DIAGRAM_6_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_5_t;

// diagram_6_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_5_REAL_WIDTH 16
#define CSR_DIAGRAM_6_5_REAL_LSB 0
#define CSR_DIAGRAM_6_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_5_REAL_RESET 0x0

// diagram_6_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_5_IMAG_LSB 16
#define CSR_DIAGRAM_6_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_5_IMAG_RESET 0x0

// diagram_6_6 - 
#define CSR_DIAGRAM_6_6_ADDR 0x10c
#define CSR_DIAGRAM_6_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_6_t;

// diagram_6_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_6_REAL_WIDTH 16
#define CSR_DIAGRAM_6_6_REAL_LSB 0
#define CSR_DIAGRAM_6_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_6_REAL_RESET 0x0

// diagram_6_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_6_IMAG_LSB 16
#define CSR_DIAGRAM_6_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_6_IMAG_RESET 0x0

// diagram_6_7 - 
#define CSR_DIAGRAM_6_7_ADDR 0x110
#define CSR_DIAGRAM_6_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_6_7_t;

// diagram_6_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_7_REAL_WIDTH 16
#define CSR_DIAGRAM_6_7_REAL_LSB 0
#define CSR_DIAGRAM_6_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_6_7_REAL_RESET 0x0

// diagram_6_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_6_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_6_7_IMAG_LSB 16
#define CSR_DIAGRAM_6_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_6_7_IMAG_RESET 0x0

// diagram_7_0 - 
#define CSR_DIAGRAM_7_0_ADDR 0x114
#define CSR_DIAGRAM_7_0_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_0_t;

// diagram_7_0.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_0_REAL_WIDTH 16
#define CSR_DIAGRAM_7_0_REAL_LSB 0
#define CSR_DIAGRAM_7_0_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_0_REAL_RESET 0x0

// diagram_7_0.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_0_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_0_IMAG_LSB 16
#define CSR_DIAGRAM_7_0_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_0_IMAG_RESET 0x0

// diagram_7_1 - 
#define CSR_DIAGRAM_7_1_ADDR 0x118
#define CSR_DIAGRAM_7_1_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_1_t;

// diagram_7_1.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_1_REAL_WIDTH 16
#define CSR_DIAGRAM_7_1_REAL_LSB 0
#define CSR_DIAGRAM_7_1_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_1_REAL_RESET 0x0

// diagram_7_1.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_1_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_1_IMAG_LSB 16
#define CSR_DIAGRAM_7_1_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_1_IMAG_RESET 0x0

// diagram_7_2 - 
#define CSR_DIAGRAM_7_2_ADDR 0x11c
#define CSR_DIAGRAM_7_2_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_2_t;

// diagram_7_2.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_2_REAL_WIDTH 16
#define CSR_DIAGRAM_7_2_REAL_LSB 0
#define CSR_DIAGRAM_7_2_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_2_REAL_RESET 0x0

// diagram_7_2.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_2_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_2_IMAG_LSB 16
#define CSR_DIAGRAM_7_2_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_2_IMAG_RESET 0x0

// diagram_7_3 - 
#define CSR_DIAGRAM_7_3_ADDR 0x120
#define CSR_DIAGRAM_7_3_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_3_t;

// diagram_7_3.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_3_REAL_WIDTH 16
#define CSR_DIAGRAM_7_3_REAL_LSB 0
#define CSR_DIAGRAM_7_3_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_3_REAL_RESET 0x0

// diagram_7_3.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_3_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_3_IMAG_LSB 16
#define CSR_DIAGRAM_7_3_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_3_IMAG_RESET 0x0

// diagram_7_4 - 
#define CSR_DIAGRAM_7_4_ADDR 0x124
#define CSR_DIAGRAM_7_4_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_4_t;

// diagram_7_4.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_4_REAL_WIDTH 16
#define CSR_DIAGRAM_7_4_REAL_LSB 0
#define CSR_DIAGRAM_7_4_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_4_REAL_RESET 0x0

// diagram_7_4.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_4_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_4_IMAG_LSB 16
#define CSR_DIAGRAM_7_4_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_4_IMAG_RESET 0x0

// diagram_7_5 - 
#define CSR_DIAGRAM_7_5_ADDR 0x128
#define CSR_DIAGRAM_7_5_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_5_t;

// diagram_7_5.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_5_REAL_WIDTH 16
#define CSR_DIAGRAM_7_5_REAL_LSB 0
#define CSR_DIAGRAM_7_5_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_5_REAL_RESET 0x0

// diagram_7_5.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_5_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_5_IMAG_LSB 16
#define CSR_DIAGRAM_7_5_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_5_IMAG_RESET 0x0

// diagram_7_6 - 
#define CSR_DIAGRAM_7_6_ADDR 0x12c
#define CSR_DIAGRAM_7_6_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_6_t;

// diagram_7_6.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_6_REAL_WIDTH 16
#define CSR_DIAGRAM_7_6_REAL_LSB 0
#define CSR_DIAGRAM_7_6_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_6_REAL_RESET 0x0

// diagram_7_6.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_6_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_6_IMAG_LSB 16
#define CSR_DIAGRAM_7_6_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_6_IMAG_RESET 0x0

// diagram_7_7 - 
#define CSR_DIAGRAM_7_7_ADDR 0x130
#define CSR_DIAGRAM_7_7_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG : 16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} csr_diagram_7_7_t;

// diagram_7_7.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_7_REAL_WIDTH 16
#define CSR_DIAGRAM_7_7_REAL_LSB 0
#define CSR_DIAGRAM_7_7_REAL_MASK 0xffff
#define CSR_DIAGRAM_7_7_REAL_RESET 0x0

// diagram_7_7.imag - Imaginary part, signed 2s complement, 2**14 = 1.0
#define CSR_DIAGRAM_7_7_IMAG_WIDTH 16
#define CSR_DIAGRAM_7_7_IMAG_LSB 16
#define CSR_DIAGRAM_7_7_IMAG_MASK 0xffff0000
#define CSR_DIAGRAM_7_7_IMAG_RESET 0x0

// motion_selector - 
#define CSR_MOTION_SELECTOR_ADDR 0x134
#define CSR_MOTION_SELECTOR_RESET 0x0
typedef struct {
    uint32_t FILTER : 8; // Motion selector filter control
    uint32_t ONOFF : 1; // Motion selector on/off
    uint32_t : 23; // reserved
} csr_motion_selector_t;

// motion_selector.filter - Motion selector filter control
#define CSR_MOTION_SELECTOR_FILTER_WIDTH 8
#define CSR_MOTION_SELECTOR_FILTER_LSB 0
#define CSR_MOTION_SELECTOR_FILTER_MASK 0xff
#define CSR_MOTION_SELECTOR_FILTER_RESET 0x0

// motion_selector.onoff - Motion selector on/off
#define CSR_MOTION_SELECTOR_ONOFF_WIDTH 1
#define CSR_MOTION_SELECTOR_ONOFF_LSB 8
#define CSR_MOTION_SELECTOR_ONOFF_MASK 0x100
#define CSR_MOTION_SELECTOR_ONOFF_RESET 0x0

// diagram_angle_0 - 
#define CSR_DIAGRAM_ANGLE_0_ADDR 0x138
#define CSR_DIAGRAM_ANGLE_0_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_0_t;

// diagram_angle_0.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_0_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_0_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_0_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_0_ANGLE_RESET 0x0

// diagram_angle_1 - 
#define CSR_DIAGRAM_ANGLE_1_ADDR 0x13c
#define CSR_DIAGRAM_ANGLE_1_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_1_t;

// diagram_angle_1.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_1_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_1_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_1_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_1_ANGLE_RESET 0x0

// diagram_angle_2 - 
#define CSR_DIAGRAM_ANGLE_2_ADDR 0x140
#define CSR_DIAGRAM_ANGLE_2_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_2_t;

// diagram_angle_2.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_2_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_2_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_2_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_2_ANGLE_RESET 0x0

// diagram_angle_3 - 
#define CSR_DIAGRAM_ANGLE_3_ADDR 0x144
#define CSR_DIAGRAM_ANGLE_3_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_3_t;

// diagram_angle_3.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_3_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_3_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_3_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_3_ANGLE_RESET 0x0

// diagram_angle_4 - 
#define CSR_DIAGRAM_ANGLE_4_ADDR 0x148
#define CSR_DIAGRAM_ANGLE_4_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_4_t;

// diagram_angle_4.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_4_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_4_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_4_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_4_ANGLE_RESET 0x0

// diagram_angle_5 - 
#define CSR_DIAGRAM_ANGLE_5_ADDR 0x14c
#define CSR_DIAGRAM_ANGLE_5_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_5_t;

// diagram_angle_5.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_5_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_5_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_5_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_5_ANGLE_RESET 0x0

// diagram_angle_6 - 
#define CSR_DIAGRAM_ANGLE_6_ADDR 0x150
#define CSR_DIAGRAM_ANGLE_6_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_6_t;

// diagram_angle_6.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_6_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_6_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_6_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_6_ANGLE_RESET 0x0

// diagram_angle_7 - 
#define CSR_DIAGRAM_ANGLE_7_ADDR 0x154
#define CSR_DIAGRAM_ANGLE_7_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_diagram_angle_7_t;

// diagram_angle_7.angle - 2**32 = 2 pi
#define CSR_DIAGRAM_ANGLE_7_ANGLE_WIDTH 32
#define CSR_DIAGRAM_ANGLE_7_ANGLE_LSB 0
#define CSR_DIAGRAM_ANGLE_7_ANGLE_MASK 0xffffffff
#define CSR_DIAGRAM_ANGLE_7_ANGLE_RESET 0x0

// output_source - 
#define CSR_OUTPUT_SOURCE_ADDR 0x158
#define CSR_OUTPUT_SOURCE_RESET 0x0
typedef struct {
    uint32_t SOURCE : 16; // Source for output data
    uint32_t SOURCE_CHANNEL : 16; // Source channel for output data (if exists)
} csr_output_source_t;

// output_source.source - Source for output data
#define CSR_OUTPUT_SOURCE_SOURCE_WIDTH 16
#define CSR_OUTPUT_SOURCE_SOURCE_LSB 0
#define CSR_OUTPUT_SOURCE_SOURCE_MASK 0xffff
#define CSR_OUTPUT_SOURCE_SOURCE_RESET 0x0

// output_source.source_channel - Source channel for output data (if exists)
#define CSR_OUTPUT_SOURCE_SOURCE_CHANNEL_WIDTH 16
#define CSR_OUTPUT_SOURCE_SOURCE_CHANNEL_LSB 16
#define CSR_OUTPUT_SOURCE_SOURCE_CHANNEL_MASK 0xffff0000
#define CSR_OUTPUT_SOURCE_SOURCE_CHANNEL_RESET 0x0

// apu_rank - 
#define CSR_APU_RANK_ADDR 0x15c
#define CSR_APU_RANK_RESET 0x0
typedef struct {
    uint32_t RANK : 8; // rank for APU
    uint32_t WINDOW : 8; // window length
    uint32_t : 16; // reserved
} csr_apu_rank_t;

// apu_rank.rank - rank for APU
#define CSR_APU_RANK_RANK_WIDTH 8
#define CSR_APU_RANK_RANK_LSB 0
#define CSR_APU_RANK_RANK_MASK 0xff
#define CSR_APU_RANK_RANK_RESET 0x0

// apu_rank.window - window length
#define CSR_APU_RANK_WINDOW_WIDTH 8
#define CSR_APU_RANK_WINDOW_LSB 8
#define CSR_APU_RANK_WINDOW_MASK 0xff00
#define CSR_APU_RANK_WINDOW_RESET 0x0

// detector_level_0 - 
#define CSR_DETECTOR_LEVEL_0_ADDR 0x160
#define CSR_DETECTOR_LEVEL_0_RESET 0x0
typedef struct {
    uint32_t LEVEL : 32; // detector comparation level
} csr_detector_level_0_t;

// detector_level_0.level - detector comparation level
#define CSR_DETECTOR_LEVEL_0_LEVEL_WIDTH 32
#define CSR_DETECTOR_LEVEL_0_LEVEL_LSB 0
#define CSR_DETECTOR_LEVEL_0_LEVEL_MASK 0xffffffff
#define CSR_DETECTOR_LEVEL_0_LEVEL_RESET 0x0

// detector_level_1 - 
#define CSR_DETECTOR_LEVEL_1_ADDR 0x164
#define CSR_DETECTOR_LEVEL_1_RESET 0x0
typedef struct {
    uint32_t LEVEL : 32; // detector comparation level
} csr_detector_level_1_t;

// detector_level_1.level - detector comparation level
#define CSR_DETECTOR_LEVEL_1_LEVEL_WIDTH 32
#define CSR_DETECTOR_LEVEL_1_LEVEL_LSB 0
#define CSR_DETECTOR_LEVEL_1_LEVEL_MASK 0xffffffff
#define CSR_DETECTOR_LEVEL_1_LEVEL_RESET 0x0

// azimuth_angle - 
#define CSR_AZIMUTH_ANGLE_ADDR 0x168
#define CSR_AZIMUTH_ANGLE_RESET 0x0
typedef struct {
    uint32_t ANGLE : 32; // 2**32 = 2 pi
} csr_azimuth_angle_t;

// azimuth_angle.angle - 2**32 = 2 pi
#define CSR_AZIMUTH_ANGLE_ANGLE_WIDTH 32
#define CSR_AZIMUTH_ANGLE_ANGLE_LSB 0
#define CSR_AZIMUTH_ANGLE_ANGLE_MASK 0xffffffff
#define CSR_AZIMUTH_ANGLE_ANGLE_RESET 0x0

// apply - 
#define CSR_APPLY_ADDR 0x16c
#define CSR_APPLY_RESET 0x0
typedef struct {
    uint32_t APPLY : 1; // XOR to apply reg changes
    uint32_t : 31; // reserved
} csr_apply_t;

// apply.apply - XOR to apply reg changes
#define CSR_APPLY_APPLY_WIDTH 1
#define CSR_APPLY_APPLY_LSB 0
#define CSR_APPLY_APPLY_MASK 0x1
#define CSR_APPLY_APPLY_RESET 0x0

// compensation_reference - 
#define CSR_COMPENSATION_REFERENCE_ADDR 0x170
#define CSR_COMPENSATION_REFERENCE_RESET 0x0
typedef struct {
    uint32_t REAL : 16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t : 16; // reserved
} csr_compensation_reference_t;

// compensation_reference.real - Real part, signed 2s complement, 2**14 = 1.0
#define CSR_COMPENSATION_REFERENCE_REAL_WIDTH 16
#define CSR_COMPENSATION_REFERENCE_REAL_LSB 0
#define CSR_COMPENSATION_REFERENCE_REAL_MASK 0xffff
#define CSR_COMPENSATION_REFERENCE_REAL_RESET 0x0


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
        __IO uint32_t COMPENSATION_MODE; // 
        __IO csr_compensation_mode_t COMPENSATION_MODE_bf; // Bit access for COMPENSATION_MODE register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_0; // 
        __IO csr_manual_compensation_0_t MANUAL_COMPENSATION_0_bf; // Bit access for MANUAL_COMPENSATION_0 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_1; // 
        __IO csr_manual_compensation_1_t MANUAL_COMPENSATION_1_bf; // Bit access for MANUAL_COMPENSATION_1 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_2; // 
        __IO csr_manual_compensation_2_t MANUAL_COMPENSATION_2_bf; // Bit access for MANUAL_COMPENSATION_2 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_3; // 
        __IO csr_manual_compensation_3_t MANUAL_COMPENSATION_3_bf; // Bit access for MANUAL_COMPENSATION_3 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_4; // 
        __IO csr_manual_compensation_4_t MANUAL_COMPENSATION_4_bf; // Bit access for MANUAL_COMPENSATION_4 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_5; // 
        __IO csr_manual_compensation_5_t MANUAL_COMPENSATION_5_bf; // Bit access for MANUAL_COMPENSATION_5 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_6; // 
        __IO csr_manual_compensation_6_t MANUAL_COMPENSATION_6_bf; // Bit access for MANUAL_COMPENSATION_6 register
    };
    union {
        __IO uint32_t MANUAL_COMPENSATION_7; // 
        __IO csr_manual_compensation_7_t MANUAL_COMPENSATION_7_bf; // Bit access for MANUAL_COMPENSATION_7 register
    };
    union {
        __IO uint32_t DIAGRAM_0_0; // 
        __IO csr_diagram_0_0_t DIAGRAM_0_0_bf; // Bit access for DIAGRAM_0_0 register
    };
    union {
        __IO uint32_t DIAGRAM_0_1; // 
        __IO csr_diagram_0_1_t DIAGRAM_0_1_bf; // Bit access for DIAGRAM_0_1 register
    };
    union {
        __IO uint32_t DIAGRAM_0_2; // 
        __IO csr_diagram_0_2_t DIAGRAM_0_2_bf; // Bit access for DIAGRAM_0_2 register
    };
    union {
        __IO uint32_t DIAGRAM_0_3; // 
        __IO csr_diagram_0_3_t DIAGRAM_0_3_bf; // Bit access for DIAGRAM_0_3 register
    };
    union {
        __IO uint32_t DIAGRAM_0_4; // 
        __IO csr_diagram_0_4_t DIAGRAM_0_4_bf; // Bit access for DIAGRAM_0_4 register
    };
    union {
        __IO uint32_t DIAGRAM_0_5; // 
        __IO csr_diagram_0_5_t DIAGRAM_0_5_bf; // Bit access for DIAGRAM_0_5 register
    };
    union {
        __IO uint32_t DIAGRAM_0_6; // 
        __IO csr_diagram_0_6_t DIAGRAM_0_6_bf; // Bit access for DIAGRAM_0_6 register
    };
    union {
        __IO uint32_t DIAGRAM_0_7; // 
        __IO csr_diagram_0_7_t DIAGRAM_0_7_bf; // Bit access for DIAGRAM_0_7 register
    };
    union {
        __IO uint32_t DIAGRAM_1_0; // 
        __IO csr_diagram_1_0_t DIAGRAM_1_0_bf; // Bit access for DIAGRAM_1_0 register
    };
    union {
        __IO uint32_t DIAGRAM_1_1; // 
        __IO csr_diagram_1_1_t DIAGRAM_1_1_bf; // Bit access for DIAGRAM_1_1 register
    };
    union {
        __IO uint32_t DIAGRAM_1_2; // 
        __IO csr_diagram_1_2_t DIAGRAM_1_2_bf; // Bit access for DIAGRAM_1_2 register
    };
    union {
        __IO uint32_t DIAGRAM_1_3; // 
        __IO csr_diagram_1_3_t DIAGRAM_1_3_bf; // Bit access for DIAGRAM_1_3 register
    };
    union {
        __IO uint32_t DIAGRAM_1_4; // 
        __IO csr_diagram_1_4_t DIAGRAM_1_4_bf; // Bit access for DIAGRAM_1_4 register
    };
    union {
        __IO uint32_t DIAGRAM_1_5; // 
        __IO csr_diagram_1_5_t DIAGRAM_1_5_bf; // Bit access for DIAGRAM_1_5 register
    };
    union {
        __IO uint32_t DIAGRAM_1_6; // 
        __IO csr_diagram_1_6_t DIAGRAM_1_6_bf; // Bit access for DIAGRAM_1_6 register
    };
    union {
        __IO uint32_t DIAGRAM_1_7; // 
        __IO csr_diagram_1_7_t DIAGRAM_1_7_bf; // Bit access for DIAGRAM_1_7 register
    };
    union {
        __IO uint32_t DIAGRAM_2_0; // 
        __IO csr_diagram_2_0_t DIAGRAM_2_0_bf; // Bit access for DIAGRAM_2_0 register
    };
    union {
        __IO uint32_t DIAGRAM_2_1; // 
        __IO csr_diagram_2_1_t DIAGRAM_2_1_bf; // Bit access for DIAGRAM_2_1 register
    };
    union {
        __IO uint32_t DIAGRAM_2_2; // 
        __IO csr_diagram_2_2_t DIAGRAM_2_2_bf; // Bit access for DIAGRAM_2_2 register
    };
    union {
        __IO uint32_t DIAGRAM_2_3; // 
        __IO csr_diagram_2_3_t DIAGRAM_2_3_bf; // Bit access for DIAGRAM_2_3 register
    };
    union {
        __IO uint32_t DIAGRAM_2_4; // 
        __IO csr_diagram_2_4_t DIAGRAM_2_4_bf; // Bit access for DIAGRAM_2_4 register
    };
    union {
        __IO uint32_t DIAGRAM_2_5; // 
        __IO csr_diagram_2_5_t DIAGRAM_2_5_bf; // Bit access for DIAGRAM_2_5 register
    };
    union {
        __IO uint32_t DIAGRAM_2_6; // 
        __IO csr_diagram_2_6_t DIAGRAM_2_6_bf; // Bit access for DIAGRAM_2_6 register
    };
    union {
        __IO uint32_t DIAGRAM_2_7; // 
        __IO csr_diagram_2_7_t DIAGRAM_2_7_bf; // Bit access for DIAGRAM_2_7 register
    };
    union {
        __IO uint32_t DIAGRAM_3_0; // 
        __IO csr_diagram_3_0_t DIAGRAM_3_0_bf; // Bit access for DIAGRAM_3_0 register
    };
    union {
        __IO uint32_t DIAGRAM_3_1; // 
        __IO csr_diagram_3_1_t DIAGRAM_3_1_bf; // Bit access for DIAGRAM_3_1 register
    };
    union {
        __IO uint32_t DIAGRAM_3_2; // 
        __IO csr_diagram_3_2_t DIAGRAM_3_2_bf; // Bit access for DIAGRAM_3_2 register
    };
    union {
        __IO uint32_t DIAGRAM_3_3; // 
        __IO csr_diagram_3_3_t DIAGRAM_3_3_bf; // Bit access for DIAGRAM_3_3 register
    };
    union {
        __IO uint32_t DIAGRAM_3_4; // 
        __IO csr_diagram_3_4_t DIAGRAM_3_4_bf; // Bit access for DIAGRAM_3_4 register
    };
    union {
        __IO uint32_t DIAGRAM_3_5; // 
        __IO csr_diagram_3_5_t DIAGRAM_3_5_bf; // Bit access for DIAGRAM_3_5 register
    };
    union {
        __IO uint32_t DIAGRAM_3_6; // 
        __IO csr_diagram_3_6_t DIAGRAM_3_6_bf; // Bit access for DIAGRAM_3_6 register
    };
    union {
        __IO uint32_t DIAGRAM_3_7; // 
        __IO csr_diagram_3_7_t DIAGRAM_3_7_bf; // Bit access for DIAGRAM_3_7 register
    };
    union {
        __IO uint32_t DIAGRAM_4_0; // 
        __IO csr_diagram_4_0_t DIAGRAM_4_0_bf; // Bit access for DIAGRAM_4_0 register
    };
    union {
        __IO uint32_t DIAGRAM_4_1; // 
        __IO csr_diagram_4_1_t DIAGRAM_4_1_bf; // Bit access for DIAGRAM_4_1 register
    };
    union {
        __IO uint32_t DIAGRAM_4_2; // 
        __IO csr_diagram_4_2_t DIAGRAM_4_2_bf; // Bit access for DIAGRAM_4_2 register
    };
    union {
        __IO uint32_t DIAGRAM_4_3; // 
        __IO csr_diagram_4_3_t DIAGRAM_4_3_bf; // Bit access for DIAGRAM_4_3 register
    };
    union {
        __IO uint32_t DIAGRAM_4_4; // 
        __IO csr_diagram_4_4_t DIAGRAM_4_4_bf; // Bit access for DIAGRAM_4_4 register
    };
    union {
        __IO uint32_t DIAGRAM_4_5; // 
        __IO csr_diagram_4_5_t DIAGRAM_4_5_bf; // Bit access for DIAGRAM_4_5 register
    };
    union {
        __IO uint32_t DIAGRAM_4_6; // 
        __IO csr_diagram_4_6_t DIAGRAM_4_6_bf; // Bit access for DIAGRAM_4_6 register
    };
    union {
        __IO uint32_t DIAGRAM_4_7; // 
        __IO csr_diagram_4_7_t DIAGRAM_4_7_bf; // Bit access for DIAGRAM_4_7 register
    };
    union {
        __IO uint32_t DIAGRAM_5_0; // 
        __IO csr_diagram_5_0_t DIAGRAM_5_0_bf; // Bit access for DIAGRAM_5_0 register
    };
    union {
        __IO uint32_t DIAGRAM_5_1; // 
        __IO csr_diagram_5_1_t DIAGRAM_5_1_bf; // Bit access for DIAGRAM_5_1 register
    };
    union {
        __IO uint32_t DIAGRAM_5_2; // 
        __IO csr_diagram_5_2_t DIAGRAM_5_2_bf; // Bit access for DIAGRAM_5_2 register
    };
    union {
        __IO uint32_t DIAGRAM_5_3; // 
        __IO csr_diagram_5_3_t DIAGRAM_5_3_bf; // Bit access for DIAGRAM_5_3 register
    };
    union {
        __IO uint32_t DIAGRAM_5_4; // 
        __IO csr_diagram_5_4_t DIAGRAM_5_4_bf; // Bit access for DIAGRAM_5_4 register
    };
    union {
        __IO uint32_t DIAGRAM_5_5; // 
        __IO csr_diagram_5_5_t DIAGRAM_5_5_bf; // Bit access for DIAGRAM_5_5 register
    };
    union {
        __IO uint32_t DIAGRAM_5_6; // 
        __IO csr_diagram_5_6_t DIAGRAM_5_6_bf; // Bit access for DIAGRAM_5_6 register
    };
    union {
        __IO uint32_t DIAGRAM_5_7; // 
        __IO csr_diagram_5_7_t DIAGRAM_5_7_bf; // Bit access for DIAGRAM_5_7 register
    };
    union {
        __IO uint32_t DIAGRAM_6_0; // 
        __IO csr_diagram_6_0_t DIAGRAM_6_0_bf; // Bit access for DIAGRAM_6_0 register
    };
    union {
        __IO uint32_t DIAGRAM_6_1; // 
        __IO csr_diagram_6_1_t DIAGRAM_6_1_bf; // Bit access for DIAGRAM_6_1 register
    };
    union {
        __IO uint32_t DIAGRAM_6_2; // 
        __IO csr_diagram_6_2_t DIAGRAM_6_2_bf; // Bit access for DIAGRAM_6_2 register
    };
    union {
        __IO uint32_t DIAGRAM_6_3; // 
        __IO csr_diagram_6_3_t DIAGRAM_6_3_bf; // Bit access for DIAGRAM_6_3 register
    };
    union {
        __IO uint32_t DIAGRAM_6_4; // 
        __IO csr_diagram_6_4_t DIAGRAM_6_4_bf; // Bit access for DIAGRAM_6_4 register
    };
    union {
        __IO uint32_t DIAGRAM_6_5; // 
        __IO csr_diagram_6_5_t DIAGRAM_6_5_bf; // Bit access for DIAGRAM_6_5 register
    };
    union {
        __IO uint32_t DIAGRAM_6_6; // 
        __IO csr_diagram_6_6_t DIAGRAM_6_6_bf; // Bit access for DIAGRAM_6_6 register
    };
    union {
        __IO uint32_t DIAGRAM_6_7; // 
        __IO csr_diagram_6_7_t DIAGRAM_6_7_bf; // Bit access for DIAGRAM_6_7 register
    };
    union {
        __IO uint32_t DIAGRAM_7_0; // 
        __IO csr_diagram_7_0_t DIAGRAM_7_0_bf; // Bit access for DIAGRAM_7_0 register
    };
    union {
        __IO uint32_t DIAGRAM_7_1; // 
        __IO csr_diagram_7_1_t DIAGRAM_7_1_bf; // Bit access for DIAGRAM_7_1 register
    };
    union {
        __IO uint32_t DIAGRAM_7_2; // 
        __IO csr_diagram_7_2_t DIAGRAM_7_2_bf; // Bit access for DIAGRAM_7_2 register
    };
    union {
        __IO uint32_t DIAGRAM_7_3; // 
        __IO csr_diagram_7_3_t DIAGRAM_7_3_bf; // Bit access for DIAGRAM_7_3 register
    };
    union {
        __IO uint32_t DIAGRAM_7_4; // 
        __IO csr_diagram_7_4_t DIAGRAM_7_4_bf; // Bit access for DIAGRAM_7_4 register
    };
    union {
        __IO uint32_t DIAGRAM_7_5; // 
        __IO csr_diagram_7_5_t DIAGRAM_7_5_bf; // Bit access for DIAGRAM_7_5 register
    };
    union {
        __IO uint32_t DIAGRAM_7_6; // 
        __IO csr_diagram_7_6_t DIAGRAM_7_6_bf; // Bit access for DIAGRAM_7_6 register
    };
    union {
        __IO uint32_t DIAGRAM_7_7; // 
        __IO csr_diagram_7_7_t DIAGRAM_7_7_bf; // Bit access for DIAGRAM_7_7 register
    };
    union {
        __IO uint32_t MOTION_SELECTOR; // 
        __IO csr_motion_selector_t MOTION_SELECTOR_bf; // Bit access for MOTION_SELECTOR register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_0; // 
        __IO csr_diagram_angle_0_t DIAGRAM_ANGLE_0_bf; // Bit access for DIAGRAM_ANGLE_0 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_1; // 
        __IO csr_diagram_angle_1_t DIAGRAM_ANGLE_1_bf; // Bit access for DIAGRAM_ANGLE_1 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_2; // 
        __IO csr_diagram_angle_2_t DIAGRAM_ANGLE_2_bf; // Bit access for DIAGRAM_ANGLE_2 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_3; // 
        __IO csr_diagram_angle_3_t DIAGRAM_ANGLE_3_bf; // Bit access for DIAGRAM_ANGLE_3 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_4; // 
        __IO csr_diagram_angle_4_t DIAGRAM_ANGLE_4_bf; // Bit access for DIAGRAM_ANGLE_4 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_5; // 
        __IO csr_diagram_angle_5_t DIAGRAM_ANGLE_5_bf; // Bit access for DIAGRAM_ANGLE_5 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_6; // 
        __IO csr_diagram_angle_6_t DIAGRAM_ANGLE_6_bf; // Bit access for DIAGRAM_ANGLE_6 register
    };
    union {
        __IO uint32_t DIAGRAM_ANGLE_7; // 
        __IO csr_diagram_angle_7_t DIAGRAM_ANGLE_7_bf; // Bit access for DIAGRAM_ANGLE_7 register
    };
    union {
        __IO uint32_t OUTPUT_SOURCE; // 
        __IO csr_output_source_t OUTPUT_SOURCE_bf; // Bit access for OUTPUT_SOURCE register
    };
    union {
        __IO uint32_t APU_RANK; // 
        __IO csr_apu_rank_t APU_RANK_bf; // Bit access for APU_RANK register
    };
    union {
        __IO uint32_t DETECTOR_LEVEL_0; // 
        __IO csr_detector_level_0_t DETECTOR_LEVEL_0_bf; // Bit access for DETECTOR_LEVEL_0 register
    };
    union {
        __IO uint32_t DETECTOR_LEVEL_1; // 
        __IO csr_detector_level_1_t DETECTOR_LEVEL_1_bf; // Bit access for DETECTOR_LEVEL_1 register
    };
    union {
        __IO uint32_t AZIMUTH_ANGLE; // 
        __IO csr_azimuth_angle_t AZIMUTH_ANGLE_bf; // Bit access for AZIMUTH_ANGLE register
    };
    union {
        __IO uint32_t APPLY; // 
        __IO csr_apply_t APPLY_bf; // Bit access for APPLY register
    };
    union {
        __IO uint32_t COMPENSATION_REFERENCE; // 
        __IO csr_compensation_reference_t COMPENSATION_REFERENCE_bf; // Bit access for COMPENSATION_REFERENCE register
    };
} csr_t;

#define CSR ((csr_t*)(CSR_BASE_ADDR))

#ifdef __cplusplus
}
#endif

#endif /* __REGS_H */