#include "Application/Packets/AbstractPacket.hpp"
#include "Application/PacketRouter/PacketRouter.hpp"
#include "utils/stream_formats.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

namespace Packet {

  vector<uint8_t> AbstractPacket::to_bytes() const {
    switch(m_current_type) {
      case Packet::Type::MGMT:
      {
        MGMTFormatBuilder builder(m_controller_id);
        return serialize(builder);
      }

      case Packet::Type::HCI:
      {
        HCIFormatBuilder builder(m_controller_id);
        builder.set_connection_handle(m_connection_handle);

        return serialize(builder);
      }

      case Packet::Type::FLATBUFFERS:
      {
        FlatbuffersFormatBuilder builder(m_controller_id, m_uuid_request, m_native_class);
        builder.set_status(m_status, m_native_status);
        return serialize(builder);
      }

      case Packet::Type::NONE:
        throw runtime_error("Can't serialize NONE type packet.");
    }
  }

  void AbstractPacket::from_bytes(const shared_ptr<AbstractExtractor>& extractor) {
    try {
      m_controller_id = extractor->get_controller_id();
      m_connection_handle = extractor->get_connection_handle();

      switch(m_current_type) {
        case Packet::Type::MGMT:
        {
          auto mgmt_extractor = dynamic_pointer_cast<MGMTFormatExtractor>(extractor);
          if(mgmt_extractor == nullptr) {
            throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't import data into packet: wrong extractor provided.");
          }
          return unserialize(*mgmt_extractor);
        }

        case Packet::Type::HCI:
        {
          auto hci_extractor = dynamic_pointer_cast<HCIFormatExtractor>(extractor);
          if(hci_extractor == nullptr) {
            throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't import data into packet: wrong extractor provided.");
          }
          return unserialize(*hci_extractor);
        }

        case Packet::Type::FLATBUFFERS:
        {
          auto fb_extractor = dynamic_pointer_cast<FlatbuffersFormatExtractor>(extractor);
          if(fb_extractor == nullptr) {
            throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't import data into packet: wrong extractor provided.");
          }
          m_uuid_request = fb_extractor->get_uuid_request();
          return unserialize(*fb_extractor);
        }

        case Packet::Type::NONE:
          throw Exceptions::BaBLEException(BaBLE::StatusCode::Failed, "Can't import data into NONE type packet.");
      }

    } catch (Exceptions::BaBLEException& err) {
      err.set_uuid_request(m_uuid_request);
      throw;
    }
  }

  void AbstractPacket::translate() {
    m_current_type = m_final_type;
  }

  const string AbstractPacket::stringify() const {
    string status_name = BaBLE::EnumNameStatusCode(m_status);

    stringstream result;
    result << "Controller ID: " << to_string(m_controller_id) << ", "
           << "UUID request: " <<  m_uuid_request << ", "
           << "Packet code: " <<  HEX(m_packet_code) << ", "
           << "Connection ID: " <<  HEX(m_connection_handle) << ", "
           << "Status: " << status_name << ", "
           << "Native class: " <<  m_native_class << ", "
           << "Native status: " <<  HEX(m_native_status);

    return result.str();
  }

  AbstractPacket::AbstractPacket(Packet::Id id, Packet::Type initial_type, Packet::Type final_type, uint16_t packet_code) {
    m_id = id;
    m_current_type = initial_type;
    m_final_type = final_type;
    m_packet_code = packet_code;

    m_controller_id = NON_CONTROLLER_ID;
    m_connection_handle = 0;
    m_status = BaBLE::StatusCode::Success;
    m_native_status = 0x00;
  };

  const PacketUuid AbstractPacket::get_uuid() const {
    return PacketUuid{
        m_current_type,
        m_controller_id,
        m_connection_handle,
        m_packet_code
    };
  };

  void AbstractPacket::set_status(uint8_t native_status, bool compute_status, const std::string& native_class) {
    m_native_status = native_status;

    if (compute_status) {
      compute_bable_status();
    }

    if (!native_class.empty()) {
      m_native_class = native_class;
    }
  }

