#include "util.hpp"

#include <array>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>

extern "C" {

void mrb_init_mrblib(mrb_state* mrb) {}
void mrb_init_mrbgems(mrb_state* mrb) {}
void mrb_final_mrbgems(mrb_state* mrb) {}

}

std::pair<int, int> util::parse_width_height(int inWidth, int inHeight, const std::string& widthExpr, const std::string& heightExpr) noexcept {
    auto* mrb = mrb_open();

    mrb_define_global_const(mrb, "IW", mrb_fixnum_value(inWidth));
    mrb_define_method(mrb, mrb->object_class, "iw", [](auto* mrb, auto self) {
        return mrb_const_get(mrb, mrb_obj_value(mrb->object_class), mrb_intern_static(mrb, "IW", 2));
    }, MRB_ARGS_NONE());

    mrb_define_global_const(mrb, "IH", mrb_fixnum_value(inHeight));
    mrb_define_method(mrb, mrb->object_class, "ih", [](auto* mrb, auto self) {
        return mrb_const_get(mrb, mrb_obj_value(mrb->object_class), mrb_intern_static(mrb, "IH", 2));
    }, MRB_ARGS_NONE());

    auto rw = mrb_load_string(mrb, widthExpr.c_str());
    const auto outWidth = mrb_fixnum(rw);

    auto rh = mrb_load_string(mrb, heightExpr.c_str());
    const auto outHeight = mrb_fixnum(rh);

    mrb_close(mrb);

    return {outWidth, outHeight};
}

std::vector<char> util::repack_data(const std::vector<std::size_t>& data, std::size_t bpp) noexcept {
    const auto mask = (bpp << 1) - 1;

    auto size = bpp * data.size();
    size = (size + 7) / 8;

    auto result = std::vector<char>{};
    result.reserve(size);

    auto buflen = std::make_signed_t<std::size_t>{};
    auto buffer = std::size_t{};

    const auto copy_to_result = [&]() {
        auto bytes = std::array<char, sizeof(buffer)>{};

        const auto copy = (buflen + 7) / 8;
        std::memcpy(&bytes, &buffer, copy);
        buffer >>= buflen;

        int ii = 0;
        while (buflen >= 8 && ii < copy) {
            result.emplace_back(bytes[ii++]);
            buflen -= 8;
        }
    };

    for (auto v : data) {
        const auto d = v & mask;
        buffer |= d << buflen;
        buflen += static_cast<decltype(buflen)>(bpp);

        if (buflen >= 8) {
            copy_to_result();
        }
    }
    if (buflen) {
        copy_to_result();
    }

    return result;
}
