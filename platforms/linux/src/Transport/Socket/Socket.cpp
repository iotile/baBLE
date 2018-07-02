#include <unistd.h>
#include "Socket.hpp"
#include "Exceptions/BaBLEException.hpp"


using namespace std;

Socket::Socket(sa_family_t domain, int type, int protocol) {
  m_domain = domain;
  m_socket = ::socket(m_domain, type, protocol);

  if (m_socket < 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while creating the socket: " + string(strerror(errno))
    );
  }
}

void Socket::bind(uint16_t device, uint16_t channel){
  struct sockaddr_hci addr {
      m_domain,
      device,
      channel
  };

  int result = ::bind(m_socket, (struct sockaddr*) &addr, sizeof(addr));
  if (result != 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while binding the socket: " + string(strerror(errno))
    );
  }
}

void Socket::bind(const array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel){
  bdaddr_t bdaddr{};
  copy(address.begin(), address.end(), begin(bdaddr.b));

  struct sockaddr_l2 addr{
      m_domain,
      0,
      bdaddr,
      channel,
      address_type
  };

  int result = ::bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
  if (result != 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while binding the socket: " + string(strerror(errno))
    );
  }
}

void Socket::write(const vector<uint8_t>& data){
  ssize_t result = ::write(m_socket, data.data(), data.size());
  if (result < 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error occured while sending the packet: " +  string(strerror(errno))
    );
  }
}

ssize_t Socket::read(vector<uint8_t>& data, bool peek) {
  int flags = 0;
  if (peek) {
    flags |= MSG_PEEK;
  }

  ssize_t result = ::recv(m_socket, data.data(), data.size(), flags);
  if (result != data.size()) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while reading the socket: " + string(strerror(errno))
    );
  }
}

void Socket::set_option(int level, int name, const void *val, socklen_t len) {
  int result = ::setsockopt(m_socket, level, name, val, len);
  if (result != 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while setting options on the socket: " + string(strerror(errno))
    );
  }
}

void Socket::ioctl(uint64_t request, void* param) {
  int result = ::ioctl(m_socket, request, param);
  if (result != 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while sending ioctl request: " + string(strerror(errno))
    );
  }
}

void Socket::connect(const array<uint8_t, 6>& address, uint8_t address_type, uint16_t channel) {
  bdaddr_t bdaddr{};
  copy(address.begin(), address.end(), begin(bdaddr.b));

  struct sockaddr_l2 addr{
      m_domain,
      0,
      bdaddr,
      channel,
      address_type
  };

  int result = ::connect(m_socket, (struct sockaddr *)&addr, sizeof(addr)) == 0;
  if (result != 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while connecting the socket: " + string(strerror(errno))
    );
  }
}

void Socket::close() {
  int result = ::close(m_socket);
  if (result != 0) {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::SocketError,
        "Error while closing the socket: " + string(strerror(errno))
    );
  }
}

int Socket::get_raw() {
  return m_socket;
}
