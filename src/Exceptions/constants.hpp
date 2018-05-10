#ifndef BABLE_LINUX_EXCEPTIONS_CONSTANTS_HPP
#define BABLE_LINUX_EXCEPTIONS_CONSTANTS_HPP

namespace Exceptions {

  enum class Type {
    Unknown,
    NotFound,
    SocketError,
    WrongFormat,
    InvalidCommand
  };

}

#endif //BABLE_LINUX_EXCEPTIONS_CONSTANTS_HPP
