#include "lv_port_myfatfs.h"

void WriteFile(fs::FS &fs, const char *path, uint8_t *buf, int len)
{
  //unsigned long start_time = millis();
  Serial.printf("write [%s]...\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  
  if (!file.write(buf, len)) {
      Serial.println("Write failed");
      return;
    }

  file.flush();
  file.close();

  Serial.printf("Write [%s] Complete", path);
}

void ReadFile(fs::FS &fs, const char *path, uint8_t *buf, int len)
{
  Serial.printf("read [%s]...\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  if (!file.read(buf, len)) {
      Serial.println("Read failed");
      return;
  }
  
  file.close();

  Serial.printf("Read [%s] Complete: %s", path, buf);
}

void SD_MMC_Init()
{
  SD_MMC.setPins(12, 11, 41);
  if (!SD_MMC.begin("/cdcard", true))
  {
    Serial.println("Card Mount Failed");
    return;
  }
}   

void testIO(fs::FS &fs)
{
  char buf[] = "hello world";

  WriteFile(fs, "/test.txt", (uint8_t *)buf, strlen(buf));
  ReadFile(fs, "/test.txt", (uint8_t *)buf, strlen(buf));
}


void init_fs_config(void)
{
    static lv_fs_drv_t pcfs_drv;                         /*A driver descriptor*/
    lv_fs_drv_init(&pcfs_drv);
    pcfs_drv.user_data = NULL;
    pcfs_drv.letter = 'S';
    pcfs_drv.ready_cb = pcfs_ready;
    pcfs_drv.write_cb = pcfs_write;
    pcfs_drv.open_cb = pcfs_open;
    pcfs_drv.close_cb = pcfs_close;
    pcfs_drv.read_cb = pcfs_read;
    pcfs_drv.seek_cb = pcfs_seek;
    pcfs_drv.tell_cb = pcfs_tell;
    lv_fs_drv_register(&pcfs_drv);
}


//文件系统是否准备好被回调函数pcfs_ready
bool pcfs_ready(lv_fs_drv_t* drv)
{
    (void)drv; /*Unused*/
    return true;  //这里仅返回true,如果是嵌入式，则是返回嵌入式文件系统挂载成功与否的标志
}
//打开文件被回调的函数pcfs_open
static void* pcfs_open(lv_fs_drv_t* drv, const char* fn, lv_fs_mode_t mode)
{
    (void)drv; /*Unused*/

    errno = 0;

    const char* flags = "";

    if (mode == LV_FS_MODE_WR) flags = "wb";
    else if (mode == LV_FS_MODE_RD) flags = "rb";
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "a+";

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    sprintf(buf, "./%s", fn);

    return fopen(buf, flags);  
}
//写入文件被回调的函数pcfs_write
static lv_fs_res_t pcfs_write(struct _lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw)
{
    (void)drv; /*Unused*/

    *bw = (uint32_t)fwrite(buf, 1, btw, (FILE*)file_p);

    return LV_FS_RES_OK;
}
//读取文件被回调的函数pcfs_read
static lv_fs_res_t pcfs_read(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br)
{
    (void)drv; /*Unused*/
    
    //pc_file_t* fp = file_p;        /*Just avoid the confusing casings*/
    *br = (uint32_t)fread(buf, 1, btr, (FILE*)file_p);
    return LV_FS_RES_OK;
}
//关闭文件被回调的函数pcfs_close
static lv_fs_res_t pcfs_close(lv_fs_drv_t* drv, void* file_p)
{
    (void)drv; /*Unused*/

    return fclose((FILE*)file_p);
}
//移动文件位置被回调的函数pcfs_seek
static lv_fs_res_t pcfs_seek(lv_fs_drv_t* drv, void* file_p, uint32_t pos,lv_fs_whence_t whence)
{
    (void)drv; /*Unused*/

    return fseek((FILE*)file_p, pos, whence);
}
//获取文件位置被回调的函数pcfs_tell
static lv_fs_res_t pcfs_tell(lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p)
{
    (void)drv; /*Unused*/
    *pos_p = ftell((FILE*)file_p);
    return LV_FS_RES_OK;
}

