#include "verify.h"
#include "flash/flash.h"

DigitalOut Green(D2);
DigitalOut Red(D6);
DigitalOut BUZZ(A0);
DigitalOut LOCK(D3);

uint8_t UID[4]; 
uint8_t Type[2];
uint8_t admin_UID[4] = {0x10,0x8B,0x89,0x7F};

extern BufferedSerial serial_data;
uint8_t reply = 0;

uint8_t count_p = 0;

uint8_t* RC522_ReadIC_UID()
{
    if(PcdRequest(PICC_REQALL,Type) == MI_OK)//寻卡
	{		
		uint16_t cardType = (Type[0] << 8) | Type[1];
        switch (cardType)
        {
            #if USB_DEBUG
                case 0x4400:
                    printf("\r\nMifare UltraLight\r\n");
                    break;
                case 0x0400:
                    printf("\r\nMifare One(S50)\r\n");
                    break;
                case 0x0200:
                    printf("\r\nMifare One(S70)\r\n");
                    break;
                case 0x0800:
                    printf("\r\nMifare Pro(X)\r\n");
                    break;
                case 0x4403:
                    printf("\r\nMifare DESFire\r\n");
                    break;
                default:
                    printf("\r\nUnknown Card\r\n");
                    break;
            #endif
        }
		if(PcdAnticoll(UID)==MI_OK)
		{
            #if USB_DEBUG
                printf("\r\ncard_ID: %02X:%02X:%02X:%02X\r\n",UID[0],UID[1],UID[2],UID[3]); 
            #endif
		}
	}
	else
	{
        UID[0]=0;
        UID[1]=0;
        UID[2]=0;
        UID[3]=0;
	}
 
	PcdAntennaOff();//关闭天线发射
			
	ThisThread::sleep_for(10ms);
	PcdAntennaOn();//开启天线发射	
    return UID;
}

uint32_t merge_UID(uint8_t* data)
{
    uint32_t re = (data[0]<<24) | (data[1]<<16) | (data[2]<<8) | data[3];
    return re;
}

bool Table_Verify(vector<uint32_t>& UID_Table, uint32_t User)
{
    uint32_t UID_read = merge_UID(RC522_ReadIC_UID());
    
    for(vector<uint32_t>::iterator it = UID_Table.begin();it != UID_Table.end();it++)
    {
        if(*it == User)
            return true;
    }
    return false;
}

// H ---- --认证标志位 寻卡标志位 L
uint8_t UID_Verify(vector<uint32_t>& UID_Table)
{
    uint32_t UID_read = merge_UID(RC522_ReadIC_UID());
    if(UID_read == 0)
    {
        #if USB_DEBUG
            printf("Card not Found!\n");
        #endif
        return 0x00;
    }
    else
    {
        if(Table_Verify(UID_Table,UID_read))
            return 0x03;
        else
            return 0x01;
    }
}


void LED_demo(vector<uint32_t>& UID_Table)
{
    uint8_t user1[4]={0x10,0x8B,0x89,0x7F};
    uint8_t user_status = UID_Verify(UID_Table);
    if(user_status & 0x01)
    {
        if(user_status & 0x02)
        {
            count_p++;
            reply = USER_S;
            serial_data.write(&reply,1);
            reply = 0;
            Green = 1;
            Red = 0;
            LOCK = 0;
            BUZZ_ok();
            ThisThread::sleep_for(5s);
            LOCK = 1;
        }
        else 
        {
            reply = USER_F;
            serial_data.write(&reply,1);
            reply = 0;
            Green = 0;
            Red = 1;
            LOCK = 1;
            BUZZ_fail();
        }
    }
    else 
    {
        Green = 0;
        Red = 0;
        LOCK = 1;
    }
}

void add_UID()
{
    uint32_t UID_W = 0;
    while(!(UID_W = merge_UID(RC522_ReadIC_UID())))
    {
        #if USB_DEBUG
            printf("WAIT CARD\n");
        #endif
        ThisThread::sleep_for(500ms);
    }
    Write_UID(UID_W);
    #if USB_DEBUG
        printf("ADD SUCESS!\n");
    #endif
}

void BUZZ_ok()
{
    BUZZ = 1;
    ThisThread::sleep_for(500ms);
    BUZZ = 0;
}

void BUZZ_fail()
{
    BUZZ = 1;
    ThisThread::sleep_for(400ms);
    BUZZ = 0;
    ThisThread::sleep_for(400ms);
    BUZZ = 1;
    ThisThread::sleep_for(400ms);
    BUZZ = 0;
    ThisThread::sleep_for(400ms);
    BUZZ = 1;
    ThisThread::sleep_for(400ms);
    BUZZ = 0;
}

void open()
{
    LOCK = 0;
    ThisThread::sleep_for(5s);
    LOCK = 1;
}

bool admin()
{
    uint32_t UID_read = 0;
    int cnt = 0;
    while(!(UID_read = merge_UID(RC522_ReadIC_UID())) && cnt < 100)
    {
        cnt++;
        ThisThread::sleep_for(100ms);
    }
    if(UID_read == merge_UID(admin_UID))
        return true;
    else 
        return false;
}