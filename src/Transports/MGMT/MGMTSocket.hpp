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

class MGMTSocket : public AbstractSocket {

public:
  MGMTSocket();

  bool send(const std::vector<uint8_t>& data) override;
  void poll(std::shared_ptr<uvw::Loop> loop, CallbackFunction on_received) override;

  void set_writable(bool is_writable);

  ~MGMTSocket() override;

private:
  inline const size_t get_header_size() const override {
    return 6;
  }

  bool bind_socket();
  std::vector<uint8_t> receive();

  uvw::OSSocketHandle::Type m_socket;
  std::queue<std::vector<uint8_t>> m_send_queue;
  bool m_writable;

};

#endif //BABLE_LINUX_MGMTSOCKET_HPP
