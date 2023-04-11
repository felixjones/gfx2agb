#include "script.hpp"

#include <fstream>

#include <fmt/format.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/variable.h>

int script(ctopt::args::const_iterator begin, ctopt::args::const_iterator end) {
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
    return mrb_fixnum(res);
}
