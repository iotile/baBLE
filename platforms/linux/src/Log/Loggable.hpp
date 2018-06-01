#ifndef BABLE_LINUX_LOGGABLE_HPP
#define BABLE_LINUX_LOGGABLE_HPP

// Interface making a class "loggable" meaning that we can have a string representation to log
class Loggable {

public:
  virtual const std::string stringify() const = 0;

};

#endif //BABLE_LINUX_LOGGABLE_HPP
