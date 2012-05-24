#ifndef _LOGIC_DB_H_
#define _LOGIC_DB_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Prototype section
extern void db_init(void);
extern void db_reset(void);
extern u16 db_id(void);
extern u8 db_query(u16 id, u8 *key, u8 *seq, u32 *token);
extern u8 db_save_key(u16 id, u8 *key);
extern u8 db_save_seq(u16 id, u8 *seq);
extern void db_clear_token(u16 id);
extern void db_clear(void);

#endif /*_LOGIC_DB_H_*/

