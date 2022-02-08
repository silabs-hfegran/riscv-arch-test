// RISC-V Compliance Test Header File
// Copyright (c) 2017, Codasip Ltd. All Rights Reserved.
// See LICENSE for license details.
//
// Description: Common header file for RV32I tests

#ifndef _COMPLIANCE_TEST_H
#define _COMPLIANCE_TEST_H

#include "riscv_test.h"

//-----------------------------------------------------------------------
// RV Compliance Macros
//-----------------------------------------------------------------------

/* RVMODEL_DATA_BEGIN
 * RVMODEL_DATA_END
 * RVMODEL_HALT
 *
 * RVMODEL_BOOT
 * RVMODEL_IO_INIT
 * RVMODEL_IO_CHECK
 * 
 *
 *
 *
 *
 *
 */

#define TESTUTIL_BASE 0x20000000
#define TESTUTIL_ADDR_HALT (TESTUTIL_BASE + 0x10)
#define TESTUTIL_ADDR_BEGIN_SIGNATURE (TESTUTIL_BASE + 0x8)
#define TESTUTIL_ADDR_END_SIGNATURE (TESTUTIL_BASE + 0xc)

#define RVMODEL_HALT                                                    \
        /* tell simulation about location of begin_signature */               \
        la t0, begin_signature;                                               \
        li t1, TESTUTIL_ADDR_BEGIN_SIGNATURE;                                 \
        sw t0, 0(t1);                                                         \
        /* tell simulation about location of end_signature */                 \
        la t0, end_signature;                                                 \
        li t1, TESTUTIL_ADDR_END_SIGNATURE;                                   \
        sw t0, 0(t1);                                                         \
        /* dump signature and terminate simulation */                         \
        li t0, 1;                                                             \
        li t1, TESTUTIL_ADDR_HALT;                                            \
        sw t0, 0(t1);                                                         \
        RVTEST_PASS                                                           \

#define RVMODEL_BOOT \
        RVMODEL_IO_INIT; \
        RVTEST_CODE_BEGIN_OLD \

#define RVMODEL_SET_MSW_INT
#define RVMODEL_CLEAR_MSW_INT
#define RVMODEL_CLEAR_MTIMER_INT
#define RVMODEL_CLEAR_MEXT_INT

//#define RV_COMPLIANCE_CODE_BEGIN                                        \
//        RVTEST_CODE_BEGIN_OLD                                           \

//#define RV_COMPLIANCE_CODE_END                                          \
//        RVTEST_CODE_END_OLD                                             \

#define RVMODEL_DATA_BEGIN                                              \
        RVTEST_DATA_BEGIN_OLD                                           \

#define RVMODEL_DATA_END                                                \
        RVTEST_DATA_END_OLD                                             \

#endif // _COMPLIANCE_TEST_H

#ifndef _COMPLIANCE_IO_H
#define _COMPLIANCE_IO_H

#ifdef  RVTEST_ASSERT
#  define RVTEST_IO_QUIET 0
#else
#  define RVTEST_IO_QUIET 1
#endif

//-----------------------------------------------------------------------
// RV IO Macros (Character transfer by custom instruction)
//-----------------------------------------------------------------------
#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)

#define RVTEST_CUSTOM1 0x10000000

#ifdef RVTEST_IO_QUIET

#define RVMODEL_IO_WRITE_STR(_SP, _STR)
#define RVMODEL_IO_CHECK()
#define RVMODEL_IO_ASSERT_GPR_EQ(_SP, _R, _I)
#define RVMODEL_IO_ASSERT_SFPR_EQ(_F, _R, _I)
#define RVMODEL_IO_ASSERT_DFPR_EQ(_D, _R, _I)

#else

