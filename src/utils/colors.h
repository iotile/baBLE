#ifndef BABLE_LINUX_COLORS_H
#define BABLE_LINUX_COLORS_H

#include <string>

#define RESET_COLOR "\033[0m"

#define RED_COLOR "\033[1;31m"
#define GREEN_COLOR "\033[1;32m"
#define YELLOW_COLOR "\033[1;33m"
#define BLUE_COLOR "\033[1;34m"
#define PURPLE_COLOR "\033[1;35m"
#define WHITE_COLOR "\033[1;37m"

#define COLORIZE(color, txt) ((color) + std::string(txt) + RESET_COLOR)

#define RED(txt) COLORIZE(RED_COLOR, txt)
#define GREEN(txt) COLORIZE(GREEN_COLOR, txt)
#define YELLOW(txt) COLORIZE(YELLOW_COLOR, txt)
#define BLUE(txt) COLORIZE(BLUE_COLOR, txt)
#define PURPLE(txt) COLORIZE(PURPLE_COLOR, txt)

#endif //BABLE_LINUX_COLORS_H
