#include "ReadByTypeRequest.hpp"
#include "../../../../Exceptions/InvalidCommand/InvalidCommandException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    ReadByTypeRequest::ReadByTypeRequest(uint16_t starting_handle, uint16_t ending_handle, uint16_t uuid)
        : HostToControllerPacket(Packet::Id::ReadByTypeRequest, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::AttributeCode::ReadByTypeResponse;

      m_starting_handle = 0x0001;
      m_ending_handle = 0xFFFF;
      m_uuid = static_cast<uint16_t>(Format::HCI::UUID::GattCharacteristicDeclaration);
    }

    void ReadByTypeRequest::set_handles(uint16_t starting_handle, uint16_t ending_handle) {
      m_starting_handle = starting_handle;
      m_ending_handle = ending_handle;
    }

    vector<uint8_t> ReadByTypeRequest::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_starting_handle)
          .add(m_ending_handle)
          .add(m_uuid);

      return builder.build(Format::HCI::Type::AsyncData);
    }

    const string ReadByTypeRequest::stringify() const {
      stringstream result;

      result << "<ReadByTypeRequest> "
             << AbstractPacket::stringify() << ", "
             << "Starting handle: " << to_string(m_starting_handle) << ", "
             << "Ending handle: " << to_string(m_ending_handle) << ", "
             << "GATT UUID: " <<  to_string(m_uuid);

      return result.str();
    }

    void ReadByTypeRequest::prepare(const shared_ptr<PacketRouter>& router) {
      HostToControllerPacket::prepare(router);

      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      auto error_callback =
          [this](const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> packet) {
            return on_error_response_received(router, packet);
          };
      router->add_callback(error_uuid, shared_from(this), error_callback);
    }

    shared_ptr<Packet::AbstractPacket> ReadByTypeRequest::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                               const shared_ptr<Packet::AbstractPacket>& packet) {
      LOG.debug("Response received", "ReadByTypeRequest");
      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      router->remove_callback(error_uuid);

      return HostToControllerPacket::on_response_received(router, packet);
    }

    shared_ptr<Packet::AbstractPacket> ReadByTypeRequest::on_error_response_received(const shared_ptr<PacketRouter>& router,
                                                                                     const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("ErrorResponse received", "ReadByTypeRequest");
      PacketUuid response_uuid = get_response_uuid();
      router->remove_callback(response_uuid);

      import_status(*packet);

      return shared_from(this);
    }

  }

}