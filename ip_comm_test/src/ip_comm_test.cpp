#include "axi_dsp.h"
#include "fpga_dma.hpp"
#include "misc.h"
#include "picout.h"
#include "rls.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <new>
#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>
#include <pistring_std.h>
#include <queue>
#include <stdio.h>
#include <vector>

namespace fs = std::filesystem;

static void save_buf_to_file(FILE * file, const void * buffer, int n) {
	if (file == nullptr) {
		piCout << "ERROR: dump file is NULL, cannot save";
		return;
	}

	const auto * buf32 = reinterpret_cast<const uint32_t *>(buffer);

	for (int i = 0; i < n; ++i) {
		fprintf(file, "%08X\n", buf32[i]);
	}
}

static int load_8chs_from_file(const char * filename, struct iq_sample * buffers[fpga_dma::NUM_TX_CHANNELS], size_t & file_samples) {
	for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
		buffers[ch] = nullptr;
	}

	file_samples = 0;

	FILE * fp    = fopen(filename, "r");
	if (!fp) {
		piCout << "Failed to open input file " << filename;
		return -1;
	}

	// Одна валидная строка = один IQ sample на каждый канал
	file_samples = misc_count_8chs_samples(fp);

	if (file_samples == 0) {
		piCout << "Input file contains no valid samples";
		fclose(fp);
		return -1;
	}

	try {
		for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
			buffers[ch] = new struct iq_sample[file_samples];
		}
	} catch (const std::bad_alloc &) {
		piCout << "Failed to allocate input buffers";

		fclose(fp);

		for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
			delete[] buffers[ch];
			buffers[ch] = nullptr;
		}

		file_samples = 0;
		return -1;
	}

	rewind(fp);

	// misc_read_8chs пока принимает uint8_t*,
	// поэтому только здесь делаем преобразование.
	uint8_t * raw_buffers[fpga_dma::NUM_TX_CHANNELS];

	for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
		raw_buffers[ch] = reinterpret_cast<uint8_t *>(buffers[ch]);
	}

	const size_t buffer_size_bytes = file_samples * sizeof(struct iq_sample);

	const int ret                  = misc_read_8chs(fp, raw_buffers, buffer_size_bytes);

	fclose(fp);

	if (ret < 0) {
		piCout << "Failed to read input file";

		for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
			delete[] buffers[ch];
			buffers[ch] = nullptr;
		}

		file_samples = 0;
		return -1;
	}

	return 0;
}

