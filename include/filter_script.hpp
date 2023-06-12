#pragma once

#include <string>
#include <vector>

namespace filter {

    using vector_string = std::vector<std::string>;

    int evaluate(const char* script, const vector_string& input, const vector_string& output) noexcept;

}
