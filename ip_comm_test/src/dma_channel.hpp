#pragma once

#include "dma-proxy.h"

#include <cstdint>
#include <piprotectedvariable.h>
#include <pisemaphore.h>
#include <pithread.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>


#define N_SAMPS_IN_TX_BUF 232
#define N_PACKS_IN_TX_BUF 4
#define HDR_SIZE 6

class dma_channel: public PIThread {
	PIOBJECT_SUBCLASS(dma_channel, PIThread)

private:
	struct channel {
		channel_contagious_buffer * buf_ptr = nullptr; // proxy‑driver ring
		int fd                   = -1;
		int buffer_size          = 0;
		int counter              = 0;
		int buffer_id            = 0;
		int in_progress_count    = 0;
		int buffer_count         = 1;
	} ch;

	int num_transfers  = 0;
	bool flag_save_buf = false;
	FILE * dump_file;
	int n_samps_per_buf = 232;

	void save_buf_to_file(void * buffer, int N);

public:
	struct ch_config {
		std::string devnode;
		int buffer_size;
		int buffer_count;
	} config;

	int init(ch_config cfg);
	void cleanup();
	void single_transfer_one_buf();
	void single_transfer_all_bufs();
	void start_transfer();
	void start_transfer_for_buf(int buffer_id);
	int wait_for_transfer();

	void begin() override {
		printf("Start DMA devnode %s\n", config.devnode.c_str());

		for (ch.buffer_id = 0; ch.buffer_id < ch.buffer_count; ++ch.buffer_id) {
			ch.buf_ptr->states[ch.buffer_id].length = ch.buffer_size;
			start_transfer_for_buf(ch.buffer_id);
			if (num_transfers && (ch.in_progress_count >= num_transfers)) break;
		}

		ch.buffer_id = 0;
	}

	void run() override {
		if (wait_for_transfer() || (num_transfers && (ch.counter >= num_transfers))) {
			stop();
			return;
		}

		if (num_transfers && ((ch.counter + ch.in_progress_count) < num_transfers)) {
			start_transfer_for_buf(ch.buffer_id);
			piCout << "Started transfer for buffer " << ch.buffer_id << "global cnt is " << ch.buffer_count;
		}
		ch.buffer_id = (ch.buffer_id + 1) % ch.buffer_count;
	}

	void end() override { cleanup(); }


	void * get_buffer(size_t num) const { return (void *)&ch.buf_ptr->buffers[num].buffer; }

	void get_all_buffers(void ** buffer_array) const {
		for (size_t i = 0; i < ch.buffer_count; ++i) {
			buffer_array[i] = static_cast<void *>(&ch.buf_ptr->buffers[i].buffer);
		}
	}

	void set_num_transfers(int n_trans) { num_transfers = n_trans; }

	void set_save_to_file(PIString f_name, int n_samps) {
		flag_save_buf    = true;
		n_samps_per_buf  = n_samps;
		dump_file = fopen(f_name.data(), "w");
	}
};