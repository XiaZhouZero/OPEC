#ifndef __MPU_API_H
#define __MPU_API_H


//APIs for MPU_conifg
void MPU_Switch();      //当进入一个新的Operation前调用它,该函数负责读取并应用下一个Operation的MPU_config                             
void MPU_apply_Config (MPU_Region_InitTypeDef MPU_InitStruct[]);   //apply an operation's MPU configuration（包含8个region）
MPU_Region_InitTypeDef MPU_decode(MPU_Type MPU_reg);               //decode the MPU from RBSR RASR to MPU_Region_InitTypeDef
// void MPU_print(MPU_Region_InitTypeDef MPU_Init);                   //print MPU_Region_InitTypeDef

void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);

#endif