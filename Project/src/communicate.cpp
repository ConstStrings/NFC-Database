#include <lvgl.h>
#include "SD.h"
#include "communicate.h"
#include "lvgl.h"
//自己包装的文件系统初始化函数
void my_fs_drv_init(void)
{
    SPIClass sd_spi;
    sd_spi.begin(12, 41, 11, 42);
    if(!SD.begin(42,sd_spi))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    else
    {
        Serial.println("Card Mount Success");
    }
    uint8_t cardType = SD.cardType();
 
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    lv_fs_fatfs_init();  //lvgl 文件系统初始化 放在SD卡初始化之后就行
}
//测试读取函数
void lv_fs_test(void)
{
	lv_fs_file_t f;
	lv_fs_res_t res;
	res = lv_fs_open(&f, "S:/sd/test.txt", LV_FS_MODE_RD);
	if(res != LV_FS_RES_OK) 
	{
		Serial.println("Open fail");
	}
	else
	{
		Serial.println("Open OK");
		uint32_t read_num;
		uint8_t buf[8];
		res = lv_fs_read(&f, buf, 8, &read_num);
		Serial.printf("read:%s",buf);
		lv_fs_close(&f);
	}
		
}


