#include "script.hpp"

#include <fstream>

#include <fmt/format.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/error.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include "image_io.hpp"
#include "logging.hpp"

static void add_classes(mrb_state* mrb) noexcept;

int script(ctopt::args::const_iterator begin, const ctopt::args::const_iterator& end) {
    if (begin == end) {
        fmt::print(stderr, "Missing required script");
        return 1;
    }

    const auto src = *begin++;

    auto fp = std::fopen(src.c_str(), "rb");
    if (!fp) {
        fmt::print(stderr, "Could not open file {}", src);
        return 1;
    }

    auto* mrb = mrb_open();
    add_classes(mrb);

    auto arguments = std::vector<mrb_value>{};
    std::transform(begin, end, std::back_insert_iterator(arguments), [&](auto str) {
        return mrb_str_new_static(mrb, str.data(), str.size());
    });

    const auto argv = mrb_ary_new_from_values(mrb, static_cast<mrb_int>(arguments.size()), arguments.data());

    mrb_define_global_const(mrb, "ARGV", argv);

    const auto res = mrb_load_file(mrb, fp);
    std::fclose(fp);

    if (mrb->exc) {
        mrb_print_error(mrb);
        return 1;
    }

    mrb_close(mrb);
    return mrb_as_int(mrb, res);
}

static void add_classes(mrb_state* mrb) noexcept {
    struct image_type {
        std::vector<float> data;
        int width, height, channels;
    };

    static constexpr auto image_mrb_type = mrb_data_type{"Image", [](auto* mrb, auto* data) {
        delete reinterpret_cast<image_type*>(data);
    }};

    auto* imageClass = mrb_define_class(mrb, "Image", mrb->object_class);
    MRB_SET_INSTANCE_TT(imageClass, MRB_TT_CDATA);

    mrb_define_method(mrb, imageClass, "initialize", [](auto* mrb, auto self) {
        const char *path;
        mrb_get_args(mrb, "z", &path);

        int width, height, channels;
        vlog::print("Reading image {}", [&]() {return fmt::make_format_args(path);});
        auto img = image::load(path, width, height, channels);
        if (!img) {
            fmt::print(stderr, "Could not read image {}", path);
            mrb_sys_fail(mrb, path);
        }

        mrb_data_init(self, new image_type{
            std::move(image::to_float(img, width, height, 2.2)),
            width, height, channels
        }, &image_mrb_type);

        return self;
    }, MRB_ARGS_REQ(1));

    mrb_define_method(mrb, imageClass, "width", [](auto* mrb, auto self) {
        return mrb_int_value(mrb, reinterpret_cast<image_type*>(DATA_PTR(self))->width);
    }, MRB_ARGS_NONE());

    mrb_define_method(mrb, imageClass, "height", [](auto* mrb, auto self) {
        return mrb_int_value(mrb, reinterpret_cast<image_type*>(DATA_PTR(self))->height);
    }, MRB_ARGS_NONE());
}
