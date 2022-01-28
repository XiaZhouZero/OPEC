.section  .rodata.HexboxAccessList

.global __hexbox_comp95_acl
.type  __hexbox_comp95_acl, %object
__hexbox_comp95_acl:
  .word 0x7
  .word 0x2001f824 /* Name: SDFatFs, Addr:0x2001f824, Size: 560 */
  .word 0x2001fa54
  .word 0x2001fc00 /* Name: MyFile, Addr:0x2001fc00, Size: 556 */
  .word 0x2001fe2c
  .word 0x40012c24 /* Name: recording, Addr:0x40012c24, Size: 12 */
  .word 0x40012c30
  .word 0x40012c08 /* Name: recording, Addr:0x40012c08, Size: 8 */
  .word 0x40012c10
  .word 0x40012c00 /* Name: recording, Addr:0x40012c00, Size: 4 */
  .word 0x40012c04
  .word 0x40012c38 /* Name: recording, Addr:0x40012c38, Size: 4 */
  .word 0x40012c3c
  .word 0x40012c80 /* Name: recording, Addr:0x40012c80, Size: 4 */
  .word 0x40012c84
  .size  __hexbox_comp95_acl, .-__hexbox_comp95_acl

.global __hexbox_comp99_acl
.type  __hexbox_comp99_acl, %object
__hexbox_comp99_acl:
  .word 0x1
  .word 0x420e0040 /* Name: recording, Addr:0x420e0040, Size: 8 */
  .word 0x420e0048
  .size  __hexbox_comp99_acl, .-__hexbox_comp99_acl

.global __hexbox_comp92_acl
.type  __hexbox_comp92_acl, %object
__hexbox_comp92_acl:
  .word 0x1
  .word 0x2001f400 /* Name: uSdHandle, Addr:0x2001f400, Size: 96 */
  .word 0x2001f460
  .size  __hexbox_comp92_acl, .-__hexbox_comp92_acl

.global __hexbox_comp156_acl
.type  __hexbox_comp156_acl, %object
__hexbox_comp156_acl:
  .word 0x3
  .word 0x2001f824 /* Name: SDFatFs, Addr:0x2001f824, Size: 560 */
  .word 0x2001fa54
  .word 0x2001fc00 /* Name: MyFile, Addr:0x2001fc00, Size: 556 */
  .word 0x2001fe2c
  .word 0x20000fd4 /* Name: recording, Addr:0x20000fd4, Size: 8 */
  .word 0x20000fdc
  .size  __hexbox_comp156_acl, .-__hexbox_comp156_acl

.global __hexbox_acl_lut
.type  __hexbox_acl_lut, %object
__hexbox_acl_lut:
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word __hexbox_comp156_acl
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
  .word __hexbox_comp92_acl
  .word __hexbox_comp95_acl
  .word __hexbox_comp99_acl
  .word 0
  .size  __hexbox_acl_lut, .-__hexbox_acl_lut

