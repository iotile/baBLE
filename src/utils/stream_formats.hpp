#ifndef BABLE_LINUX_STREAM_FORMATS_HPP
#define BABLE_LINUX_STREAM_FORMATS_HPP

#include <iomanip>

#define HEX(value) "0x" << setfill('0') << setw(2) << hex << static_cast<int>(value) << dec

#endif //BABLE_LINUX_STREAM_FORMATS_HPP
