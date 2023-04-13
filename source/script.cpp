#include "script.hpp"

#include <fmt/format.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>

#include "image_io.hpp"

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

    const auto retval = static_cast<int>(mrb_as_int(mrb, mrb_convert_type(mrb, res, MRB_TT_INTEGER, "Integer", "to_i")));
    mrb_close(mrb);
    return retval;
}

static void add_classes(mrb_state* mrb) noexcept {
    image::register_script(mrb);
}
