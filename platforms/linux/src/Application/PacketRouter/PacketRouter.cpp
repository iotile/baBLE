#include "PacketRouter.hpp"

PacketRouter::PacketRouter() = default;

PacketRouter& PacketRouter::add_callback(Packet::PacketUuid uuid, const CallbackFunction& callback) {
  TimePoint inserted_time = std::chrono::steady_clock::now();

  auto waiting_it = m_callbacks.find(uuid);
  if (waiting_it != m_callbacks.end()) {
    throw Exceptions::RuntimeErrorException("Command already in flight.");
  }

  m_callbacks.emplace(uuid, callback);
  m_timestamps.emplace(inserted_time, uuid); // To expire waiting packets without response

  return *this;
};

PacketRouter& PacketRouter::remove_callback(Packet::PacketUuid uuid) {
  for (auto expiration_it = m_timestamps.begin(); expiration_it != m_timestamps.end(); ++expiration_it) {
    if (expiration_it->second == uuid) {
      m_timestamps.erase(expiration_it);
      break;
    }
  }
  m_callbacks.erase(uuid);

  return *this;
};

std::shared_ptr<Packet::AbstractPacket> PacketRouter::route(std::shared_ptr<Packet::AbstractPacket> packet) {
  Packet::PacketUuid key = packet->get_uuid();

  if (packet->get_id() == Packet::Id::ErrorResponse) {
    auto error_packet = std::dynamic_pointer_cast<Packet::Errors::ErrorResponse>(packet);
    if(error_packet == nullptr) {
      throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to ErrorResponse packet.");
    }
    key.packet_code = error_packet->get_opcode();
  }

  auto callback_it = m_callbacks.find(key);
  if (callback_it != m_callbacks.end()) {
    CallbackFunction callback = callback_it->second;

    std::shared_ptr<Packet::AbstractPacket> packet_routed = callback(packet);

    remove_callback(key);
    return packet_routed;
  }

  return packet;
};

void PacketRouter::expire_waiting_packets(unsigned int expiration_duration_seconds) {
  if (m_timestamps.empty()) {
    return;
  }

  TimePoint current_time = std::chrono::steady_clock::now();
  auto expiration_start_time = current_time - std::chrono::seconds(expiration_duration_seconds);

  auto it_last_expired = m_timestamps.upper_bound(expiration_start_time);
  if (it_last_expired == m_timestamps.begin()) {
    LOG.debug("No packet to expire.", "PacketBuilder");
    return;
  }

  if (it_last_expired == m_timestamps.end()) {
    LOG.warning("Expiring all waiting packets", "PacketBuilder");

    LOG.debug("Waiting packets UUID: ", "PacketBuilder");
    for (auto& kv : m_timestamps) {
      LOG.debug("\t - " + std::to_string(kv.second.packet_code), "PacketBuilder");
    }

    m_callbacks.clear();
    m_timestamps.clear();
    return;
  }

  for (auto it = m_timestamps.begin(); it != it_last_expired; ++it) {
    LOG.warning("Expiring packet (" + std::to_string(it->second.packet_code) + ")", "PacketBuilder");
    m_callbacks.erase(it->second);
  }

  m_timestamps.erase(m_timestamps.begin(), it_last_expired);
};

void PacketRouter::start_expiration_timer(const std::shared_ptr<uvw::Loop>& loop, unsigned int expiration_duration_seconds) {
  std::shared_ptr<uvw::TimerHandle> expiration_timer = loop->resource<uvw::TimerHandle>();

  expiration_timer->on<uvw::TimerEvent>([this, expiration_duration_seconds](const uvw::TimerEvent&, uvw::TimerHandle& t) {
    expire_waiting_packets(expiration_duration_seconds);
  });

  expiration_timer->start(
      std::chrono::seconds(expiration_duration_seconds),
      std::chrono::seconds(expiration_duration_seconds)
  );
};

const std::string PacketRouter::stringify() const {
  std::stringstream result;

  result << "Router" << std::endl;
  for (auto& element : m_callbacks) {
    result << "\tType: ";
    switch (element.first.packet_type) {
      case Packet::Type::MGMT:
        result << "MGMT";
        break;
      case Packet::Type::HCI:
        result << "HCI";
        break;
      case Packet::Type::ASCII:
        result << "ASCII";
        break;
      case Packet::Type::FLATBUFFERS:
        result << "FLATBUFFERS";
        break;
      case Packet::Type::NONE:
        result << "NONE";
        break;
    }
    result << ", Packet code: " << element.first.packet_code << std::endl;
  }

  return result.str();
}
