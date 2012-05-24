// *************************************************************************************************
// Doorlock common radio functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "project.h"

#include <string.h>

// drivers
#include "driver/idle.h"
#include "driver/timer.h"
#include "driver/radio.h"
#include "driver/aes.h"

// doorlock
#include "doorlock/radio.h"
#include "doorlock/sequence.h"
#include "doorlock/db.h"
#include "doorlock/packet.h"
#include "doorlock/error.h"

// *************************************************************************************************
// Prototypes section

u8 doorlock_radio_request(doorlock_radio_session_t *session, packet_t *packet);

u8 doorlock_radio_session_create(doorlock_radio_session_t *session);
u8 doorlock_radio_session_request(doorlock_radio_session_t *session, packet_t *packet);

u8 doorlock_radio_request_seq(doorlock_radio_session_t *session, u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH]);
u8 doorlock_radio_request_key(doorlock_radio_session_t *session, u8 key[8]);
u8 doorlock_radio_request_pwd(doorlock_radio_session_t *session, u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH]);

u8 doorlock_radio_request_raw(u8 key[16]);

void doorlock_radio_timer(void);


// *************************************************************************************************
// Global variable section

volatile u8 doorlock_radio_timeout = 0;

// *************************************************************************************************
// @fn          doorlock_radio_request
// @brief       try to send a packet and receive response
// @param       packet - packet to be sent, session - session information
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_request(doorlock_radio_session_t *session, packet_t *packet)
{
	u8 trials = DOORLOCK_RADIO_TRIALS;
	u8 type = packet->type;
	
	// assume that radio has not been started
	radio_start();
	
	// make sure we have the correct information in packet
	packet->src = db_id();
	packet->version = PACKET_VERSION;
	
	// send packet
	radio_transmit((u8*)packet, PACKET_TRANSMIT_LENGTH);
	
	// set a timer for radio timeout
	doorlock_radio_timeout = 0;
	fptr_Timer0_A1_function = doorlock_radio_timer;
	Timer0_A1_Start(CONV_MS_TO_TICKS(DOORLOCK_RADIO_TIMEOUT));
	
	for(;;)
	{
		idle();
		
		// packet transmitted interrupt
		if (request.flag.radio_transmitted)
		{
			// call this to set radio into rx mode
			radio_transmitted();
		}
		
		// look for rf receiving event
		if (!request.flag.radio_received)
		{
			if (doorlock_radio_timeout)
			{
				if(--trials)
				{
					// retransmit
					radio_transmit((u8*)packet, PACKET_TRANSMIT_LENGTH);
					
					// start timer again
					doorlock_radio_timeout = 0;
					fptr_Timer0_A1_function = doorlock_radio_timer;
					Timer0_A1_Start(CONV_MS_TO_TICKS(DOORLOCK_RADIO_TIMEOUT));
				}
				else
				{
					// tried many times, give up
					radio_stop();
					return DOORLOCK_ERROR_TIMEOUT;
				}
			}
			continue;
		}
		
		// reset packet received flag
		request.flag.radio_received = 0;
		
		// retrieve the data and put radio back into rx mode
		radio_receive((u8*)packet, PACKET_RECEIVE_LENGTH);
		
		//
		// Filter packets.
		//
		
		// ignore invalid packets, basic check includes destination and version
		if (packet->dst != db_id() ||
			packet->version != PACKET_VERSION)
		{
			continue;
		}
		
		// complex check
		if (type == PACKET_TYPE_SYN || type == PACKET_TYPE_RAW)
		{
			// SYN and RAW packet requires a SEC reponse
			if (packet->type != PACKET_TYPE_SEC)
				continue;
		}
		else
		{
			// other packets requires an ack/nck response and requires a session first
			if (packet->src != session->door)
				continue;
			if (packet->type != PACKET_TYPE_ACK)
				continue;
		}
		
		// further check
		// look up in table and find door and key if we tried to send SYN packet
		if (type == PACKET_TYPE_SYN)
		{
			// ignore packet from those who we don't know
			if (db_query(packet->src, session->key))
				continue;
		}
		
		// we found a valid packet, let's break out of the loop
		break;
	}

	// stop radio
	radio_stop();
	
	// stop timeout timer
	Timer0_A1_Stop();
	
	return DOORLOCK_ERROR_SUCCESS;
}

// *************************************************************************************************
// @fn          doorlock_radio_create_session
// @brief       send syn to door and wait for sec reply
// @param       session - session information output
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_session_create(doorlock_radio_session_t *session)
{
	packet_t packet = {0};
	
	// initilize
	memset(session, 0, sizeof(doorlock_radio_session_t));
	
	// send a broadcast syn packet
	memset(&packet, 0, sizeof(packet_t));
	packet.dst = 0xffff;
	packet.type = PACKET_TYPE_SYN;
	
	// try
	if (doorlock_radio_request(session, &packet) != DOORLOCK_ERROR_SUCCESS)
	{
		memset(session, 0, sizeof(doorlock_radio_session_t));
		return DOORLOCK_ERROR_TIMEOUT;
	}
	
	// decrypt packet
	aes_decrypt(packet.payload.raw, session->key);
	
	// check pattern in payload
	// bug in string.h. we have to use 11 for 12
	if (strncmp(PACKET_PATTERN_SEC, (const char*)packet.payload.sec.pattern, 11))
	{
		return DOORLOCK_ERROR_INVALID;
	}
	
	// save info into session
	session->door = packet.src;
	session->token = packet.payload.sec.token;
	
	return DOORLOCK_ERROR_SUCCESS;
}

