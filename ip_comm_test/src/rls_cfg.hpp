#pragma once

#include <cstddef>

namespace rls_cfg {

constexpr std::size_t N_SAMPS_IN_PACK   = 232;
constexpr std::size_t N_PACKS_IN_TX_BUF = 20;
constexpr std::size_t HDR_SIZE          = 6;
constexpr std::size_t N_SAMPS_IN_TX_BUF = N_SAMPS_IN_PACK * N_PACKS_IN_TX_BUF;
constexpr std::size_t TX_BUF_SIZE       = sizeof(unsigned int) * N_SAMPS_IN_TX_BUF;
constexpr std::size_t NUM_CHANNELS      = 8;

} // namespace rls_cfg