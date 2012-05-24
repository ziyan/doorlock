// *************************************************************************************************
// Doorlock functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section
#include "project.h"

#include <string.h>

#include "driver/display.h"
#include "driver/ports.h"
#include "driver/idle.h"
#include "driver/timer.h"
#include "driver/buzzer.h"

#include "logic/menu.h"
#include "logic/doorlock.h"

#include "doorlock/db.h"
#include "doorlock/error.h"
#include "doorlock/radio.h"
#include "doorlock/random.h"
#include "doorlock/sequence.h"


// *************************************************************************************************
// Prototypes section
void reset_doorlock(void);
void sx_doorlock(u8 line);
void mx_doorlock(u8 line);
void display_doorlock(u8 line, u8 update);

void doorlock_unlock(void);
void doorlock_pair(void);
void doorlock_passwd(void);
void doorlock_renew(void);
void doorlock_clear(void);

void doorlock_signal_success();
void doorlock_signal_failure();
void doorlock_signal_timeout();
void doorlock_signal_invalid();

// *************************************************************************************************
// Defines section
#define DOORLOCK_SEQUENCE_SIMILARITY (50u)

// *************************************************************************************************
// Extern section

// *************************************************************************************************
// Global Variable section

void doorlock_unlock(void)
{
	doorlock_radio_session_t session = {0};
	u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH] = {0};
	u8 error = DOORLOCK_ERROR_SUCCESS;
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"FIND", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"DOOR", SEG_ON);
	
	error = doorlock_radio_session_create(&session);
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"DOOR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FOUND", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT ", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
		return;
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
		return;
	} 
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"PLEAS", SEG_ON);
	
	// collect sequence
	error = doorlock_sequence(sequence);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"DOOR", SEG_ON);
		display_chars(LCD_SEG_L2_4_0, (u8*)"CHECK", SEG_ON);
        doorlock_signal_success();
	}
	else
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FAIL", SEG_ON);
        doorlock_signal_failure();
		return;
	}

	// send seq
	error = doorlock_radio_request_seq(&session, sequence);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"DOOR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"OPEN", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_FAILURE)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FAIL", SEG_ON);
        doorlock_signal_failure();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT ", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
	}
}

void doorlock_pair(void)
{
	u8 key[16] = {0};
	u8 error = DOORLOCK_ERROR_SUCCESS;
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"PLIS", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"DANCE", SEG_ON);
	doorlock_signal_success();
	
	// collection random bits
	doorlock_random(key);
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"DOOR", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"PAIR", SEG_ON);
	
	error = doorlock_radio_request_raw(key);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"PAIR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"OK  ", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_FAILURE)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"PAIR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FAIL", SEG_ON);
        doorlock_signal_failure();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT ", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
	}
}

void doorlock_renew(void)
{
	doorlock_radio_session_t session = {0};
	u8 key[16] = {0};
	u8 i = 0;
	u8 error = DOORLOCK_ERROR_SUCCESS;
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"FIND", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"DOOR", SEG_ON);
	
	error = doorlock_radio_session_create(&session);
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"DOOR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FOUND", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT ", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
		return;
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
		return;
	} 
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"PLIS", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"DANCE", SEG_ON);
	doorlock_signal_success();
	
	// collect random key
	doorlock_random(key);
	
	// condense key
	for (i = 0; i < 8; i++)
	{
		key[i] ^= key[i + 8];
	}
	
	// send key
	error = doorlock_radio_request_key(&session, key);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"KEY", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"SYNC", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT ", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
	}
}

void doorlock_passwd(void)
{
	doorlock_radio_session_t session = {0};
	u8 sequence[DOORLOCK_SEQUENCE_MAX_LENGTH] = {0};
	u8 sequence_again[DOORLOCK_SEQUENCE_MAX_LENGTH] = {0};
	u8 error = DOORLOCK_ERROR_SUCCESS;
	u8 i = 0;
	u16 avg = 0;
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"FIND", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"DOOR", SEG_ON);
	
	error = doorlock_radio_session_create(&session);
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"DOOR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FOUND", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
		return;
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
		return;
	}
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"PLEAS", SEG_ON);
	
	// collect sequence
	error = doorlock_sequence(sequence);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"AGAIN", SEG_ON);
        doorlock_signal_success();
	}
	else
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FAIL", SEG_ON);
        doorlock_signal_failure();
		return;
	}
	
	// collect sequence
	error = doorlock_sequence(sequence_again);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"CHECK", SEG_ON);
        doorlock_signal_success();
	}
	else
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"FAIL", SEG_ON);
        doorlock_signal_failure();
		return;
	}
	
	for (i = 0; i < DOORLOCK_SEQUENCE_MAX_LENGTH; i++)
	{
		if (sequence[i] > sequence_again[i])
		{
			if (sequence[i] - sequence_again[i] > DOORLOCK_SEQUENCE_SIMILARITY)
			{
				error = DOORLOCK_ERROR_FAILURE;
				break;
			}			
		}
		else
		{
			if (sequence_again[i] - sequence[i] > DOORLOCK_SEQUENCE_SIMILARITY)
			{
				error = DOORLOCK_ERROR_FAILURE;
				break;
			}
		}
		avg = sequence[i];
		avg += sequence_again[i];
		sequence[i] = avg / 2;
	}
	
	
	if (error != DOORLOCK_ERROR_SUCCESS)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"MIS-", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"MATCH", SEG_ON);
        doorlock_signal_failure();
		return;
	}
	
	display_chars(LCD_SEG_L1_3_0, (u8*)"SEND", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"ING", SEG_ON);

	// send seq
	error = doorlock_radio_request_pwd(&session, sequence);
	
	if (error == DOORLOCK_ERROR_SUCCESS)
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"CODE", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"SYNC", SEG_ON);
        doorlock_signal_success();
	}
	else if (error == DOORLOCK_ERROR_TIMEOUT)
	{
        display_chars(LCD_SEG_L1_3_0, (u8*)"OUT ", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"RANGE", SEG_ON);
        doorlock_signal_timeout();
	}
	else
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"ERR", SEG_ON);
        display_chars(LCD_SEG_L2_4_0, (u8*)"ERROR", SEG_ON);
        doorlock_signal_invalid();
	}
}

