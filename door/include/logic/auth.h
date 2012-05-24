#ifndef _LOGIC_AUTH_H_
#define _LOGIC_AUTH_H_

#include "project.h"
#include "logic/packet.h"

extern void auth_init(void);
extern u8 auth_process_packet(packet_t *packet);

#endif /*_LOGIC_AUTH_H_*/
