#ifndef BABLE_LINUX_ASCIIFORMATBUILDER_HPP
#define BABLE_LINUX_ASCIIFORMATBUILDER_HPP

#include <array>
#include <sstream>
#include <string>
#include <vector>

class AsciiFormatBuilder {


public:
  AsciiFormatBuilder() {
    m_name = "Generic";
  };

  AsciiFormatBuilder& set_name(const std::string& name) {
    m_name = name;
  }

  AsciiFormatBuilder& add(const std::string& field_name, const std::string& value) {
    std::stringstream stream;
    stream << field_name << ": " << value;

    m_data_vector.push_back(stream.str());

    return *this;
  };

  template<typename T>
  AsciiFormatBuilder& add(const std::string& field_name, const T& value) {
    return add(field_name, std::to_string(value));
  };

  template<size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const std::array<std::string,N>& container) {
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
  };

  template<typename T, size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const std::array<T,N>& container) {
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
  };

  AsciiFormatBuilder& add(const std::string& field_name, const std::vector<std::string>& container) {
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
  };

  template<typename T>
  AsciiFormatBuilder& add(const std::string& field_name, const std::vector<T>& container) {
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
  };

  template<size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const char (&container)[N]) {
    std::string str(container);

    return add(field_name, str);
  };

  template<size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const std::string (&container)[N]) {
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
  };

  template<typename T, size_t N>
  AsciiFormatBuilder& add(const std::string& field_name, const T (&container)[N]) {
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
  };

  std::vector<uint8_t> build() {
    std::vector<uint8_t> result = generate_header();
    result.push_back(static_cast<uint8_t>('{'));

    for(auto it = m_data_vector.begin(); it != m_data_vector.end(); ++it) {

      result.reserve(result.size() + it->size() + 1);
      for (const char& c : *it) {
        result.push_back(static_cast<uint8_t>(c));
      }

      if (std::next(it) != m_data_vector.end()) {
        result.push_back(static_cast<uint8_t>(','));
      }
    }

    result.push_back(static_cast<uint8_t>('}'));

    return result;
  };

private:
  std::vector<uint8_t> generate_header() {
    std::vector<uint8_t> header;
    header.reserve(m_name.size() + 2);

    header.push_back(static_cast<uint8_t>('<'));

    for (const char& c : m_name) {
      header.push_back(static_cast<uint8_t>(c));
    }

    header.push_back(static_cast<uint8_t>('>'));

    return header;
  };

  std::string m_name;
  std::vector<std::string> m_data_vector;

};

#endif //BABLE_LINUX_ASCIIFORMATBUILDER_HPP