  void AbstractPacket::set_uuid_request(const string& uuid_request) {
    m_uuid_request = uuid_request;
  }

  void AbstractPacket::set_controller_id(uint16_t controller_id) {
    m_controller_id = controller_id;
  }

  void AbstractPacket::set_connection_handle(uint16_t connection_handle) {
    m_connection_handle = connection_handle;
  }

  void AbstractPacket::import_status(const shared_ptr<AbstractPacket>& packet) {
    m_status = packet->m_status;
    m_native_class = packet->m_native_class;
    m_native_status = packet->m_native_status;
  }

  void AbstractPacket::compute_bable_status() {
    switch(m_current_type) {

      case Type::MGMT:
        switch (m_native_status) {
          case Format::MGMT::Success:
            m_status = BaBLE::StatusCode::Success;
            break;

          case Format::MGMT::Busy:
          case Format::MGMT::Rejected:
          case Format::MGMT::NotSupported:
          case Format::MGMT::AlreadyPaired:
            m_status = BaBLE::StatusCode::Rejected;
            break;

          case Format::MGMT::PermissionDenied:
            m_status = BaBLE::StatusCode::Denied;
            break;

          case Format::MGMT::Cancelled:
            m_status = BaBLE::StatusCode::Cancelled;
            break;

          case Format::MGMT::NotPowered:
            m_status = BaBLE::StatusCode::NotPowered;
            break;

          case Format::MGMT::Failed:
          case Format::MGMT::ConnectFailed:
          case Format::MGMT::AuthenticationFailed:
          case Format::MGMT::RFKilled:
            m_status = BaBLE::StatusCode::Failed;
            break;

          case Format::MGMT::UnknownCommand:
          case Format::MGMT::InvalidParameters:
          case Format::MGMT::InvalidIndex:
            m_status = BaBLE::StatusCode::InvalidCommand;
            break;

          case Format::MGMT::NotConnected:
            m_status = BaBLE::StatusCode::NotConnected;
            break;

          default:
            m_status = BaBLE::StatusCode::Unknown;
            break;
        }
        break;

      case Type::HCI:
        switch (m_native_status) {
          case Format::HCI::Success:
            m_status = BaBLE::StatusCode::Success;
            break;

          case Format::HCI::ConnectionRejectedLimitedResources:
          case Format::HCI::ConnectionRejectedSecurityReasons:
          case Format::HCI::ConnectionRejectedUnacceptableBDADDR:
          case Format::HCI::HostBusyPairing:
          case Format::HCI::ControllerBusy:
            m_status = BaBLE::StatusCode::Rejected;
            break;

          case Format::HCI::CommandDisallowed:
          case Format::HCI::PairingNotAllowed:
            m_status = BaBLE::StatusCode::Denied;
            break;

          case Format::HCI::OperationCancelledHost:
            m_status = BaBLE::StatusCode::Cancelled;
            break;

          case Format::HCI::HardwareFailure:
          case Format::HCI::AuthenticationFailed:
          case Format::HCI::ConnectionAlreadyExists:
          case Format::HCI::ConnectionFailedEstablished:
          case Format::HCI::MACConnectionFailed:
            m_status = BaBLE::StatusCode::Failed;
            break;

          case Format::HCI::UnknownHCICommand:
          case Format::HCI::UnknownConnectionIdentifier:
          case Format::HCI::InvalidHCICommandParameters:
          case Format::HCI::UnacceptableConnectionParameters:
            m_status = BaBLE::StatusCode::InvalidCommand;
            break;

          default:
            m_status = BaBLE::StatusCode::Unknown;
            break;
        }
        break;

      default:
        throw Exceptions::BaBLEException(
            BaBLE::StatusCode::Failed,
            "Can't convert status from current type: " + to_string(m_native_status)
        );
    }
  };

}
