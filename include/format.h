#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
};                                    // namespace Format


// template <typename T>
// std::string to_string_with_precision(const T a_value, const int n = 6)
// {
//     std::ostringstream out;
//     out.precision(n);
//     out << std::fixed << a_value;
//     return out.str();
// }

#endif