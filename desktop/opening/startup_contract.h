// SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
#include <string_view>

namespace g1 {
inline constexpr std::string_view ready_marker = "G1_NATIVE_MENU_VISIBLE_V1\n";
inline constexpr std::uint64_t decoder_timeout_ms = 60000;
inline constexpr std::uint64_t startup_timeout_ms = 90000;

constexpr bool valid_marker(std::string_view bytes) noexcept {
    return bytes == ready_marker;
}
struct Rect { int x, y, width, height; };
// Integer arithmetic avoids floating-point rounding and overflow at 8K/ultrawide sizes.
constexpr Rect fit_image(int vw, int vh, int iw, int ih) noexcept {
    if (vw <= 0 || vh <= 0 || iw <= 0 || ih <= 0) return {0, 0, 0, 0};
    const auto wide = static_cast<std::int64_t>(vw) * ih;
    const auto high = static_cast<std::int64_t>(vh) * iw;
    const int w = wide > high ? static_cast<int>(static_cast<std::int64_t>(vh) * iw / ih) : vw;
    const int h = wide > high ? vh : static_cast<int>(static_cast<std::int64_t>(vw) * ih / iw);
    return {(vw - w) / 2, (vh - h) / 2, w, h};
}
enum class Phase { playing, loading, complete, failed_open };
constexpr Phase phase(bool ended_or_skipped, bool ready, bool media_error,
                      std::uint64_t elapsed_ms) noexcept {
    if (media_error) return Phase::failed_open;
    if (ended_or_skipped && ready) return Phase::complete;
    if (elapsed_ms >= startup_timeout_ms ||
        (!ended_or_skipped && elapsed_ms >= decoder_timeout_ms)) return Phase::failed_open;
    return ended_or_skipped ? Phase::loading : Phase::playing;
}
} // namespace g1
