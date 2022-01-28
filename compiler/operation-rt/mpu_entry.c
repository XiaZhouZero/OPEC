#include "mpu_entry.h"

uint32_t locate_shadow_data_section(unsigned int operation_id){
  uint32_t shadow_data_addr;
  struct MPU_Policy* mpu_policy_ptr;
  mpu_policy_ptr = (struct MPU_Policy *)(POLICY_BASE_ADDR + operation_id * PER_MPU_POLICY_SIZE);
  shadow_data_addr = (*mpu_policy_ptr).base_addr[1]; //这个地址是shadow_data_section的基地址，具体参见oi-script/generate_policy.py/gen_mpu_configs
  return shadow_data_addr;
}

void shadow_data_table_init(unsigned int operation_id){
  int i = 0;
  uint32_t shadow_data_offset = 0;
  uint32_t shadow_data_base_addr = 0; //当前operation_id下shadow_data section的基地址
  shadow_data_base_addr = locate_shadow_data_section(operation_id);//该地址可以从MPU配置中解析获得
  for(int i=0;i<SDT_MAXITEM;i++){
    sdt[i].start = NULL;
    sdt[i].orginal = NULL;
    sdt[i].size = 0;
  }
  /*
  //未完代码....    为shadow_data section中的每个shadow data在sdt[37]中注册一个表项
  //while(current_shadow_data!=NULL){
      sdt[i].start = shadow_data_offset;
      sdt[i].size = sizeof(current_shadow_data);
      sdt[i].orginal = ??? ;
      shadow_data_offset += sizeof(current_shadow_data);
      i++;
  }*/
}

// global_variable -> shadow_variable
void pull_shadow_vars(unsigned int operation_id){
  int  i = 0;
  while(sdt[i].size != 0){
    memcpy(sdt[i].start,sdt[i].orginal,sdt[i].size);
    i++;
  }
}

// shadow_variable -> global_variable
void push_shadow_vars(unsigned int operation_id){
  int  i = 0;
  while(sdt[i].size != 0){
    memcpy(sdt[i].orginal,sdt[i].start,sdt[i].size);
    i++;
  }
}

void default_action(void)
{
  return;
}

void switch_mpu_config(unsigned int current_operation_id)      //TBD
{
  uint32_t mpu_policy_addr;
  MPU_Type MPU_reg[8];              // 寄存器格式的MPU_policy
  MPU_Region_InitTypeDef MPU_conf[8];
  struct MPU_Policy* mpu_policy_ptr;

  // 读取operation_id对应的mpu设置
  mpu_policy_ptr = (struct MPU_Policy *)(POLICY_BASE_ADDR + current_operation_id * PER_MPU_POLICY_SIZE);
  
  for(int i=0; i<8; i++){
    MPU_reg[i].RASR= (*mpu_policy_ptr).attr[i];          // 仅用于测试,实际从ELF_MPU_POLICY_SECTION获取
    MPU_reg[i].RBAR= (*mpu_policy_ptr).base_addr[i];     // 仅用于测试,实际从ELF_MPU_POLICY_SECTION获取
    MPU_reg[i].RNR = i;             // 指定MPU_region
    MPU_conf[i]= MPU_decode(MPU_reg[i]);
    MPU_print(MPU_conf[i]);         // 可选
  }
  //MPU_Switch(MPU_conf);           // 需在ARM平台上运行
}