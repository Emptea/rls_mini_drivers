#include "axi_dsp.h"
#include "misc.h"
#include <cstdint>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define MANUAL_COMPENSATION_ORD 14
#define ANGLE_ORD               32
static int fd;

static uint32_t axi_write(uint32_t val, uint32_t regmap_offset)
{
    lseek(fd, regmap_offset, SEEK_SET);
    if (write(fd, &val, sizeof(val)) != sizeof(val)) {
        perror("write");
        printf("Write error\n");
        axi_dsp_deinit();
        return FD_ERR_WRITE;
    }

    return FD_ERR_NONE;
}

static uint32_t axi_read(uint32_t *val, uint32_t regmap_offset)
{
    uint32_t rd;

    lseek(fd, regmap_offset, SEEK_SET);
    if (read(fd, &rd, sizeof(rd)) != sizeof(rd)) {
        perror("read");
        printf("Read error\n");
        axi_dsp_deinit();
        return FD_ERR_READ;
    }

    *val = rd;
    return FD_ERR_NONE;
}

static cmplx_f64 read_cmplx_num(uint32_t addr)
{
    uint32_t raw;
    cmplx_i32 num_u32;
    cmplx_f64 num_f64;

    axi_read(&raw, addr);
    num_u32.REAL = (raw >> 0) & 0xFFFF;
    num_u32.IMAG = (raw >> 16) & 0xFFFF;

    num_f64.real = fix_to_float(num_u32.REAL, MANUAL_COMPENSATION_ORD);
    num_f64.imag = fix_to_float(num_u32.IMAG, MANUAL_COMPENSATION_ORD);

    return num_f64;
}

static void write_cmplx_num(cmplx_f64 num_f64, uint32_t addr)
{
    union {
        cmplx_i32 cmplx;
        uint32_t raw;
    } num;

    num.cmplx.REAL = float_to_fix(num_f64.real, MANUAL_COMPENSATION_ORD);
    num.cmplx.IMAG = float_to_fix(num_f64.imag, MANUAL_COMPENSATION_ORD);
    axi_write(num.raw, addr);
}

static float read_angle(uint32_t addr)
{
    uint32_t raw;
    axi_read(&raw, addr);
    float angle = fix_to_deg(raw, ANGLE_ORD);
    return angle;
}

static void write_angle(float angle, uint32_t addr)
{
    uint32_t raw = deg_to_fix(angle, ANGLE_ORD);
    axi_write(raw, addr);
}

static uint32_t read_u32(uint32_t addr)
{
    uint32_t raw;
    axi_read(&raw, addr);
    return raw;
}

uint32_t axi_dsp_init()
{
    printf("Debug: Trying to open /dev/rls_mini_pl\n");
    
    fd = open("/dev/rls_mini_pl", O_RDWR);
    if (fd < 0) {
        printf("Debug: open() failed with errno=%d\n", errno);
        perror("open");
        
        // Try with O_RDONLY as fallback
        printf("Debug: Trying O_RDONLY\n");
        fd = open("/dev/rls_mini_pl", O_RDONLY);
        if (fd < 0) {
            perror("open (readonly)");
            return FD_ERR_NO_DEVICE;
        }
        printf("Debug: Opened readonly successfully\n");
        return FD_ERR_NONE;
    }
    
    printf("Debug: Opened successfully, fd=%d\n", fd);
    return FD_ERR_NONE;
}

void axi_dsp_deinit()
{
    close(fd);
}

/* Getters */
csr_ip_ver_t axi_dsp_get_ip_ver()
{
    uint32_t raw;
    csr_ip_ver_t ver;

    axi_read(&raw, CSR_IP_VER_ADDR);

    ver.MIN_VER = (raw >> CSR_IP_VER_MIN_VER_LSB) & 0xFFFF;
    ver.MAJ_VER = (raw >> CSR_IP_VER_MAJ_VER_LSB) & 0xFFFF;

    return ver;
}

uint32_t axi_dsp_get_kill()
{
    return read_u32(CSR_KILL_ADDR);
}

uint32_t axi_dsp_get_test_point()
{
    return read_u32(CSR_TEST_POINT_ADDR);
}

uint32_t axi_dsp_get_channel()
{
    return read_u32(CSR_CHANNEL_ADDR);
}

uint32_t axi_dsp_get_compensation_mode()
{
    return read_u32(CSR_COMPENSATION_MODE_ADDR);
}

