#include "pl_dma.hpp"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

uint64_t get_posix_clock_time_usec() {
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		return uint64_t(ts.tv_sec) * 1000000ULL + ts.tv_nsec / 1000;
	else
		return 0ULL;
}

pl_dma::pl_dma(pl_dma && other) noexcept
	: m_tx_ch(std::move(other.m_tx_ch))
	, m_rx_ch(std::move(other.m_rx_ch))
	, m_start_time(other.m_start_time)
	, m_end_time(other.m_end_time) {
	other.m_start_time = 0;
	other.m_end_time   = 0;
	tx_thread.setPriority(PIThread::Priority::piLow);
}

pl_dma::~pl_dma() {}

pl_dma & pl_dma::operator=(pl_dma && other) noexcept {
	if (this != &other) {
		cleanup();
		m_tx_ch            = std::move(other.m_tx_ch);
		m_rx_ch            = std::move(other.m_rx_ch);
		m_start_time       = other.m_start_time;
		m_end_time         = other.m_end_time;
		other.m_start_time = 0;
		other.m_end_time   = 0;
	}
	return *this;
}

int pl_dma::init(const std::vector<ch_config> & tx_channels, const std::vector<ch_config> & rx_channels) {
	cleanup();

	m_tx_ch.resize(tx_channels.size());
	for (size_t i = 0; i < tx_channels.size(); ++i) {
		auto & ch        = m_tx_ch[i];
		const auto & cfg = tx_channels[i];

		ch.fd            = ::open(cfg.devnode.c_str(), O_RDWR);
		if (ch.fd < 1) {
			printf("Unable to open DMA proxy device file: %s\r", cfg.devnode.c_str());
			return -1;
		}

		ch.buf_ptr = static_cast<channel_buffer *>(
			mmap(nullptr, sizeof(channel_buffer) * TX_BUFFER_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, ch.fd, 0));
		if (ch.buf_ptr == MAP_FAILED) {
			ch.buf_ptr = nullptr;
			printf("Memory map failed for tx buffer");
			return -1;
		}

		ch.buffer_size = cfg.buffer_size;
		config = cfg;
	}

	m_rx_ch.resize(rx_channels.size());
	for (size_t i = 0; i < rx_channels.size(); ++i) {
		auto & ch        = m_rx_ch[i];
		const auto & cfg = rx_channels[i];

		ch.fd            = ::open(cfg.devnode.c_str(), O_RDWR);
		if (ch.fd < 1) {
			printf("Unable to open DMA proxy device file: %s\r", cfg.devnode.c_str());
			return -1;
		}

		ch.buf_ptr = static_cast<channel_buffer *>(
			mmap(nullptr, sizeof(channel_buffer) * RX_BUFFER_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, ch.fd, 0));
		if (ch.buf_ptr == MAP_FAILED) {
			ch.buf_ptr = nullptr;
			printf("Memory map failed for rx buffer");
			return -1;
		}

		ch.buffer_size = cfg.buffer_size;
	}

	return 0;
}

int pl_dma::init(const std::vector<ch_config> & tx_channels) {
	std::vector<ch_config> empty_rx; // no RX channels
	return init(tx_channels, empty_rx);
}

void pl_dma::transmit() {
	printf("Start transmit\n");

	auto channel_ptr = static_cast<channel *>(&m_tx_ch[0]);

	int counter = 0, buffer_id, in_progress_count = 0;

	for (buffer_id = 0; buffer_id < TX_BUFFER_COUNT; ++buffer_id) {
		channel_ptr->buf_ptr[buffer_id].length = channel_ptr->buffer_size;

		ioctl(channel_ptr->fd, START_XFER, &buffer_id);
		printf("Start transfer for tx buffer %d\n", buffer_id);
		in_progress_count++;
		// if (++in_progress_count >= num_transfers) break;
	}

	buffer_id = 0;
	while (true) {
		if (in_progress_count) {
			ioctl(channel_ptr->fd, FINISH_XFER, &buffer_id);
			
			if (channel_ptr->buf_ptr[buffer_id].status != channel_buffer::proxy_status::PROXY_NO_ERROR) {
				printf("Proxy tx transfer error, # transfers %d, # completed %d, # in progress %d\n",
				       num_transfers,
				       counter,
				       in_progress_count);
				if (channel_ptr->buf_ptr[buffer_id].status == channel_buffer::proxy_status::PROXY_BUSY) {
					fprintf(stderr, "Proxy tx busy\n");
				}
				if (channel_ptr->buf_ptr[buffer_id].status == channel_buffer::proxy_status::PROXY_TIMEOUT) {
					fprintf(stderr, "Proxy tx timeout\n");
				}
				errors_cnt.tx_errs++;
				break;
			}

			in_progress_count--;
			counter++;
		}

		if (stop_in_progress && (counter >= num_transfers)) break;


		if (channel_ptr->stop && !stop_in_progress) {
			stop_in_progress = true;
			num_transfers    = counter + RX_BUFFER_COUNT;
			printf("Stop in progress, num_transfers = %d\n", num_transfers);
		}

		if (!channel_ptr->stop || ((counter + in_progress_count) < num_transfers)) {
			/* Restart the completed channel buffer to start another transfer and keep
			 * track of the number of transfers in progress
			 */
			ioctl(channel_ptr->fd, START_XFER, &buffer_id);
			in_progress_count++;
			// if (stop_in_progress) printf("Tx counter + in progress: %d, num_transfers %d\n", counter + in_progress_count, num_transfers);
		}
		buffer_id = (buffer_id + 1) % TX_BUFFER_COUNT;
	}
	printf("Proxy tx transfer stopped for devnode %s, # transfers %d, # completed %d, # in progress %d\n", config.devnode.c_str(), num_transfers, counter, in_progress_count);
}

