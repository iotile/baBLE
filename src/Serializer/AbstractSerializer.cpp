#include "AbstractSerializer.hpp"

using namespace std;

AbstractSerializer& AbstractSerializer::operator=(const AbstractSerializer& other) {
  if (&other == this) {
    return *this;
  }
  import(other.m_buffer);

  return *this;
}

size_t AbstractSerializer::size() const {
  return m_buffer.size();
}

const uint8_t* AbstractSerializer::buffer() const {
  return m_buffer.data();
}

void AbstractSerializer::import(const uint8_t* buffer, size_t size) {
  throw runtime_error("Can't run `import` function on the AbstractSerializer.");
}

void AbstractSerializer::import(const vector<uint8_t>& buffer) {
  throw runtime_error("Can't run `import` function on the AbstractSerializer.");
}

ostream& operator<<(ostream& output, const AbstractSerializer& self) {
  output << "<" << typeid(self).name() << "[" << self.size() << "]> { ";

  for (const uint8_t value : self.m_buffer) {
    output << HEX(value) << " ";
  }

  output << "}" << endl;

  return output;
}
