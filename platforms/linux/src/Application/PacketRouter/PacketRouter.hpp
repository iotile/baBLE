#ifndef BABLE_LINUX_PACKETROUTER_HPP
#define BABLE_LINUX_PACKETROUTER_HPP

#include <chrono>
#include <functional>
#include <map>
#include <uvw.hpp>
#include "../Packets/Errors/ErrorResponse/ErrorResponse.hpp"
#include "../../Log/Loggable.hpp"
#include "../PacketUuid.hpp"

class PacketRouter : public Loggable {
  using CallbackFunction = std::function<std::shared_ptr<Packet::AbstractPacket>(std::shared_ptr<Packet::AbstractPacket>)>;
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

public:
  PacketRouter();

  PacketRouter& add_callback(Packet::PacketUuid uuid, const CallbackFunction& callback);
  PacketRouter& remove_callback(Packet::PacketUuid uuid);

  std::shared_ptr<Packet::AbstractPacket> route(std::shared_ptr<Packet::AbstractPacket> packet);

  void expire_waiting_packets(unsigned int expiration_duration_seconds);
  void start_expiration_timer(const std::shared_ptr<uvw::Loop>& loop, unsigned int expiration_duration_seconds);

  const std::string stringify() const override;

private:
  std::unordered_map<Packet::PacketUuid, CallbackFunction> m_callbacks;
  std::multimap<TimePoint, Packet::PacketUuid> m_timestamps;

};

#endif //BABLE_LINUX_PACKETROUTER_HPP
