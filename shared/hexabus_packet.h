// Hexabus packet definition
#include <stdint.h>

// TODO make the structs packed

struct hxb_packet_header { // just the packet header. You can cast to a pointer to this in order to find out the packet type and flags
  uint8_t type;   // packet type -- see below
  uint8_t flags;  // flags -- see below
};

struct hxb_packet_req { // request a value
  uint8_t type;
  uint8_t flags;
  uint8_t vid;    // value ID
  uint16_t crc;   // CRC16-Kermit / Contiki's crc16_data()
};

struct hxb_packet_bool { // set or broadcast a boolean value
  uint8_t type;
  uint8_t flags;
  uint8_t vid;
  uint8_t value;  // 0 = false, anything else = true
  uint16_t crc;
};

struct hxb_packet_int { // set or broadcast an integer value
  uint8_t type;
  uint8_t flags;
  uint8_t vid;
  uint16_t value;
  uint16_t crc;
};

struct hxb_packet_event {
  uint8_t type;
  uint8_t flags;
  uint8_t eid;    // event ID
  uint16_t data;   // TODO this probably needs to be bigger, and more generic.
  uint16_t crc;
};

// Defines for bool-packet
#define HXB_TRUE  1
#define HXB_FALSE 0

// Defines for Packet types
#define HXB_PTYPE_BROADCAST 0x01  // Broadcast of a sensor value, either following a REQVALUE or periodically
#define HXB_PTYPE_SETVALUE  0x02  // Target device is requested to set [vid] := value
#define HXB_PTYPE_REQVALUE  0x03  // Target device is requested to broadcast the value with the ID vid
#define HXB_PTYPE_REPLY     0x04  // This is the reply to a REQVALUE - it looks the same as a BROADCAST, except ist not sent to a multicast address, but to the Device that sent the REQVALUE
#define HXB_PTYPE_EVENT     0x05  // An event packet.
//TODO HXB_PTYPE_ACK, HXB_PTYPE_RESET, HXB_PTYPE_HEARTBEAT

// Defines for flags
#define HXB_FLAG_IMPORTANT  0x01  // Packet should be ACKnowledged, otherwise it is retransmitted
#define HXB_FLAG_RETRANSMIT 0x02  // Packet is a retransmission
