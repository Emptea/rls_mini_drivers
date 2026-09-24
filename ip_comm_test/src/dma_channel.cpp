#include "dma_channel.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void print_work(void * data) {
	struct work_posthdr * work = (struct work_posthdr *)data;

	piCout << "Packet Number" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << work->packet_number;
	piCout << "Number of detections" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << work->n_work_packets;

	const struct work_packet * packets = reinterpret_cast<const struct work_packet *>(work + 1);
	for (uint32_t i = 0; i < work->n_work_packets; ++i) {
		const struct work_packet & packet = packets[i];
		piCout << PICoutManipulators::PICoutSpecialChar::NewLine;
		piCout << "Work packet" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
			   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << i + 1;
		piCout << "Range" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
			   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
			   << PICoutManipulators::PICoutSpecialChar::Tab << static_cast<unsigned int>(packet.range);
		piCout << "Main amplitude at sample" << packet.main_diagram_number << PICoutManipulators::PICoutSpecialChar::Tab
			   << PICoutManipulators::PICoutSpecialChar::Tab << packet.main_amplitude;
		piCout << "Neighbour amplitude at sample" << packet.main_diagram_number - 1 + 2 * packet.neighbor_diagram_side
			   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << packet.neighbor_amplitude;
		piCout << "Frequency channel" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
			   << PICoutManipulators::PICoutSpecialChar::Tab << packet.frequency_channel;
		piCout << "Ranker output" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
			   << PICoutManipulators::PICoutSpecialChar::Tab << packet.rank_out;
	}
	piCout << PICoutManipulators::PICoutSpecialChar::NewLine;
}

static void print_hdr(void * data) {
	struct header * hdr = (struct header *)data;

	PICout(PICoutManipulators::AddNone) << "Delimiter" << PICoutManipulators::PICoutSpecialChar::Tab
										<< PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
										<< PICoutManipulators::PICoutSpecialChar::Tab << " 0x" << PICoutManipulators::PICoutFormat::Hex
										<< hdr->del_high << "_" << hdr->del_low << PICoutManipulators::PICoutSpecialChar::NewLine;
	piCout << "Packet Number" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << hdr->packet_number;
	piCout << "Timestamp" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << hdr->timestamp;
	piCout << "Channel" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << hdr->channel;
	piCout << "Range gate" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << hdr->range;
	piCout << "Test point" << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab
		   << PICoutManipulators::PICoutSpecialChar::Tab << PICoutManipulators::PICoutSpecialChar::Tab << hdr->tp
		   << PICoutManipulators::PICoutSpecialChar::NewLine;

	if (hdr->tp == TP_WORK) {
		print_work((uint32_t *)data + HDR_SIZE);
	}
}

void dma_channel::save_buf_to_file(void * buffer, int N) {
	// piCout << "Saving started for buffer " << PICoutManipulators::PICoutFormat::Hex << buffer;
	// const int16_t * buf16 = reinterpret_cast<const int16_t *>(buffer);
	const uint32_t * buf32 = reinterpret_cast<const uint32_t *>(buffer);
	if (dump_file == nullptr) {
		piCout << "ERROR: dump_file is NULL, cannot save";
		return;
	}

	// size_t num_int16 = N * 2;
	// for (size_t i = 0; i < num_int16; i += 2) {
	// 	if (i + 1 >= num_int16) break;
	// 	fprintf(dump_file, "%04X%04X\n", (uint16_t)buf16[i + 1], (uint16_t)buf16[i]);
	// }
	for (size_t i = 0; i < N; i++) {
		fprintf(dump_file, "%08X\n", buf32[i]);
	}


	// Flush periodically
	if (ch.counter % 10 == 0) fflush(dump_file);
}

