#include "axi_dsp.h"
#include "dma_channel.hpp"
#include "fpga_dma.hpp"
#include "misc.h"
#include "picout.h"

#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>
#include <pistring_std.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace fs = std::filesystem;

#define N_SAMPS_IN_PACK   232
#define N_PACKS_IN_TX_BUF 20
#define N_SAMPS_IN_TX_BUF (N_SAMPS_IN_PACK * N_PACKS_IN_TX_BUF)
#define TX_BUF_SIZE       (sizeof(unsigned int) * N_SAMPS_IN_TX_BUF)
#define HDR_SIZE          6

#define RX_DEV            "/dev/dma_proxy_rx"
#define TX_DEV_CH0        "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1        "/dev/dma_proxy_tx_ch1"
#define TX_DEV_CH2        "/dev/dma_proxy_tx_ch2"
#define TX_DEV_CH3        "/dev/dma_proxy_tx_ch3"
#define TX_DEV_CH4        "/dev/dma_proxy_tx_ch4"
#define TX_DEV_CH5        "/dev/dma_proxy_tx_ch5"
#define TX_DEV_CH6        "/dev/dma_proxy_tx_ch6"
#define TX_DEV_CH7        "/dev/dma_proxy_tx_ch7"

#define NUM_CHANNELS_RX   1
#define NUM_CHANNELS_TX   8

constexpr size_t RX_PIPELINE_DEPTH = 8;

// static_assert(PIPELINE_DEPTH <= TX_BUFFER_COUNT);
// static_assert(PIPELINE_DEPTH <= RX_BUFFER_COUNT);


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

static void save_buf_to_file(void * buffer, int N) {
	// piCout << "Saving started for buffer " << PICoutManipulators::PICoutFormat::Hex << buffer;
	// const int16_t * buf16 = reinterpret_cast<const int16_t *>(buffer);
	const uint32_t * buf32 = reinterpret_cast<const uint32_t *>(buffer);
	if (dump_file == nullptr) {
		piCout << "ERROR: dump_file is NULL, cannot save";
		return;
	}
	for (size_t i = 0; i < N; i++) {
		fprintf(dump_file, "%08X\n", buf32[i]);
	}


	// Flush periodically
	if (ch.counter % 10 == 0) fflush(dump_file);
}


static int load_8chs_from_file(const char * filename, struct iq_sample * buffers[NUM_CHANNELS_TX], size_t & file_samples) {
	for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
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
		for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
			buffers[ch] = new struct iq_sample[file_samples];
		}
	} catch (const std::bad_alloc &) {
		piCout << "Failed to allocate input buffers";

		fclose(fp);

		for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
			delete[] buffers[ch];
			buffers[ch] = nullptr;
		}

		file_samples = 0;
		return -1;
	}

	rewind(fp);

	// misc_read_8chs пока принимает uint8_t*,
	// поэтому только здесь делаем преобразование.
	uint8_t * raw_buffers[NUM_CHANNELS_TX];

	for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
		raw_buffers[ch] = reinterpret_cast<uint8_t *>(buffers[ch]);
	}

	const size_t buffer_size_bytes = file_samples * sizeof(struct iq_sample);

	const int ret                  = misc_read_8chs(fp, raw_buffers, buffer_size_bytes);

	fclose(fp);

	if (ret < 0) {
		piCout << "Failed to read input file";

		for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
			delete[] buffers[ch];
			buffers[ch] = nullptr;
		}

		file_samples = 0;
		return -1;
	}

	return 0;
}

