#include "axi_dsp.h"
#include "dma_channel.hpp"
#include "misc.h"

#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>
#include <pistring_std.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

#define RX_DEV          "/dev/dma_proxy_rx"
#define TX_DEV_CH0      "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1      "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH2      "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH3      "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH4      "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH5      "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH6      "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH7      "/dev/dma_proxy_tx_ch7"

#define NUM_CHANNELS_RX 1
#define NUM_CHANNELS_TX 8

int main(int argc, char * argv[]) {
	if (argc < 5) {
		printf("usage: %s <test_point> <channel> <range_gate> <num_transfers> <input_file> <output_file>\n", argv[0]);
		return 1;
	}

	PIString dir_path_str = StdString2PIString(misc_get_date());
	fs::path dir_path     = PIString2StdString(dir_path_str);

	if (fs::create_directories(dir_path)) {
		piCout << "Created directory" << dir_path_str;
	}
	piCout << "Save to directory" << dir_path_str;

	PISignals::setSlot([](PISignals::Signal s) {
		piCout << "Signal" << s;
		PIKbdListener::exiting = true;
		PISignals::releaseSignals(s);
	});
	PISignals::grabSignals(PISignals::Interrupt | PISignals::Termination);


	PIKbdListener * kbd = nullptr;


	kbd                 = new PIKbdListener(nullptr, nullptr, false);
	kbd->enableExitCapture(PIKbdListener::F10);


	uint32_t test_point     = (uint32_t)strtol(argv[1], NULL, 0);
	uint32_t channel        = (uint32_t)strtol(argv[2], NULL, 0);
	uint32_t range_gate     = (uint32_t)strtol(argv[3], NULL, 0);
	uint32_t num_transfers  = (uint32_t)strtol(argv[4], NULL, 0);
	const char * input_file = argv[5];
	PIString output_file    = dir_path_str + "/" + argv[6]; // File to dump RX data (optional, can be empty string)

	axi_dsp_init();
	axi_dsp_kill();
	axi_dsp_set_output_source(test_point, channel, range_gate);
	auto v = axi_dsp_get_output_source();
	piCout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << ", RANGE_GATE: " << v.RANGE_GATE << "\n";
	cmplx_f64 manual_comp       = {.real = 1, .imag = 0};
	cmplx_f64 diagrams_even     = {.real = 1, .imag = 0};
	cmplx_f64 diagrams_odd      = {.real = 0, .imag = 1};

	cmplx_f64 diagrams_0_all[8] = {
		{.real = 0.5250, .imag = 0.8511 },
		{.real = 0.7470, .imag = 0.6648 },
		{.real = 0.9063, .imag = 0.4226 },
		{.real = 0.9894, .imag = 0.1449 },
		{.real = 0.9894, .imag = -0.1449},
		{.real = 0.9063, .imag = -0.4226},
		{.real = 0.7470, .imag = -0.6648},
		{.real = 0.5250, .imag = -0.8511},
	};

	cmplx_f64 diagrams_1_all[8] = {
		{.real = -0.9925, .imag = 0.1220 },
		{.real = -0.5529, .imag = 0.8333 },
		{.real = 0.2733,  .imag = 0.9619 },
		{.real = 0.9084,  .imag = 0.4181 },
		{.real = 0.9084,  .imag = -0.4181},
		{.real = 0.2733,  .imag = -0.9619},
		{.real = -0.5529, .imag = -0.8333},
		{.real = -0.9925, .imag = -0.1220},
	};

	cmplx_f64 diagrams_2_all[8] = {
		{.real = 0.2031,  .imag = -0.9792},
		{.real = -0.9321, .imag = -0.3621},
		{.real = -0.5111, .imag = 0.8595 },
		{.real = 0.7633,  .imag = 0.6461 },
		{.real = 0.7633,  .imag = -0.6461},
		{.real = -0.5111, .imag = -0.8595},
		{.real = -0.9321, .imag = 0.3621 },
		{.real = 0.2031,  .imag = 0.9792 },
	};

	cmplx_f64 diagrams_3_all[8] = {
		{.real = 0.9349,  .imag = 0.3549 },
		{.real = 0.0347,  .imag = -0.9994},
		{.real = -0.9573, .imag = 0.2891 },
		{.real = 0.5821,  .imag = 0.8131 },
		{.real = 0.5821,  .imag = -0.8131},
		{.real = -0.9573, .imag = -0.2891},
		{.real = 0.0347,  .imag = 0.9994 },
		{.real = 0.9349,  .imag = -0.3549},
	};

	cmplx_f64 diagrams_4_all[8] = {
		{.real = -0.2890, .imag = 0.9573 },
		{.real = 0.8944,  .imag = -0.4473},
		{.real = -0.9394, .imag = -0.3430},
		{.real = 0.3958,  .imag = 0.9183 },
		{.real = 0.3958,  .imag = -0.9183},
		{.real = -0.9394, .imag = 0.3430 },
		{.real = 0.8944,  .imag = 0.4473 },
		{.real = -0.2890, .imag = -0.9573},
	};

	cmplx_f64 diagrams_5_all[8] = {
		{.real = -0.9984, .imag = 0.0558 },
		{.real = 0.9175,  .imag = 0.3977 },
		{.real = -0.6420, .imag = -0.7667},
		{.real = 0.2303,  .imag = 0.9731 },
		{.real = 0.2303,  .imag = -0.9731},
		{.real = -0.6420, .imag = 0.7667 },
		{.real = 0.9175,  .imag = -0.3977},
		{.real = -0.9984, .imag = -0.0558},
	};

	cmplx_f64 diagrams_6_all[8] = {
		{.real = -0.6639, .imag = -0.7478},
		{.real = 0.4957,  .imag = 0.8685 },
		{.real = -0.3062, .imag = -0.9520},
		{.real = 0.1035,  .imag = 0.9946 },
		{.real = 0.1035,  .imag = -0.9946},
		{.real = -0.3062, .imag = 0.9520 },
		{.real = 0.4957,  .imag = -0.8685},
		{.real = -0.6639, .imag = 0.7478 },
	};

	cmplx_f64 diagrams_7_all[8] = {
		{.real = -0.1767, .imag = -0.9843},
		{.real = 0.1265,  .imag = 0.9920 },
		{.real = -0.0761, .imag = -0.9971},
		{.real = 0.0254,  .imag = 0.9997 },
		{.real = 0.0254,  .imag = -0.9997},
		{.real = -0.0761, .imag = 0.9971 },
		{.real = 0.1265,  .imag = -0.9920},
		{.real = -0.1767, .imag = 0.9843 },
	};

	for (size_t i = 0; i < NUM_CHANNELS_TX; i++) {
		axi_dsp_set_manual_compensation(manual_comp, i);
		axi_dsp_set_diagram_0(diagrams_0_all[i], i);
		axi_dsp_set_diagram_1(diagrams_1_all[i], i);
		axi_dsp_set_diagram_2(diagrams_2_all[i], i);
		axi_dsp_set_diagram_3(diagrams_3_all[i], i);
		axi_dsp_set_diagram_4(diagrams_4_all[i], i);
		axi_dsp_set_diagram_5(diagrams_5_all[i], i);
		axi_dsp_set_diagram_6(diagrams_6_all[i], i);
		axi_dsp_set_diagram_7(diagrams_7_all[i], i);
	}
	axi_dsp_set_compensation_mode(0);
	axi_dsp_set_compensation_ref(1e-3);
	axi_dsp_apply();

	int buf_size             = BUFFER_SIZE;
	// uint32_t num_transfers   = 16;
	uint32_t n_samps_per_buf = 141;
	uint32_t num_rx_transfer = num_transfers * N_PACKS_IN_TX_BUF;

	switch (test_point) {
	case TP_WORK: {
		n_samps_per_buf = sizeof(work_data);
		num_rx_transfer = num_rx_transfer / 20;
		break;
	}
	case TP_BYPASS: {
		n_samps_per_buf = N_SAMPS_IN_TX_BUF;
		break;
	}
	case TP_CUT:
	case TP_FAPCH:
	case TP_LOU:
	case TP_SF: {
		n_samps_per_buf = 141;
		break;
	}
	case TP_DDR:
	case TP_FFT: {
		n_samps_per_buf = 512;
		num_rx_transfer = num_rx_transfer / 20;
		break;
	}
	case TP_MAX: {
		n_samps_per_buf = 141;
		num_rx_transfer = num_rx_transfer / 20;
		break;
	}
	case TP_FIND: {
		n_samps_per_buf = 423;
		num_rx_transfer = num_rx_transfer / 20;
		break;
	}
	default: {
		break;
	}
	}
	n_samps_per_buf += HDR_SIZE;

	PIVector<dma_channel *> dma_channels;

	void * rx_buffers[RX_BUFFER_COUNT];
	void * tx_buffers[NUM_CHANNELS_TX][TX_BUFFER_COUNT];

	dma_channels.resize(NUM_CHANNELS_TX + NUM_CHANNELS_RX);
	for (int i = 0; i < NUM_CHANNELS_TX + NUM_CHANNELS_RX; i++) {
		dma_channels[i] = new dma_channel();
	}

	PIString tx_devnodes[NUM_CHANNELS_TX] =
		{TX_DEV_CH0, TX_DEV_CH1, TX_DEV_CH2, TX_DEV_CH3, TX_DEV_CH4, TX_DEV_CH5, TX_DEV_CH6, TX_DEV_CH7};

	dma_channel::ch_config rx_config = {.devnode = RX_DEV, .buffer_size = BUFFER_SIZE, .buffer_count = RX_BUFFER_COUNT};
	dma_channel::ch_config tx_config = {.buffer_size = BUFFER_SIZE, .buffer_count = TX_BUFFER_COUNT};
	piCout << "Wait for DMA init";
	dma_channels[0]->init(rx_config);
	dma_channels[0]->set_save_to_file(output_file, n_samps_per_buf);
	dma_channels[0]->set_num_transfers(num_rx_transfer);
	for (size_t i = 0; i < RX_BUFFER_COUNT; i++) {
		rx_buffers[i] = dma_channels[0]->get_buffer(i);
	}
	piCout << "Rx buffers adresses are:";
	for (size_t i = 0; i < RX_BUFFER_COUNT; i++) {
		piCout << "num " << i << " " << PICoutManipulators::PICoutFormat::Hex << rx_buffers[i];
	}

	piCout << "Tx buffer adresses are:";
	for (size_t i = 0; i < NUM_CHANNELS_TX; i++) {
		tx_config.devnode = PIString2StdString(tx_devnodes[i]);
		dma_channels[i + 1]->init(tx_config);
		dma_channels[i + 1]->set_num_transfers(num_transfers);
		dma_channels[i + 1]->get_all_buffers(tx_buffers[i]);
		for (size_t k = 0; k < TX_BUFFER_COUNT; k++) {
			piCout << "ch" << i << " buf" << k << " " << PICoutManipulators::PICoutFormat::Hex << tx_buffers[i][k];
		}
	}

	uint8_t * current_buffers[NUM_CHANNELS_TX];
	for (size_t k = 0; k < NUM_CHANNELS_TX; k++) {
		// dma_channels[k + 1]->get_all_buffers(tx_buffers[k]);
		current_buffers[k] = (uint8_t *)tx_buffers[k][0];
	}
	size_t cnt = 0;

	dma_channels[0]->start();
	int buff_id = 0;
	for (size_t i = 0; i < num_transfers; i++) {
		if (!(i % TX_BUFFER_COUNT)) {
			misc_read_8chs_from_file(input_file,
			                         current_buffers,
			                         buf_size * TX_BUFFER_COUNT,
			                         buf_size / sizeof(unsigned int) * TX_BUFFER_COUNT * cnt);
			cnt++;
		}

		size_t offset = i * n_samps_per_buf * N_PACKS_IN_TX_BUF;
		PISystemTime t0_send, t1_send;
		t0_send = PISystemTime::current();
		for (int k = dma_channels.size() - 1; k >= 1; k--) {
			dma_channels[k]->start_transfer();
		}
		t1_send = PISystemTime::current();
		piCout << "start transfer - exit from start transfer time = " << t1_send - t0_send;

		PISystemTime t0, t1;
		125_us .sleep();

		t0 = PISystemTime::current();
		for (int k = dma_channels.size() - 1; k >= 1; k--) {
			dma_channels[k]->wait_for_transfer();
			// piCout << "start wait - stop transfer time for channel" << k -1 << " = " << t1 - t0;
		}
		t1 = PISystemTime::current();
		piCout << "start wait - stop transfer time = " << t1 - t0;
		buff_id = (buff_id + 1) % TX_BUFFER_COUNT;
	}
	dma_channels[0]->waitForFinish();

	// WAIT_FOR_EXIT;

	for (int k = dma_channels.size() - 1; k >= 0; k--) {
		dma_channels[k]->cleanup();
		delete dma_channels[k];
		dma_channels[k] = nullptr;
	}
	axi_dsp_deinit();
	piDeleteSafety(kbd);

	return 0;
}