// *************************************************************************************************
// @fn          doorlock_radio_session_request
// @brief       encrypt and send request, need a session first
// @param       session - session information, packet - to be sent
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_session_request(doorlock_radio_session_t *session, packet_t *packet)
{	
	// destination is always the door which this session is based on
	packet->dst = session->door;

	// load the token
	if (packet->type == PACKET_TYPE_SEQ)
	{
		packet->payload.seq.token = session->token;
	}
	else if (packet->type == PACKET_TYPE_PWD)
	{
		packet->payload.pwd.token = session->token;
	}
	else if (packet->type == PACKET_TYPE_KEY)
	{
		packet->payload.key.token = session->token;
	}

	// encrypt
	aes_encrypt(packet->payload.raw, session->key);
	
	// try
	if (doorlock_radio_request(session, packet) != DOORLOCK_ERROR_SUCCESS)
	{
		return DOORLOCK_ERROR_TIMEOUT;
	}
	
	// decrypt packet
	aes_decrypt(packet->payload.raw, session->key);
	
	// check token in payload
	if (packet->payload.ack.token != session->token)
	{
		return DOORLOCK_ERROR_INVALID;
	}

	return (packet->payload.ack.status) ? DOORLOCK_ERROR_SUCCESS : DOORLOCK_ERROR_FAILURE;
}


// *************************************************************************************************
// @fn          doorlock_radio_request_seq
// @brief       send seq packet, to send unlock sequence code
// @param       session - session information, sequence - unlock code
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_request_seq(doorlock_radio_session_t *session, u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH])
{
	packet_t packet = {0};
	
	packet.type = PACKET_TYPE_SEQ;
	memcpy(packet.payload.seq.seq, sequence, sizeof(u8) * DOORLOCK_SEQUENCE_MAX_LENGTH);
	
	return doorlock_radio_session_request(session, &packet);
}

// *************************************************************************************************
// @fn          doorlock_radio_request_key
// @brief       send key packet, to renew key
// @param       session - session information, sequence - unlock code
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_request_key(doorlock_radio_session_t *session, u8 key[8])
{
	u8 error = DOORLOCK_ERROR_SUCCESS;
	u8 i = 0;
	packet_t packet = {0};
	
	packet.type = PACKET_TYPE_KEY;
	memcpy(packet.payload.key.key, key, sizeof(u8) * 8);
	
	error = doorlock_radio_session_request(session, &packet);
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		// update new key, better hope you get this message
		for (i = 0; i < 8; i++)
		{
			session->key[i] ^= session->key[i + 8];
			session->key[i + 8] = key[i];
		}
		
		// should always work
		if(db_save(session->door, session->key))
		{
			error = DOORLOCK_ERROR_INVALID;
		}
	}
	return error;
}

// *************************************************************************************************
// @fn          doorlock_radio_request_pwd
// @brief       send pwd packet, to set new unlock sequence (door has to be in pairing mode)
// @param       session - session information, sequence - new unlock code
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_request_pwd(doorlock_radio_session_t *session, u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH])
{
	packet_t packet = {0};
	
	packet.type = PACKET_TYPE_PWD;
	memcpy(packet.payload.pwd.seq, sequence, sizeof(u8) * DOORLOCK_SEQUENCE_MAX_LENGTH);
	
	return doorlock_radio_session_request(session, &packet);
}


// *************************************************************************************************
// @fn          doorlock_radio_request_raw
// @brief       send raw packet, to pair with door (door has to be in pairing mode)
// @param       key - shared key to be paired with door
// @return      doorlock error code
// *************************************************************************************************
u8 doorlock_radio_request_raw(u8 key[16])
{
	packet_t packet = {0};
	
	// send a broadcast raw packet
	packet.dst = 0xffff;
	packet.type = PACKET_TYPE_RAW;
	memcpy(packet.payload.raw, key, sizeof(u8) * 16);
	
	// try
	if (doorlock_radio_request(0, &packet) != DOORLOCK_ERROR_SUCCESS)
	{
		return DOORLOCK_ERROR_TIMEOUT;
	}
	
	// decrypt packet
	aes_decrypt(packet.payload.raw, key);
	
	// check pattern in payload
	// bug in string.h. we have to use 11 for 12
	if (strncmp(PACKET_PATTERN_SEC, (const char*)packet.payload.sec.pattern, 11))
	{
		return DOORLOCK_ERROR_INVALID;
	}
	
	// save key
	if (db_save(packet.src, key))
	{
		// database may have been full
		return DOORLOCK_ERROR_FAILURE;
	}
	
	return DOORLOCK_ERROR_SUCCESS;
}

// *************************************************************************************************
// @fn          doorlock_radio_timer
// @brief       timer callback to make sure door response is not timing out
// @param       none
// @return      none
// *************************************************************************************************
void doorlock_radio_timer(void)
{
	Timer0_A1_Stop();
	doorlock_radio_timeout = 1;
}


