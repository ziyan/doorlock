#ifndef _LOGIC_PACKET_H_
#define _LOGIC_PACKET_H_

#include "project.h"

typedef struct packet
{
	u16 dst;
	u16 src;
	u8 type;
	u8 version;

	union
	{
		u8 raw[16];
		struct { u8 reserved[16]; } syn;
		struct { u32 token; u8 seq[12]; } seq;
		struct { u32 token; u8 seq[12]; } pwd;
		struct { u32 token; u8 key[8]; u8 reserved[4]; } key;
		struct { u32 token; u8 pattern[12]; } sec;
		struct { u32 token; u8 status; u8 reserved[11]; } ack;
	} payload;

	struct
	{
		u16 rssi:8;
		u16 lqi:7;
		u16 crc_ok:1;
	} status;

} packet_t;

#define PACKET_TRANSMIT_LENGTH			(sizeof(packet_t) - 2)
#define PACKET_RECEIVE_LENGTH			(sizeof(packet_t))
#define PACKET_IS_ENCRYPTED(packet)		(packet.type & 0x80)
#define PACKET_IS_SENT_BY_DOOR(packet)	(packet.type & 0x10)
#define PACKET_IS_SENT_BY_WATCH(packet)	(!(packet.type & 0x10))

#define PACKET_TYPE_SYN	(0x01u) // start a session, ack with a session token
#define PACKET_TYPE_SEQ	(0x82u) // send knock sequence
#define PACKET_TYPE_PWD	(0x83u) // set new knock sequence as new password
#define PACKET_TYPE_KEY	(0x84u) // update key (secure), set half a new key
#define PACKET_TYPE_RAW	(0x05u) // raw key exchange, used to pair with new door lock
#define PACKET_TYPE_SEC	(0x91u) // security token ack
#define PACKET_TYPE_ACK	(0x92u) // ack with a session token

#define PACKET_STATUS_ACK	(1u)
#define PACKET_STATUS_NCK	(0u)

#define PACKET_VERSION (0u)

#define PACKET_PATTERN_SEC "Hello World"

#endif /*_LOGIC_PACKET_H_*/
