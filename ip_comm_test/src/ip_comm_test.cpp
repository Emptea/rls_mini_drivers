#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "axi_multiplier.h"
#include "pl_dma.hpp" 

#include <picli.h>
#include <pikbdlistener.h>
#include <piliterals_time.h>
#include <piscreen.h>
#include <pisignals.h>

#define RX_DEV "/dev/dma_proxy_rx"
#define TX_DEV_CH0 "/dev/dma_proxy_tx_ch0"
#define TX_DEV_CH1 "/dev/dma_proxy_tx_ch1"

/**
 * Fill a uint8_t buffer with int16_t values starting from `start_cnt`.
 *
 * Assumptions:
 * - buffer_size is at least even (multiple of 2) in bytes.
 * - buffer is large enough for `(count) int16_t` values.
 */
void fill_int16_buffer(
    uint8_t* buffer,
    size_t buffer_size_bytes,
    int16_t start_cnt,
    int16_t step = 1
)
{
    int16_t* buf16 = reinterpret_cast<int16_t*>(buffer);
    size_t max_elements = buffer_size_bytes / sizeof(int16_t);

    for (size_t i = 0; i < max_elements; ++i) {
        buf16[i] = start_cnt + static_cast<int16_t>(i * step);
    }
}


int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("usage: %s <mult_ch0> <mult_ch1> <num_transfers>\n", argv[0]);
        return 1;
    }

    	PISignals::setSlot([](PISignals::Signal s) {
		piCout << "Signal" << s;
		PIKbdListener::exiting = true;
		PISignals::releaseSignals(s);
	});
	PISignals::grabSignals(PISignals::Interrupt | PISignals::Termination);

	PIKbdListener * kbd = nullptr;

	kbd                 = new PIKbdListener(nullptr, nullptr, false);
	kbd->enableExitCapture(PIKbdListener::F10);

    int16_t mult0 = (int16_t)strtol(argv[1], NULL, 0);
    int16_t mult1 = (int16_t)strtol(argv[2], NULL, 0);
    int16_t num_transfers = (int16_t)strtol(argv[3], NULL, 0);


    
    axi_multiplier_init();
    axi_multiplier_set_mult(mult0, 0);
    axi_multiplier_set_mult(mult1, 1);

    int16_t start_cnt = 1;  // change this as needed

    pl_dma dma_ch0, dma_ch1;

    std::vector<pl_dma::ch_config> tx_ch0 = {{
        .devnode = TX_DEV_CH0,
        .buffer_size = 4 * 1024,
    }};

    std::vector<pl_dma::ch_config> rx_ch0 = {{
        .devnode = RX_DEV,
        .buffer_size = 4 * 1024,
    }};


    std::vector<pl_dma::ch_config> tx_ch1 = {{
        .devnode = TX_DEV_CH1,
        .buffer_size = 4 * 1024,
    }};


    if (dma_ch0.init(tx_ch0, rx_ch0) != 0) {
        printf("Init dma for ch0 failed\n");
        return 1;
    }
    dma_ch0.set_num_transfers(num_transfers);

    if (dma_ch1.init(tx_ch1) != 0) {
        printf("Init dma for ch1 failed\n");
        return 1;
    }
    dma_ch1.set_num_transfers(num_transfers);

    uint8_t *rx_buf =  (uint8_t *)dma_ch0.get_rx_buffer(0);
    uint8_t *tx_buf0 = (uint8_t *)dma_ch0.get_tx_buffer(0);
    uint8_t *tx_buf1 = (uint8_t *)dma_ch1.get_tx_buffer(0);

    fill_int16_buffer(tx_buf0, 4 * 1024, start_cnt);
    fill_int16_buffer(tx_buf1, 4 * 1024, start_cnt + 10);  // different sequence
    axi_multiplier_set_ch(0);
    dma_ch0.start();
    dma_ch1.start();
	WAIT_FOR_EXIT;
    dma_ch0.stop();
    dma_ch1.stop();

    auto stats = dma_ch0.get_stats();
    printf("Throughput: %d MB/s\n", stats.mb_per_sec);

    dma_ch0.cleanup();
    dma_ch1.cleanup();
    axi_multiplier_deinit();
	piDeleteSafety(kbd);

    return 0;
}