cmplx_f64 axi_dsp_get_manual_compensation(uint32_t channel)
{
    return read_cmplx_num(CSR_MANUAL_COMPENSATION_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_0(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_0_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_1(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_1_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_2(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_2_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_3(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_3_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_4(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_4_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_5(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_5_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_6(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_6_0_ADDR + (channel * 0x04));
}

cmplx_f64 axi_dsp_get_diagram_7(uint32_t channel)
{
    return read_cmplx_num(CSR_DIAGRAM_7_0_ADDR + (channel * 0x04));
}

csr_motion_selector_t axi_dsp_get_motion_selector()
{
    uint32_t raw;
    csr_motion_selector_t motion_selector;
    axi_read(&raw, CSR_MOTION_SELECTOR_ADDR);
    motion_selector.FILTER = ((raw & CSR_MOTION_SELECTOR_FILTER_MASK) >> CSR_MOTION_SELECTOR_FILTER_LSB);
    motion_selector.ONOFF = ((raw & CSR_MOTION_SELECTOR_ONOFF_MASK) >> CSR_MOTION_SELECTOR_ONOFF_LSB);
    return motion_selector;
}

float axi_dsp_get_diagram_angle(uint32_t channel)
{
    return read_angle(CSR_DIAGRAM_ANGLE_0_ADDR + (channel * 4));
}

csr_output_source_t axi_dsp_get_output_source()
{
    uint32_t raw;
    csr_output_source_t src;

    axi_read(&raw, CSR_OUTPUT_SOURCE_ADDR);
    src.SOURCE = (raw & CSR_OUTPUT_SOURCE_SOURCE_MASK) >> CSR_OUTPUT_SOURCE_SOURCE_LSB;
    src.SOURCE_CHANNEL = (raw & CSR_OUTPUT_SOURCE_SOURCE_CHANNEL_MASK) >> CSR_OUTPUT_SOURCE_SOURCE_CHANNEL_LSB;

    return src;
}

csr_apu_rank_t axi_dsp_get_apu_rank()
{
    uint32_t raw;
    csr_apu_rank_t apu_rank;

    axi_read(&raw, CSR_OUTPUT_SOURCE_ADDR);
    apu_rank.RANK = (raw & CSR_APU_RANK_RANK_MASK) >> CSR_APU_RANK_RANK_LSB;
    apu_rank.WINDOW = (raw & CSR_APU_RANK_WINDOW_MASK) >> CSR_APU_RANK_WINDOW_LSB;

    return apu_rank;
}

uint32_t axi_dsp_get_detector_level(uint32_t num)
{
    return read_u32(CSR_OUTPUT_SOURCE_ADDR + (num * 4));
}

float axi_dsp_get_azimuth_angle()
{
    return read_angle(CSR_AZIMUTH_ANGLE_ADDR);
}

uint32_t axi_dsp_get_apply()
{
    return read_u32(CSR_APPLY_ADDR);
}

/* Setters */
void axi_dsp_set_test_point(uint32_t tp)
{
    axi_write(tp, CSR_TEST_POINT_ADDR);
}

void axi_dsp_set_channel(uint32_t channel)
{
    axi_write(channel, CSR_CHANNEL_ADDR);
}

void axi_dsp_set_compensation_mode(uint32_t compensation_mode)
{
    axi_write(compensation_mode, CSR_COMPENSATION_MODE_ADDR);
}

void axi_dsp_set_manual_compensation(cmplx_f64 manual_comp, uint32_t channel)
{
    write_cmplx_num(manual_comp, CSR_MANUAL_COMPENSATION_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_0(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_0_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_1(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_1_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_2(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_2_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_3(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_3_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_4(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_4_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_5(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_5_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_6(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_6_0_ADDR + (channel * 4));
}

void axi_dsp_set_diagram_7(cmplx_f64 diagram, uint32_t channel)
{
    write_cmplx_num(diagram, CSR_DIAGRAM_7_0_ADDR + (channel * 4));
}

void axi_dsp_set_motion_selector(uint32_t filter, uint32_t onoff)
{
    union {
        csr_motion_selector_t motion_selector;
        uint32_t raw;
    } motion_sel_un = {
        .motion_selector{.FILTER = filter, .ONOFF = onoff}
    };

    axi_write(motion_sel_un.raw, CSR_MOTION_SELECTOR_ADDR);
}

void axi_dsp_set_diagram_angle(float angle, uint32_t channel)
{
    write_angle(angle, CSR_DIAGRAM_ANGLE_0_ADDR + (channel * 4));
}

void axi_dsp_set_output_source(uint32_t src, uint32_t src_channel)
{
    union {
        csr_output_source_t src;
        uint32_t raw;
    } source_un = {
        .src{.SOURCE = src, .SOURCE_CHANNEL = src_channel}
    };

    axi_write(source_un.raw, CSR_OUTPUT_SOURCE_ADDR);
}

void axi_dsp_set_apu_rank(uint32_t rank, uint32_t window)
{
    union {
        csr_apu_rank_t apu_rank;
        uint32_t raw;
    } apu_rank_un = {
        .apu_rank = {.RANK = rank, .WINDOW = window}
    };

    axi_write(apu_rank_un.raw, CSR_APU_RANK_ADDR);
}

void axi_dsp_set_detector_level(uint32_t level, uint32_t num)
{
    axi_write(level, CSR_DETECTOR_LEVEL_0_ADDR + (num * 4));
}

void axi_dsp_set_azimuth_angle(float angle)
{
    write_angle(angle, CSR_AZIMUTH_ANGLE_ADDR);
}

void axi_dsp_kill()
{
    axi_write(1, CSR_KILL_ADDR);
}

void axi_dsp_apply()
{
    uint32_t prev_apply = (!axi_dsp_get_apply()) & 0x01;
    axi_write(prev_apply, CSR_APPLY_ADDR);
}
