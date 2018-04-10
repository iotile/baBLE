#ifndef BABLE_LINUX_ABSTRACTSERIALIZER_HPP
#define BABLE_LINUX_ABSTRACTSERIALIZER_HPP

#include <cstdint>
#include <iostream>
#include <typeinfo>
#include <vector>

#include "../utils/stream_formats.hpp"

class AbstractSerializer {

public:
  size_t size() const;
  const uint8_t* buffer() const;

  friend std::ostream& operator<<(std::ostream& output, const AbstractSerializer& self);

  virtual ~AbstractSerializer() = default;

protected:
  AbstractSerializer& operator=(const AbstractSerializer& other);

  virtual void import(const uint8_t* buffer, size_t size) = 0;
  virtual void import(const std::vector<uint8_t>& buffer) = 0;

  std::vector<uint8_t> m_buffer;

};

#endif //BABLE_LINUX_ABSTRACTSERIALIZER_HPP
