#ifndef NOR_FLASH_H_INCLUDED
#define NOR_FLASH_H_INCLUDED
#include "sys.h"


#define NOR_FLASH_ADDR		0x0008	//System variable address
#define NOR_FLASH_WRITE		0xA5	//Write operation
#define NOR_FLASH_READ		0x5A	//Read operation

#define CACHE_ADDR			0xFF00	//0XFF00-0XFFFF is used as a buffer area, which has about 256 words and 512 bytes. Note that this area should not be used during GUI development.


//Function declaration
void Norflash_write(u32 addr,u8* buff,u16 len);
void Norflash_read(u32 addr,u8* buff,u16 len);

#endif //NOR_FLASH_H_INCLUDED