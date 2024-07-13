# NFC Database

### Introduction

  **The project builds an  Access control system based on NFC (Near Field Communication) technology. Use  STM32L432KCU6 as microcontroller, RC522 NFC module, W25Q64 flash chip. Realized  the user authentication unlocking, new user adding, database deletion,  administrator authentication, number statistics and other functions. In addition,  a touch screen is used for human-computer interaction. The system can be  widely used in the scenarios requiring personnel control, and has the  advantages of simple structure, convenient management and low cost.**  



  **Access control system  is commonly used in the daily life. The project is aiming to make an access  control system that achieve five basic functions.**   **The first function is Swiping  the card to open the door, utilizing NFC technology to verify the users. If  the visitor is in the database, the electromagnetic push rod will open, then,  the door is opened.**   **The second function is  new user information input, allowing the administer to add new users to the  database. Utilizing W25Q64 flash chip, the data in the database can be saved,  in the case of power failure. When the new user is put into the database, the  data will be synchronized to flash for instance.**   **The third function is deleting  the whole database. Deleting the database means all the users in the flash  chip will be erase as well. This function can be considered as restore to  factory settings.**   **The fourth function is  Administrator permission authentication. A certain ID card can be designated  as an administrator card. Once the administrator permission authentication succeeds,  the actions of adding new users, deleting the database, opening the door are  allowed.**  **The fifth function is Personnel  in and out statistics. The system can count the total number of people  entering and leaving and display the results on the screen.**  



### Function of Each Part

**The whole system can be divided into four parts:**

**1.**  **The microcontroller STM32L432KC board**

**2.**  **The NFC module RC522**

**3.**  **The flash chip W25Q64**

**4.**  **The display module** 

 

**STM32L432 is used to control all the modules. I have written 3 libraries to do the tasks. They are rc522.h, flash.h, verify.h. And in main.cpp, I just call these functions.**

 

**In rc522.h, it defines some commands for rc522. With this library, we can detect card and read card UID.**

**You can see the origin library in the reference. However, the origin library is for STM32F103 and is written in HAL library. I found it really does not work on MbedOS.**

**Therefore, I rewrite it in MbedOS, change the functions related to SPI into Mbed-style. After doing this, the codes can run on my board.**

**In flash.h, all the functions are written by me. The flash is used to store user data even if there is no electricity. This chip W25Q64 has the capacity of 64Mbit and communicate with the host via SPI protocol. Its memory space is divided into blocks, then, sectors, then, pages. We can write or read data from a single page. However, the minimum erase unit is sector. One of the most important point is that we cannot change the data in flash directly, we should erase it first, then, write the new data into the flash. That is because the flash can only change from 0 to 1 by erase. I will explain how it works later.**

**To find the position to store data, there is a uint32_t data at first sector (address 0x00000FFF) that stores the address of last page has been used. If we want to add a new data to the flash, it will be stored at the address last + 1.** 

**The function Flash_Init () ensures that the address in first sector is the second sector page0 if the chip is empty.**

**To ensure the speed of verification. There is a table in memory which is a copy of database in flash. Once the database in the flash changes, the table will change as well. Use function Update_UIDTable ( ) to update the database table. Ensure you call Update_UIDTable ( ) each time after you write some into the flash.**

**The functions about the man-machine interaction are in library verify.h. This library makes the connection between re522.h and flash.h, enabling users to read UID and then add to flash or read UID and compare the database in the flash.** 

**In order to make it more convenient for users to use, I made an LCD driver board to communicate with the stm32l432 through the serial port. Therefore, the user can control and monitor the operation of the entire system through the touch screen. Regarding the touch screen section, the related hardware and code I have open sourced on GitHub, if you wander more information, you can access:** [**https://github.com/ConstStrings/ESP32-S3-LVGL-Board**](https://github.com/ConstStrings/ESP32-S3-LVGL-Board)**.**

**In the main loop, the program constantly checks if there has command. Once the MCU receive command, use the swich branch to select the function you want to call.**

**After that, the MCU returns the status of the program execution (Done or Fail).**

### System Test

![](.\3.imgs\test.png)

