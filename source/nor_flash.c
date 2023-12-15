#include "nor_flash.h"

xdata u8 norflash_cmd[8];

//Write data to the built-in nor flash of T5L
//addr: address of nor flash, it must be an even number, the range is: 0x000000-0x027FFE, and then one address corresponds to 2 bytes, that is, the total capacity is 320KB
//buff: The buffer to be written, the length is at least greater than len*2
//len: The number of words written must be an even number, and the maximum length of writing is also limited, which is related to the value of CACHE_ADDR
void Norflash_write(u32 addr,u8* buff,u16 len)
{
	u8 *buf_ptr = norflash_cmd;
	//1.First write the data in the buff to the DGUSII available variable area (CACHE_ADDR) cache.
	write_dgus_vp(CACHE_ADDR, buff, len);
	
	//2.Then write the contents of CACHE_ADDR to nor flash
	*buf_ptr++ = NOR_FLASH_WRITE;//Write operation
	*buf_ptr++ = (addr>>16)&0xff;//nor flash address
	*buf_ptr++ = (addr>>8)&0xff;
	*buf_ptr++ = addr&0xff;
	*buf_ptr++ = (CACHE_ADDR>>8)&0xff;//dgusii data address
	*buf_ptr++ = CACHE_ADDR&0xff;
	*buf_ptr++ = (len>>8)&0xff;//Length of data written
	*buf_ptr++ = len&0xff;
	write_dgus_vp(NOR_FLASH_ADDR, norflash_cmd,4);
	
	//3.Wait for the operation to complete
	while(1)
	{
		read_dgus_vp(NOR_FLASH_ADDR,norflash_cmd,2);//Just read the first 2 words of the 0x0008 variable, and then judge whether D7 is 0
		if(norflash_cmd[0]==0)
			break;
		delay_ms(1);//This delay must be added to prevent inexplicable errors
	}
}



//Read data from the built-in nor flash of T5L
//addr: address of nor flash, it must be an even number, the range is: 0x000000-0x027FFE, and then one address corresponds to 2 bytes, that is, the total capacity is 320KB
//buff: The buffer to be read, the length is at least greater than len*2
//len: The number of words read must be an even number, and the maximum length of writing is also limited, which is related to the value of CACHE_ADDR
void Norflash_read(u32 addr,u8* buff,u16 len)
{
	u8 *buf_ptr = norflash_cmd;
	//1.Read the contents of nor flash to CACHE_ADDR
	*buf_ptr++ = NOR_FLASH_READ;//Read operation
	*buf_ptr++ = (addr>>16)&0xff;//nor flash address
	*buf_ptr++ = (addr>>8)&0xff;
	*buf_ptr++ = addr&0xff;
	*buf_ptr++ = (CACHE_ADDR>>8)&0xff;//dgusii data address
	*buf_ptr++ = CACHE_ADDR&0xff;
	*buf_ptr++ = (len>>8)&0xff;//Length of data written
	*buf_ptr++ = len&0xff;
	write_dgus_vp(NOR_FLASH_ADDR,norflash_cmd,4);
	
	//2.Wait for the operation to complete
	while(1)
	{
		read_dgus_vp(NOR_FLASH_ADDR,norflash_cmd,2);//Just read the first 4 bytes of the 0x0008 variable, and then judge whether D7 is 0
		if(norflash_cmd[0]==0)
			break;
		delay_ms(1);//This delay must be added to prevent inexplicable errors
	}
	
	//3.Then read the data at CACHE_ADDR into the buff
	read_dgus_vp(CACHE_ADDR,buff,len);
}
