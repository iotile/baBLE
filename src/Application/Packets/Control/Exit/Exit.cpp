#include "Exit.hpp"

using namespace std;

namespace Packet::Control {

  Exit::Exit(Packet::Type initial_type, Packet::Type translated_type)
      : AbstractPacket(initial_type, translated_type) {};

  void Exit::unserialize(AsciiFormatExtractor& extractor) {};

  void Exit::unserialize(FlatbuffersFormatExtractor& extractor) {};

}