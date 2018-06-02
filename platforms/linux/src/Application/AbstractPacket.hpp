#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include "PacketUuid.hpp"
#include "../Format/Ascii/AsciiFormat.hpp"
#include "../Format/HCI/HCIFormat.hpp"
#include "../Format/MGMT/MGMTFormat.hpp"
#include "../Format/Flatbuffers/FlatbuffersFormat.hpp"

// FIXME: Need it to avoid cyclic dependency...
class PacketRouter;

namespace Packet {

  class AbstractPacket : public Loggable, public std::enable_shared_from_this<AbstractPacket> {

  public:
    static const uint16_t packet_code(Packet::Type type) {
      throw std::runtime_error("packet_code(Packet::Type) not defined.");
    };

    std::vector<uint8_t> to_bytes() const;
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

    void from_bytes(const std::shared_ptr<AbstractExtractor>& extractor);
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

    inline const Packet::Type get_current_type() const {
      return m_current_type;
    };

    inline const std::string get_uuid_request() const {
      return m_uuid_request;
    };

    inline const uint16_t get_controller_id() const {
      return m_controller_id;
    };

    inline const uint16_t get_connection_id() const {
      return m_connection_id;
    };

    inline const BaBLE::StatusCode get_status() const {
      return m_status;
    };

    virtual const PacketUuid get_uuid() const {
      return PacketUuid{
          m_current_type,
          m_controller_id,
          m_connection_id,
          m_packet_code
      };
    };

    inline const Packet::Id get_id() const {
      return m_id;
    };

    void set_uuid_request(const std::string& uuid_request);
    void set_controller_id(uint16_t controller_id);

    virtual void before_sent(const std::shared_ptr<PacketRouter>& router);

    const std::string stringify() const override;

  protected:
    AbstractPacket(Packet::Type initial_type, Packet::Type translated_type);

    void set_status(uint8_t native_status, bool compute_status = true);
    void import_status(const AbstractPacket& packet);

    template <class T>
    inline std::shared_ptr<T> shared_from(T* that)
    {
      return std::static_pointer_cast<T>(shared_from_this());
    }

    Packet::Id m_id;

    Packet::Type m_initial_type;
    Packet::Type m_translated_type;
    Packet::Type m_current_type;

    uint16_t m_controller_id;
    uint16_t m_connection_id;
    uint16_t m_packet_code;
    std::string m_uuid_request;

    std::string m_native_class;
    BaBLE::StatusCode m_status;

  private:
    void compute_bable_status();

    uint8_t m_native_status;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
