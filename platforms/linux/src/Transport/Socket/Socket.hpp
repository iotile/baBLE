#ifndef BABLE_SOCKET_HPP
#define BABLE_SOCKET_HPP

#include <array>
#include <cstdint>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <vector>

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

  virtual void bind(uint16_t device, uint16_t channel);
  virtual void bind(const std::array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel);

  virtual void write(const std::vector<uint8_t>& data);
  virtual ssize_t read(std::vector<uint8_t>& data, bool peek);

  virtual void set_option(int level, int name, const void *val, socklen_t len);
  virtual void ioctl(uint64_t request, void* param);
  virtual void connect(const std::array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel);
  virtual void close();

  int get_raw();

  inline bool is_open() {
    return m_open;
  };
  inline bool is_binded() {
    return m_binded;
  };
  inline bool is_option_set() {
    return m_option_set;
  };
  inline bool is_connected() {
    return m_connected;
  };

protected:
  Socket();
  bool m_open;
  bool m_binded;
  bool m_option_set;
  bool m_connected;

private:
  sa_family_t m_domain;
  int m_socket;

};

#endif //BABLE_SOCKET_HPP
