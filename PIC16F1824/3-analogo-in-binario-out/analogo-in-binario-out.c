/* 
 * File:   an-in-b-out.c
 * Author: Daniel
 * Descripción:  Lee un valor análogo en el PIN A0, lo convierte a digital y escribe el dato binario en el puerto C
 *
 * Created on 25 de febrero de 2014, 01:00 PM
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

/* Ajustar la velocidad del reloj a 8 MHz mediante software */
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
/*
 *	Ciclo for para introducir un retraso. puede usarse funcion __delay_ms o __delay_us.
 *	debe buscarse en la documentacion del XC8
 */


void del(int x)
{
    for(int i=0;i<10000*x;i++)
    {

    }
}


void CLOCK_setup(void)
{
	//OSCCONbits.SCS = 0b11;    // Use internal Clock defined by IRCF
    OSCCONbits.IRCF = 0b11110;  // Set clock to 8 MHz, uncomment lines above to set to 32 MHz, see datasheet
}


void ADC_setup(void)
{

    ANSELA=0b00000001; // Choose Analog Pin A0
    TRISAbits.TRISA0 = 1; //Configure Pin as input

    /* ADCON1 Register
     * | Bit 7 | Bit 6 -> Bit 4 |  Bit3  | Bit 2|Bit 1 -> Bit 0|
     * | ADFM  |   ADCS <2:0>   |  N/A   |ADNREF| ADPREF <1:0> |
     * |justif.|     CHS<4:0>   | Go/Done| ADON |              |*/
    /*
     * ADFM Left (0) or Right (1) justified result in ADRESH ADRESL
     * ADCS Choose conversion clock select bits
     * ADNREF Negative voltage reference
     * ADPREF Positive voltage reference
     */
//    ADCON1=0b11000000;  // Configure Clock and Justification of result
    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCS = 0b001; // FOSC/8 clock conversion for 8 Mhz clock speed
    ADCON1bits.ADNREF = 0;
    ADCON1bits.ADPREF = 0b00;

    /* ADCON0 Register
     * | Bit 7 | Bit 6 -> Bit 2 |  Bit1   | Bit 0|
     * |No Uso |CH0, CH1,CH2,CH3|
     * |   x   |     CHS<4:0>   | Go/Done | ADON |*/
    /* CHS se usa para elegir el canal de conversión -> CH0
     * ADON es para encender (1) o apagar (0) el conversor -> 1
     */
    //ADCON0=0b00000011;    // Choose chanel CH0 for conversion
    ADCON0bits.CHS = 0b0000;
//    ADCON0bits.GO_nDONE = 1;
    //ADCON0bits.GO = 1;
    ADCON0bits.ADON = 1;

    PIE1bits.ADIE = 1;   //Enable ADC Interrupts

}


void ADC_convert(void)
{
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO!=0);
//    while(!PIR1bits.ADIF);
}



int main(int argc, char** argv)
{
	CLOCK_setup();
	
	ADC_setup();

    /*Usar puerto C como salida*/
    TRISC = 0b0000000;

    int result;

    while(1)
    {

		ADC_convert();

		// Corremos el resultado ADRESH en 8 bits a la izquierda, lo concatenamos con ADRESL.
		// Esto se hace porque el resultado está justificado a la derecha.
		result = (ADRESH << 8) | ADRESL;
		
		// Tomar en cuenta que el puerto es de 6 bits RC0-RC5, y no de 10 bits
		// por lo tanto la variable no se almacerá correctamente.
		// Consulte el tipo int16_t para manejar datos de 10bits en los PIC. 
		PORTC = result;
    }

    return (EXIT_SUCCESS);
}

