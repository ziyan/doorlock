#ifndef _DOORLOCK_PACKET_H_
#define _DOORLOCK_PACKET_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Defines section

// doorlock packet format
typedef struct packet {
	u16 dst;	// packet destination, door id
	u16 src;	// packet source, watch id
	u8 type;	// packet type, see type definition
	u8 version;	// packet version
	union {
		u8 raw[16];												// raw packet, used for key exchange
		struct { u8 reserved[16]; } syn;						// syn packet, used for requesting a session token
		struct { u32 token; u8 seq[12]; } seq;					// seq packet, used for sending unlock sequence code
		struct { u32 token; u8 seq[12]; } pwd;					// pwd packet, used for changing unlock sequence code
		struct { u32 token; u8 key[8]; u8 reserved[4]; } key;	// key packet, used for renew encryption key
		struct { u32 token; u8 pattern[12]; } sec;				// sec packet, door's response to syn packet
		struct { u32 token; u8 status; u8 reserved[11]; } ack;	// ack packet, door's response
	} payload;
	struct {
		u16 rssi:8;
		u16 crc_ok:1;
		u16 lqi:7;
	} status;	// radio status, appended by firmware
} packet_t;

#define PACKET_TRANSMIT_LENGTH (sizeof(packet_t) - 2)
#define PACKET_RECEIVE_LENGTH (sizeof(packet_t))

#define PACKET_IS_ENCRYPTED(packet) (packet.type & 0x80)
#define PACKET_IS_SENT_BY_DOOR(packet) (packet.type & 0x10)
#define PACKET_IS_SENT_BY_WATCH(packet) (!(packet.type & 0x10))

#define PACKET_TYPE_SYN (0x01u) // start a session, ack with a session token
#define PACKET_TYPE_SEQ (0x82u) // send knock sequence
#define PACKET_TYPE_PWD (0x83u) // set new knock sequence as new password
#define PACKET_TYPE_KEY (0x84u) // update key (secure), set half a new key
#define PACKET_TYPE_RAW (0x05u) // raw key exchange, used to pair with new door lock
#define PACKET_TYPE_SEC (0x91u) // security token ack
#define PACKET_TYPE_ACK (0x92u) // ack with a session token

#define PACKET_VERSION (0u)		// packet version is currently 0

// pattern used in sec packet to verify that door really have the correct AES key
#define PACKET_PATTERN_SEC "Hello World"

#endif /*_DOORLOCK_PACKET_H_*/
