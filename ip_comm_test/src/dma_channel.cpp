#include "dma_channel.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

static save_buf_to_file(void * buffer, int N) {
	const int16_t * buf16 = reinterpret_cast<const int16_t *>(buffer);

	// fprintf(dump_file, "# Buffer %d (seq %d)\n", buffer_id, counter);

	size_t num_int16      = N * 2;
	for (size_t i = 0; i < num_int16; i += 2) {
		if (i + 1 >= num_int16) break;
		fprintf(dump_file, "%04X%04X\n", (uint16_t)buf16[i], (uint16_t)buf16[i + 1]);
	}

	// Flush periodically
	if (counter % 10 == 0) fflush(dump_file);
}

int dma_channel::init(ch_config cfg) {
	ch.fd = ::open(cfg.devnode.c_str(), O_RDWR);
	if (ch.fd < 1) {
		printf("Unable to open DMA proxy device file: %s\r", cfg.devnode.c_str());
		return -1;
	}

	ch.buf_ptr = static_cast<channel_buffer *>(
		mmap(nullptr, sizeof(channel_buffer) * ch.buffer_count, PROT_READ | PROT_WRITE, MAP_SHARED, ch.fd, 0));
	if (ch.buf_ptr == MAP_FAILED) {
		ch.buf_ptr = nullptr;
		printf("Memory map failed for DMA buffer devnode %s", cfg.devnode.c_str());
		return -1;
	}

	ch.buffer_size = cfg.buffer_size;
	config         = cfg;

	return 0;
}

void dma_channel::single_transfer_one_buf() {
	ch.buf_ptr[ch.buffer_id].length = ch.buffer_size;
	ioctl(ch.fd, XFER, &ch.buffer_id);
	ch.buffer_id = (ch.buffer_id + 1) % ch.buffer_count;
}

void dma_channel::single_transfer_all_bufs() {
	for (ch.buffer_id = 0; ch.buffer_id < ch.buffer_count; ++ch.buffer_id) {
		ch.buf_ptr[ch.buffer_id].length = ch.buffer_size;
		ioctl(ch.fd, XFER, &ch.buffer_id);
	}
}

void dma_channel::begin() override {
	printf("Start transmit\n");

	for (ch.buffer_id = 0; ch.buffer_id < ch.buffer_count; ++ch.buffer_id) {
		ch.buf_ptr[ch.buffer_id].length = ch.buffer_size;

		ioctl(ch.fd, START_XFER, &ch.buffer_id);
		printf("Start transfer for DMA buffer %d devnode %s\n", ch.buffer_id, config.devnode.c_str());
		ch.in_progress_count++;
		if (num_transfers && (++in_progress_count >= num_transfers)) break;
	}

	ch.buffer_id = 0;
}

void dma_channel::run() override {
	if (ch.in_progress_count) {
		ioctl(ch.fd, FINISH_XFER, &ch.buffer_id);

		if (ch.buf_ptr[ch.buffer_id].status != channel_buffer::proxy_status::PROXY_NO_ERROR) {
			printf("DMA transfer error devnode %s, # transfers %d, # completed %d, # in progress %d\n",
			       config.devnode.c_str(),
			       num_transfers,
			       ch.counter,
			       ch.in_progress_count);
			if (ch.buf_ptr[ch.buffer_id].status == channel_buffer::proxy_status::PROXY_BUSY) {
				fprintf(stderr, "DMA devnode %s busy\n", config.devnode.c_str());
			}
			if (ch.buf_ptr[ch.buffer_id].status == channel_buffer::proxy_status::PROXY_TIMEOUT) {
				fprintf(stderr, "DMA devnode %s timeout\n", config.devnode.c_str());
			}
			return;
		}

		if (flag_save_buf) {
			save_buf_to_file(&ch.buf_ptr[buffer_id].buffer, n_samps_per_buf);
		}
		ch.in_progress_count--;
		ch.counter++;
	}

	if (num_transfers && (ch.counter >= num_transfers)) return;

	if (num_transfers && ((ch.counter + ch.in_progress_count) < num_transfers)) {
		ioctl(ch.fd, START_XFER, &ch.buffer_id);
		ch.in_progress_count++;
	}
	ch.buffer_id = (ch.buffer_id + 1) % ch.buffer_count;
}

void dma_channel::cleanup() {
	munmap(ch.buf_ptr, sizeof(channel_buffer));
	ch.buf_ptr = nullptr;
	close(ch.fd);

	printf("DMA transfer stopped for devnode %s, # transfers %d, # completed %d, # in progress %d\n",
	       config.devnode.c_str(),
	       num_transfers,
	       ch.counter,
	       ch.in_progress_count);
}

void dma_channel::end() override {
	cleanup();
}
