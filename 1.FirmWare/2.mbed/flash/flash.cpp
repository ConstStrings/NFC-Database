#include "flash.h"
#include <cstdint>

SPI flash_spi(A6, A5, A4);
DigitalOut flash_CS(A3);

uint16_t W25Q64_GetID(void)
{
	uint16_t id = 0;
	FLASH_CS_Enable();
	flash_spi.write(0x90);
	flash_spi.write(0x00);
	flash_spi.write(0x00);
	flash_spi.write(0x00);
	id = flash_spi.write(0xFF)<<8;	
	id |= flash_spi.write(0xFF);	
	FLASH_CS_Disable();
	return id;
}

uint8_t Read_Raw(uint8_t regAdd)
{
    uint8_t data = 0;
    FLASH_CS_Enable();
	flash_spi.write(regAdd);
    data = flash_spi.write(0xFF);
	FLASH_CS_Disable();
    return data;
}

void W25Q64_WriteEnable()
{
    FLASH_CS_Enable();
	flash_spi.write(0x06);
	FLASH_CS_Disable();
}

bool W25Q64_BusyState()
{
    uint8_t state = Read_Raw(0x05);
    //printf("%d\n",state);
    if(state & 0x01)
        return true;
    else 
        return false;
}


void W25Q64_SectorErase(uint32_t addr)
{
	while( W25Q64_BusyState());
	W25Q64_WriteEnable();
    FLASH_CS_Enable();
	flash_spi.write(0x20);
	flash_spi.write((addr&0xFF0000)>>16);//发送24位地址
	flash_spi.write((addr&0xFF00)>>8);
	flash_spi.write((addr&0xFF));
	FLASH_CS_Disable();
}


void W25Q64_ReadData(uint32_t addr,uint8_t *data,uint16_t size)
{
	if(addr+size > W25Q64_END_ADDR) 
	{
		return ;
	}
	uint8_t *pData = data;
	while(W25Q64_BusyState());
    FLASH_CS_Enable();
	flash_spi.write(0x03);
	flash_spi.write((addr&0xFF0000)>>16);
	flash_spi.write((addr&0xFF00)>>8);
	flash_spi.write((addr&0xFF));
	while(size--)
	{
		*pData = flash_spi.write(0xFF);
        //printf("%d",*pData);
		pData++;
	}
    //printf("-----\n");
	FLASH_CS_Disable();
}


void W25Q64_PageWrite(uint32_t addr,uint8_t *data,uint16_t size)
{
	if(addr+size > W25Q64_END_ADDR) 
		return ;
	uint8_t *pData = data;
	while(W25Q64_BusyState());
	W25Q64_WriteEnable();
    FLASH_CS_Enable();
	//while(!(W25Q64_ReadState() & W25Q64_WEL));//等待写使能完成
	flash_spi.write(0x02);//页写指令
	flash_spi.write((addr & 0xFF0000)>>16);//发送24位地址
	flash_spi.write((addr & 0xFF00)>>8);
	flash_spi.write((addr & 0xFF));
	while(size--)
	{
		flash_spi.write(*pData);
		pData++;
	}
	FLASH_CS_Disable();
}

uint32_t W25Q64_Readu32(uint32_t addr)
{
    uint8_t data[4] = {0};
    W25Q64_ReadData(addr,data,4);
    uint32_t re = 0;
    re |= (data[0]<<24);
    re |= (data[1]<<16);
    re |= (data[2]<<8);
    re |= data[3];
    #if USB_DEBUG
        printf("%d-%d-%d-%d-%u\n",data[0],data[1],data[2],data[3],re);
    #endif
    return re;
}

void W25Q64_Writeu32(uint32_t addr,uint32_t data)
{
    uint8_t data_arr[4]={0};
    data_arr[0]=(data>>24);
    data_arr[1]=(data&0xFF0000)>>16;
    data_arr[2]=(data&0xFF00)>>8;
    data_arr[3]=data&0xFF;
    W25Q64_PageWrite(addr,data_arr,4);
}

void Flash_Init()
{
    //W25Q64_Writeu32(0x00000000,0x00000fff);
    uint32_t last_add = W25Q64_Readu32(0x00000000);
    #if USB_DEBUG
        printf("LAST:%u\n",last_add);
    #endif
    if(last_add == 0xffffffff)    //第一次使用从0X000FFF开始写
    {
        W25Q64_Writeu32(0x000000,0x00000fff);
    }
}

void Write_UID(uint32_t UID) 
{
    uint32_t last = W25Q64_Readu32(0x00000000);
    //printf("LAST:%u\n",last);
    W25Q64_Writeu32(last+1U,UID);
    ThisThread::sleep_for(10ms);
    
    W25Q64_SectorErase(0x00000000);
    W25Q64_Writeu32(0x00000000,(uint32_t)(last+4U));
    //printf("WRITE:%u\n",(uint32_t)(last+4U));
    ThisThread::sleep_for(10ms);
    //printf("NEW LAST:%u\n",W25Q64_Readu32(0x00000000));
}

void Update_UIDTable(vector<uint32_t>& table)
{
    table.clear();
    uint32_t pUID = 0x00000FFF + 1;
    uint32_t pUID_end = W25Q64_Readu32(0x00000000);
    while(pUID < pUID_end)
    {   uint32_t new_uid = W25Q64_Readu32(pUID);
        #if USB_DEBUG
            printf("UPDATE UID:%u\n",new_uid);
        #endif
        table.push_back(new_uid);
        pUID += 4;
    }
}

void Chip_Erase()
{
    W25Q64_WriteEnable();
    FLASH_CS_Enable();
	flash_spi.write(0xC7);
	FLASH_CS_Disable();
}

void Test()
{
    W25Q64_Writeu32(0x00,0x0000ffff);
    //W25Q64_SectorErase(0x00);
    uint32_t state = W25Q64_Readu32(0x00);
    //printf("%d\n",state);
}