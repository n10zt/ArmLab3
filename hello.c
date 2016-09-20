//*****************************************************************************
//
// hello.c - Simple hello world example.
//
// Copyright (c) 2011-2015 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.2.111 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************

#include <stdio.h>


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "utils/uartstdio.h"
#include "driverlib/gpio.h"


// Division
#define rQuotient	r0
#define rDividend r1
#define rDivisor	r2

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
// Definitions
//*****************************************************************************
typedef unsigned int sint32;
typedef unsigned long uint32;

//*****************************************************************************
// Function Prototypes
//*****************************************************************************
__asm void Add32 (sint32 *C, sint32 A, sint32 B);
__asm void Mtply32 (sint32 *C, sint32 A, sint32 B);
__asm void MLA32 (sint32 *C, sint32 A, sint32 B, sint32 D);
__asm void RegSwap (sint32 *A, sint32 *B);
__asm void AbsVal (int *A);
//__asm void DivideUnsigned32 (sint32 *C, sint32 A, sint32 B, sint32 *D);
__asm void DivideUnsigned32 (int *C, sint32 A, int B, int *D);

//*****************************************************************************
// FUNCTION: Add32
// DESCRIPTION: Add two number (A and B) and return a 32 bit result C = A+B
// PARAMETERS: r0 = address of C
// 		r1 = A
// 		r2 = B
// RETURN: None
//*****************************************************************************
__asm void Add32 (sint32 *C, sint32 A, sint32 B)
{
	ADDS 	r3, r1, r2				// r3 = A+B
	STR 	r3, [r0]					// C = r3
	BX 		lr								// return
}

//*****************************************************************************
// FUNCTION: Mtply32
// DESCRIPTION: Multiply two number (A and B) and return a 32 bit result C = A+B
// PARAMETERS: r0 = address of C
// 		r1 = A
// 		r2 = B
// RETURN: None
//*****************************************************************************
__asm void Mtply32 (sint32 *C, sint32 A, sint32 B)
{
	MUL 	r3, r1, r2				// r3 = A*B
	STR 	r3, [r0]					// C = r3
	BX 		lr								// return
}

//*****************************************************************************
// FUNCTION: MLA32
// DESCRIPTION: Multiply two number (A and B) and return a 32 bit result C = A+B
// PARAMETERS: r0 = address of C
// 		r1 = A
// 		r2 = B
// 		r3 = D
// RETURN: None
//*****************************************************************************
__asm void MLA32 (sint32 *C, sint32 A, sint32 B, sint32 D)
{
	MLA 	r4, r1, r2, r3		// r4 = (A*B) + C
	STR 	r4, [r0]					// C = r4
	BX 		lr								// return
}

//*****************************************************************************
// FUNCTION: RegSwap
// DESCRIPTION: Swap register A and B
// PARAMETERS: r0 = A, r1 = B
// RETURN: None
//*****************************************************************************
__asm void RegSwap (sint32 *A, sint32 *B)
{
	LDR		r2, [r0]			// r2 = *A
	LDR		r3, [r1]			// r3 = *B
	EOR		r2, r2, r3		// *A = *A EOR *B
	EOR		r3, r2, r3		// *B = *A EOR *B
	EOR		r2, r2, r3		// *A = *A EOR *B
	STR		r2, [r0]			// *A = r2
	STR		r3, [r1]			// *B = r3
	BX 		lr						// return
}

//*****************************************************************************
// FUNCTION: AbsVal
// DESCRIPTION: Get the absolute value of a number
// PARAMETERS: r0 = input number
// RETURN: output number
//*****************************************************************************
__asm void AbsVal (int *A)
{
	LDR		r1, [r0]
	ADD 	r2, r1, r1, ASR #31 	// R1 = x  + (x >= 0) ? 0 : -1
	EOR 	r1, r2, r1, ASR #31 	// R0 = R1 ^ (x >= 0) ? 0 : -1	STR 	r1, [r0]
	STR 	r1, [r0]
	BX 		lr						// return
}

