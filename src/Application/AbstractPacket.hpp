#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include "Packets/constants.hpp"
#include "ResponseId.hpp"
#include "../Format/Ascii/AsciiFormat.hpp"
#include "../Format/HCI/HCIFormat.hpp"
#include "../Format/MGMT/MGMTFormat.hpp"
#include "../Format/Flatbuffers/FlatbuffersFormat.hpp"

namespace Packet {

  class AbstractPacket : public Loggable {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      throw std::runtime_error("packet_code(Packet::Type) not defined.");
    };

    virtual std::vector<uint8_t> to_bytes() const {
      switch(m_current_type) {
        case Packet::Type::MGMT:
        {
          MGMTFormatBuilder builder(m_controller_id);
          return serialize(builder);
        }

        case Packet::Type::HCI:
        {
          HCIFormatBuilder builder(m_controller_id);
          return serialize(builder);
        }

        case Packet::Type::ASCII:
        {
          std::string status_name = BaBLE::EnumNameStatusCode(m_status);

          AsciiFormatBuilder builder;
          builder
              .add("Controller ID", m_controller_id)
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
    };

    virtual void from_bytes(const std::shared_ptr<AbstractExtractor>& extractor) {
      m_controller_id = extractor->get_controller_id();

      switch(m_current_type) {
        case Packet::Type::MGMT:
        {
          auto mgmt_extractor = std::dynamic_pointer_cast<MGMTFormatExtractor>(extractor);
          if(mgmt_extractor == nullptr) {
            throw std::runtime_error("Can't import data into packet: wrong extractor provided.");
          }
          m_native_class = "MGMT";
          return unserialize(*mgmt_extractor);
        }

        case Packet::Type::HCI:
        {
          auto hci_extractor = std::dynamic_pointer_cast<HCIFormatExtractor>(extractor);
          if(hci_extractor == nullptr) {
            throw std::runtime_error("Can't import data into packet: wrong extractor provided.");
          }
          m_native_class = "HCI";
          return unserialize(*hci_extractor);
        }

        case Packet::Type::ASCII:
        {
          auto ascii_extractor = std::dynamic_pointer_cast<AsciiFormatExtractor>(extractor);
          if(ascii_extractor == nullptr) {
            throw std::runtime_error("Can't import data into packet: wrong extractor provided.");
          }
          m_uuid_request = ascii_extractor->get_uuid_request();
          m_native_class = "ASCII";
          return unserialize(*ascii_extractor);
        }

        case Packet::Type::FLATBUFFERS:
        {
          auto fb_extractor = std::dynamic_pointer_cast<FlatbuffersFormatExtractor>(extractor);
          if(fb_extractor == nullptr) {
            throw std::runtime_error("Can't import data into packet: wrong extractor provided.");
          }
          m_uuid_request = fb_extractor->get_uuid_request();
          m_native_class = "FLATBUFFERS";
          return unserialize(*fb_extractor);
        }

        case Packet::Type::NONE:
          throw std::runtime_error("Can't import data into NONE type packet.");
      }
    };

    virtual std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const {
      throw std::runtime_error("serialize(MGMTFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const {
      throw std::runtime_error("serialize(HCIFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(AsciiFormatBuilder& builder) const {
      throw std::runtime_error("serialize(AsciiFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(FlatbuffersFormatBuilder& builder) const {
      throw std::runtime_error("serialize(FlatbuffersFormatBuilder&) not defined.");
    };

    virtual void unserialize(MGMTFormatExtractor& extractor) {
      throw std::runtime_error("unserialize() not defined for MGMTFormatExtractor.");
    };
    virtual void unserialize(HCIFormatExtractor& extractor) {
      throw std::runtime_error("unserialize() not defined for HCIFormatExtractor.");
    };
    virtual void unserialize(AsciiFormatExtractor& extractor) {
      throw std::runtime_error("unserialize() not defined for AsciiFormatExtractor.");
    };
    virtual void unserialize(FlatbuffersFormatExtractor& extractor) {
      throw std::runtime_error("unserialize() not defined for FlatbuffersFormatExtractor.");
    };

    const Packet::Type get_current_type() const {
      return m_current_type;
    };

    const std::string get_uuid_request() const {
      return m_uuid_request;
    };

    const uint16_t get_controller_id() const {
      return m_controller_id;
    };

    const BaBLE::StatusCode get_status() const {
      return m_status;
    };

    const BaBLE::Payload get_id() const {
      return m_id;
    };

    virtual void translate() {
      m_current_type = m_current_type == m_initial_type ? m_translated_type : m_initial_type;
      after_translate();
    };

    virtual std::vector<ResponseId> expected_response_ids() {
      return {};
    };
    virtual bool on_response_received(Packet::Type packet_type, const std::shared_ptr<AbstractExtractor>& extractor) {
      throw std::runtime_error("on_response_received(Packet::Type, vector<uint8_t>&) callback not defined.");
    };

    const std::string stringify() const override {
      std::string status_name = BaBLE::EnumNameStatusCode(m_status);

      AsciiFormatBuilder builder;
      builder
          .add("Controller ID", m_controller_id)
          .add("UUID request", m_uuid_request)
          .add("Status", status_name)
          .add("Native class", m_native_class)
          .add("Native status", m_native_status);

      std::vector<uint8_t> data = serialize(builder);
      return AsciiFormat::bytes_to_string(data);
    };

    virtual ~AbstractPacket() = default;

  protected:
    AbstractPacket(Packet::Type initial_type, Packet::Type translated_type) {
      m_id = BaBLE::Payload::NONE;
      m_initial_type = initial_type;
      m_translated_type = translated_type;
      m_current_type = m_initial_type;
      m_controller_id = NON_CONTROLLER_ID;
      m_status = BaBLE::StatusCode::Success;
      m_native_status = 0x00;
    };

    virtual void after_translate() {};

    void set_status(uint8_t native_status) {
      m_native_status = native_status;
      compute_bable_status();
    };

    void import_status(AbstractPacket& packet) {
      m_status = packet.m_status;
      m_native_class = packet.m_native_class;
      m_native_status = packet.m_native_status;
    }

    BaBLE::Payload m_id;

    Packet::Type m_initial_type;
    Packet::Type m_translated_type;
    Packet::Type m_current_type;

    uint16_t m_controller_id;
    std::string m_uuid_request;

    std::string m_native_class;
    BaBLE::StatusCode m_status;

  private:
    void compute_bable_status() {
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
          throw std::runtime_error("Can't convert status from current type.");
      }
    };

    uint8_t m_native_status;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
