#ifndef BABLE_LINUX_ASCIIFORMATBUILDER_HPP
#define BABLE_LINUX_ASCIIFORMATBUILDER_HPP

#include <array>
#include <sstream>
#include <vector>

class AsciiFormatBuilder {

public:
  // Constructors
  AsciiFormatBuilder();

  // Setters
  AsciiFormatBuilder& set_name(const std::string& name);

  // To add data to current building object
  AsciiFormatBuilder& add(const std::string& field_name, const std::string& value);
  template<typename T>
  AsciiFormatBuilder& add(const std::string& field_name, const T& value);
  template<size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const std::array<std::string,N>& container);
  template<typename T, size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const std::array<T,N>& container);
  AsciiFormatBuilder& add(const std::string& field_name, const std::vector<std::string>& container);
  template<typename T>
  AsciiFormatBuilder& add(const std::string& field_name, const std::vector<T>& container);
  template<size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const char (&container)[N]);
  template<size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const std::string (&container)[N]);
  template<typename T, size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const T (&container)[N]);

  // To build and finish the result object
  std::vector<uint8_t> build();

private:
  std::vector<uint8_t> generate_header();

  std::string m_name;
  std::vector<std::string> m_data_vector;

};

template<typename T>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const T& value) {
  return add(field_name, std::to_string(value));
}

template<size_t N>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const std::array<std::string,N>& container) {
  std::stringstream stream;
  stream << field_name << ": [";

  for(auto it = container.begin(); it != container.end(); ++it) {
    stream << *it;
    if (std::next(it) != container.end()) {
      stream << ", ";
    }
  }

  stream << "]";

  m_data_vector.push_back(stream.str());

  return *this;
}

template<typename T, size_t N>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const std::array<T,N>& container) {
  std::stringstream stream;
  stream << field_name << ": [";

  for(auto it = container.begin(); it != container.end(); ++it) {
    stream << std::to_string(*it);
    if (std::next(it) != container.end()) {
      stream << ", ";
    }
  }

  stream << "]";

  m_data_vector.push_back(stream.str());

  return *this;
}

template<typename T>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const std::vector<T>& container) {
  std::stringstream stream;
  stream << field_name << ": [";

  for(auto it = container.begin(); it != container.end(); ++it) {
    stream << std::to_string(*it);
    if (std::next(it) != container.end()) {
      stream << ", ";
    }
  }

  stream << "]";

  m_data_vector.push_back(stream.str());

  return *this;
}

template<size_t N>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const char (&container)[N]) {
  std::string str(container);

  return add(field_name, str);
}

template<size_t N>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const std::string (&container)[N]) {
  std::stringstream stream;
  stream << field_name << ": [";

  for(size_t i = 0; i < N; i++) {
    stream << container[i];
    if (i != N - 1) {
      stream << ", ";
    }
  }

  stream << "]";

  m_data_vector.push_back(stream.str());

  return *this;
}

template<typename T, size_t N>
AsciiFormatBuilder& AsciiFormatBuilder::add(const std::string& field_name, const T (&container)[N]) {
  std::stringstream stream;
  stream << field_name << ": [";

  for(size_t i = 0; i < N; i++) {
    stream << std::to_string(container[i]);
    if (i != N - 1) {
      stream << ", ";
    }
  }

  stream << "]";

  m_data_vector.push_back(stream.str());

  return *this;
}

#endif //BABLE_LINUX_ASCIIFORMATBUILDER_HPP
