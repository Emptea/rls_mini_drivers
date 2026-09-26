#include "dma_channel.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int dma_channel::init(const ch_config & cfg) {
	printf("Init started for devnode %s\n", cfg.devnode.c_str());
	ch.buf_count = cfg.buf_count;
	ch.buf_size  = cfg.buf_size;
	config       = cfg;
	ch.fd        = ::open(cfg.devnode.c_str(), O_RDWR);
	if (ch.fd < 0) {
		printf("Unable to open DMA proxy device file: %s\r", cfg.devnode.c_str());
		return -1;
	}
	ch.buf_ptr = static_cast<channel_contagious_buffer *>(
		mmap(nullptr, sizeof(channel_contagious_buffer), PROT_READ | PROT_WRITE, MAP_SHARED, ch.fd, 0));
	if (ch.buf_ptr == MAP_FAILED) {
		ch.buf_ptr = nullptr;
		printf("Memory map failed for DMA buffer devnode %s", cfg.devnode.c_str());
		close(ch.fd);
		ch.fd = -1;
		return -1;
	}

	for (int id = 0; id < ch.buf_count; ++id) {
		ch.buf_ptr->states[id].length = ch.buf_size;
	}

	printf("Init complete for devnode %s\n", config.devnode.c_str());
	return 0;
}

int dma_channel::start_transfer(int buf_id) {
	// printf("Start transfer for DMA buffer %d devnode %s\n", buf_id, config.devnode.c_str());
	int ret = ioctl(ch.fd, START_XFER, &buf_id);
	if (ret < 0) {
		perror("START_XFER");
		return -1;
	}

	ch.in_progress_count++;
	return 0;
}

int dma_channel::wait_for_transfer(int buf_id) {
	int ret = ioctl(ch.fd, FINISH_XFER, &buf_id);

	if (ret < 0) {
		perror("FINISH_XFER");
		return -1;
	}

	const auto status = ch.buf_ptr->states[buf_id].status;
	if (ch.in_progress_count > 0) {
		--ch.in_progress_count;
	}

	if (status != proxy_status::PROXY_NO_ERROR) {
		fprintf(stderr, "DMA transfer error: buffer=%d dev=%s status=%d\n", buf_id, config.devnode.c_str(), status);

		return status;
	}

	++ch.counter;
	return 0;
}

void * dma_channel::get_buffer(int buf_id) {
	return ch.buf_ptr->buffers[buf_id].buffer;
}

void dma_channel::cleanup() {
	if (ch.buf_ptr != nullptr) {
		if (munmap(ch.buf_ptr, sizeof(channel_contagious_buffer)) == -1) {
			perror("munmap failed");
		}

		ch.buf_ptr = nullptr;
	}

	if (ch.fd >= 0) {
		close(ch.fd);
		ch.fd = -1;
	}

	printf("DMA stopped %s, # completed %d, # in progress %d\n", config.devnode.c_str(), ch.counter, ch.in_progress_count);
}
