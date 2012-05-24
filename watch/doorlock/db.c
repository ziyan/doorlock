// *************************************************************************************************
// Doorlock database keeps information about paired doors and encryption keys
// Each door/watch pair has its own sequence (stored on door side) and shared key (stored on both
// watch and door). Each entry in the database on the watch is indexed by the corresponding door
// id. And each entry in the database on the door is index by the corresponding watch.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "project.h"

#include "doorlock/db.h"
#include "driver/flash.h"
#include "driver/crc.h"

#include "logic/clock.h"
#include "logic/date.h"
#include "logic/battery.h"

#include <string.h>

// *************************************************************************************************
// Prototypes section
void db_reset(void);
u16 db_id(void);
s16 db_batt_offset(void);
s16 db_temp_offset(void);
u8 db_query(u16 id, u8 key[16]);
u8 db_save(u16 id, u8 key[16]);
void db_clear(void);


// *************************************************************************************************
// Defines section

#define RECORDS_PER_TABLE	(7u)
#define MAX_TABLES			(3u)

typedef struct db_record
{
	u16 id;
	u8 key[16];
} db_record_t;

typedef struct db_table
{
	db_record_t records[RECORDS_PER_TABLE];
	u16 crc;
} db_table_t;

typedef struct db_info
{
	u16 id;
	s16 batt_offset;
	s16 temp_offset;
	u8 reserved[120];
	u16 crc;
} db_info_t;


// *************************************************************************************************
// Global Variable section

// flash addresses of the tables
db_table_t* tables_in_flash[MAX_TABLES] = {
	(db_table_t*)FLASH_INFO_SEG_A_ADDR,
	(db_table_t*)FLASH_INFO_SEG_B_ADDR,
	(db_table_t*)FLASH_INFO_SEG_C_ADDR
};
db_info_t* info_in_flash = (db_info_t*)FLASH_INFO_SEG_D_ADDR;

// db table in RAM
db_table_t tables[MAX_TABLES] = {{0}, {0}, {0}};
db_info_t info = {0};

// *************************************************************************************************
// Extern section


// *************************************************************************************************
// @fn          db_reset
// @brief       reload records from flash to RAM, done at init
// @param       none
// @return      none
// *************************************************************************************************
void db_reset(void)
{
	u8 i;
	
	// Get all records from Flash into RAM
	for (i = 0; i < MAX_TABLES; ++i)
	{
		memcpy(&tables[i], tables_in_flash[i], sizeof(db_table_t));
		
		// crc match?
		if (tables[i].crc != crc_compute((u8*)&tables[i],  RECORDS_PER_TABLE * sizeof(db_record_t)))
		{
			memset(&tables[i], 0, sizeof(db_table_t));
		}
	}
	
	// load info into RAM
	memcpy(&info, info_in_flash, sizeof(db_info_t));
	if (info.crc != crc_compute((u8*)&info,  sizeof(db_info_t) - 2))
	{
		memset(&info, 0, sizeof(db_info_t));
		info.batt_offset = -10;
		info.temp_offset = -250;
	}
}

u16 db_id(void)
{
	if (info.id && info.id != 0xFFFFu)
		return info.id;

	info.id = 0;
	while (!info.id || info.id == 0xFFFFu)
	{
		info.id ^= TA0R;
		info.id ^= crc_compute((u8*)&sDate,  sizeof(struct date));
		info.id ^= crc_compute((u8*)&sTime,  sizeof(struct time));
		info.id ^= crc_compute((u8*)&sBatt,  sizeof(struct batt));
	}

	flash_erase_segment((u8*)info_in_flash);
	info.crc = crc_compute((u8*)&info,  sizeof(db_info_t) - 2);
	flash_write_segment((u8*)info_in_flash, (u8*)(&info));
	return info.id;
}

s16 db_batt_offset(void)
{
	return info.batt_offset;
}
s16 db_temp_offset(void)
{
	return info.temp_offset;
}

// *************************************************************************************************
// @fn          db_query
// @brief       query encryption key for a door
// @param       id, key (output)
// @return      status, 0 = success, 1 = not found
// *************************************************************************************************
u8 db_query(u16 id, u8 key[16])
{
	u8 i, j;
	
	for (i = 0; i < MAX_TABLES; ++i)
	{
		for (j = 0; j < RECORDS_PER_TABLE; ++j)
		{
			if (tables[i].records[j].id == id)
			{
				memcpy(key, tables[i].records[j].key, 16);
				return 0;
			}
		}
	}
	
	// not found
	return 1;
}


// *************************************************************************************************
// @fn          db_save
// @brief       save a encryption key for a door
// @param       id, key
// @return      status, 0 = success, 1 = db full
// *************************************************************************************************
u8 db_save(u16 id, u8 key[16])
{
	u8 i, j;
	u8 empty_found = 0, empty_i = 0, empty_j = 0;
	
	// first find if the record already exists
	for (i = 0; i < MAX_TABLES; ++i)
	{
		for (j = 0; j < RECORDS_PER_TABLE; ++j)
		{
			if (tables[i].records[j].id == id)
			{
				// found it
				// erase
				flash_erase_segment((u8*)tables_in_flash[i]);
				
				// update RAM first
				memcpy(tables[i].records[j].key, key, 16);
				
				// calculat new crc
				tables[i].crc = crc_compute((u8*)&tables[i],  RECORDS_PER_TABLE * sizeof(db_record_t));
				
				// write only one segment
				flash_write_segment((u8*)tables_in_flash[i], (u8*)(&tables[i]));
				return 0;
			}
			else if (!empty_found && !tables[i].records[j].id)
			{
				empty_i = i;
				empty_j = j;
				empty_found = 1;
			}
		}
	}
	
	// is database all full?
	if (!empty_found) return 1;
	
	// erase
	flash_erase_segment((u8*)tables_in_flash[empty_i]);
	
	// write to a new slot
	tables[empty_i].records[empty_j].id = id;
	memcpy(tables[empty_i].records[empty_j].key, key, 16);
	
	// calculat new crc
	tables[empty_i].crc = crc_compute((u8*)&tables[empty_i],  RECORDS_PER_TABLE * sizeof(db_record_t));
	
	// write only one segment
	flash_write_segment((u8*)tables_in_flash[empty_i], (u8*)(&tables[empty_i]));
	
	return 0;
}


// *************************************************************************************************
// @fn          db_clear
// @brief       Clear the dataabase
// @param       none
// @return      none
// *************************************************************************************************
void db_clear(void)
{
	u8 i;
	for (i = 0; i < MAX_TABLES; ++i)
	{
		memset(&tables[i], 0, sizeof(db_table_t));
		flash_erase_segment((u8*)tables_in_flash[i]);
	}
	memset(&info, 0, sizeof(db_info_t));
	flash_erase_segment((u8*)info_in_flash);
}

