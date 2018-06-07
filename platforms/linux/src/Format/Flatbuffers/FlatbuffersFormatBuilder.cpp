#include "FlatbuffersFormatBuilder.hpp"

FlatbuffersFormatBuilder::FlatbuffersFormatBuilder(uint16_t controller_id, const std::string& uuid_request, const std::string& native_class)
  : flatbuffers::FlatBufferBuilder::FlatBufferBuilder(0) {
  m_controller_id = controller_id;
  m_native_class = native_class;
  m_uuid_request = uuid_request;
  m_status = BaBLE::StatusCode::Success;
  m_native_status = 0x00;
}

FlatbuffersFormatBuilder& FlatbuffersFormatBuilder::set_status(BaBLE::StatusCode status, uint8_t native_status) {
  m_status = status;
  m_native_status = native_status;

  return *this;
}
