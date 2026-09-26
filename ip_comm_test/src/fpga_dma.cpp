#include "fpga_dma.hpp"
#include "rls_cfg.hpp"

#include <cstring>
#include <stdio.h>

#define RX_DEV            "/dev/dma_proxy_rx"
#define TX_DEV_CH0        "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1        "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH2        "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH3        "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH4        "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH5        "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH6        "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH7        "/dev/dma_proxy_tx_ch7"

constexpr size_t RX_PIPELINE_DEPTH = 8;

int fpga_dma::init() {
	config cfg;

	cfg.rx_devnode     = RX_DEV;

	cfg.tx_devnodes[0] = TX_DEV_CH0;
	cfg.tx_devnodes[1] = TX_DEV_CH1;
	cfg.tx_devnodes[2] = TX_DEV_CH2;
	cfg.tx_devnodes[3] = TX_DEV_CH3;
	cfg.tx_devnodes[4] = TX_DEV_CH4;
	cfg.tx_devnodes[5] = TX_DEV_CH5;
	cfg.tx_devnodes[6] = TX_DEV_CH6;
	cfg.tx_devnodes[7] = TX_DEV_CH7;

	cfg.tx_buf_size    = rls_cfg::TX_BUF_SIZE;
	cfg.rx_buf_size    = BUFFER_SIZE;
	cfg.rx_buf_count   = RX_BUFFER_COUNT;
 
	tx_buf_size            = cfg.tx_buf_size;
	rx_buf_size            = cfg.rx_buf_size;
	rx_buf_count           = cfg.rx_buf_count;
	submitted              = 0;
	completed              = 0;
	last_rx_buf_id         = -1;

	dma_channel::ch_config rx_cfg;

	rx_cfg.devnode   = cfg.rx_devnode;
	rx_cfg.buf_size  = cfg.rx_buf_size;
	rx_cfg.buf_count = cfg.rx_buf_count;

	if (rx_channel.init(rx_cfg) != 0) {
		fprintf(stderr, "Failed to initialize RX DMA channel\n");
		return -1;
	}

	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		dma_channel::ch_config tx_cfg;

		tx_cfg.devnode   = cfg.tx_devnodes[ch];
		tx_cfg.buf_size  = cfg.tx_buf_size;
		tx_cfg.buf_count = 1;

		if (tx_channels[ch].init(tx_cfg) != 0) {
			fprintf(stderr, "Failed to initialize TX DMA channel %d\n", ch);
			return -1;
		}
	}

	return 0;
}

bool fpga_dma::can_send() const {
	return submitted - completed < static_cast<size_t>(rx_buf_count);
}

int fpga_dma::send(const void * data[NUM_TX_CHANNELS]) {
	const int rx_buf_id = static_cast<int>(submitted % rx_buf_count);

	rx_channel.start_transfer(rx_buf_id);

	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		void * tx_buffer = tx_channels[ch].get_buffer(0);

		memcpy(tx_buffer, data[ch], tx_buf_size);
	}

	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		tx_channels[ch].start_transfer(0);
	}

	int result = 0;

	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		int ret = tx_channels[ch].wait_for_transfer(0);

		if (ret != 0) {
			fprintf(stderr, "TX ERROR ch=%d transaction=%zu ret=%d\n", ch, submitted, ret);

			result = ret;
		}
	}

	++submitted;

	return result;
}

int fpga_dma::send() {
	const int rx_buf_id = static_cast<int>(submitted % rx_buf_count);

	rx_channel.start_transfer(rx_buf_id);

	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		tx_channels[ch].start_transfer(0);
	}

	int result = 0;

	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		int ret = tx_channels[ch].wait_for_transfer(0);

		if (ret != 0) {
			fprintf(stderr, "TX ERROR ch=%d transaction=%zu ret=%d\n", ch, submitted, ret);

			result = ret;
		}
	}

	++submitted;

	return result;
}

int fpga_dma::receive() {
	if (completed >= submitted) {
		return -1;
	}

	const int rx_buf_id = static_cast<int>(completed % rx_buf_count);

	int ret             = rx_channel.wait_for_transfer(rx_buf_id);

	if (ret != 0) {
		fprintf(stderr,
		        "RX ERROR transaction=%zu buf=%d "
		        "sent=%zu received=%zu\n",
		        completed,
		        rx_buf_id,
		        submitted,
		        completed);

		return ret;
	}

	last_rx_buf_id = rx_buf_id;

	++completed;

	return 0;
}

void * fpga_dma::get_rx_buffer() {
	if (last_rx_buf_id < 0) {
		return nullptr;
	}

	return rx_channel.get_buffer(last_rx_buf_id);
}

void * fpga_dma::get_tx_buffer(int ch) {
	return tx_channels[ch].get_buffer(0);
}

size_t fpga_dma::get_submitted() const {
	return submitted;
}

size_t fpga_dma::get_completed() const {
	return completed;
}

void fpga_dma::cleanup() {
	for (int ch = 0; ch < NUM_TX_CHANNELS; ++ch) {
		tx_channels[ch].cleanup();
	}

	rx_channel.cleanup();
}