int main(int argc, char * argv[]) {
	if (argc < 5) {
		printf("usage: %s <test_point> <channel> <range_gate> <num_transfers> <input_file> <output_file>\n", argv[0]);
		return 1;
	}

	PIString dir_path_str = StdString2PIString(misc_get_date());
	fs::path dir_path     = PIString2StdString(dir_path_str);

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


	PIKbdListener * kbd = nullptr;


	kbd                 = new PIKbdListener(nullptr, nullptr, false);
	kbd->enableExitCapture(PIKbdListener::F10);


	uint32_t test_point     = (uint32_t)strtol(argv[1], NULL, 0);
	uint32_t channel        = (uint32_t)strtol(argv[2], NULL, 0);
	uint32_t range_gate     = (uint32_t)strtol(argv[3], NULL, 0);
	uint32_t num_transfers  = (uint32_t)strtol(argv[4], NULL, 0);
	const char * input_file = argv[5];
	PIString output_file    = dir_path_str + "/" + argv[6]; // File to dump RX data (optional, can be empty string)

	// Чтение файла данных в буфер file_buffers
	struct iq_sample * file_buffers[NUM_CHANNELS_TX];
	size_t file_samples = 0;

	if (load_8chs_from_file(input_file, file_buffers, file_samples) != 0) {
		return 1;
	}

	axi_dsp_init();

	auto ip_ver = axi_dsp_get_ip_ver();
	PICout(PICoutManipulators::AddNone) << "\nIP Version: " << ip_ver.MAJ_VER << "." << ip_ver.MIN_VER << "\n" << "\n";

	axi_dsp_kill();
	axi_dsp_set_motion_selector(1, 1);
	axi_dsp_set_output_source(test_point, channel, range_gate);
	auto v = axi_dsp_get_output_source();
	piCout << "SOURCE: " << v.SOURCE << ", SOURCE_CHANNEL: " << v.SOURCE_CHANNEL << ", RANGE_GATE: " << v.RANGE_GATE << "\n";

	cmplx_f64 manual_comp       = {.real = 1, .imag = 0};
	cmplx_f64 diagrams_even     = {.real = 1, .imag = 0};
	cmplx_f64 diagrams_odd      = {.real = 0, .imag = 1};

	cmplx_f64 diagrams_0_all[8] = {
		{.real = 0.5250, .imag = 0.8511 },
		{.real = 0.7470, .imag = 0.6648 },
		{.real = 0.9063, .imag = 0.4226 },
		{.real = 0.9894, .imag = 0.1449 },
		{.real = 0.9894, .imag = -0.1449},
		{.real = 0.9063, .imag = -0.4226},
		{.real = 0.7470, .imag = -0.6648},
		{.real = 0.5250, .imag = -0.8511},
	};

	cmplx_f64 diagrams_1_all[8] = {
		{.real = -0.9925, .imag = 0.1220 },
		{.real = -0.5529, .imag = 0.8333 },
		{.real = 0.2733,  .imag = 0.9619 },
		{.real = 0.9084,  .imag = 0.4181 },
		{.real = 0.9084,  .imag = -0.4181},
		{.real = 0.2733,  .imag = -0.9619},
		{.real = -0.5529, .imag = -0.8333},
		{.real = -0.9925, .imag = -0.1220},
	};

	cmplx_f64 diagrams_2_all[8] = {
		{.real = 0.2031,  .imag = -0.9792},
		{.real = -0.9321, .imag = -0.3621},
		{.real = -0.5111, .imag = 0.8595 },
		{.real = 0.7633,  .imag = 0.6461 },
		{.real = 0.7633,  .imag = -0.6461},
		{.real = -0.5111, .imag = -0.8595},
		{.real = -0.9321, .imag = 0.3621 },
		{.real = 0.2031,  .imag = 0.9792 },
	};

	cmplx_f64 diagrams_3_all[8] = {
		{.real = 0.9349,  .imag = 0.3549 },
		{.real = 0.0347,  .imag = -0.9994},
		{.real = -0.9573, .imag = 0.2891 },
		{.real = 0.5821,  .imag = 0.8131 },
		{.real = 0.5821,  .imag = -0.8131},
		{.real = -0.9573, .imag = -0.2891},
		{.real = 0.0347,  .imag = 0.9994 },
		{.real = 0.9349,  .imag = -0.3549},
	};

	cmplx_f64 diagrams_4_all[8] = {
		{.real = -0.2890, .imag = 0.9573 },
		{.real = 0.8944,  .imag = -0.4473},
		{.real = -0.9394, .imag = -0.3430},
		{.real = 0.3958,  .imag = 0.9183 },
		{.real = 0.3958,  .imag = -0.9183},
		{.real = -0.9394, .imag = 0.3430 },
		{.real = 0.8944,  .imag = 0.4473 },
		{.real = -0.2890, .imag = -0.9573},
	};

	cmplx_f64 diagrams_5_all[8] = {
		{.real = -0.9984, .imag = 0.0558 },
		{.real = 0.9175,  .imag = 0.3977 },
		{.real = -0.6420, .imag = -0.7667},
		{.real = 0.2303,  .imag = 0.9731 },
		{.real = 0.2303,  .imag = -0.9731},
		{.real = -0.6420, .imag = 0.7667 },
		{.real = 0.9175,  .imag = -0.3977},
		{.real = -0.9984, .imag = -0.0558},
	};

	cmplx_f64 diagrams_6_all[8] = {
		{.real = -0.6639, .imag = -0.7478},
		{.real = 0.4957,  .imag = 0.8685 },
		{.real = -0.3062, .imag = -0.9520},
		{.real = 0.1035,  .imag = 0.9946 },
		{.real = 0.1035,  .imag = -0.9946},
		{.real = -0.3062, .imag = 0.9520 },
		{.real = 0.4957,  .imag = -0.8685},
		{.real = -0.6639, .imag = 0.7478 },
	};

	cmplx_f64 diagrams_7_all[8] = {
		{.real = -0.1767, .imag = -0.9843},
		{.real = 0.1265,  .imag = 0.9920 },
		{.real = -0.0761, .imag = -0.9971},
		{.real = 0.0254,  .imag = 0.9997 },
		{.real = 0.0254,  .imag = -0.9997},
		{.real = -0.0761, .imag = 0.9971 },
		{.real = 0.1265,  .imag = -0.9920},
		{.real = -0.1767, .imag = 0.9843 },
	};

	for (size_t i = 0; i < NUM_CHANNELS_TX; i++) {
		axi_dsp_set_manual_compensation(manual_comp, i);
		axi_dsp_set_diagram_0(diagrams_0_all[i], i);
		axi_dsp_set_diagram_1(diagrams_1_all[i], i);
		axi_dsp_set_diagram_2(diagrams_2_all[i], i);
		axi_dsp_set_diagram_3(diagrams_3_all[i], i);
		axi_dsp_set_diagram_4(diagrams_4_all[i], i);
		axi_dsp_set_diagram_5(diagrams_5_all[i], i);
		axi_dsp_set_diagram_6(diagrams_6_all[i], i);
		axi_dsp_set_diagram_7(diagrams_7_all[i], i);
	}
	axi_dsp_set_compensation_mode(0);
	axi_dsp_set_compensation_ref((uint32_t)1575);
	axi_dsp_set_apu_rank(9, 15);
	axi_dsp_set_detector_level(36, 0);
	axi_dsp_set_detector_level(0, 1);
	axi_dsp_set_channel_mask(0xFF);
	axi_dsp_apply();

	// uint32_t num_transfers   = 16;
	uint32_t n_samps_per_buf = (141 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
	uint32_t num_rx_transfer = num_transfers * N_PACKS_IN_TX_BUF;

	switch (test_point) {
	case TP_WORK: {
		n_samps_per_buf = sizeof(work_posthdr) + HDR_SIZE;
		break;
	}
	case TP_BYPASS: {
		n_samps_per_buf = (N_SAMPS_IN_PACK + HDR_SIZE) * N_PACKS_IN_TX_BUF;
		break;
	}
	case TP_CUT:
	case TP_FAPCH:
	case TP_LOU: {
		n_samps_per_buf = (164 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
		break;
	}
	case TP_MTI:
	case TP_SF: {
		n_samps_per_buf = (141 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
		break;
	}
	case TP_MAX:
	case TP_RANK:
	case TP_APU: {
		n_samps_per_buf = (141 + HDR_SIZE);
		break;
	}
	case TP_DDR:
	case TP_FFT:
	case TP_WEIGHT_OUT: {
		n_samps_per_buf = 512 + HDR_SIZE;
		break;
	}
	case TP_FIND: {
		n_samps_per_buf = 141 * 5 + HDR_SIZE;
		break;
	}
	case TP_FAPCH_COEFFS: {
		n_samps_per_buf = (8 + HDR_SIZE) * N_PACKS_IN_TX_BUF;
		break;
	}
	default: {
		break;
	}
	}

	fpga_dma dma;
	fpga_dma::config dma_cfg;

	dma_cfg.rx_devnode     = RX_DEV;

	dma_cfg.tx_devnodes[0] = TX_DEV_CH0;
	dma_cfg.tx_devnodes[1] = TX_DEV_CH1;
	dma_cfg.tx_devnodes[2] = TX_DEV_CH2;
	dma_cfg.tx_devnodes[3] = TX_DEV_CH3;
	dma_cfg.tx_devnodes[4] = TX_DEV_CH4;
	dma_cfg.tx_devnodes[5] = TX_DEV_CH5;
	dma_cfg.tx_devnodes[6] = TX_DEV_CH6;
	dma_cfg.tx_devnodes[7] = TX_DEV_CH7;

	dma_cfg.tx_buf_size    = TX_BUF_SIZE;
	dma_cfg.rx_buf_size    = BUFFER_SIZE;
	dma_cfg.rx_buf_count   = RX_PIPELINE_DEPTH;

	if (dma.init(dma_cfg) != 0) {
		fprintf(stderr, "Failed to initialize FPGA DMA\n");
		return 1;
	}

	int buff_id          = 0;
	PISystemTime t_start = PISystemTime::current();
	PISystemTime t_end   = PISystemTime::current();
	size_t submitted     = 0;
	size_t completed     = 0;
	piCout << "Start Transfer";
	size_t file_pos         = 0;
	const size_t tx_samples = TX_BUF_SIZE / sizeof(struct iq_sample);
	size_t file_pos         = 0;
	const size_t tx_samples = TX_BUF_SIZE / sizeof(struct iq_sample);

	while (dma.get_completed() < num_transfers) {
		if (dma.get_submitted() < num_transfers && dma.can_send()) {
			for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
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
	}

	// 		if (flag_save_buf) {
	// 	auto * buffer       = ch.buf_ptr->buffers[buffer_id].buffer;

	// 	const auto * hdr    = reinterpret_cast<const struct header *>(buffer);

	// 	int n_samps_to_save = n_samps_per_buf;

	// 	if (hdr->tp == TP_WORK) {
	// 		const auto * work = reinterpret_cast<const struct work_posthdr *>(reinterpret_cast<const uint32_t *>(buffer) + HDR_SIZE);

	// 		n_samps_to_save   = HDR_SIZE + (sizeof(work_posthdr) + work->n_work_packets * sizeof(work_packet)) / sizeof(uint32_t);
	// 	}

	// 	dataQueue.emplace(buffer, buffer + n_samps_to_save);
	// }


	// piCout << "RX DONE transaction=" << completed << " rx_buf=" << rx_buf_id << " sent=" << submitted << " received=" << completed +
	// 1;
	++completed;
}

piCout << "====";
piCout << "Mean: transfer time = " << (t_end - t_start) / completed;
piCout << "====";

for (int k = dma_channels.size() - 1; k >= 0; k--) {
	dma_channels[k]->cleanup();
	delete dma_channels[k];
	dma_channels[k] = nullptr;
}

if (flag_save_buf) {
	while (!dataQueue.empty()) {
		auto & samples = dataQueue.front();

		save_buf_to_file(samples.data(), static_cast<int>(samples.size()));

		dataQueue.pop();
	}
}

for (size_t ch = 0; ch < NUM_CHANNELS_TX; ++ch) {
	delete[] file_buffers[ch];
	file_buffers[ch] = nullptr;
}

axi_dsp_deinit();
piDeleteSafety(kbd);

return 0;
}