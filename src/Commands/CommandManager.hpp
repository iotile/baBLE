#ifndef BABLE_LINUX_COMMANDMANAGER_HPP
#define BABLE_LINUX_COMMANDMANAGER_HPP

#include <memory>
#include <unordered_map>

#include "../Serializer/Deserializer.hpp"
#include "../Socket/MGMTSocket.hpp"
#include "AbstractCommand.hpp"

class CommandManager {

public:
  using CommandCreateMethod = std::function<std::unique_ptr<AbstractCommand>()>;

  CommandManager() = delete;
  CommandManager(CommandManager const&) = delete;
  CommandManager& operator=(CommandManager const&) = delete;

  static void Process(const std::string& command_name, const std::shared_ptr<MGMTSocket>& sock) {
    std::unique_ptr<AbstractCommand> command = Invoke(command_name); // TODO: handle wrong command_name
    Serializer ser = command->serialize();
    sock->send(ser);
  }

  // Extract header with event_code, controller_index, and send to commands to treat the payload
  static void Process(Deserializer& deser, _IO_FILE* output) {
    // Extract event_code from deser
    uint16_t event_code;
    uint16_t controller_index;
    uint16_t payload_length;
    deser >> event_code >> controller_index >> payload_length;

    // If command event, extract command_code from deser
    if (event_code == 0x0001 || event_code == 0x0002) {
      uint16_t command_code;
      uint8_t status;

      deser >> command_code >> status;

      // Get command from command_code
      std::unique_ptr<AbstractCommand> command = Invoke(command_code);

      // Run it to parse the payload
      command->extract(deser);

      // Send stringified to output
      std::string response = command->write();
      fwrite(response.c_str(), 1, response.size(), output);
      fflush(output);
    }

  }

  static void Register(const std::string& command_name, uint16_t command_code, const CommandCreateMethod& fn) {
    m_commands_code().emplace(command_code, fn);
    m_commands_name().emplace(command_name, fn);
  };

  static std::unique_ptr<AbstractCommand> Invoke(const std::string& name) {
    CommandCreateMethod fn = m_commands_name()[name];
    if (fn == nullptr) {
      throw std::invalid_argument("Command name not found");
    }

    return fn();
  };

  static std::unique_ptr<AbstractCommand> Invoke(uint16_t command_code) {
    CommandCreateMethod fn = m_commands_code()[command_code];
    if (fn == nullptr) {
      throw std::invalid_argument("Command code not found");
    }

    return fn();
  };

private:
  static std::unordered_map<uint16_t, CommandCreateMethod>& m_commands_code() {
    static std::unordered_map<uint16_t, CommandCreateMethod> instance;
    return instance;
  };

  static std::unordered_map<std::string, CommandCreateMethod>& m_commands_name() {
    static std::unordered_map<std::string, CommandCreateMethod> instance;
    return instance;
  };

};

template<typename T>
struct CommandFactoryRegister {
  CommandFactoryRegister(const std::string& command_name, uint16_t command_code) {
    CommandManager::Register(command_name, command_code, []() { return std::make_unique<T>(); });
  }
};


#endif //BABLE_LINUX_COMMANDMANAGER_HPP
