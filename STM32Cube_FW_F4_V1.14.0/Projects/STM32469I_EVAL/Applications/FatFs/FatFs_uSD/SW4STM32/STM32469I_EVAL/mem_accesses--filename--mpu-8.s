.section  .rodata.HexboxAccessList

.global __hexbox_comp10_acl
.type  __hexbox_comp10_acl, %object
__hexbox_comp10_acl:
  .word 0x6
  .word 0x2001f924 /* Name: SDFatFs_MyFile, Addr:0x2001f924, Size: 1116 */
  .word 0x2001fd80
  .word 0x2001fdac /* Name: uSdHandle, Addr:0x2001fdac, Size: 96 */
  .word 0x2001fe0c
  .word 0x2001fe10 /* Name: uSdCardInfo, Addr:0x2001fe10, Size: 88 */
  .word 0x2001fe68
  .word 0x40012c24 /* Name: recording, Addr:0x40012c24, Size: 12 */
  .word 0x40012c30
  .word 0x40012c04 /* Name: recording, Addr:0x40012c04, Size: 12 */
  .word 0x40012c10
  .word 0x40012c38 /* Name: recording, Addr:0x40012c38, Size: 4 */
  .word 0x40012c3c
  .size  __hexbox_comp10_acl, .-__hexbox_comp10_acl

.global __hexbox_comp12_acl
.type  __hexbox_comp12_acl, %object
__hexbox_comp12_acl:
  .word 0x2
  .word 0x40005400 /* Name: recording, Addr:0x40005400, Size: 16 */
  .word 0x40005410
  .word 0x4000541c /* Name: recording, Addr:0x4000541c, Size: 8 */
  .word 0x40005424
  .size  __hexbox_comp12_acl, .-__hexbox_comp12_acl

.global __hexbox_comp0_acl
.type  __hexbox_comp0_acl, %object
__hexbox_comp0_acl:
  .word 0x3
  .word 0x40012c38 /* Name: recording, Addr:0x40012c38, Size: 4 */
  .word 0x40012c3c
  .word 0x40012c80 /* Name: recording, Addr:0x40012c80, Size: 4 */
  .word 0x40012c84
  .word 0x40012c00 /* Name: recording, Addr:0x40012c00, Size: 4 */
  .word 0x40012c04
  .size  __hexbox_comp0_acl, .-__hexbox_comp0_acl

.global __hexbox_comp16_acl
.type  __hexbox_comp16_acl, %object
__hexbox_comp16_acl:
  .word 0x2
  .word 0x40012c38 /* Name: recording, Addr:0x40012c38, Size: 4 */
  .word 0x40012c3c
  .word 0x40012c2c /* Name: recording, Addr:0x40012c2c, Size: 4 */
  .word 0x40012c30
  .size  __hexbox_comp16_acl, .-__hexbox_comp16_acl

.global __hexbox_acl_lut
.type  __hexbox_acl_lut, %object
__hexbox_acl_lut:
  .word __hexbox_comp0_acl
  .word __hexbox_comp10_acl
  .word 0
  .word __hexbox_comp12_acl
  .word 0
  .word 0
  .word 0
  .word __hexbox_comp16_acl
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .size  __hexbox_acl_lut, .-__hexbox_acl_lut

