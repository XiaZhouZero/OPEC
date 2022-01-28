#include "mpu_entry.h"

void MPU_Switch(MPU_Region_InitTypeDef MPU_Target[]){
      MPU_apply_Config(MPU_Target);                             //应用一个operation的MPU配置
}


void MPU_apply_Config(MPU_Region_InitTypeDef MPU_Target[]){
  for(int i=0;i<REGION_NUM;i++){
    HAL_MPU_Disable();
    HAL_MPU_ConfigRegion(&MPU_Target[i]);
    HAL_MPU_Enable(MPU_HFNMI_PRIVDEF_NONE);
  }
}


/**
  * @brief  decode the MPU from RBSR RASR to MPU_Region_InitTypeDef
  */
MPU_Region_InitTypeDef MPU_decode(MPU_Type MPU_reg){
      MPU_Region_InitTypeDef MPU_Init;

      MPU_Init.Enable = (MPU_reg.RASR & MPU_RASR_ENABLE_Msk) >> MPU_RASR_ENABLE_Pos;
      MPU_Init.Number = MPU_reg.RNR;
      MPU_Init.BaseAddress = MPU_reg.RBAR;
      MPU_Init.Size = (MPU_reg.RASR & MPU_RASR_SIZE_Msk) >> MPU_RASR_SIZE_Pos;
      MPU_Init.SubRegionDisable = (MPU_reg.RASR & MPU_RASR_SRD_Msk) >> MPU_RASR_SRD_Pos;
      MPU_Init.TypeExtField = (MPU_reg.RASR & MPU_RASR_TEX_Msk) >> MPU_RASR_TEX_Pos;
      MPU_Init.AccessPermission = (MPU_reg.RASR & MPU_RASR_AP_Msk) >> MPU_RASR_AP_Pos;
      MPU_Init.DisableExec = (MPU_reg.RASR & MPU_RASR_XN_Msk) >> MPU_RASR_XN_Pos;
      MPU_Init.IsShareable = (MPU_reg.RASR & MPU_RASR_S_Msk) >> MPU_RASR_S_Pos;
      MPU_Init.IsCacheable = (MPU_reg.RASR & MPU_RASR_C_Msk)  >> MPU_RASR_C_Pos;
      MPU_Init.IsBufferable = (MPU_reg.RASR & MPU_RASR_B_Msk) >> MPU_RASR_B_Pos;
      return MPU_Init;
}


void MPU_print(MPU_Region_InitTypeDef MPU_Init){
        printf("MPU-Enable =              0x%08x\n",MPU_Init.Enable  );
        printf("MPU-Number =              0x%08x\n",MPU_Init.Number  );
        printf("MPU-BaseAddress =         0x%08lx\n",MPU_Init.BaseAddress & MPU_RBAR_ADDR_Msk );
        printf("MPU-Size =                0x%08x\n",MPU_Init.Size  );
        printf("MPU-region_number_valid=  0x%08lx\n",(MPU_Init.BaseAddress & MPU_RBAR_VALID_Msk) >> MPU_RBAR_VALID_Pos );
                                            
        printf("MPU-region_field =        0x%08lx\n",MPU_Init.BaseAddress & MPU_RBAR_REGION_Msk);
        printf("MPU-SubRegionDisable=     0x%08x\n",MPU_Init.SubRegionDisable  );
        printf("MPU-TypeExtField =        0x%08x\n",MPU_Init.TypeExtField  );
        printf("MPU-AccessPermission=     0x%08x\n",MPU_Init.AccessPermission  );
        printf("MPU-DisableExec =         0x%08x\n",MPU_Init.DisableExec  );
        printf("MPU-IsShareable =         0x%08x\n",MPU_Init.IsShareable  );
        printf("MPU-IsCacheable =         0x%08x\n",MPU_Init.IsCacheable  );
        printf("MPU-IsBufferable =        0x%08x\n",MPU_Init.IsBufferable  );
}



