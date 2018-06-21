#include "WriteRequest.hpp"
#include "./WriteResponse.hpp"
#include "../../../../Exceptions/RuntimeError/RuntimeErrorException.hpp"
#include "../../../../utils/string_formats.hpp"

using namespace std;

namespace Packet {

  namespace Commands {

    WriteRequest::WriteRequest(uint16_t attribute_handle, vector<uint8_t> data)
        : HostToControllerPacket(Packet::Id::WriteRequest, final_type(), final_packet_code()) {
      m_response_packet_code = Format::HCI::AttributeCode::WriteResponse;

      m_attribute_handle = attribute_handle;
      m_data_to_write = move(data);
    }

    void WriteRequest::unserialize(FlatbuffersFormatExtractor& extractor) {
      auto payload = extractor.get_payload<const BaBLE::Write*>();

      m_connection_id = payload->connection_handle();
      m_attribute_handle = payload->attribute_handle();
      auto raw_data_to_write = payload->value();
      m_data_to_write.assign(raw_data_to_write->begin(), raw_data_to_write->end());
    }

    vector<uint8_t> WriteRequest::serialize(HCIFormatBuilder& builder) const {
      HostToControllerPacket::serialize(builder);

      builder
          .add(m_attribute_handle)
          .add(m_data_to_write);

      return builder.build(Format::HCI::Type::AsyncData);
    }

    const string WriteRequest::stringify() const {
      stringstream result;

      result << "<WriteRequest> "
             << AbstractPacket::stringify() << ", "
             << "Attribute handle: " << to_string(m_attribute_handle) << ", "
             << "Data to write: " << Utils::format_bytes_array(m_data_to_write);

      return result.str();
    }

    void WriteRequest::prepare(const shared_ptr<PacketRouter>& router) {
      HostToControllerPacket::prepare(router);

      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      auto error_callback =
          [this](const shared_ptr<PacketRouter>& router, shared_ptr<Packet::AbstractPacket> packet) {
            return on_error_response_received(router, packet);
          };
      router->add_callback(error_uuid, shared_from(this), error_callback);
    }

    shared_ptr<Packet::AbstractPacket> WriteRequest::on_response_received(const shared_ptr<PacketRouter>& router,
                                                                          const shared_ptr<Packet::AbstractPacket>& packet) {
      LOG.debug("Response received", "WriteRequest");
      PacketUuid error_uuid = get_uuid();
      error_uuid.response_packet_code = Format::HCI::AttributeCode::ErrorResponse;
      router->remove_callback(error_uuid);

      auto write_response_packet = dynamic_pointer_cast<Packet::Commands::WriteResponse>(packet);
      if (write_response_packet == nullptr) {
        throw Exceptions::RuntimeErrorException("Can't downcast AbstractPacket to WriteResponse packet.", m_uuid_request);
      }

      write_response_packet->set_uuid_request(m_uuid_request);
      write_response_packet->set_attribute_handle(m_attribute_handle);

      return write_response_packet;
    }

    shared_ptr<Packet::AbstractPacket> WriteRequest::on_error_response_received(const shared_ptr<PacketRouter>& router,
                                                                                const shared_ptr<AbstractPacket>& packet) {
      LOG.debug("ErrorResponse received", "WriteRequest");
      PacketUuid response_uuid = get_response_uuid();
      router->remove_callback(response_uuid);

      shared_ptr<Packet::Commands::WriteResponse> response_packet = make_shared<Packet::Commands::WriteResponse>();
      response_packet->import_status(packet);
      response_packet->set_uuid_request(m_uuid_request);
      response_packet->set_attribute_handle(m_attribute_handle);
      response_packet->set_controller_id(m_controller_id);
      response_packet->set_connection_id(m_connection_id);

      return response_packet;
    }

  }

}