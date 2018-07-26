#ifndef BABLE_LOGGABLE_HPP
#define BABLE_LOGGABLE_HPP

#include <string>

// Interface making a class "loggable" meaning that we can have a string representation to log
class Loggable {

public:
  virtual const std::string stringify() const = 0;

};

#endif //BABLE_LOGGABLE_HPP