// /**
//   * @brief  Disables the MPU
//   * @retval None
//   */
// void HAL_MPU_Disable(void)
// {
//   /* Make sure outstanding transfers are done */
//   __DMB();

//   /* Disable fault exceptions */
//   SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;

//   /* Disable the MPU and clear the control register*/
//   MPU->CTRL = 0;
// }

/**
  * @brief  Enables the MPU
  * @param  MPU_Control Specifies the control mode of the MPU during hard fault,
  *         NMI, FAULTMASK and privileged access to the default memory
  *         This parameter can be one of the following values:
  *            @arg MPU_HFNMI_PRIVDEF_NONE
  *            @arg MPU_HARDFAULT_NMI
  *            @arg MPU_PRIVILEGED_DEFAULT
  *            @arg MPU_HFNMI_PRIVDEF
  * @retval None
  */
// void HAL_MPU_Enable(uint32_t MPU_Control)
// {
//   /* Enable the MPU */
//   MPU->CTRL = MPU_Control | MPU_CTRL_ENABLE_Msk;

//   /* Enable fault exceptions */
//   SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

//   /* Ensure MPU setting take effects */
//   __DSB();
//   __ISB();
// }
// /**
//   * @brief  Initializes and configures the Region and the memory to be protected.
//   * @param  MPU_Init Pointer to a MPU_Region_InitTypeDef structure that contains
//   *                the initialization and configuration information.
//   * @retval None
//   */
// void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init)
// {
//   /* Check the parameters */
//   assert_param(IS_MPU_REGION_NUMBER(MPU_Init->Number));
//   assert_param(IS_MPU_REGION_ENABLE(MPU_Init->Enable));

//   /* Set the Region number */
//   MPU->RNR = MPU_Init->Number;

//   if ((MPU_Init->Enable) != RESET)
//   {
//     /* Check the parameters */
//     assert_param(IS_MPU_INSTRUCTION_ACCESS(MPU_Init->DisableExec));
//     assert_param(IS_MPU_REGION_PERMISSION_ATTRIBUTE(MPU_Init->AccessPermission));
//     assert_param(IS_MPU_TEX_LEVEL(MPU_Init->TypeExtField));
//     assert_param(IS_MPU_ACCESS_SHAREABLE(MPU_Init->IsShareable));
//     assert_param(IS_MPU_ACCESS_CACHEABLE(MPU_Init->IsCacheable));
//     assert_param(IS_MPU_ACCESS_BUFFERABLE(MPU_Init->IsBufferable));
//     assert_param(IS_MPU_SUB_REGION_DISABLE(MPU_Init->SubRegionDisable));
//     assert_param(IS_MPU_REGION_SIZE(MPU_Init->Size));

//     MPU->RBAR = MPU_Init->BaseAddress;
//     MPU->RASR = ((uint32_t)MPU_Init->DisableExec             << MPU_RASR_XN_Pos)   |
//                 ((uint32_t)MPU_Init->AccessPermission        << MPU_RASR_AP_Pos)   |
//                 ((uint32_t)MPU_Init->TypeExtField            << MPU_RASR_TEX_Pos)  |
//                 ((uint32_t)MPU_Init->IsShareable             << MPU_RASR_S_Pos)    |
//                 ((uint32_t)MPU_Init->IsCacheable             << MPU_RASR_C_Pos)    |
//                 ((uint32_t)MPU_Init->IsBufferable            << MPU_RASR_B_Pos)    |
//                 ((uint32_t)MPU_Init->SubRegionDisable        << MPU_RASR_SRD_Pos)  |
//                 ((uint32_t)MPU_Init->Size                    << MPU_RASR_SIZE_Pos) |
//                 ((uint32_t)MPU_Init->Enable                  << MPU_RASR_ENABLE_Pos);
//   }
//   else
//   {
//     MPU->RBAR = 0x00;
//     MPU->RASR = 0x00;
//   }
// }