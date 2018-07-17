#include "ReadByGroupTypeRequest.hpp"
#include "ReadByGroupTypeResponse.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    namespace Central {

      ReadByGroupTypeRequest::ReadByGroupTypeRequest(uint16_t starting_handle, uint16_t ending_handle, uint16_t uuid)
          : HostToControllerPacket(Packet::Id::ReadByGroupTypeRequest, final_type(), final_packet_code()) {
        m_response_packet_code = Format::HCI::AttributeCode::ReadByGroupTypeResponse;

        m_starting_handle = starting_handle;
        m_ending_handle = ending_handle;
        m_uuid = uuid;
      }

      void ReadByGroupTypeRequest::set_handles(uint16_t starting_handle, uint16_t ending_handle) {
        m_starting_handle = starting_handle;
        m_ending_handle = ending_handle;
      }

      vector<uint8_t> ReadByGroupTypeRequest::serialize(HCIFormatBuilder& builder) const {
        HostToControllerPacket::serialize(builder);

        builder
            .add(m_starting_handle)
            .add(m_ending_handle)
            .add(m_uuid);

        return builder.build(Format::HCI::Type::AsyncData);
      }

      const string ReadByGroupTypeRequest::stringify() const {
        stringstream result;

        result << "<ReadByGroupTypeRequest> "
               << AbstractPacket::stringify() << ", "
               << "Starting handle: " << to_string(m_starting_handle) << ", "
               << "Ending handle: " << to_string(m_ending_handle) << ", "
               << "GATT UUID: " << to_string(m_uuid);

        return result.str();
      }

      void ReadByGroupTypeRequest::prepare(const shared_ptr<PacketRouter>& router) {
        HostToControllerPacket::prepare(router);

        PacketUuid error_uuid = get_uuid();
        error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
        auto error_callback =
            [this](const shared_ptr<PacketRouter>& router, shared_ptr<AbstractPacket> packet) {
              return on_error_response_received(router, packet);
            };
        router->add_callback(error_uuid, shared_from(this), error_callback);
      }

      shared_ptr<AbstractPacket> ReadByGroupTypeRequest::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                                      const shared_ptr<AbstractPacket>& packet) {
        LOG.debug("Response received", "ReadByGroupTypeRequest");
        PacketUuid error_uuid = get_uuid();
        error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
        router->remove_callback(error_uuid);

        return HostToControllerPacket::on_response_received(router, packet);
      }

      shared_ptr<AbstractPacket> ReadByGroupTypeRequest::on_error_response_received(const shared_ptr<
          PacketRouter>& router, const shared_ptr<AbstractPacket>& packet) {
        LOG.debug("ErrorResponse received", "ReadByGroupTypeRequest");
        PacketUuid response_uuid = get_response_uuid();
        router->remove_callback(response_uuid);

        shared_ptr<Central::ReadByGroupTypeResponse> response_packet = make_shared<Central::ReadByGroupTypeResponse>();
        response_packet->import_status(packet);
        response_packet->set_uuid_request(m_uuid_request);
        response_packet->set_controller_id(m_controller_id);
        response_packet->set_connection_handle(m_connection_handle);

        return response_packet;
      }

    }

  }

}
