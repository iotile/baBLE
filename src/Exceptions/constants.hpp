#ifndef BABLE_LINUX_EXCEPTIONS_CONSTANTS_HPP
#define BABLE_LINUX_EXCEPTIONS_CONSTANTS_HPP

namespace Exceptions {

  enum class Type {
    InvalidCommand,
    NotFound,
    RuntimeError,
    SocketError,
    Unknown,
    WrongFormat
  };

}

#endif //BABLE_LINUX_EXCEPTIONS_CONSTANTS_HPP
