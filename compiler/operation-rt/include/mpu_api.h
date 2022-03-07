#ifndef __MPU_API_H
#define __MPU_API_H

/* HAL MPU Functions */
void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);

#endif