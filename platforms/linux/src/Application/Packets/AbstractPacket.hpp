#ifndef BABLE_LINUX_ABSTRACTPACKET_HPP
#define BABLE_LINUX_ABSTRACTPACKET_HPP

#include "PacketUuid.hpp"
#include "Log/Loggable.hpp"
#include "Format/HCI/HCIFormat.hpp"
#include "Format/MGMT/MGMTFormat.hpp"
#include "Format/Flatbuffers/FlatbuffersFormat.hpp"

// FIXME: Need it to avoid cyclic dependency...
class PacketRouter;

namespace Packet {

  class AbstractPacket : public Loggable, public std::enable_shared_from_this<AbstractPacket> {

  public:
    std::vector<uint8_t> to_bytes() const;
    virtual std::vector<uint8_t> serialize(MGMTFormatBuilder& builder) const {
      throw std::runtime_error("serialize(MGMTFormatBuilder&) not defined.");
    };
    virtual std::vector<uint8_t> serialize(HCIFormatBuilder& builder) const {
      throw std::runtime_error("serialize(HCIFormatBuilder&) not defined.");
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
    virtual void unserialize(FlatbuffersFormatExtractor& extractor) {
      throw std::runtime_error("unserialize() not defined for FlatbuffersFormatExtractor.");
    };

    inline const Packet::Type get_type() const {
      return m_current_type;
    };

    inline const std::string get_uuid_request() const {
      return m_uuid_request;
    };

    inline const uint16_t get_controller_id() const {
      return m_controller_id;
    };

    inline const uint16_t get_connection_handle() const {
      return m_connection_handle;
    };

    inline const BaBLE::StatusCode get_status() const {
      return m_status;
    };

    virtual const PacketUuid get_uuid() const;

    inline const Packet::Id get_id() const {
      return m_id;
    };

    void set_uuid_request(const std::string& uuid_request);
    void set_controller_id(uint16_t controller_id);
    void set_connection_handle(uint16_t connection_handle);
    void set_status(uint8_t native_status, bool compute_status = true, const std::string& native_class = "");

    void translate();
    virtual void prepare(const std::shared_ptr<PacketRouter>& router) = 0;

    void import_status(const std::shared_ptr<AbstractPacket>& packet);

    const std::string stringify() const override;

  protected:
    AbstractPacket(Packet::Id id, Packet::Type initial_type, Packet::Type final_type, uint16_t packet_code);

    template <class T>
    inline std::shared_ptr<T> shared_from(T* that) {
      return std::static_pointer_cast<T>(shared_from_this());
    }

    Packet::Id m_id;

    Packet::Type m_final_type;
    Packet::Type m_current_type;
    uint16_t m_packet_code;

    uint16_t m_controller_id;
    uint16_t m_connection_handle;
    std::string m_uuid_request;

    std::string m_native_class;
    BaBLE::StatusCode m_status;

  private:
    void compute_bable_status();

    uint8_t m_native_status;

  };

}

#endif //BABLE_LINUX_ABSTRACTPACKET_HPP
