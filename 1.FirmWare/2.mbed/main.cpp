#include "mbed.h"
#include "sys.h"
#include "rc522.h" 
#include "verify.h"
#include "flash/flash.h"
#include <cstdio>
#include <vector>
#include "com.h"


#define USB_DEBUG 0

extern BufferedSerial serial_port;
extern uint8_t count_p;
vector<uint32_t> UID_Table;

InterruptIn s(D3);

BufferedSerial serial_data(D1,D0,115200);

int main(void)
{		  
    RC522_Init();
    Flash_Init();
    Update_UIDTable(UID_Table);

	while(1)
	{
        s.fall()
        uint8_t command = 0;
        uint8_t reply = 0;
        if(serial_data.readable())
        {
            serial_data.read(&command,1);
            #if USB_DEBUG
                printf("RECEIVE:%d\n",command);
            #endif
        }
        switch (command)
        {
            case NEW_UID:
                add_UID();
                Update_UIDTable(UID_Table);

                #if USB_DEBUG
                    printf("NEW_UID_DONE\n");
                #endif

                reply = NEW_UID_DONE;
                serial_data.write(&reply,1);
                break;

            case ERASE_CHIP:
                Chip_Erase();
                while(W25Q64_BusyState());
                Flash_Init();
                Update_UIDTable(UID_Table);

                #if USB_DEBUG
                    printf("ERASE_DONE\n");
                #endif

                reply = ERASE_DONE;
                serial_data.write(&reply,1);
                break;
            
            case ADMIN:
                if(admin())
                {
                    #if USB_DEBUG
                        printf("ADMIN_OK\n");
                    #endif
                    
                    reply = ADMIN_OK;
                    serial_data.write(&reply,1);
                }
                else
                {
                    #if USB_DEBUG
                        printf("ADMIN_FAIL\n");
                    #endif
                    
                    reply = ADMIN_FAIL;
                    serial_data.write(&reply,1);                
                }
                break;

            case ASK_NUM:
                    reply = count_p;
                    serial_data.write(&reply,1);  
                break;

            case OPEN:
                open();
                break;
        }
        LED_demo(UID_Table);
		ThisThread::sleep_for(100ms);       
	}	 
}