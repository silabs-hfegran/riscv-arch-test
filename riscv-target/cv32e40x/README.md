##Failing tests

#The following tests fail due to not all bits in mtvec being writable in machine mode
* cebreak-01
* ebreak
* ecall

#The following tests fail due to hardware-support for misaligned load/stores
* misalign-lh-01
* misalign-lhu-01
* misalign-lw-01
* misalign-sh-01
* misalign-sw-01