int main(int argc, char * argv[]) {
	if (argc < 6) {
		printf("usage: %s <test_point> <channel> <range_gate> "
		       "<num_transfers> <input_file> [output_file]\n",
		       argv[0]);
		return 1;
	}

	const bool flag_save_buf = argc >= 7;

	PIString dir_path_str    = StdString2PIString(misc_get_date());
	fs::path dir_path        = PIString2StdString(dir_path_str);

	if (fs::create_directories(dir_path)) {
		piCout << "Created directory" << dir_path_str;
	}
	piCout << "Save to directory" << dir_path_str;

	PISignals::setSlot([](PISignals::Signal s) {
		piCout << "Signal" << s;
		PIKbdListener::exiting = true;
		PISignals::releaseSignals(s);
	});
	PISignals::grabSignals(PISignals::Interrupt | PISignals::Termination);

	PIKbdListener * kbd = new PIKbdListener(nullptr, nullptr, false);
	kbd->enableExitCapture(PIKbdListener::F10);


	const uint32_t test_point    = static_cast<uint32_t>(std::strtol(argv[1], nullptr, 0));
	const uint32_t channel       = static_cast<uint32_t>(std::strtol(argv[2], nullptr, 0));
	const uint32_t range_gate    = static_cast<uint32_t>(std::strtol(argv[3], nullptr, 0));
	const uint32_t num_transfers = static_cast<uint32_t>(std::strtol(argv[4], nullptr, 0));

	const char * input_file      = argv[5];
	FILE * dump_file             = nullptr;

	if (flag_save_buf) {
		PIString output_file = dir_path_str + "/" + argv[6];

		dump_file            = fopen(PIString2StdString(output_file).c_str(), "w");

		if (dump_file == nullptr) {
			perror("Failed to open output file");
			return 1;
		}
	}


	// Чтение файла данных в буфер file_buffers
	struct iq_sample * file_buffers[fpga_dma::NUM_TX_CHANNELS];
	size_t file_samples = 0;

	if (load_8chs_from_file(input_file, file_buffers, file_samples) != 0) {
		return 1;
	}

	if (axi_dsp_init() != FD_ERR_NONE) {
		piCout << "Failed to initialize AXI DSP";
		return 1;
	}
	axi_dsp_configure();

	axi_dsp_set_output_source(test_point, channel, range_gate);
	auto v = axi_dsp_get_output_source();
	piCout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << ", RANGE_GATE: " << v.RANGE_GATE << "\n";
	axi_dsp_set_channel_mask(0xFF);
	axi_dsp_apply();

	fpga_dma dma;
	if (dma.init() != 0) {
		fprintf(stderr, "Failed to initialize FPGA DMA\n");
		return 1;
	}

	PISystemTime t_start = PISystemTime::current();
	piCout << "Start Transfer";
	size_t file_pos         = 0;
	const size_t tx_samples = rls::TX_BUF_SIZE / sizeof(struct iq_sample);
	std::queue<std::vector<uint32_t>> dataQueue;

	while (dma.get_completed() < num_transfers) {
		if (dma.get_submitted() < num_transfers && dma.can_send()) {
			for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
				misc_copy_cyclic_iq(reinterpret_cast<struct iq_sample *>(dma.get_tx_buffer(ch)),
				                    file_buffers[ch],
				                    file_samples,
				                    file_pos,
				                    tx_samples);
			}
			file_pos = (file_pos + tx_samples) % file_samples;

			int ret  = dma.send();
			if (ret != 0) {
				fprintf(stderr, "TX ERROR transaction=%zu ret=%d\n", dma.get_submitted() - 1, ret);
			}
			continue;
		}

		int ret = dma.receive();
		if (ret != 0) {
			fprintf(stderr, "RX ERROR transaction=%zu ret=%d\n", dma.get_completed(), ret);
			break;
		}

		void * rx_buffer = dma.get_rx_buffer();

		// обработка / сохранение rx_buffer
		if (flag_save_buf) {
			auto * buffer       = reinterpret_cast<uint32_t *>(rx_buffer);
			const auto * hdr    = reinterpret_cast<const rls::header *>(buffer);
			int n_samps_to_save = rls::get_rx_words_per_buf(test_point);
			if (hdr->tp == rls::TP_WORK) {
				const auto * work = reinterpret_cast<const rls::work_posthdr *>(buffer + rls::HDR_SIZE);
				n_samps_to_save =
					rls::HDR_SIZE + (sizeof(rls::work_posthdr) + work->n_work_packets * sizeof(rls::work_packet)) / sizeof(uint32_t);
			}
			dataQueue.emplace(buffer, buffer + n_samps_to_save);
		}
	}


	// piCout << "RX DONE transaction=" << completed << " rx_buf=" << rx_buf_id << " sent=" << submitted << " received=" << completed +
	// 1;
	PISystemTime t_end = PISystemTime::current();
	piCout << "====";
	if (dma.get_completed() > 0) {
		piCout << "Mean: transfer time = " << (t_end - t_start) / dma.get_completed();
	}
	piCout << "====";
	dma.cleanup();

	if (flag_save_buf) {
		while (!dataQueue.empty()) {
			auto & samples = dataQueue.front();
			save_buf_to_file(dump_file, samples.data(), static_cast<int>(samples.size()));
			dataQueue.pop();
		}
		fflush(dump_file);
	}

	if (dump_file != nullptr) {
		fclose(dump_file);
		dump_file = nullptr;
	}

	for (size_t ch = 0; ch < fpga_dma::NUM_TX_CHANNELS; ++ch) {
		delete[] file_buffers[ch];
		file_buffers[ch] = nullptr;
	}

	axi_dsp_deinit();
	piDeleteSafety(kbd);

	return 0;
}