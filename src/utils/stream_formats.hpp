#ifndef BABLE_LINUX_STREAM_FORMATS_HPP
#define BABLE_LINUX_STREAM_FORMATS_HPP

#include <iomanip>

#define HEX(value) "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(value) << std::dec

#endif //BABLE_LINUX_STREAM_FORMATS_HPP
