#include "ReadRequest.hpp"
#include "./ReadResponse.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    ReadRequest::ReadRequest(uint16_t attribute_handle)
        : HostToControllerPacket(Packet::Id::ReadRequest, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::AttributeCode::ReadResponse;

      m_attribute_handle = attribute_handle;
    }

    void ReadRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Read*>();

      m_connection_handle = payload->connection_handle();
      m_attribute_handle = payload->attribute_handle();
    }

    vector<uint8_t> ReadRequest::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder.add(m_attribute_handle);

      return builder.build(Format::HCI::Type::AsyncData);
    }

    const string ReadRequest::stringify() const {
      stringstream result;

      result << "<ReadRequest> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle);

      return result.str();
    }

    void ReadRequest::prepare(const shared_ptr<PacketRouter>& router) {
      HostToControllerPacket::prepare(router);

      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      auto error_callback =
          [this](const shared_ptr<PacketRouter>& router, shared_ptr<AbstractPacket> packet) {
            return on_error_response_received(router, packet);
          };
      router->add_callback(error_uuid, shared_from(this), error_callback);
    }

    shared_ptr<AbstractPacket> ReadRequest::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                         const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("Response received", "ReadRequest");
      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      router->remove_callback(error_uuid);

      auto read_response_packet = dynamic_pointer_cast<Commands::ReadResponse>(packet);
      if (read_response_packet == nullptr) {
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't downcast AbstractPacket to ReadResponse packet.",
            m_uuid_request
        );
      }

      read_response_packet->set_uuid_request(m_uuid_request);
      read_response_packet->set_attribute_handle(m_attribute_handle);

      return read_response_packet;
    }

    shared_ptr<AbstractPacket> ReadRequest::on_error_response_received(const shared_ptr<PacketRouter>& router,
                                                                               const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("ErrorResponse received", "ReadRequest");
      PacketUuid response_uuid = get_response_uuid();
      router->remove_callback(response_uuid);

      shared_ptr<Commands::ReadResponse> response_packet = make_shared<Commands::ReadResponse>();
      response_packet->import_status(packet);
      response_packet->set_uuid_request(m_uuid_request);
      response_packet->set_attribute_handle(m_attribute_handle);
      response_packet->set_controller_id(m_controller_id);
      response_packet->set_connection_handle(m_connection_handle);

      return response_packet;
    }

  }

}