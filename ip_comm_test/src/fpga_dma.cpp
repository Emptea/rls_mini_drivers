#include "fpga_dma.hpp"

#include <cstring>
#include <stdio.h>

int fpga_dma::init(const config & cfg) {
	tx_buf_size    = cfg.tx_buf_size;
	rx_buf_size    = cfg.rx_buf_size;
	rx_buf_count   = cfg.rx_buf_count;
	submitted      = 0;
	completed      = 0;
	last_rx_buf_id = -1;

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
