#ifndef BABLE_LINUX_MGMTSOCKET_HPP
#define BABLE_LINUX_MGMTSOCKET_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <uvw.hpp>
#include <memory>
#include <queue>

#include "Serializer/Serializer.hpp"
#include "Serializer/Deserializer.hpp"
#include "Log/Log.hpp"

#define BTPROTO_HCI   1
#define HCI_DEV_NONE  0xffff
#define HCI_CHANNEL_CONTROL 3

struct sockaddr_hci {
  sa_family_t     hci_family;
  unsigned short  hci_dev;
  unsigned short  hci_channel;
};

class MGMTSocket : public uvw::Emitter<MGMTSocket> {

public:
  MGMTSocket();

  bool bind_socket();

  bool send(const Serializer& ser);

  Deserializer receive();

  void set_writable();

  uvw::OSSocketHandle::Type& get_socket();

  ~MGMTSocket() override;

private:
  uvw::OSSocketHandle::Type m_socket;
  std::queue<Serializer> m_send_queue;
  bool m_writable;

};

#endif //BABLE_LINUX_MGMTSOCKET_HPP
