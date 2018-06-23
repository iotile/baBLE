#ifndef BABLE_LINUX_PACKETROUTER_HPP
#define BABLE_LINUX_PACKETROUTER_HPP

#include <chrono>
#include <map>
#include <uv.h>
#include "../Packets/AbstractPacket.hpp"
#include "../../Log/Loggable.hpp"

class PacketRouter : public Loggable {

  using CallbackFunction = std::function<std::shared_ptr<Packet::AbstractPacket>(const std::shared_ptr<PacketRouter>& router, const std::shared_ptr<Packet::AbstractPacket>&)>;
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

public:
  static std::shared_ptr<Packet::AbstractPacket> route(const std::shared_ptr<PacketRouter>& router, std::shared_ptr<Packet::AbstractPacket> packet);
  static void on_expiration_timer_tic(uv_timer_t* handle);

  explicit PacketRouter(uv_loop_t* loop);

  void add_callback(Packet::PacketUuid uuid, std::shared_ptr<Packet::AbstractPacket> packet, const CallbackFunction& callback);
  void remove_callback_timestamp(Packet::PacketUuid uuid);
  void remove_callback(Packet::PacketUuid uuid);

  void expire_waiting_packets(uint64_t expiration_duration_seconds);
  void start_expiration_timer(uint64_t expiration_duration_seconds);

  const std::string stringify() const override;

private:
  std::unique_ptr<uv_timer_t> m_expiration_timer;

  std::vector<std::tuple<Packet::PacketUuid, std::shared_ptr<Packet::AbstractPacket>, CallbackFunction>> m_callbacks;
  std::multimap<TimePoint, Packet::PacketUuid> m_timestamps;

};

#endif //BABLE_LINUX_PACKETROUTER_HPP
