# ArmLab3
RegSwap, AbsVal, DivideUnsigned32

Lab 3A 
The EOR instruction is a fast way to swap the contents of two registers without using an intermediate storage register.  For example, to exchange the values in registers A and B, use the following: 
       A = A EOR B   
       B = A EOR B   
       A = A EOR B 
Write an assembly function RegSwap.  Display the original numbers on the first line and the swapped numbers on the second line of the display.  
Why does this algorithm work?

Lab 3B 
Write an assembly function AbsVal – to compute the absolute value of a number.  Display the original number on the first line and the absolute value on the second line of the display.  Try to use as few instructions as possible. 
Run the program with both positive and negative numbers. 
How many instructions did you need to do the job? 

Lab 3C 
Write an assembly function DivideUnsigned32.  Assume both the dividend and the divisor are positive.  Display the dividend and divisor on the first line and the quotient and remainder on the second line of the display.  
One way to do this is to repeatedly subtract the divisor from the dividend in a loop until the dividend becomes smaller than the divisor.  That is the remainder, and the number of subtractions is the quotient. 
What happens if the dividend and/or the divisor are negative? 

