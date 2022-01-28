.section  .rodata.HexboxAccessList

.global __hexbox_comp0_acl
.type  __hexbox_comp0_acl, %object
__hexbox_comp0_acl:
  .word 0x4
  .word 0x2001f804 /* Name: SDFatFs_MyFile, Addr:0x2001f804, Size: 1116 */
  .word 0x2001fc60
  .word 0x2001f600 /* Name: uSdHandle_uSdCardInfo, Addr:0x2001f600, Size: 184 */
  .word 0x2001f6b8
  .word 0x40012c38 /* Name: recording, Addr:0x40012c38, Size: 4 */
  .word 0x40012c3c
  .word 0x40012c2c /* Name: recording, Addr:0x40012c2c, Size: 4 */
  .word 0x40012c30
  .size  __hexbox_comp0_acl, .-__hexbox_comp0_acl

.global __hexbox_comp21_acl
.type  __hexbox_comp21_acl, %object
__hexbox_comp21_acl:
  .word 0x4
  .word 0x2001f6b8 /* Name: BSP_SD_MspInit.dma_rx_handle_BSP_SD_MspInit.dma_tx_handle, Addr:0x2001f6b8, Size: 192 */
  .word 0x2001f778
  .word 0x400264a0 /* Name: recording, Addr:0x400264a0, Size: 24 */
  .word 0x400264b8
  .word 0x40026458 /* Name: recording, Addr:0x40026458, Size: 24 */
  .word 0x40026470
  .word 0x40026408 /* Name: recording, Addr:0x40026408, Size: 8 */
  .word 0x40026410
  .size  __hexbox_comp21_acl, .-__hexbox_comp21_acl

.global __hexbox_comp12_acl
.type  __hexbox_comp12_acl, %object
__hexbox_comp12_acl:
  .word 0x3
  .word 0x40012c00 /* Name: recording, Addr:0x40012c00, Size: 16 */
  .word 0x40012c10
  .word 0x40012c24 /* Name: recording, Addr:0x40012c24, Size: 12 */
  .word 0x40012c30
  .word 0x40012c80 /* Name: recording, Addr:0x40012c80, Size: 4 */
  .word 0x40012c84
  .size  __hexbox_comp12_acl, .-__hexbox_comp12_acl

.global __hexbox_comp13_acl
.type  __hexbox_comp13_acl, %object
__hexbox_comp13_acl:
  .word 0x1
  .word 0x420e0040 /* Name: recording, Addr:0x420e0040, Size: 8 */
  .word 0x420e0048
  .size  __hexbox_comp13_acl, .-__hexbox_comp13_acl

.global __hexbox_comp10_acl
.type  __hexbox_comp10_acl, %object
__hexbox_comp10_acl:
  .word 0x1
  .word 0x2001f800 /* Name: SDPath, Addr:0x2001f800, Size: 4 */
  .word 0x2001f804
  .size  __hexbox_comp10_acl, .-__hexbox_comp10_acl

.global __hexbox_comp3_acl
.type  __hexbox_comp3_acl, %object
__hexbox_comp3_acl:
  .word 0x3
  .word 0x2001f490 /* Name: heval_I2c, Addr:0x2001f490, Size: 84 */
  .word 0x2001f4e4
  .word 0x40005400 /* Name: recording, Addr:0x40005400, Size: 20 */
  .word 0x40005414
  .word 0x4000541c /* Name: recording, Addr:0x4000541c, Size: 8 */
  .word 0x40005424
  .size  __hexbox_comp3_acl, .-__hexbox_comp3_acl

.global __hexbox_comp20_acl
.type  __hexbox_comp20_acl, %object
__hexbox_comp20_acl:
  .word 0x2
  .word 0x2001f804 /* Name: MyFile_SDFatFs, Addr:0x2001f804, Size: 1116 */
  .word 0x2001fc60
  .word 0x20000fd4 /* Name: recording, Addr:0x20000fd4, Size: 8 */
  .word 0x20000fdc
  .size  __hexbox_comp20_acl, .-__hexbox_comp20_acl

.global __hexbox_acl_lut
.type  __hexbox_acl_lut, %object
__hexbox_acl_lut:
  .word __hexbox_comp0_acl
  .word __hexbox_comp10_acl
  .word 0
  .word __hexbox_comp12_acl
  .word __hexbox_comp13_acl
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word __hexbox_comp20_acl
  .word __hexbox_comp21_acl
  .word 0
  .word __hexbox_comp3_acl
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .size  __hexbox_acl_lut, .-__hexbox_acl_lut

