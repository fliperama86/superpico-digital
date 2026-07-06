#ifndef MENU_DIAG_EXPERIMENT_H
#define MENU_DIAG_EXPERIMENT_H

#include "config.h"

#if ENABLE_REBOOT_MODE_SWITCH
#include "video/video_pipeline.h"
#endif

void menu_diag_experiment_init(void);
void menu_diag_experiment_on_menu_open(void);
void menu_diag_experiment_on_menu_close(void);
void menu_diag_experiment_tick_background(void);

#if ENABLE_REBOOT_MODE_SWITCH && ENABLE_OSD_RES_CONFIRM
void menu_diag_experiment_arm_res_confirm(video_pipeline_reboot_mode_t new_mode,
                                          video_pipeline_reboot_mode_t previous_mode);
#endif

#endif // MENU_DIAG_EXPERIMENT_H