void doorlock_clear(void)
{
	display_chars(LCD_SEG_L1_3_0, (u8*)"SURE", SEG_ON);
	display_chars(LCD_SEG_L2_4_0, (u8*)"CLEAR", SEG_ON);
	doorlock_signal_invalid();
    
    for(;;)
    {
    	idle();
    	
    	// Idle timeout exit
		if (sys.flag.idle_timeout)
		{
			break;
		}
		// m1 is used to exit
		else if (button.flag.m1) 
		{
			break;
		}
		else if (button.flag.m2_long)
		{
			db_clear();
			
			display_chars(LCD_SEG_L1_3_0, (u8*)"DATA", SEG_ON);
		    display_chars(LCD_SEG_L2_4_0, (u8*)"CLEAR", SEG_ON);
		    doorlock_signal_success();
		    break;
		}
    }
    
}


// *************************************************************************************************
// Simple user notification via buzzer
// *************************************************************************************************

// *************************************************************************************************
// @fn          doorlock_signal_success
// @brief       simple two beeps means success
// @param       none
// @return      none
// *************************************************************************************************
void doorlock_signal_success()
{
	start_buzzer(2, CONV_MS_TO_TICKS(100), CONV_MS_TO_TICKS(50));
    Timer0_A4_Delay(CONV_MS_TO_TICKS(300));
    stop_buzzer();
}

// *************************************************************************************************
// @fn          doorlock_signal_failure
// @brief       3 beeps means failure
// @param       none
// @return      none
// *************************************************************************************************
void doorlock_signal_failure()
{
	start_buzzer(3, CONV_MS_TO_TICKS(100), CONV_MS_TO_TICKS(50));
    Timer0_A4_Delay(CONV_MS_TO_TICKS(450));
    stop_buzzer();
}

// *************************************************************************************************
// @fn          doorlock_signal_timeout
// @brief       4 beeps means timeout
// @param       none
// @return      none
// *************************************************************************************************
void doorlock_signal_timeout()
{
	start_buzzer(4, CONV_MS_TO_TICKS(100), CONV_MS_TO_TICKS(50));
    Timer0_A4_Delay(CONV_MS_TO_TICKS(600));
    stop_buzzer();
}

// *************************************************************************************************
// @fn          doorlock_signal_invalid
// @brief       5 beeps means something terrible has happened, e.g. someone is trying to hack us
// @param       none
// @return      none
// *************************************************************************************************
void doorlock_signal_invalid()
{
	start_buzzer(1, CONV_MS_TO_TICKS(1000), CONV_MS_TO_TICKS(10));
    Timer0_A4_Delay(CONV_MS_TO_TICKS(1010));
    stop_buzzer();
}




// *************************************************************************************************
// @fn          reset_doorlock
// @brief       Reset doorlock variables.
// @param      	none
// @return      none
// *************************************************************************************************
void reset_doorlock(void)
{
	db_reset();
}


// *************************************************************************************************
// @fn          sx_doorlock
// @brief       Altitude direct function. This starts sequence mode.
// @param       u8 line	LINE1, LINE2
// @return      none
// *************************************************************************************************
void sx_doorlock(u8 line)
{
	clear_display_all();
	doorlock_unlock();
	display.flag.full_update = 1;
}


// *************************************************************************************************
// @fn          mx_doorlock
// @brief       Mx button handler to start key mode
// @param       u8 line		LINE1
// @return      none
// *************************************************************************************************
void mx_doorlock(u8 line)
{
	clear_display_all();
	
	for(;;) 
	{
		display_chars(LCD_SEG_L1_3_0, (u8*)"CONF", SEG_ON);
		display_chars(LCD_SEG_L2_4_0, (u8*)"TOOLS", SEG_ON);
		
		idle();
		
		// Idle timeout exit
		if (sys.flag.idle_timeout)
		{
			break;
		}
		// m1 is used to exit
		else if (button.flag.m1) 
		{
			break;
		}
		// s1 is used to renew key 
		else if (button.flag.s1)
		{
			clear_display_all();
			doorlock_renew();
		}
		// s2 is used to change knock sequence
		else if (button.flag.s2)
		{
			clear_display_all();
			doorlock_passwd();
		}
		// m2 long is used to pair with new door
		else if (button.flag.m2_long)
		{
			clear_display_all();
			doorlock_pair();
		}
		// m1 long is used to clear db
		else if (button.flag.m1_long)
		{
			clear_display_all();
			doorlock_clear();
		}
		
		// Clear button flags
		button.all_flags = 0;
	}

	// Clear button flags
	button.all_flags = 0;
	
	display.flag.full_update = 1;
}

// *************************************************************************************************
// @fn          display_doorlock
// @brief       Display routine.
// @param       u8 line			LINE2
//				u8 update		DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_CLEAR
// @return      none
// *************************************************************************************************
void display_doorlock(u8 line, u8 update)
{
	// Redraw whole screen
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
		display_chars(LCD_SEG_L2_4_0, (u8*)"DOOR ", SEG_ON);
	}
	else if (update == DISPLAY_LINE_UPDATE_FULL)
	{
		display_chars(LCD_SEG_L2_4_0, (u8*)"DOOR ", SEG_ON);
	}
	else if (update == DISPLAY_LINE_CLEAR)
	{
	}
}

