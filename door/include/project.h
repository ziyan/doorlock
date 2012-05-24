// *************************************************************************************************
// This file is included in every header file.
// *************************************************************************************************

#ifndef _PROJECT_H_
#define _PROJECT_H_

// *************************************************************************************************
// Include section
#include <ioCC1111.h>
#include <intrinsics.h>

#include "hal/ioCCxx10_bitdef.h"
#include "hal/hal_cc8051.h"

// *************************************************************************************************
// Define section

// Flags
typedef union
{
	struct
	{
		u8 door_update				: 1;
		u8 lock_update				: 1;
		u8 lock_lock				: 1;
		u8 lock_unlock				: 1;
		u8 mode_changed				: 1;
		u8 door_changed				: 1;
		u8 lock_changed				: 1;
	} flag;
	u8 all_flags;  
} request_flags_t;
extern volatile request_flags_t request;

typedef union
{
	struct
	{
		u8 rx						: 1;
		u8 tx						: 1;
		u8 timeout					: 1;
		u8 error					: 1;
	} flag;
	u8 all_flags;  
} radio_flags_t;
extern volatile radio_flags_t radio;

typedef union
{
	struct
	{
		u8 sleep_wakeup				: 1;
		u8 delay_over				: 1;
		u8 button					: 1; // button is pressed
		u8 button_long				: 1; // button is hold long enough
		u8 unlock					: 1; // the reed switch sense an unlock
		u8 lock						: 1; // the reed switch sense a lock
	} flag;
	u8 all_flags;
} sys_flags_t;
extern volatile sys_flags_t sys;

typedef union
{
	struct
	{
		u8 mode						: 2;
		u8 door						: 1;
		u8 lock						: 2;
	} flag;
	u8 all_flags;
} state_flags_t;
extern volatile state_flags_t state;

#define MODE_STATE_NORMAL		0
#define MODE_STATE_PASSWD		1
#define MODE_STATE_PAIR			2
#define MODE_STATE(new_state) st(if (state.flag.mode != new_state) { state.flag.mode = new_state; request.flag.mode_changed = 1; })

#define DOOR_STATE_OPEN			0
#define DOOR_STATE_CLOSED		1
#define DOOR_STATE(new_state) st(if (state.flag.door != new_state) { state.flag.door = new_state; request.flag.door_changed = 1; })

#define LOCK_STATE_UNLOCKED		0
#define LOCK_STATE_LOCKED		1
#define LOCK_STATE_UNLOCKING	2
#define LOCK_STATE_LOCKING		3
#define LOCK_STATE(new_state) st(if (state.flag.lock != new_state) { state.flag.lock = new_state; request.flag.lock_changed = 1; })


#define DOORLOCK_SEQUENCE_MAX_LENGTH (12u)
#define DOORLOCK_SEQUENCE_SIMILARITY (50u)

#endif // _PROJECT_H_
