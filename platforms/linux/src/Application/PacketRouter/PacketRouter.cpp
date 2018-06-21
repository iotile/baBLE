#include "PacketRouter.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/RuntimeError/RuntimeErrorException.hpp"

using namespace std;

PacketRouter::PacketRouter() = default;

void PacketRouter::add_callback(Packet::PacketUuid waiting_uuid, shared_ptr<Packet::AbstractPacket> packet, const CallbackFunction& callback) {
  TimePoint inserted_time = chrono::steady_clock::now();

  m_callbacks.emplace_back(waiting_uuid, packet, callback);
  m_timestamps.emplace(inserted_time, waiting_uuid); // To expire waiting packets without response
}

void PacketRouter::remove_callback_timestamp(Packet::PacketUuid uuid) {
  for (auto expiration_it = m_timestamps.begin(); expiration_it != m_timestamps.end(); ++expiration_it) {
    if (uuid == expiration_it->second) {
      m_timestamps.erase(expiration_it);
      break;
    }
  }
}

void PacketRouter::remove_callback(Packet::PacketUuid uuid) {
  for (auto callback_it = m_callbacks.begin(); callback_it != m_callbacks.end(); ++callback_it) {
    if (uuid == get<0>(*callback_it)) {
      m_callbacks.erase(callback_it);
      break;
    }
  }
}

shared_ptr<Packet::AbstractPacket> PacketRouter::route(const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> received_packet) {
  Packet::PacketUuid received_uuid = received_packet->get_uuid();

  for (auto callback_it = router->m_callbacks.begin(); callback_it != router->m_callbacks.end(); ++callback_it) {
    Packet::PacketUuid uuid = get<0>(*callback_it);
    if (received_uuid.match(uuid)) {
      shared_ptr<Packet::AbstractPacket> request_packet = get<1>(*callback_it);
      CallbackFunction callback = get<2>(*callback_it);

      router->remove_callback_timestamp(uuid);
      router->m_callbacks.erase(callback_it);

      shared_ptr<Packet::AbstractPacket> packet_routed = callback(router, received_packet);

      return packet_routed;
    }
  }

  return received_packet;
}

void PacketRouter::expire_waiting_packets(unsigned int expiration_duration_seconds) {
  if (m_timestamps.empty()) {
    return;
  }

  TimePoint current_time = chrono::steady_clock::now();
  auto expiration_start_time = current_time - chrono::seconds(expiration_duration_seconds);

  auto it_last_expired = m_timestamps.upper_bound(expiration_start_time);
  if (it_last_expired == m_timestamps.begin()) {
    LOG.debug("No packet to expire.", "PacketBuilder");
    return;
  }

  if (it_last_expired == m_timestamps.end()) {
    LOG.warning("Expiring all waiting packets", "PacketBuilder");

    LOG.debug("Waiting packets UUID: ", "PacketBuilder");
    for (auto& kv : m_timestamps) {
      LOG.debug("\t - " + to_string(kv.second.response_packet_code), "PacketBuilder");
    }

    m_callbacks.clear();
    m_timestamps.clear();
    return;
  }

  auto callback_it = m_callbacks.begin();
  for (auto it = m_timestamps.begin(); it != it_last_expired; ++it) {
    LOG.warning("Expiring packet (" + to_string(it->second.response_packet_code) + ")", "PacketBuilder");
    while (callback_it != m_callbacks.end()) {
      if (get<0>(*callback_it) == it->second) {
        m_callbacks.erase(callback_it);
        break;
      } else {
        ++callback_it;
      }
    }
  }

  m_timestamps.erase(m_timestamps.begin(), it_last_expired);
}

void PacketRouter::start_expiration_timer(const shared_ptr<uvw::Loop>& loop, unsigned int expiration_duration_seconds) {
  shared_ptr<uvw::TimerHandle> expiration_timer = loop->resource<uvw::TimerHandle>();

  expiration_timer->on<uvw::TimerEvent>([this, expiration_duration_seconds](const uvw::TimerEvent&, uvw::TimerHandle& t) {
    expire_waiting_packets(expiration_duration_seconds);
  });

  expiration_timer->start(
      chrono::seconds(expiration_duration_seconds),
      chrono::seconds(expiration_duration_seconds)
  );
}

const string PacketRouter::stringify() const {
  stringstream result;

  result << "Router" << endl;
  for (auto& element : m_callbacks) {
    Packet::PacketUuid uuid = get<0>(element);

    result << "\tType: ";
    switch (uuid.packet_type) {
      case Packet::Type::MGMT:
        result << "MGMT";
        break;
      case Packet::Type::HCI:
        result << "HCI";
        break;
      case Packet::Type::FLATBUFFERS:
        result << "FLATBUFFERS";
        break;
      case Packet::Type::NONE:
        result << "NONE";
        break;
    }
    result << ", Packet code: " << uuid.response_packet_code << endl;
  }

  return result.str();
}
