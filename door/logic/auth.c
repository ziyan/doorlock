// *************************************************************************************************
// Include section
#include <string.h>

#include "driver/aes.h"
#include "driver/buzzer.h"
#include "driver/timer.h"

#include "logic/db.h"
#include "logic/auth.h"
#include "logic/lock.h"
#include "logic/mode.h"

// *************************************************************************************************
// Define section
typedef struct auth_context
{
	u16 id;
	u32 token;
	u8 key[AES_KEY_SIZE];
	u8 seq[DOORLOCK_SEQUENCE_MAX_LENGTH];
} auth_context_t;

// *************************************************************************************************
// Prototype section
void auth_init(void);
u8 auth_process_packet(packet_t *packet);

u8 auth_send_sec(packet_t *packet, auth_context_t *context);
u8 auth_send_ack(packet_t *packet, auth_context_t *context, u8 status);

void auth_signal_success(void);
void auth_signal_failure(void);
void auth_signal_error(void);

void auth_init(void)
{
}

u8 auth_process_packet(packet_t *packet)
{
	u8 i = 0;
	u8 key[AES_KEY_SIZE] = {0};
	auth_context_t context = {0};

	// source
	context.id = packet->src;
	
	if (packet->type == PACKET_TYPE_SYN)
	{
		// SYN respond with a token
		if (packet->dst != 0xFFFFu) return 1;
		if (db_query(context.id, context.key, 0, &context.token)) return 1;
		return auth_send_sec(packet, &context);
	}
	else if (packet->type == PACKET_TYPE_SEQ)
	{
		// SEQ for door unlock sequence
		if (packet->dst != db_id()) return 1;
		if (db_query(context.id, context.key, context.seq, &context.token)) return 1;
		aes_decrypt(packet->payload.raw, context.key);
		if (context.token != packet->payload.seq.token)
		{
			auth_signal_error();
			return 1;
		}
		db_clear_token(context.id);
		
		for (i = 0; i < DOORLOCK_SEQUENCE_MAX_LENGTH; i++)
        {
			if (context.seq[i] > packet->payload.seq.seq[i])
			{
				if (context.seq[i] - packet->payload.seq.seq[i] > DOORLOCK_SEQUENCE_SIMILARITY)
				{
					break;
				}                       
			}
			else
			{
				if (packet->payload.seq.seq[i] - context.seq[i] > DOORLOCK_SEQUENCE_SIMILARITY)
				{
					break;
				}
			}
        }
		
		if (i == DOORLOCK_SEQUENCE_MAX_LENGTH)
		{
			request.flag.lock_unlock = 1;
			auth_signal_success();
			return auth_send_ack(packet, &context, PACKET_STATUS_ACK);
		}
		else
		{
			auth_signal_failure();
			return auth_send_ack(packet, &context, PACKET_STATUS_NCK);
		}
	}
	else if (packet->type == PACKET_TYPE_KEY)
	{
		// KEY for partial key rotation
		if (packet->dst != db_id()) return 1;
		if (db_query(context.id, context.key, 0, &context.token)) return 1;
		aes_decrypt(packet->payload.raw, context.key);
		if (context.token != packet->payload.key.token)
		{
			auth_signal_error();
			return 1;
		}
		db_clear_token(context.id);

		// rotate key		
		for (i = 0; i < 8; i++)
		{
			key[i] = context.key[i] ^ context.key[i + 8];
			key[i + 8] = packet->payload.key.key[i];
		}
		
		// save to database
		if (db_save_key(context.id, key))
		{
			auth_signal_failure();
			return auth_send_ack(packet, &context, PACKET_STATUS_NCK);
		}
		else
		{
			auth_signal_success();
			return auth_send_ack(packet, &context, PACKET_STATUS_ACK);
		}
	}
	else if (packet->type == PACKET_TYPE_PWD)
	{
		// PWD for changing password (only available in passwd mode)
		if (packet->dst != db_id()) return 1;
		if (db_query(context.id, context.key, 0, &context.token)) return 1;
		aes_decrypt(packet->payload.raw, context.key);
		if (context.token != packet->payload.pwd.token)
		{
			auth_signal_error();
			return 1;
		}
		db_clear_token(context.id);

		// if we are not in the right mode, send nck
		if (state.flag.mode != MODE_STATE_PASSWD)
		{
			auth_signal_error();
			return auth_send_ack(packet, &context, PACKET_STATUS_NCK);
		}
		
		// go back to normal mode afterwards
		MODE_STATE(MODE_STATE_NORMAL);
		
		// save to database
		if (db_save_seq(context.id, packet->payload.pwd.seq))
		{
			auth_signal_failure();
			return auth_send_ack(packet, &context, PACKET_STATUS_NCK);
		}
		else
		{
			auth_signal_success();
			return auth_send_ack(packet, &context, PACKET_STATUS_ACK);
		}
	}
	else if (packet->type == PACKET_TYPE_RAW)
	{
		// RAW for pairing (only available in pairing mode)
		if (packet->dst != 0xFFFFu) return 1;
		if (state.flag.mode != MODE_STATE_PAIR)
		{
			auth_signal_error();
			return 1;
		}
		
		// go back to normal mode afterwards
		MODE_STATE(MODE_STATE_PASSWD);

		// save the key
		if (db_save_key(context.id, packet->payload.raw) ||
			db_query(context.id, context.key, 0, &context.token))
		{
			auth_signal_failure();
			return 1;
		}
		else
		{
			auth_signal_success();
			return auth_send_sec(packet, &context);
		}
	}
	return 1;
}

u8 auth_send_sec(packet_t *packet, auth_context_t *context)
{
	memset(packet, 0, sizeof(packet_t));
	packet->dst = context->id;
	packet->src = db_id();
	packet->type = PACKET_TYPE_SEC;
	packet->version = PACKET_VERSION;
	
	packet->payload.sec.token = context->token;
	memcpy(packet->payload.sec.pattern, (u8*)PACKET_PATTERN_SEC, 12);
	aes_encrypt(packet->payload.raw, context->key);
	// we leave it to main.c to transmit the packet
	return 0;
}

u8 auth_send_ack(packet_t *packet, auth_context_t *context, u8 status)
{
	memset(packet, 0, sizeof(packet_t));
	packet->dst = context->id;
	packet->src = db_id();
	packet->type = PACKET_TYPE_ACK;
	packet->version = PACKET_VERSION;
	
	packet->payload.ack.token = context->token;
	packet->payload.ack.status = status;
	aes_encrypt(packet->payload.raw, context->key);
	// we leave it to main.c to transmit the packet
	return 0;
}

void auth_signal_success(void)
{
	buzzer_start(2, CONV_MS_TO_TICKS(50), CONV_MS_TO_TICKS(50), BUZZER_OPTION_TOGGLE_LED);
	timer2_delay(200);
	mode_update();
}

void auth_signal_failure(void)
{
	buzzer_start(3, CONV_MS_TO_TICKS(50), CONV_MS_TO_TICKS(50), BUZZER_OPTION_TOGGLE_LED);
	timer2_delay(300);
	mode_update();
}

void auth_signal_error(void)
{
	buzzer_start(1, CONV_MS_TO_TICKS(1000), CONV_MS_TO_TICKS(50), BUZZER_OPTION_TOGGLE_LED);
	timer2_delay(1050);
	mode_update();
}