void pl_dma::receive() {
	printf("Start receive\n");
	auto channel_ptr      = static_cast<channel *>(&m_rx_ch[0]);
	int in_progress_count = 0, buffer_id = 0;
	int counter = 0;

	// Start all RX buffers
	for (buffer_id = 0; buffer_id < RX_BUFFER_COUNT; ++buffer_id) {
		channel_ptr->buf_ptr[buffer_id].length = channel_ptr->buffer_size;
		ioctl(channel_ptr->fd, START_XFER, &buffer_id);
		printf("Start transfer for rx buffer %d\n", buffer_id);
		in_progress_count++;
		// if (++in_progress_count >= num_transfers) break;
	}

	buffer_id = 0;
	while (true) {
		if (in_progress_count) {
			ioctl(channel_ptr->fd, FINISH_XFER, &buffer_id);

			if (channel_ptr->buf_ptr[buffer_id].status != channel_buffer::proxy_status::PROXY_NO_ERROR) {
				printf("Proxy rx transfer error, # transfers %d, # completed %d, # in progress %d\n",
				       num_transfers,
				       counter,
				       in_progress_count);
				fprintf(stderr, "Proxy rx transfer error\n");
				if (channel_ptr->buf_ptr[buffer_id].status == channel_buffer::proxy_status::PROXY_BUSY) {
					fprintf(stderr, "Proxy rx busy\n");
				}
				if (channel_ptr->buf_ptr[buffer_id].status == channel_buffer::proxy_status::PROXY_TIMEOUT) {
					fprintf(stderr, "Proxy rx timeout\n");
				}
				errors_cnt.rx_errs++;
				break;
			} else {
				in_progress_count--;
				counter++;
			}
		}

		if (stop_in_progress && (counter >= num_transfers)) break;

		/* If the ones in progress will complete the number of transfers then don't start more
		 * but finish the ones that are already started
		 */
		if (!stop_in_progress || ((counter + in_progress_count) < num_transfers)) {
			ioctl(channel_ptr->fd, START_XFER, &buffer_id);
			in_progress_count++;
			// if (stop_in_progress) printf("Rx counter + in progress: %d, num_transfers %d\n", counter + in_progress_count, num_transfers);
		}


		buffer_id = (buffer_id + 1) % RX_BUFFER_COUNT;
	}
	printf("Proxy rx transfer stopped, # transfers %d, # completed %d, # in progress %d\n", num_transfers, counter, in_progress_count);
}

int pl_dma::setup_threads() {
	if (m_rx_ch.size()) rx_thread.startOnce([this]() { receive(); });
	tx_thread.startOnce([this]() { transmit(); });
	printf("Created %ld tx threads and %ld rx threads\n", m_tx_ch.size(), m_rx_ch.size());
	return 0;
}

int pl_dma::start() {
	m_start_time = get_posix_clock_time_usec();
	return setup_threads();
}

void pl_dma::stop() {
	if (m_rx_ch.size()) rx_thread.stopAndWait();
	tx_thread.stopAndWait();

	m_end_time = get_posix_clock_time_usec();
}

pl_dma::Stats pl_dma::get_stats() const {
	Stats s;
	s.usec = m_end_time - m_start_time;

	if (s.usec == 0) return s;

	int total_buffers = 0;
	for (const auto & ch: m_tx_ch)
		total_buffers += ch.num_transfers;
	for (const auto & ch: m_rx_ch)
		total_buffers += ch.num_transfers;

	double total_bytes = 0;
	for (const auto & ch: m_tx_ch)
		total_bytes += ch.num_transfers * ch.buffer_size;
	for (const auto & ch: m_rx_ch)
		total_bytes += ch.num_transfers * ch.buffer_size;

	double mb         = total_bytes / 1000000.;
	double mb_per_sec = (1000000. / s.usec) * mb;
	s.mb_per_sec      = int(mb_per_sec);

	printf("Time: %ld microseconds\n", s.usec);
	printf("Transfer size: %lld KB\n", (long long)(total_bytes / 1024));
	printf("Throughput: %d MB / sec \n", s.mb_per_sec);


	return s;
}

void pl_dma::cleanup_channels() {
	for (auto & ch: m_tx_ch) {
		munmap(ch.buf_ptr, sizeof(channel_buffer));
		ch.buf_ptr = nullptr;
		close(ch.fd);
	}

	for (auto & ch: m_rx_ch) {
		munmap(ch.buf_ptr, sizeof(channel_buffer));
		ch.buf_ptr = nullptr;
		close(ch.fd);
	}
}

void pl_dma::cleanup() {
	cleanup_channels();
	m_tx_ch.clear();
	m_rx_ch.clear();
	m_start_time = 0;
	m_end_time   = 0;
}