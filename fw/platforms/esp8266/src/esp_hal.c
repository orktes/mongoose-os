/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

#include "common/platforms/esp8266/esp_missing_includes.h"
#include "fw/src/mgos_timers.h"
#include "fw/src/mgos_hal.h"

#include "fw/src/mgos_mongoose.h"
#include "common/umm_malloc/umm_malloc.h"

#include "fw/platforms/esp8266/src/esp_fs.h"
#include "fw/platforms/esp8266/src/esp_task.h"

#include <osapi.h>
#include <os_type.h>
#include <user_interface.h>

size_t mgos_get_heap_size(void) {
  return UMM_MALLOC_CFG__HEAP_SIZE;
}

size_t mgos_get_free_heap_size(void) {
  return umm_free_heap_size();
}

size_t mgos_get_min_free_heap_size(void) {
  return umm_min_free_heap_size();
}

extern uint32_t soft_wdt_interval;
/* Should be initialized in user_main by calling mgos_wdt_set_timeout */
static uint32_t s_saved_soft_wdt_interval;
#define WDT_MAGIC_TIME 500000

void mgos_wdt_disable(void) {
  /*
   * poor's man version: delays wdt for several hours, but
   * technically wdt is not disabled
   */
  s_saved_soft_wdt_interval = soft_wdt_interval;
  soft_wdt_interval = 0xFFFFFFFF;
  system_soft_wdt_restart();
}

void mgos_wdt_enable(void) {
  soft_wdt_interval = s_saved_soft_wdt_interval;
  system_soft_wdt_restart();
}

void mgos_wdt_feed(void) {
  system_soft_wdt_feed();
}

void mgos_wdt_set_timeout(int secs) {
  s_saved_soft_wdt_interval = soft_wdt_interval = secs * WDT_MAGIC_TIME;
  system_soft_wdt_restart();
}

void mgos_system_restart(int exit_code) {
  (void) exit_code;
  fs_umount();
  system_restart();
}

int spiffs_get_memory_usage();

void mgos_usleep(int usecs) {
  os_delay_us(usecs);
}

IRAM void mongoose_schedule_poll(void) {
  mg_lwip_mgr_schedule_poll(mgos_get_mgr());
}
