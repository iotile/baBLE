#ifndef BABLE_STREAM_FORMATS_HPP
#define BABLE_STREAM_FORMATS_HPP

#include <iomanip>

#define HEX(value) "0x" << RAW_HEX(value, 2)
#define RAW_HEX(value, width) std::setfill('0') << std::setw(width) << std::hex << static_cast<int>(value) << std::dec

#endif //BABLE_STREAM_FORMATS_HPP
