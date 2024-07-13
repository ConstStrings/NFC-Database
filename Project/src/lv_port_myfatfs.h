#include "FS.h"
#include "SD.h"
#include "SD_MMC.h"
#include "Arduino.h"
#include "lvgl.h"

void SD_MMC_Init();
void ReadFile(fs::FS &fs, const char *path, uint8_t *buf, int len);
void WriteFile(fs::FS &fs, const char *path, uint8_t *buf, int len);
void init_fs_config(void);
bool pcfs_ready(lv_fs_drv_t *drv);
static void *pcfs_open(lv_fs_drv_t *drv, const char *fn, lv_fs_mode_t mode);
static lv_fs_res_t pcfs_write(struct _lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);
static lv_fs_res_t pcfs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t pcfs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t pcfs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t pcfs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);