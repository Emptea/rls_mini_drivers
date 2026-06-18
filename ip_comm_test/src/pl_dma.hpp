#pragma once

#include "dma-proxy.h"

#include <cstdint>
#include <piprotectedvariable.h>
#include <pithread.h>
#include <string>
#include <vector>

class pl_dma: public PIObject {
	PIOBJECT(pl_dma)

protected:
	PIThread tx_thread;
	PIThread rx_thread;

private:
	struct channel {
		channel_buffer * buf_ptr = nullptr; // proxy‑driver ring
		int fd                   = -1;
		int buffer_size          = 0;
		int num_transfers        = 0;
		uint8_t * user_buffer    = nullptr;
		pthread_t tid            = 0;
		int stop;
	};

	struct {
		int rx_errs = 0;
		int tx_errs = 0;
	} errors_cnt;

	std::vector<channel> m_tx_ch;
	std::vector<channel> m_rx_ch;

	uint64_t m_start_time = 0;
	uint64_t m_end_time   = 0;

	int init_num_transfers     = 0;
	int num_transfers     = 0;
	bool stop_in_progress = false;

	int setup_threads();
	void cleanup_channels();
	void transmit();
	void receive();

public:
	struct Stats {
		uint64_t usec  = 0;
		int mb_per_sec = 0;
	};

	struct ch_config {
		std::string devnode;
		int buffer_size;
	} config;

	pl_dma() = default;
	~pl_dma();

	pl_dma(const pl_dma &)             = delete;
	pl_dma & operator=(const pl_dma &) = delete;

	pl_dma(pl_dma && other) noexcept;
	pl_dma & operator=(pl_dma && other) noexcept;

	/**
	 * Initialize this instance for multiple TX/RX channels with user‑provided buffers.
	 *
	 * Note: your buffers must be mmap'able / coherent as expected by the dma_proxy driver.
	 */
	int init(const std::vector<ch_config> & tx_channels, const std::vector<ch_config> & rx_channels);
	int init(const std::vector<ch_config> & tx_channels);

	int start();
	void stop();
	Stats get_stats() const;
	void cleanup();


	void * get_tx_buffer(size_t num) const {
		if (!m_tx_ch.size()) return nullptr;
		return &m_tx_ch[0].buf_ptr[num].buffer;
	}

	void * get_rx_buffer(size_t num) const {
		if (!m_rx_ch.size()) return nullptr;
		return &m_rx_ch[0].buf_ptr[num].buffer;
	}
	size_t get_rx_buffer_size() const {
		if (m_rx_ch.size()) return m_rx_ch[0].buffer_size;
		return 0;
	}

	size_t get_tx_buffer_size() const {
		if (m_tx_ch.size()) return m_tx_ch[0].buffer_size;
		return 0;
	}

	void stop_transfer() {
		for (auto & ch: m_tx_ch) {
			ch.stop = 1;
		}
	}

	void stop_receive() {
		for (auto & ch: m_rx_ch) {
			ch.stop = 1;
		}
	}

	void set_num_transfers(int n_trans) { num_transfers = n_trans; }
};