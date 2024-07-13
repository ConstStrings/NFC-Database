#pragma once
#include "mbed.h"
#include <vector>

#define USB_DEBUG 0

#define FLASH_CS_Enable() flash_CS=0
#define FLASH_CS_Disable() flash_CS=1

#define W25Q64_END_ADDR 0x7FFFFF

uint16_t W25Q64_GetID(void);
void W25Q64_SectorErase(uint32_t addr);
void W25Q64_ReadData(uint32_t addr,uint8_t *data,uint16_t size);
void W25Q64_PageWrite(uint32_t addr,uint8_t *data,uint16_t size);
void Write_UID(uint32_t UID);
void Flash_Init();
void Update_UIDTable(vector<uint32_t>& table);
void Chip_Erase();
bool W25Q64_BusyState();
void Test();