int dma_channel::init(ch_config cfg) {
	printf("Init started for devnode %s\n", cfg.devnode.c_str());
	ch.buffer_count = cfg.buffer_count; // Add this line!
	ch.buffer_size  = cfg.buffer_size;
	config          = cfg;
	ch.fd           = ::open(cfg.devnode.c_str(), O_RDWR);
	if (ch.fd < 1) {
		printf("Unable to open DMA proxy device file: %s\r", cfg.devnode.c_str());
		return -1;
	}
	ch.buf_ptr = static_cast<channel_contagious_buffer *>(
		mmap(nullptr, sizeof(channel_contagious_buffer), PROT_READ | PROT_WRITE, MAP_SHARED, ch.fd, 0));
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

	printf("Init complete for devnode %s\n", config.devnode.c_str());
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
	// printf("Start transfer for DMA buffer %d devnode %s\n", ch.buffer_id, config.devnode.c_str());
	ioctl(ch.fd, START_XFER, &ch.buffer_id);
	ch.in_progress_count++;
}

int dma_channel::start_transfer_for_buf(int buffer_id) {
	// printf("Start transfer for DMA buffer %d devnode %s\n", buffer_id, config.devnode.c_str());
	int ret = ioctl(ch.fd, START_XFER, &buffer_id);
	if (ret < 0) {
		perror("START_XFER");
		return -1;
	}

	ch.in_progress_count++;
	return 0;
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
			auto * buffer       = ch.buf_ptr->buffers[ch.buffer_id].buffer;
			const auto * hdr    = reinterpret_cast<const struct header *>(buffer);
			int n_samps_to_save = n_samps_per_buf;
			if (hdr->tp == TP_WORK) {
				const auto * work = reinterpret_cast<const struct work_posthdr *>(reinterpret_cast<const uint32_t *>(buffer) + HDR_SIZE);
				// save_buf_to_file expects a count of 32-bit words.
				n_samps_to_save   = HDR_SIZE + (sizeof(work_posthdr) + work->n_work_packets * sizeof(work_packet)) / sizeof(uint32_t);
			}
			// print_hdr(buffer);
			dataQueue.emplace(buffer, buffer + n_samps_to_save);
			// save_buf_to_file(buffer, n_samps_to_save);
		}
		ch.in_progress_count--;
		ch.counter++;
		// printf("Finish transfer for DMA buffer %d devnode %s # completed transfers %d\n", ch.buffer_id, config.devnode.c_str(),
		// ch.counter);
	}
	// ch.buffer_id = ch.counter % ch.buffer_count;
	return 0;
}

int dma_channel::wait_for_transfer(int buffer_id) {
	int ret = ioctl(ch.fd, FINISH_XFER, &buffer_id);

	if (ret < 0) {
		perror("FINISH_XFER");
		return -1;
	}

	const auto status = ch.buf_ptr->states[buffer_id].status;
	--ch.in_progress_count;

	if (status != proxy_status::PROXY_NO_ERROR) {
		fprintf(stderr, "DMA transfer error: buffer=%d dev=%s status=%d\n", buffer_id, config.devnode.c_str(), status);

		return status;
	}

	++ch.counter;

	// Для RX оставляем вашу обработку
	if (flag_save_buf) {
		auto * buffer       = ch.buf_ptr->buffers[buffer_id].buffer;

		const auto * hdr    = reinterpret_cast<const struct header *>(buffer);

		int n_samps_to_save = n_samps_per_buf;

		if (hdr->tp == TP_WORK) {
			const auto * work = reinterpret_cast<const struct work_posthdr *>(reinterpret_cast<const uint32_t *>(buffer) + HDR_SIZE);

			n_samps_to_save   = HDR_SIZE + (sizeof(work_posthdr) + work->n_work_packets * sizeof(work_packet)) / sizeof(uint32_t);
		}

		dataQueue.emplace(buffer, buffer + n_samps_to_save);
	}


	return 0;
}

void dma_channel::cleanup() {
	if (munmap(ch.buf_ptr, sizeof(channel_buffer) * ch.buffer_count) == -1) {
		perror("munmap failed");
	}
	ch.buf_ptr = nullptr;
	close(ch.fd);

	printf("DMA stopped %s, # transfers %d, # completed %d, # in progress %d\n",
	       config.devnode.c_str(),
	       num_transfers,
	       ch.counter,
	       ch.in_progress_count);

	if (flag_save_buf) {
		while (!dataQueue.empty()) {
			auto & samples = dataQueue.front();

			save_buf_to_file(samples.data(), static_cast<int>(samples.size()));

			dataQueue.pop();
		}
	}
}
