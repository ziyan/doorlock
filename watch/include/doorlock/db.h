// *************************************************************************************************
// Doorlock database keeps information about paired doors and encryption keys
// Each door/watch pair has its own sequence (stored on door side) and shared key (stored on both
// watch and door). Each entry in the database on the watch is indexed by the corresponding door
// id. And each entry in the database on the door is index by the corresponding watch.
// *************************************************************************************************

#ifndef _DOORLOCK_DB_H_
#define _DOORLOCK_DB_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Prototypes section
extern void db_reset(void);
extern u16 db_id(void);
extern s16 db_batt_offset(void);
extern s16 db_temp_offset(void);
extern u8 db_query(u16 id, u8 key[16]);
extern u8 db_save(u16 id, u8 key[16]);
extern void db_clear(void);

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Global Variable section

// *************************************************************************************************
// Extern section

#endif /*_DOORLOCK_DB_H_*/
