#ifndef MPU_ENTRY_H
#define MPU_ENTRY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mpu_def.h"
#include "mpu_api.h"

#define POLICY_BASE_ADDR 0x10000000      // policy section base address
#define OPERATION_NUM 4
#define PER_MPU_POLICY_SIZE 64
#define SHADOW_DATA_TABLE_BASE_ADDR  POLICY_BASE_ADDR+OPERATION_NUM*PER_MPU_POLICY_SIZE
#define SDT_MAXITEM 37

struct MPU_Policy
{
    uint32_t base_addr[8];
    uint32_t attr[8];
};

typedef struct shadow_data_table{
    void *start;
    void *orginal;
    uint32_t size;
}SDT;

#define SVC_00 __asm("SVC #0");
#define SVC_01 __asm("SVC #1");
#define SVC_02 __asm("SVC #2");
#define SVC_03 __asm("SVC #3");
#define SVC_04 __asm("SVC #4");


// 需要保护的全局变量
unsigned int former_operation_id;       // 之前的operation ID, 用于push shadow variables
//unsigned int current_operation_id;      // 当前的operation ID, 用于pull shadow variables
SDT sdt[SDT_MAXITEM];

uint32_t locate_shadow_data_section(unsigned int operation_id);
void shadow_data_table_init(unsigned int operation_id);
void pull_shadow_vars(unsigned int operation_id);
void push_shadow_vars(unsigned int operation_id);
void default_action(void);
void switch_mpu_config(unsigned int current_operation_id);

#endif