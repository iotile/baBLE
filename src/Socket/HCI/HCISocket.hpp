#ifndef BABLE_LINUX_HCI_HPP
#define BABLE_LINUX_HCI_HPP

#include "../AbstractSocket.hpp"
#include "../../Packet/constants.hpp"
#include "../../Serializer/Serializer.hpp"
#include "../../Serializer/Deserializer.hpp"

class HCISocket : public AbstractSocket  {

public:
  const Packet::Type packet_type() const override {
    return Packet::Type::HCI;
  };

  bool send(std::unique_ptr<Packet::AbstractPacket> packet) override {
    std::cerr << "HCISocket send" << std::endl;
    return true;
  };

  Deserializer receive() override {
    std::cerr << "HCISocket receive" << std::endl;

    Deserializer deser;
    return deser;
  };

};

#endif //BABLE_LINUX_HCI_HPP
