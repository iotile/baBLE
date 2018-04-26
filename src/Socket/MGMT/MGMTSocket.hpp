#ifndef BABLE_LINUX_MGMTSOCKET_HPP
#define BABLE_LINUX_MGMTSOCKET_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <memory>
#include <queue>
#include <uvw.hpp>

#include "../AbstractSocket.hpp"
#include "../../Serializer/Serializer.hpp"
#include "../../Serializer/Deserializer.hpp"
#include "../../Log/Log.hpp"
#include "../../Packet/constants.hpp"

#define BTPROTO_HCI   1
#define HCI_DEV_NONE  0xffff
#define HCI_CHANNEL_CONTROL 3

struct sockaddr_hci {
  sa_family_t     hci_family;
  unsigned short  hci_dev;
  unsigned short  hci_channel;
};

// TODO: idea -> run poller in socket ?
class MGMTSocket : public AbstractSocket {

public:
  MGMTSocket();

  const Packet::Type packet_type() const override {
    return Packet::Type::MGMT;
  };

  bool send(std::unique_ptr<Packet::AbstractPacket> packet) override;
  Deserializer receive() override;

  void set_writable(bool is_writable);
  uvw::OSSocketHandle::Type get_socket() const;

  ~MGMTSocket() override;

private:
  bool bind_socket();

  uvw::OSSocketHandle::Type m_socket;
  std::queue<std::unique_ptr<Packet::AbstractPacket>> m_send_queue;
  bool m_writable;

};

#endif //BABLE_LINUX_MGMTSOCKET_HPP
