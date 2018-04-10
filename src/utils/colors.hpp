#ifndef BABLE_LINUX_COLORS_HPP
#define BABLE_LINUX_COLORS_HPP

#include <string>

#define RESET_COLOR "\033[0m"

#define RED_COLOR "\033[1;31m"
#define GREEN_COLOR "\033[1;32m"
#define YELLOW_COLOR "\033[1;33m"
#define BLUE_COLOR "\033[1;34m"
#define PURPLE_COLOR "\033[1;35m"
#define WHITE_COLOR "\033[1;37m"

#define COLORIZE(color, txt) ((color) + std::string(txt) + RESET_COLOR)

#endif //BABLE_LINUX_COLORS_HPP
