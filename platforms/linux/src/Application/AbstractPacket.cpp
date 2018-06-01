#include "./AbstractPacket.hpp"
#include "./PacketRouter/PacketRouter.hpp"

namespace Packet {

  std::vector<uint8_t> AbstractPacket::to_bytes() const {
    switch(m_current_type) {
      case Packet::Type::MGMT:
      {
        MGMTFormatBuilder builder(m_controller_id);
        return serialize(builder);
      }

      case Packet::Type::HCI:
      {
        HCIFormatBuilder builder(m_controller_id);
        builder.set_connection_handle(m_connection_id);

        return serialize(builder);
      }

      case Packet::Type::ASCII:
      {
        std::string status_name = BaBLE::EnumNameStatusCode(m_status);

        AsciiFormatBuilder builder;
        builder
            .add("Controller ID", m_controller_id)
            .add("Connection ID", m_connection_id)
            .add("UUID request", m_uuid_request)
            .add("Status", status_name)
            .add("Native class", m_native_class)
            .add("Native status", m_native_status);
        return serialize(builder);
      }

      case Packet::Type::FLATBUFFERS:
      {
        FlatbuffersFormatBuilder builder(m_controller_id, m_uuid_request, m_native_class);
        builder.set_status(m_status, m_native_status);
        return serialize(builder);
      }

      case Packet::Type::NONE:
        throw std::runtime_error("Can't serialize NONE type packet.");
    }
  }

  void AbstractPacket::from_bytes(const std::shared_ptr<AbstractExtractor>& extractor) {
    try {
      m_controller_id = extractor->get_controller_id();
      m_connection_id = extractor->get_connection_id();

      switch(m_current_type) {
        case Packet::Type::MGMT:
        {
          auto mgmt_extractor = std::dynamic_pointer_cast<MGMTFormatExtractor>(extractor);
          if(mgmt_extractor == nullptr) {
            throw Exceptions::RuntimeErrorException("Can't import data into packet: wrong extractor provided.");
          }
          m_native_class = "MGMT";
          return unserialize(*mgmt_extractor);
        }

        case Packet::Type::HCI:
        {
          auto hci_extractor = std::dynamic_pointer_cast<HCIFormatExtractor>(extractor);
          if(hci_extractor == nullptr) {
            throw Exceptions::RuntimeErrorException("Can't import data into packet: wrong extractor provided.");
          }
          m_native_class = "HCI";
          return unserialize(*hci_extractor);
        }

        case Packet::Type::ASCII:
        {
          auto ascii_extractor = std::dynamic_pointer_cast<AsciiFormatExtractor>(extractor);
          if(ascii_extractor == nullptr) {
            throw Exceptions::RuntimeErrorException("Can't import data into packet: wrong extractor provided.");
          }
          m_uuid_request = ascii_extractor->get_uuid_request();
          m_native_class = "ASCII";
          return unserialize(*ascii_extractor);
        }

        case Packet::Type::FLATBUFFERS:
        {
          auto fb_extractor = std::dynamic_pointer_cast<FlatbuffersFormatExtractor>(extractor);
          if(fb_extractor == nullptr) {
            throw Exceptions::RuntimeErrorException("Can't import data into packet: wrong extractor provided.");
          }
          m_uuid_request = fb_extractor->get_uuid_request();
          m_native_class = "FLATBUFFERS";
          return unserialize(*fb_extractor);
        }

        case Packet::Type::NONE:
          throw Exceptions::RuntimeErrorException("Can't import data into NONE type packet.");
      }

    } catch (const Exceptions::WrongFormatException& err) {
      throw Exceptions::InvalidCommandException(err.stringify(), m_uuid_request);
    }
  }

  void AbstractPacket::before_sent(const std::shared_ptr<PacketRouter>& router) {
    m_current_type = m_translated_type;
  }

  const std::string AbstractPacket::stringify() const {
    std::string status_name = BaBLE::EnumNameStatusCode(m_status);

    AsciiFormatBuilder builder;
    builder
        .add("Controller ID", m_controller_id)
        .add("UUID request", m_uuid_request)
        .add("Packet code", m_packet_code)
        .add("Connection ID", m_connection_id)
        .add("Status", status_name)
        .add("Native class", m_native_class)
        .add("Native status", m_native_status);

    std::vector<uint8_t> data = serialize(builder);
    return AsciiFormat::bytes_to_string(data);
  }

  AbstractPacket::AbstractPacket(Packet::Type initial_type, Packet::Type translated_type) {
    m_id = Packet::Id::None;
    m_initial_type = initial_type;
    m_translated_type = translated_type;
    m_current_type = m_initial_type;
    m_controller_id = NON_CONTROLLER_ID;
    m_connection_id = 0;
    m_packet_code = 0;
    m_status = BaBLE::StatusCode::Success;
    m_native_status = 0x00;
  };

  void AbstractPacket::set_status(uint8_t native_status, bool compute_status) {
    m_native_status = native_status;
    if (compute_status) {
      compute_bable_status();
    }
  };

  void AbstractPacket::import_status(const AbstractPacket& packet) {
    m_status = packet.m_status;
    m_native_class = packet.m_native_class;
    m_native_status = packet.m_native_status;
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
        throw Exceptions::RuntimeErrorException("Can't convert status from current type.");
    }
  };

}