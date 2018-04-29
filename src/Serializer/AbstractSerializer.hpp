#ifndef BABLE_LINUX_ABSTRACTSERIALIZER_HPP
#define BABLE_LINUX_ABSTRACTSERIALIZER_HPP

#include <cstdint>
#include <iostream>
#include <typeinfo>
#include <vector>

#include "../Log/Loggable.hpp"
#include "../utils/stream_formats.hpp"

class AbstractSerializer : public Loggable {

public:
  size_t size() const;
  const uint8_t* buffer() const;
  const std::vector<uint8_t> raw_buffer() const {
    return m_buffer;
  }

  const std::string stringify() const override;

  virtual ~AbstractSerializer() = default;

protected:
  virtual void import(const uint8_t* buffer, size_t size);
  virtual void import(const std::vector<uint8_t>& buffer);

  std::vector<uint8_t> m_buffer;

};

#endif //BABLE_LINUX_ABSTRACTSERIALIZER_HPP
