#pragma once

#include "dma-proxy.h"

#include <cstdint>
#include <piprotectedvariable.h>
#include <pisemaphore.h>
#include <pithread.h>
#include <string>

#define N_SAMPS_IN_TX_BUF 232
#define N_PACKS_IN_TX_BUF 4

class dma_channel: public PIThread {
	PIOBJECT_SUBCLASS(dma_channel, PIThread)

private:
	struct channel {
		channel_buffer * buf_ptr = nullptr; // proxy‑driver ring
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

	void * get_buffer(size_t num) const { return ch.buf_ptr[num].buffer; }

	void get_all_buffers(void ** buffer_array) const {
		for (size_t i = 0; i < ch.buffer_count; ++i) {
			buffer_array[i] = &ch.buf_ptr[i].buffer;
		}
	}

	void set_num_transfers(int n_trans) { num_transfers = n_trans; }

	void set_save_to_file(PIString f_name, int n_samps) {
		flag_save_buf    = true;
		n_samps_per_buf  = n_samps;
		FILE * dump_file = fopen(f_name.data(), "w");
	}
};