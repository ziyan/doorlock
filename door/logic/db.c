// *************************************************************************************************
// Include section
#include <string.h>

#include "driver/flash.h"
#include "driver/crc.h"
#include "driver/prng.h"
#include "driver/aes.h"

#include "logic/db.h"

// *************************************************************************************************
// Define section

#define DB_RECORD_SIZE (sizeof(db_record_t))
#define DB_RECORDS_PER_TABLE (34u)
#define DB_TABLE_SIZE (sizeof(db_table_t))

typedef struct recrod
{
	u16 id;
	u8 key[AES_KEY_SIZE];
	u8 seq[DOORLOCK_SEQUENCE_MAX_LENGTH];
} db_record_t;

typedef struct table
{
	u16 id;
	db_record_t records[DB_RECORDS_PER_TABLE];
	u16 crc;
} db_table_t;

// *************************************************************************************************
// Prototype section

void db_init(void);
void db_reset(void);
u16 db_id(void);
u8 db_query(u16 id, u8 *key, u8 *seq, u32 *token);
u8 db_save_key(u16 id, u8 *key);
u8 db_save_seq(u16 id, u8 *seq);
void db_clear_token(u16 id);
void db_clear(void);

// *************************************************************************************************
// Global variable section

// reserve space at the end of our 32K flash
__no_init __code const u8 db_flash[FLASH_PAGE_SIZE] @ 0x7C00;

// db cached in ram
db_table_t db_table = {0};
u32 db_tokens[DB_RECORDS_PER_TABLE] = {0};

// *************************************************************************************************
// Implementation section

void db_init(void)
{
	db_reset();
}

void db_reset(void)
{
	flash_read_page((const u8*)db_flash, (u8*)(&db_table));
	if (crc_compute((u8*)(&db_table), DB_TABLE_SIZE - 2) != db_table.crc)
	{
		memset((u8*)(&db_table), 0, DB_TABLE_SIZE);
	}
}

u16 db_id(void)
{
	if (db_table.id && db_table.id != 0xFFFFu)
		return db_table.id;

	prng_reseed();
	while (!db_table.id || db_table.id == 0xFFFFu)
		db_table.id = prng_rand() | prng_rand() << 8;

	flash_erase_page((const u8*)db_flash);
	db_table.crc = crc_compute((u8*)(&db_table), DB_TABLE_SIZE - 2);
	flash_write_page((const u8*)db_flash, (u8*)(&db_table));

	return db_table.id;
}


u8 db_query(u16 id, u8 *key, u8 *seq, u32 *token)
{
	u8 i = 0;
	u8 rand_token[4] = {0};

	for(i = 0; i < DB_RECORDS_PER_TABLE; ++i)
	{
		if (db_table.records[i].id == id)
		{
			memcpy(key, db_table.records[i].key, AES_KEY_SIZE);
			if (seq) memcpy(seq, db_table.records[i].seq, DOORLOCK_SEQUENCE_MAX_LENGTH);
			if (!db_tokens[i])
			{
				prng_reseed();
				while (!db_tokens[i])
				{
					rand_token[0] = prng_rand();
					rand_token[1] = prng_rand();
					rand_token[2] = prng_rand();
					rand_token[3] = prng_rand();
					db_tokens[i] = *(u32*)(&rand_token);
				}
			}
			*token = db_tokens[i];
			return 0;
		}
	}
	return 1;
}


u8 db_save_key(u16 id, u8 *key)
{
	u8 i = 0, empty_i = 0, empty_found = 0;

	for(i = 0; i < DB_RECORDS_PER_TABLE; ++i)
	{
		if (db_table.records[i].id == id)
		{
			// found the old entry
			memcpy(db_table.records[i].key, key, AES_KEY_SIZE);
			flash_erase_page((const u8*)db_flash);
			db_table.crc = crc_compute((u8*)(&db_table), DB_TABLE_SIZE - 2);
			flash_write_page((const u8*)db_flash, (u8*)(&db_table));
			return 0;
		}
		if (!empty_found && !db_table.records[i].id)
		{
			// found an empty spot
			empty_i = i;
			empty_found = 1;
		}
	}

	if (!empty_found)
	{
		// we could not find an empty spot, database full
		return 1;
	}

	// save the key and the seq
	db_table.records[empty_i].id = id;
	memcpy(db_table.records[empty_i].key, key, AES_KEY_SIZE);

	flash_erase_page((const u8*)db_flash);
	db_table.crc = crc_compute((u8*)(&db_table), DB_TABLE_SIZE - 2);
	flash_write_page((const u8*)db_flash, (u8*)(&db_table));

	return 0;
}


u8 db_save_seq(u16 id, u8 *seq)
{
	u8 i = 0;

	for(i = 0; i < DB_RECORDS_PER_TABLE; ++i)
	{
		if (db_table.records[i].id == id)
		{
			// found the old entry
			memcpy(db_table.records[i].seq, seq, DOORLOCK_SEQUENCE_MAX_LENGTH);
			flash_erase_page((const u8*)db_flash);
			db_table.crc = crc_compute((u8*)(&db_table), DB_TABLE_SIZE - 2);
			flash_write_page((const u8*)db_flash, (u8*)(&db_table));
			return 0;
		}
	}

	// we cannot save the sequence for a watch that hasn't been paired yet
	return 1;
}

void db_clear_token(u16 id)
{
	u8 i = 0;

	for(i = 0; i < DB_RECORDS_PER_TABLE; ++i)
	{
		if (db_table.records[i].id == id)
		{
			// found the old entry
			db_tokens[i] = 0;
		}
	}
}

void db_clear(void)
{
	flash_erase_page((const u8*)db_flash);
	memset((u8*)(&db_table), 0, DB_TABLE_SIZE);
}







