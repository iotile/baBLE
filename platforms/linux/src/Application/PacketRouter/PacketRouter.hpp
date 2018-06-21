#ifndef BABLE_LINUX_PACKETROUTER_HPP
#define BABLE_LINUX_PACKETROUTER_HPP

#include <map>
#include <uvw.hpp>
#include "../Packets/AbstractPacket.hpp"
#include "../../Log/Loggable.hpp"

class PacketRouter : public Loggable {

  using CallbackFunction = std::function<std::shared_ptr<Packet::AbstractPacket>(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<Packet::AbstractPacket>&)>;
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

public:
  static std::shared_ptr<Packet::AbstractPacket> route(const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet);

  PacketRouter();

  void add_callback(Packet::PacketUuid uuid, std::shared_ptr<Packet::AbstractPacket> packet, const CallbackFunction& callback);
  void remove_callback_timestamp(Packet::PacketUuid uuid);
  void remove_callback(Packet::PacketUuid uuid);

  void expire_waiting_packets(unsigned int expiration_duration_seconds);
  void start_expiration_timer(const std::shared_ptr<uvw::Loop>& loop, unsigned int expiration_duration_seconds);

  const std::string stringify() const override;

private:
  std::vector<std::tuple<Packet::PacketUuid, std::shared_ptr<Packet::AbstractPacket>, CallbackFunction>> m_callbacks;
  std::multimap<TimePoint, Packet::PacketUuid> m_timestamps;

};

#endif //BABLE_LINUX_PACKETROUTER_HPP
