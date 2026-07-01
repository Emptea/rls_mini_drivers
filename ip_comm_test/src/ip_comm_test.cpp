#include "axi_dsp.h"
#include "dma_channel.hpp"
#include "misc.h"

#include <cstdint>
#include <fcntl.h>
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
		printf("usage: %s <test_point> <channel> <num_transfers> <input_file> <output_file>\n", argv[0]);
		return 1;
	}


	PISignals::setSlot([](PISignals::Signal s) {
		piCout << "Signal" << s;
		PIKbdListener::exiting = true;
		PISignals::releaseSignals(s);
	});
	PISignals::grabSignals(PISignals::Interrupt | PISignals::Termination);


	PIKbdListener * kbd = nullptr;


	kbd                 = new PIKbdListener(nullptr, nullptr, false);
	kbd->enableExitCapture(PIKbdListener::F10);


	uint32_t test_point      = (uint32_t)strtol(argv[1], NULL, 0);
	uint32_t channel         = (uint32_t)strtol(argv[2], NULL, 0);
	uint32_t num_transfers   = (uint32_t)strtol(argv[3], NULL, 0);
	const char * input_file  = argv[4];
	const char * output_file = argv[5]; // File to dump RX data (optional, can be empty string)

	axi_dsp_init();
	axi_dsp_set_output_source(test_point, channel);
	auto v = axi_dsp_get_output_source();
	piCout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << "\n";
	cmplx_f64 manual_comp   = {.real = 1, .imag = 0};
	cmplx_f64 diagrams_even = {.real = 1, .imag = 0};
	cmplx_f64 diagrams_odd  = {.real = 0, .imag = 1};
	for (size_t i = 0; i < NUM_CHANNELS_TX; i++) {
		axi_dsp_set_manual_compensation(manual_comp, i);
		axi_dsp_set_diagram_0(diagrams_even, i);
		axi_dsp_set_diagram_1(diagrams_odd, i);
		axi_dsp_set_diagram_2(diagrams_even, i);
		axi_dsp_set_diagram_3(diagrams_odd, i);
		axi_dsp_set_diagram_4(diagrams_even, i);
		axi_dsp_set_diagram_5(diagrams_odd, i);
		axi_dsp_set_diagram_6(diagrams_even, i);
		axi_dsp_set_diagram_7(diagrams_odd, i);
	}
	axi_dsp_set_compensation_mode(1);
	axi_dsp_apply();

	int buf_size             = BUFFER_SIZE;
	// uint32_t num_transfers   = 16;
	uint32_t n_samps_per_buf = 141;

	switch (test_point) {
	case TP_BYPASS: {
		n_samps_per_buf = N_SAMPS_IN_TX_BUF;
		break;
	}
	case TP_CUT:
	case TP_FAPCH:
	default: {
		n_samps_per_buf = 141;
		break;
	}
	}

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
	dma_channels[0]->set_num_transfers(num_transfers * N_PACKS_IN_TX_BUF);
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

	dma_channels[0]->start(45_us);
	int buff_id = 0;
	for (size_t i = 0; i < num_transfers; i++) {
		uint8_t * current_buffers[TX_BUFFER_COUNT];
		piCout << "Tx buffer adresses for transfer #" << i << " are:";
		for (size_t k = 0; k < NUM_CHANNELS_TX; k++) {
			dma_channels[k + 1]->get_all_buffers(tx_buffers[k]);
			current_buffers[k] = (uint8_t *)tx_buffers[k][buff_id];
			for (size_t j = 0; j < TX_BUFFER_COUNT; j++) {
				piCout << "ch" << k << " buf" << j << " " << PICoutManipulators::PICoutFormat::Hex << tx_buffers[k][j];
			}
		}
		size_t offset = i * n_samps_per_buf * N_PACKS_IN_TX_BUF;
		misc_read_8chs_from_file(input_file, current_buffers, buf_size, offset);
		for (int k = dma_channels.size() - 1; k >= 1; k--) {
			dma_channels[k]->start_transfer();
		}
		180_us .sleep();
		for (int k = dma_channels.size() - 1; k >= 1; k--) {
			dma_channels[k]->wait_for_transfer();
		}
		buff_id = (buff_id + 1) % TX_BUFFER_COUNT;
	}
	dma_channels[0]->waitForFinish();

	WAIT_FOR_EXIT;

	for (int k = dma_channels.size() - 1; k >= 0; k--) {
		dma_channels[k]->cleanup();
		delete dma_channels[k];
		dma_channels[k] = nullptr;
	}
	axi_dsp_deinit();
	piDeleteSafety(kbd);

	return 0;
}