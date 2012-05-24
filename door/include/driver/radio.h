// *************************************************************************************************
// Radio core access functions.
// *************************************************************************************************

#ifndef _DRIVER_RADIO_H_
#define _DRIVER_RADIO_H_

// *************************************************************************************************
// Include section
#include "project.h"
#include "logic/packet.h"

// *************************************************************************************************
// Define section
#define RADIO_IDLE (MARCSTATE == MARC_STATE_IDLE)

// *************************************************************************************************
// Prototype section
extern void radio_init(void);
extern void radio_receive(void);
extern void radio_retrieve(packet_t *packet);
extern void radio_transmit(packet_t *packet);

#endif /*_DRIVER_RADIO_H_*/

