#ifndef BABLE_LINUX_ABSTRACTCOMMAND_HPP
#define BABLE_LINUX_ABSTRACTCOMMAND_HPP

#include <memory>

#include "../Serializer/Serializer.hpp"
#include "../Serializer/Deserializer.hpp"

class AbstractCommand {

public:
  // Serialize will have to have some params, like a map, to add params to specific command
  virtual Serializer serialize() = 0;

  virtual void extract(Deserializer& deser) = 0;

  // Write result on stdout with flatbuffer / raw ascii
  virtual std::string write() = 0;

  void set_controller(uint16_t controller_index) {
    m_controller_index = controller_index;
  }

  virtual std::string get_name() {
    return m_name;
  }

  virtual uint16_t get_command_code() {
    return m_command_code;
  }

  virtual ~AbstractCommand() = default;

protected:
  AbstractCommand(): m_command_code(0), m_params_length(0), m_controller_index(0xFFFF) {};

  void generate_header(Serializer& ser) {
    ser << m_command_code << m_controller_index << m_params_length;
  }

  std::string m_name;
  uint16_t m_command_code;
  uint16_t m_params_length;
  uint16_t m_controller_index;

};

#endif //BABLE_LINUX_ABSTRACTCOMMAND_HPP
