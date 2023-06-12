#include "filter_script.hpp"

#include <algorithm>
#include <iterator>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>

static mrb_value filter_null(mrb_state* mrb, mrb_value self);

int filter::evaluate(const char* script, const vector_string& input, const vector_string& output) noexcept {
    auto* mrb = mrb_open();

    mrb_define_method(mrb, mrb->kernel_module, "null", filter_null, MRB_ARGS_ANY());

    auto arguments = std::vector<mrb_value>{};
    std::transform(input.cbegin(), input.cend(), std::back_insert_iterator(arguments), [&](auto str) {
        return mrb_str_new_static(mrb, str.data(), str.size());
    });

    const auto argv = mrb_ary_new_from_values(mrb, static_cast<mrb_int>(arguments.size()), arguments.data());

    mrb_define_global_const(mrb, "ARGV", argv);

    const auto res = mrb_load_string(mrb, script);

    if (mrb->exc) {
        mrb_print_error(mrb);
        return 1;
    }

    const auto retval = static_cast<int>(mrb_as_int(mrb, mrb_convert_type(mrb, res, MRB_TT_INTEGER, "Integer", "to_i")));
    mrb_close(mrb);
    return retval;
}

static mrb_value filter_null(mrb_state* mrb, mrb_value self) {
    return mrb_fixnum_value(54); //TODO: Return args in array
}
