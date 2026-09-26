#pragma once

#include "dma_channel.hpp"

#include <cstddef>
#include <string>

class fpga_dma {
public:
	static constexpr int NUM_TX_CHANNELS = 8;

	struct config {
		std::string rx_devnode;
		std::string tx_devnodes[NUM_TX_CHANNELS];

		int tx_buf_size;
		int rx_buf_size;

		int rx_buf_count;
	};

	int init(const config & cfg);
	bool can_send() const;
	int send(const void * data[NUM_TX_CHANNELS]);
	int send();
	int receive();
	void * get_rx_buffer();
	void * get_tx_buffer(int ch);
	void cleanup();
	size_t get_submitted() const;
	size_t get_completed() const;

private:
	dma_channel rx_channel;
	dma_channel tx_channels[NUM_TX_CHANNELS];

	int tx_buf_size    = 0;
	int rx_buf_size    = 0;
	int rx_buf_count   = 0;

	size_t submitted   = 0;
	size_t completed   = 0;

	int last_rx_buf_id = -1;
};