#define RSIZE 4
// _SP = (volatile register)
#define LOCAL_IO_PUSH(_SP)                                              \
    la      _SP,  begin_regstate;                                       \
    sw      x1,   (1*RSIZE)(_SP);                                       \
    sw      x5,   (5*RSIZE)(_SP);                                       \
    sw      x6,   (6*RSIZE)(_SP);                                       \
    sw      x8,   (8*RSIZE)(_SP);                                       \
    sw      x10,  (10*RSIZE)(_SP);

// _SP = (volatile register)
#define LOCAL_IO_POP(_SP)                                               \
    la      _SP,   begin_regstate;                                      \
    lw      x1,   (1*RSIZE)(_SP);                                       \
    lw      x5,   (5*RSIZE)(_SP);                                       \
    lw      x6,   (6*RSIZE)(_SP);                                       \
    lw      x8,   (8*RSIZE)(_SP);                                       \
    lw      x10,  (10*RSIZE)(_SP);

#define LOCAL_IO_WRITE_GPR(_R)                                          \
    mv          a0, _R;                                                 \
    jal         FN_WriteA0;

#define LOCAL_IO_WRITE_FPR(_F)                                          \
    fmv.x.s     a0, _F;                                                 \
    jal         FN_WriteA0;

#define LOCAL_IO_WRITE_DFPR(_V1, _V2)                                   \
    mv          a0, _V1;                                                \
    jal         FN_WriteA0;                                             \
    mv          a0, _V2;                                                \
    jal         FN_WriteA0;                                             \

#define LOCAL_IO_PUTC(_R)                                               \
    la          t3, RVTEST_CUSTOM1;                                     \
    sw          _R, (0)(t3);                                            \


