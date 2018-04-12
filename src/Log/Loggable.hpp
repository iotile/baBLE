#ifndef BABLE_LINUX_LOGGABLE_HPP
#define BABLE_LINUX_LOGGABLE_HPP

#include <string>

class Loggable {

public:
  virtual const std::string stringify() const = 0;

};

#endif //BABLE_LINUX_LOGGABLE_HPP
