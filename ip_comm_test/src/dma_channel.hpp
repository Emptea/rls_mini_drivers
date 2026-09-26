#pragma once

#include "dma-proxy.h"

#include <string>

class dma_channel {
public:
	struct ch_config {
		std::string devnode;
		int buf_size  = 0;
		int buf_count = 1;
	};

	int init(const ch_config & cfg);
	int start_transfer(int buf_id);
	int wait_for_transfer(int buf_id);
	void * get_buffer(int buf_id);
	void cleanup();

private:
	struct channel {
		channel_contagious_buffer * buf_ptr = nullptr; // proxy‑driver ring
		int fd                              = -1;
		int buf_size                        = 0;
		int buf_count                       = 1;
		int counter                         = 0;
		int in_progress_count               = 0;
	};

	channel ch;
	ch_config config;
};
