#include "AbstractSerializer.hpp"

using namespace std;

size_t AbstractSerializer::size() const {
  return m_buffer.size();
}

const uint8_t* AbstractSerializer::buffer() const {
  return m_buffer.data();
}

void AbstractSerializer::import(const uint8_t* buffer, size_t size) {
  throw runtime_error("Can't run `import` function of AbstractSerializer.");
}

void AbstractSerializer::import(const vector<uint8_t>& buffer) {
  throw runtime_error("Can't run `import` function of AbstractSerializer.");
}

const string AbstractSerializer::stringify() const {
  ostringstream data;
  data << "<" << typeid(*this).name() << "[" << size() << "]> { ";

  for (const uint8_t value : m_buffer) {
    data << HEX(value) << " ";
  }

  data << "}";

  return data.str();
}
