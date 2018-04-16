#ifndef BABLE_LINUX_GETMGMTINFOCOMMAND_HPP
#define BABLE_LINUX_GETMGMTINFOCOMMAND_HPP

#include "AbstractCommand.hpp"
#include "CommandManager.hpp"
#include "../Serializer/Serializer.hpp"
#include "../utils/stream_formats.hpp"

#define GETMGMTINFOCOMMAND_NAME "get_info"
#define GETMGMTINFOCOMMAND_CODE 0x0001

class GetMGMTInfoCommand : public AbstractCommand {

public:
  GetMGMTInfoCommand() {
    m_name = GETMGMTINFOCOMMAND_NAME;
    m_command_code = GETMGMTINFOCOMMAND_CODE;

    m_version = 0;
    m_revision = 0;
  }

  Serializer serialize() override {
    Serializer ser;
    generate_header(ser);

    return ser;
  };

  void extract(Deserializer& deser) override {
    deser >> m_version >> m_revision;
  }

  std::string write() override {
    std::ostringstream data;
    data << "Version: " << HEX(m_version) << std::endl;
    data << "Revision: " << HEX(m_revision) << std::endl;

    return data.str();
  }

private:
  uint8_t m_version;
  uint16_t m_revision;

  static CommandFactoryRegister<GetMGMTInfoCommand> AddToFactory;

};

#endif //BABLE_LINUX_GETMGMTINFOCOMMAND_HPP