//*****************************************************************************
// FUNCTION: DivideUnsigned32
// DESCRIPTION: Divide positive dividend and divisor
// PARAMETERS: r0 = Address of C, 
// r1 = A
// r2 = B
// r3 = Address of D
// RETURN: output number
//*****************************************************************************
//__asm void DivideUnsigned32 (sint32 *C, sint32 A, sint32 B, sint32 *D)
__asm void DivideUnsigned32 (int *C, sint32 A, int B, int *D)
{
	CMP		r2, #0
	BEQ		endDivision					// branch if divide by zero

	MOV		r4, #0
	MOV		r5,	#1
	
start
	CMP		r2, r1
	MOVLS r2, r2, LSL#1
	MOVLS	r5, r5, LSL#1
	BLS		start
	
next
	CMP		r1, r2
	SUBCS	r1, r1, r2
	ADDCS r4, r4, r5
	
	MOVS  r5, r5, LSR#1
	MOVCC	r2, r2, LSR#1
	
	BCC		next
endDivision
	STR 	r4,[r0]								// quotient
	STR		r1,[r3]								// remainder
	BX 		lr										// return
}

//*****************************************************************************
//
// Lab 3a/b/c.
//
//*****************************************************************************
int main(void)
{
	sint32 num1 = 11;
	int num2 = 5;
	sint32 num3 = 0x5;
	
	int num4 = -2147483647;

	int result32 = 0; 
	int remainder = 0;
	char strEquation[17];
	char strResult[17];

	tContext sContext;
	tRectangle sRect;

	//--------------------------------------------------------------
	//char displayTitle[17] = "Lab2 Addition";
	//char displayTitle[17] = "Lab2 Multiply";

	//char displayTitle[17] = "Lab3a RegSwap";
	//sprintf (displayStr,"[%x x %x]",num1,num2);	// Register swap

	//char displayTitle[17] = "Lab3b AbsVal";
	//sprintf (strEquation,"< %i", num4);	// Absolute value

	char displayTitle[17] = "Lab3c Division";
	sprintf (strEquation,"%x/%i", num1, num2);	// Division
	//--------------------------------------------------------------

	//
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	//
	ROM_FPULazyStackingEnable();

	//
	// Set the clocking to run directly from the crystal.
	//
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
										 SYSCTL_OSC_MAIN);

	//
	// Initialize the display driver.
	//
	CFAL96x64x16Init();

	//
	// Initialize the graphics context.
	//
	GrContextInit(&sContext, &g_sCFAL96x64x16);

	//
	// Fill the top 24 rows of the screen with blue to create the banner.
	//
	sRect.i16XMin = 0;
	sRect.i16YMin = 0;
	sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
	sRect.i16YMax = 18;
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &sRect);

	//
	// Put a white box around the banner.
	//
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);

	// Display banner
	GrContextFontSet(&sContext, g_psFontCm12);
	GrStringDrawCentered(&sContext, displayTitle, -1, GrContextDpyWidthGet(&sContext) / 2, 10, 0);

	// Display the equation.
	GrContextFontSet(&sContext, g_psFontCm12/*g_psFontFixed6x8*/);
	GrStringDrawCentered(&sContext, strEquation, -1, GrContextDpyWidthGet(&sContext) / 2, ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 7, 0);

	//--------------------------------------------------------------
	//Add32 (&result32, num1, num2);

	//Mtply32(&result32, num1, num2);

	//MLA32(&result32, num1, num2, num3);

	//RegSwap(&num1, &num2);
	//sprintf (displayStr,"[%x x %x]",num1,num2);	// Register swap

	//AbsVal(&num4);
	//sprintf (strResult,"> %i", num4);	// Absolute value

	DivideUnsigned32(&result32, num1, num2, &remainder);
	sprintf (strResult,"Q %i, R %i", result32, remainder);	// Division
	//--------------------------------------------------------------

	// Display the function result.
	GrContextFontSet(&sContext, g_psFontCm12/*g_psFontFixed6x8*/);
	GrStringDrawCentered(&sContext, strResult, -1, GrContextDpyWidthGet(&sContext) / 2, ((GrContextDpyHeightGet(&sContext) - 24) / 2) + 19, 0);

	// Flush any cached drawing operations.
	GrFlush(&sContext);

	//
	// We are finished. Hang around doing nothing.
	//
	while(1)
	{
	}
}
