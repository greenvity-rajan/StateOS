/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    14.11.2017
    @brief   StateOS port file for ARM Cotrex-M uC.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#if defined(__CSMC__)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

#if __CORTEX_M < 3

void PendSV_Handler( void )
{
	#asm
	xref _core_tsk_handler

	mrs   r0,    PSP
	mov   r3,    lr
	lsrs  r3,    r3, # 3
	bcs   _1
	mov   r0,    sp
	sub   sp,  # 36
_1:	subs  r0,  # 36
	stm   r0!, { r4  - r7 }
	mov   r3,    r8
	mov   r4,    r9
	mov   r5,    r10
	mov   r6,    r11
	mov   r7,    lr
	stm   r0!, { r3  - r7 }
	subs  r0,  # 36

	bl   _core_tsk_handler

	adds  r0,  # 16
	ldm   r0!, { r3  - r7 }
	mov   r8,    r3
	mov   r9,    r4
	mov   r10,   r5
	mov   r11,   r6
	mov   lr,    r7
	subs  r0,  # 36
	ldm   r0!, { r4  - r7 }
	adds  r0,  # 20
	mov   r3,    lr
	lsrs  r3,    r3, # 3
	bcs   _2
	mov   sp,    r0
	bx    lr
_2:	msr   PSP,   r0
	bx    lr

	#endasm
}

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

#if __CORTEX_M >= 3

void PendSV_Handler( void )
{
	#asm
	xref _core_tsk_handler

	tst   lr,  # 4                      ; process stack used?
	itee  ne
	mrsne r0,    PSP
	moveq r0,    sp
#if __FPU_USED
	subeq sp,  # 100
	tst   lr,  # 16                     ; fpu used?
	it    eq
 vstmdbeq r0!, { s16 - s31 }
#else
	subeq sp,  # 36
#endif
	stmdb r0!, { r4  - r11, lr }

	bl   _core_tsk_handler

	ldmia r0!, { r4  - r11, lr }
#if __FPU_USED
	tst   lr,  # 16                     ; fpu used?
	it    eq
 vldmiaeq r0!, { s16 - s31 }
#endif
	tst   lr,  # 4                      ; process stack used?
	ite   ne
	msrne PSP,   r0
	moveq sp,    r0
	bx    lr

	#endasm
}

#endif//__CORTEX_M

/* -------------------------------------------------------------------------- */

void core_tsk_flip( void *sp )
{
	#asm
	xref _core_tsk_loop

	mov   sp,    r0
	bl   _core_tsk_loop

	#endasm
}

/* -------------------------------------------------------------------------- */

#endif // __CSMC__
