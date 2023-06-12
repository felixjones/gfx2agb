#pragma once

#include <ctopt.hpp>
#include <fmt/format.h>

namespace options {

    static constexpr auto option_help = ctopt::option('h', "help").help_text("Print help").flag_counter();
    static constexpr auto option_dump_version = ctopt::option("dump-version").help_text("Print version").flag_counter();
    static constexpr auto option_verbose = ctopt::option('v', "verbose").help_text("Verbose logging").flag_counter();

    static constexpr auto option_input = ctopt::option('i', "input").min(1).max(std::numeric_limits<std::size_t>::max()).required();
    static constexpr auto option_output = ctopt::option('o', "output").min(1).max(std::numeric_limits<std::size_t>::max()).required();
    static constexpr auto option_filter = ctopt::option('f', "filter").help_text("Filter script").meta("<Ruby>");

    static constexpr auto get_opts = make_options_no_error(
        option_help,
        option_dump_version,
        option_verbose,

        option_input,
        option_output,
        option_filter
    );

    static inline const auto help_str = fmt::format(R"({})",
        get_opts.help_str()
    );
}
