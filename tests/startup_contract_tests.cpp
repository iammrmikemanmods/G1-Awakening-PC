// SPDX-License-Identifier: MIT
#include "startup_contract.h"
#include <climits>
#include <iostream>
#include <string>

int main() {
    int checks = 0, failures = 0;
    auto check = [&](bool condition, const char* name) {
        ++checks;
        if (!condition) { ++failures; std::cerr << "FAIL: " << name << '\n'; }
    };
    check(g1::valid_marker("G1_NATIVE_MENU_VISIBLE_V1\n"), "exact marker");
    check(!g1::valid_marker(""), "empty marker");
    check(!g1::valid_marker("G1_NATIVE_MENU_VISIBLE_V1"), "partial marker");
    check(!g1::valid_marker("G1_NATIVE_MENU_VISIBLE_V1\r\n"), "unexpected newline");
    check(!g1::valid_marker("G1_NATIVE_MENU_VISIBLE_V2\n"), "wrong version");
    check(!g1::valid_marker("G1_NATIVE_MENU_VISIBLE_V1\nextra"), "trailing bytes");
    const std::string nul = std::string(g1::ready_marker) + '\0';
    check(!g1::valid_marker(nul), "embedded zero");
    auto rect = [&](int vw, int vh, int iw, int ih, g1::Rect expected, const char* name) {
        const auto r = g1::fit_image(vw, vh, iw, ih);
        check(r.x == expected.x && r.y == expected.y && r.width == expected.width && r.height == expected.height, name);
    };
    rect(1920, 1080, 1920, 1080, {0,0,1920,1080}, "1080p identity");
    rect(3840, 2160, 1920, 1080, {0,0,3840,2160}, "4K scale");
    rect(3440, 1440, 1920, 1080, {440,0,2560,1440}, "ultrawide pillarbox");
    rect(1920, 1200, 1920, 1080, {0,60,1920,1080}, "16:10 letterbox");
    rect(1080, 1920, 1920, 1080, {0,656,1080,607}, "portrait letterbox");
    rect(0, 1080, 1920, 1080, {0,0,0,0}, "zero viewport");
    rect(-1, 1080, 1920, 1080, {0,0,0,0}, "negative viewport");
    rect(1920, 1080, 0, 1080, {0,0,0,0}, "zero image");
    rect(INT_MAX, INT_MAX, INT_MAX, INT_MAX, {0,0,INT_MAX,INT_MAX}, "overflow boundary");
    using P = g1::Phase;
    check(g1::phase(false,false,false,0) == P::playing, "start playing");
    check(g1::phase(false,true,false,1) == P::playing, "ready does not cut movie");
    check(g1::phase(true,false,false,1) == P::loading, "skip still waits for loading");
    check(g1::phase(true,true,false,1) == P::complete, "skip after readiness");
    check(g1::phase(true,true,false,90000) == P::complete, "ready completion wins timeout");
    check(g1::phase(false,false,true,1) == P::failed_open, "decoder error");
    check(g1::phase(true,true,true,1) == P::failed_open, "error remains diagnostic");
    check(g1::phase(false,false,false,59999) == P::playing, "before decoder deadline");
    check(g1::phase(false,false,false,60000) == P::failed_open, "decoder deadline");
    check(g1::phase(true,false,false,60000) == P::loading, "skip uses startup deadline");
    check(g1::phase(true,false,false,89999) == P::loading, "before startup deadline");
    check(g1::phase(true,false,false,90000) == P::failed_open, "startup deadline");
    std::cout << checks << " checks; " << failures << " failures\n";
    return failures ? 1 : 0;
}
