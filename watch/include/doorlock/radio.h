// *************************************************************************************************
// Doorlock common radio functions.
// *************************************************************************************************

#ifndef _DOORLOCK_RADIO_H_
#define _DOORLOCK_RADIO_H_

// *************************************************************************************************
// Include section

#include "project.h"
#include "doorlock/settings.h"

// *************************************************************************************************
// Defines section

// session information
typedef struct doorlock_radio_session
{
	u32 token;
	u16 door;
	u8 key[16];
} doorlock_radio_session_t;

// *************************************************************************************************
// Prototypes section

extern u8 doorlock_radio_session_create(doorlock_radio_session_t *session);

extern u8 doorlock_radio_request_seq(doorlock_radio_session_t *session, u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH]);
extern u8 doorlock_radio_request_key(doorlock_radio_session_t *session, u8 key[8]);
extern u8 doorlock_radio_request_pwd(doorlock_radio_session_t *session, u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH]);

extern u8 doorlock_radio_request_raw(u8 key[16]);

#endif /*_DOORLOCK_RADIO_H_*/