// Assertion violation: file file.c, line 1234: (expr)
// _SP = (volatile register)
// _R = GPR
// _I = Immediate
#define RVMODEL_IO_ASSERT_GPR_EQ(_SP, _R, _I)                           \
    LOCAL_IO_PUSH(_SP)                                                  \
    mv          s0, _R;                                                 \
    li          t0, _I;                                                 \
    beq         s0, t0, 20002f;                                         \
    LOCAL_IO_WRITE_STR("Assertion violation: file ");                   \
    LOCAL_IO_WRITE_STR(__FILE__);                                       \
    LOCAL_IO_WRITE_STR(", line ");                                      \
    LOCAL_IO_WRITE_STR(TOSTRING(__LINE__));                             \
    LOCAL_IO_WRITE_STR(": ");                                           \
    LOCAL_IO_WRITE_STR(# _R);                                           \
    LOCAL_IO_WRITE_STR("(");                                            \
    LOCAL_IO_WRITE_GPR(s0);                                             \
    LOCAL_IO_WRITE_STR(") != ");                                        \
    LOCAL_IO_WRITE_STR(# _I);                                           \
    LOCAL_IO_WRITE_STR("\n");                                           \
    li TESTNUM, 100;                                                    \
    RVTEST_FAIL;                                                        \
20002:                                                                  \
    LOCAL_IO_POP(_SP)

// _F = FPR
// _C = GPR
// _I = Immediate
#define RVMODEL_IO_ASSERT_SFPR_EQ(_F, _C, _I)                           \
    fmv.x.s     t0, _F;                                                 \
    beq         _C, t0, 20003f;                                         \
    LOCAL_IO_WRITE_STR("Assertion violation: file ");                   \
    LOCAL_IO_WRITE_STR(__FILE__);                                       \
    LOCAL_IO_WRITE_STR(", line ");                                      \
    LOCAL_IO_WRITE_STR(TOSTRING(__LINE__));                             \
    LOCAL_IO_WRITE_STR(": ");                                           \
    LOCAL_IO_WRITE_STR(# _F);                                           \
    LOCAL_IO_WRITE_STR("(");                                            \
    LOCAL_IO_WRITE_FPR(_F);                                             \
    LOCAL_IO_WRITE_STR(") != ");                                        \
    LOCAL_IO_WRITE_STR(# _I);                                           \
    LOCAL_IO_WRITE_STR("\n");                                           \
    li TESTNUM, 100;                                                    \
    RVTEST_FAIL;                                                        \
20003:

// _D = DFPR
// _R = GPR
// _I = Immediate
#define RVMODEL_IO_ASSERT_DFPR_EQ(_D, _R, _I)                           \
    fmv.x.d     t0, _D;                                                 \
    beq         _R, t0, 20005f;                                         \
    LOCAL_IO_WRITE_STR("Assertion violation: file ");                   \
    LOCAL_IO_WRITE_STR(__FILE__);                                       \
    LOCAL_IO_WRITE_STR(", line ");                                      \
    LOCAL_IO_WRITE_STR(TOSTRING(__LINE__));                             \
    LOCAL_IO_WRITE_STR(": ");                                           \
    LOCAL_IO_WRITE_STR(# _D);                                           \
    LOCAL_IO_WRITE_STR("(");                                            \
    LOCAL_IO_WRITE_DFPR(_D);                                            \
    LOCAL_IO_WRITE_STR(") != ");                                        \
    LOCAL_IO_WRITE_STR(# _I);                                           \
    LOCAL_IO_WRITE_STR("\n");                                           \
    li TESTNUM, 100;                                                    \
    RVTEST_FAIL;                                                        \
20005:

// _SP = (volatile register)
#define LOCAL_IO_WRITE_STR(_STR) RVMODEL_IO_WRITE_STR(x31, _STR)
#define RVMODEL_IO_WRITE_STR(_SP, _STR)                                  \
    LOCAL_IO_PUSH(_SP)                                                  \
    .section .data.string;                                              \
20001:                                                                  \
    .string _STR;                                                       \
    .section .text;                                                     \
    la a0, 20001b;                                                      \
    jal FN_WriteStr;                                                    \
    LOCAL_IO_POP(_SP)

// generate assertion listing
#define LOCAL_CHECK() RVTEST_IO_CHECK()
#define RVTEST_IO_CHECK()                                               \
    li zero, -1;                                                        \

.section .text
//
// FN_WriteStr: Uses a0, t0
//
FN_WriteStr:
    mv          t0, a0;
10000:
    lbu         a0, (t0);
    addi        t0, t0, 1;
    beq         a0, zero, 10000f;
    LOCAL_IO_PUTC(a0);
    j           10000b;
10000:
    ret;

//
// FN_WriteA0: write register a0(x10) (destroys a0(x10), t0-t3(x5-x7, x28))
//
FN_WriteA0:
        mv          t0, a0
        // determine architectural register width
        li          a0, -1
        srli        a0, a0, 31
        srli        a0, a0, 1
        bnez        a0, FN_WriteA0_64

FN_WriteA0_32:
        // reverse register when xlen is 32
        li          t1, 8
10000:  slli        t2, t2, 4
        andi        a0, t0, 0xf
        srli        t0, t0, 4
        or          t2, t2, a0
        addi        t1, t1, -1
        bnez        t1, 10000b
        li          t1, 8
        j           FN_WriteA0_common

FN_WriteA0_64:
        // reverse register when xlen is 64
        li          t1, 16
10000:  slli        t2, t2, 4
        andi        a0, t0, 0xf
        srli        t0, t0, 4
        or          t2, t2, a0
        addi        t1, t1, -1
        bnez        t1, 10000b
        li          t1, 16

FN_WriteA0_common:
        // write reversed characters
        li          t0, 10
10000:  andi        a0, t2, 0xf
        blt         a0, t0, 10001f
        addi        a0, a0, 'a'-10
        j           10002f
10001:  addi        a0, a0, '0'
10002:  LOCAL_IO_PUTC(a0)
        srli        t2, t2, 4
        addi        t1, t1, -1
        bnez        t1, 10000b
        ret

#endif // RVTEST_IO_QUIET

#endif // _COMPLIANCE_IO_H

