/* 
 * File:   binario-in-binario-out.c
 * Author: Daniel
 * Descripción:  Lee un valor análogo en el PIN A0, lo convierte a digital y escribe el dato binario en el puerto C
 *
 * Created on 26 de febrero de 2014, 01:00 PM
 *
 * PIC MCU : PIC16F1824
 * 
 * The uChip project consist of a set of C codes to program PIC devices using XC8 compiler.  
 * In what follows "uChip" stands for every one of the codes and resources included on this repository,  
 * as a whole or each source-file individually.   
 * Copyright (C) 2013-2014  Daniel Mejía R. [danielmejia55@gmail.com](mailto:danielmejia55@gmail.com)  
 *
 * the codes included in uChip are free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, either version 3 of the License, or  
 * (at your option) any later version.  
 *
 * the codes included in uChip are distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
 * GNU General Public License for more details.  
 *
 * You should have received a copy of the GNU General Public License  
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>

/* Ajustar la velocidad del reloj a 8 MHz mediante software, esto lo usara la funcion __delay_ms() */
#define _XTAL_FREQ 8000000

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF         // Low-Voltage Programming Enable (Low-voltage programming enabled)


void CLOCK_setup(void)
{
	//OSCCONbits.SCS = 0b11;    // Use internal Clock defined by IRCF
    OSCCONbits.IRCF = 0b11110;  // Set clock to 8 MHz, uncomment lines above to set to 32 MHz, see datasheet
}


int main(int argc, char** argv)
{
	CLOCK_setup();

    /*Usar Pin RC0 como salida (puerto C como salida en RC0)*/
    TRISCbits.RC0 = 0;
    /*Usar puerto A como entrada en A0 (Pin RA0 como entrada)*/
    TRISAbits.RA0 = 1;

    while(1)
    {
		// Verificar si hay un nivel entre 3.3 V y 5 V en el pin RA0)
		if( PORTAbits.RA0 == 1 )
		{	
			// Poner RC0 en 1, es decir, salida de 5 V approximadamente)
			PORTCbits.RC0 = 1;
		}
		else
		{
			PORTCbits.RC0 = 0;
		}
    }

    return (EXIT_SUCCESS);
}

