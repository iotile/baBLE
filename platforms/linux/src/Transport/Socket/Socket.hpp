#ifndef BABLE_SOCKET_HPP
#define BABLE_SOCKET_HPP

#include <array>
#include <cstdint>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <vector>

// TODO: use this in HCISocket and MGMTSocket

struct sockaddr_hci {
  sa_family_t     hci_family;
  unsigned short  hci_dev;
  unsigned short  hci_channel;
};

typedef struct {
  uint8_t b[6];
} __attribute__((packed)) bdaddr_t;

struct sockaddr_l2 {
  sa_family_t l2_family;
  unsigned short l2_psm;
  bdaddr_t l2_bdaddr;
  unsigned short l2_cid;
  uint8_t l2_bdaddr_type;
};

class Socket {

public:
  Socket(sa_family_t domain, int type, int protocol);

  void bind(uint16_t device, uint16_t channel);
  void bind(const std::array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel);

  void write(const std::vector<uint8_t>& data);
  ssize_t read(std::vector<uint8_t>& data, bool peek = false);

  void set_option(int level, int name, const void *val, socklen_t len);
  void ioctl(uint64_t request, void* param);
  void connect(const std::array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel);
  void close();

  int get_raw();

private:
  sa_family_t m_domain;
  int m_socket;

};

#endif //BABLE_SOCKET_HPP
