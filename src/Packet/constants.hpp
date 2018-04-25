#ifndef BABLE_LINUX_MGMT_CODES_HPP
#define BABLE_LINUX_MGMT_CODES_HPP

namespace Packet {

  enum Type {
    MGMT,
    HCI,
    ASCII
  };

  namespace Commands {
    namespace MGMT {
      enum Code {
        GetMGMTInfo= 0x0001
      };
    }

    namespace Ascii {
      enum Code {
        GetMGMTInfo= 0x0001
      };
    }
  }

}

#endif //BABLE_LINUX_MGMT_CODES_HPP
