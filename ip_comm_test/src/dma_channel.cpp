#include "dma_channel.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void dma_channel::save_buf_to_file(void * buffer, int N) {
	piCout << "Saving started for buffer " << PICoutManipulators::PICoutFormat::Hex << buffer;
	const int16_t * buf16 = reinterpret_cast<const int16_t *>(buffer);
	if (dump_file == nullptr) {
		piCout << "ERROR: dump_file is NULL, cannot save";
		return;
	}

	size_t num_int16 = N * 2;
	for (size_t i = 0; i < num_int16; i += 2) {
		if (i + 1 >= num_int16) break;
		fprintf(dump_file, "%04X%04X\n", (uint16_t)buf16[i], (uint16_t)buf16[i + 1]);
	}

	// Flush periodically
	if (ch.counter % 10 == 0) fflush(dump_file);
}

int dma_channel::init(ch_config cfg) {
	// printf("Init started for devnode %s\n", cfg.devnode.c_str());
	ch.buffer_count = cfg.buffer_count; // Add this line!
	ch.buffer_size  = cfg.buffer_size;
	config          = cfg;
	ch.fd           = ::open(cfg.devnode.c_str(), O_RDWR);
	if (ch.fd < 1) {
		printf("Unable to open DMA proxy device file: %s\r", cfg.devnode.c_str());
		return -1;
	}

	ch.buf_ptr = static_cast<channel_contagious_buffer *>(
		mmap(nullptr, (sizeof(channel_buffer) +sizeof(channel_buffer_state)) * ch.buffer_count, PROT_READ | PROT_WRITE, MAP_SHARED, ch.fd, 0));
	if (ch.buf_ptr == MAP_FAILED) {
		ch.buf_ptr = nullptr;
		printf("Memory map failed for DMA buffer devnode %s", cfg.devnode.c_str());
		return -1;
	}

	ch.buffer_size = cfg.buffer_size;
	config         = cfg;

	for (ch.buffer_id = 0; ch.buffer_id < ch.buffer_count; ++ch.buffer_id) {
		ch.buf_ptr->states[ch.buffer_id].length = ch.buffer_size;
	}

	// printf("Init complete for devnode %s\n", config.devnode.c_str());
	return 0;
}

void dma_channel::single_transfer_one_buf() {
	ch.buf_ptr->states[ch.buffer_id].length = ch.buffer_size;
	ioctl(ch.fd, XFER, &ch.buffer_id);
	ch.buffer_id = (ch.buffer_id + 1) % ch.buffer_count;
	ch.counter++;
}

void dma_channel::single_transfer_all_bufs() {
	for (ch.buffer_id = 0; ch.buffer_id < ch.buffer_count; ++ch.buffer_id) {
		ch.buf_ptr->states[ch.buffer_id].length = ch.buffer_size;
		ioctl(ch.fd, XFER, &ch.buffer_id);
		ch.counter++;
	}
}

void dma_channel::start_transfer() {
	ch.buffer_id = ch.counter % ch.buffer_count;
	printf("Start transfer for DMA buffer %d devnode %s\n", ch.buffer_id, config.devnode.c_str());
	ioctl(ch.fd, START_XFER, &ch.buffer_id);
	ch.in_progress_count++;
}

void dma_channel::start_transfer_for_buf(int buffer_id) {
	printf("Start transfer for DMA buffer %d devnode %s\n", buffer_id, config.devnode.c_str());
	ioctl(ch.fd, START_XFER, &buffer_id);
	ch.in_progress_count++;
}

int dma_channel::wait_for_transfer() {
	if (ch.in_progress_count) {
		ioctl(ch.fd, FINISH_XFER, &ch.buffer_id);

		if (ch.buf_ptr->states[ch.buffer_id].status != proxy_status::PROXY_NO_ERROR) {
			printf("DMA transfer error buffer %d, devnode %s, # transfers %d, # completed %d, # in progress %d\n",
			       ch.buffer_id,
			       config.devnode.c_str(),
			       num_transfers,
			       ch.counter,
			       ch.in_progress_count);
			if (ch.buf_ptr->states[ch.buffer_id].status == proxy_status::PROXY_BUSY) {
				fprintf(stderr, "DMA devnode %s busy\n", config.devnode.c_str());
			}
			if (ch.buf_ptr->states[ch.buffer_id].status == proxy_status::PROXY_TIMEOUT) {
				fprintf(stderr, "DMA devnode %s timeout\n", config.devnode.c_str());
			}
			return ch.buf_ptr->states[ch.buffer_id].status;
		}

		if (flag_save_buf) {
			save_buf_to_file(ch.buf_ptr->buffers[ch.buffer_id].buffer, n_samps_per_buf);
		}
		ch.in_progress_count--;
		ch.counter++;
		printf("Finish transfer for DMA buffer %d devnode %s # completed transfers %d\n", ch.buffer_id, config.devnode.c_str(), ch.counter);
	}
	// ch.buffer_id = ch.counter % ch.buffer_count;
	return 0;
}
void dma_channel::cleanup() {
	if (munmap(ch.buf_ptr, sizeof(channel_buffer) * ch.buffer_count) == -1) {
		perror("munmap failed");
	}
	ch.buf_ptr = nullptr;
	close(ch.fd);

	printf("DMA transfer stopped for devnode %s, # transfers %d, # completed %d, # in progress %d\n",
	       config.devnode.c_str(),
	       num_transfers,
	       ch.counter,
	       ch.in_progress_count);
}
