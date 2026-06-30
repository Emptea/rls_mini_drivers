#ifndef AXI_DSP_H
#define AXI_DSP_H
#include <stdint.h>
#include "regs.h"

enum fd_err {
    FD_ERR_NONE,
    FD_ERR_WRITE,
    FD_ERR_READ,
    FD_ERR_NO_DEVICE,
};

enum tp {
        TP_WORK = 0,
        TP_BYPASS = 1,
        TP_CUT = 2,
        TP_FAPCH = 3,
        TP_LOU = 4,
        TP_SF = 5,
};

typedef struct {
    uint32_t REAL :16; // Real part, signed 2s complement, 2**14 = 1.0
    uint32_t IMAG :16; // Imaginary part, signed 2s complement, 2**14 = 1.0
} cmplx_i32;

typedef struct {
    float real;
    float imag;
} cmplx_f64;

uint32_t axi_dsp_init();
void axi_dsp_deinit();

csr_ip_ver_t axi_dsp_get_ip_ver();
uint32_t axi_dsp_get_kill();
uint32_t axi_dsp_get_test_point();
uint32_t axi_dsp_get_channel();
uint32_t axi_dsp_get_compensation_mode();
cmplx_f64 axi_dsp_get_manual_compensation(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_0(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_1(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_2(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_3(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_4(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_5(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_6(uint32_t channel);
cmplx_f64 axi_dsp_get_diagram_7(uint32_t channel);
csr_motion_selector_t axi_dsp_get_motion_selector();
float axi_dsp_get_diagram_angle(uint32_t channel);
csr_output_source_t axi_dsp_get_output_source();
csr_apu_rank_t axi_dsp_get_apu_rank();
uint32_t axi_dsp_get_detector_level(uint32_t num);
float axi_dsp_get_azimuth_angle();
float axi_dsp_get_compensation_ref();
uint32_t axi_dsp_get_apply();

void axi_dsp_set_test_point(uint32_t tp);
void axi_dsp_set_channel(uint32_t channel);
void axi_dsp_set_compensation_mode(uint32_t compensation_mode);
void axi_dsp_set_manual_compensation(cmplx_f64 manual_comp, uint32_t channel);
void axi_dsp_set_diagram_0(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_1(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_2(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_3(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_4(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_5(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_6(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_diagram_7(cmplx_f64 diagram, uint32_t channel);
void axi_dsp_set_motion_selector(uint32_t filter, uint32_t onoff);
void axi_dsp_set_diagram_angle(float angle, uint32_t channel);
void axi_dsp_set_output_source(uint32_t src, uint32_t src_channel);
void axi_dsp_set_apu_rank(uint32_t rank, uint32_t window);
void axi_dsp_set_detector_level(uint32_t level, uint32_t num);
void axi_dsp_set_azimuth_angle(float angle);
void axi_dsp_set_compensation_ref(float ref);

void axi_dsp_kill();
void axi_dsp_apply();

#endif