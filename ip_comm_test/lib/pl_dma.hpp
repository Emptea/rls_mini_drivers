#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "dma-proxy.h"

class pl_dma final
{
public:
    struct Stats {
        uint64_t usec = 0;
        int mb_per_sec = 0;
    };

    struct ch_config {
        std::string devnode;
        int buffer_size;     
        int num_transfers;   
    };

    pl_dma() = default;
    ~pl_dma();

    pl_dma(const pl_dma&) = delete;
    pl_dma& operator=(const pl_dma&) = delete;

    pl_dma(pl_dma&& other) noexcept;
    pl_dma& operator=(pl_dma&& other) noexcept;

    /**
     * Initialize this instance for multiple TX/RX channels with user‑provided buffers.
     *
     * Note: your buffers must be mmap'able / coherent as expected by the dma_proxy driver.
     */
    int init(
        const std::vector<ch_config>& tx_channels,
        const std::vector<ch_config>& rx_channels
    );
    int init(const std::vector<ch_config>& tx_channels);

    int start();
    void wait();
    Stats get_stats() const;
    void cleanup();

    // Inside class pl_dma, add public methods:

void* get_tx_buffer(size_t num) const {
    if (!m_tx_ch.size())
        return nullptr;
    return &m_tx_ch[0].buf_ptr[num].buffer;
}

void* get_rx_buffer(size_t num) const {
    if (!m_rx_ch.size())
        return nullptr;
    return &m_rx_ch[0].buf_ptr[num].buffer;
}
size_t get_rx_buffer_size() const {
    if (m_rx_ch.size())
        return m_rx_ch[0].buffer_size;
    return 0;
}

size_t get_tx_buffer_size() const {
    if (m_tx_ch.size())
        return m_tx_ch[0].buffer_size;
    return 0;
}

void stop_transfer(){
    for (auto& ch : m_tx_ch){
        ch.stop = 1;
    }
}


private:
    struct channel {
        channel_buffer* buf_ptr = nullptr;  // proxy‑driver ring
        int fd = -1;
        int buffer_size = 0;
        int num_transfers = 0;
        uint8_t* user_buffer = nullptr;
        pthread_t tid = 0;
        int stop;
    };

    std::vector<channel> m_tx_ch;
    std::vector<channel> m_rx_ch;

    uint64_t m_start_time = 0;
    uint64_t m_end_time = 0;
private:
    int setup_threads();
    static void* tx_thread(void* arg);
    static void* rx_thread(void *arg);
    void cleanup_channels();
};