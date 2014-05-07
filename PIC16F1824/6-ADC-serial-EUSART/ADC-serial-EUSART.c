/*
 * File:   ADC-serial-EUSART.c
 * Author: Daniel M. (danielmejia55@gmail.com)
 *
 * Created on 25 de marzo de 2014, 01:08 PM
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
/*
 *
 */
#include <xc.h>
#include <pic16f1824.h>

// CONFIG1
//#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config FOSC = INTOSC
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
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
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)


#define STRLEN 12

#define _XTAL_FREQ 32000000;

volatile unsigned char t;
volatile unsigned char rcindex;
volatile unsigned char rcbuf[STRLEN];

void delay(int N)
{
    for(int i=0;i<N*1000;i++);
}

int pow(int Num,int e)
{
    int result=1;
    for(int i=0;i<e;i++)
    {
        result*=Num;
    }

    return result;
}

void CLOCK_setup(void)
{
    OSCCONbits.SCS = 0b11;    // Use internal Clock defined by IRCF
    OSCCONbits.IRCF = 0b11110;  // Set clock to 32 MHz
}

void USART_init(void)
{
    /* Configure EUSART with baudrate 9600, at High speed with 8 bit Baud generator*/

    SPBRG = 25;
    BAUDCONbits.BRG16 = 0;  // Disable 16bit Baud generator
    TXSTAbits.BRGH = 1;     // High baud rate mode with 8bit baud generator

    TXSTAbits.SYNC = 0;     // Set asynchronous mode
    RCSTAbits.SPEN = 1;     // Enable EUSART reception

    TXSTAbits.TX9 = 0;      // Set 8 bit mode

    TXSTAbits.TXEN = 1;     // Enable EUSART transmitter PIR1.TXIF should be
                            // Enabled by this action

    INTCONbits.GIE = 1;     // enable interrupts
    INTCONbits.PEIE = 1;    // Enable Peripheral Interrupts
    PIE1bits.TXIE = 1;      // Enable EUSART TX Peripheral Interrupts
    RCSTAbits.CREN = 1;     // enable continous receiving
    PIE1bits.RCIE = 1;      // enable EUSART RX interrupt


    // configure I/O pins


    APFCON0bits.RXDTSEL = 0;  // Select RC5 as RX
    APFCON0bits.TXCKSEL = 0;  // Select RC4 as TX

    TRISCbits.TRISC4 = 0;     // TX pin is output
    TRISCbits.TRISC5 = 1;     // RX pin is input (automatically configured)
}

void USART_putc(unsigned char c)
{
    while (!TXSTAbits.TRMT); // wait until transmit shift register is empty
    TXREG = c;               // write character to TXREG and start transmission
}

void USART_puts(unsigned char *s)
{
    while (*s)
    {
        USART_putc(*s);     // send character pointed to by s
        s++;                // increase pointer location to the next character
    }
}

void USART_puti(unsigned int i)
{
    while (!TXSTAbits.TRMT); // wait until transmit shift register is empty
    TXREG = i;               // write character to TXREG and start transmission
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
    ADCON1bits.ADCS = 0b010; // FOSC/32 clock conversion for Clock at 32 MHz
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

void ADC_convert_and_send(void)
{
    int x,i,ADC_result;
    char ADC_string[10];
//    ADC_setup();
    ADC_convert();

    ADC_result = ((ADRESH << 8) + ADRESL);

    USART_puti(ADC_result);
    USART_puts(ADC_result);
//    for (i=0;i<10;i++)
//    {                             //Conversion ADC_result to string
//        x = 2*(pow(10,i));
//        //x = 2*(10^i);
//
//        if(ADC_result%(x) == 0)
//        {
//            ADC_string[i] = '0';
//        }
//        else
//        {
//            ADC_string[i] = '1';
//            ADC_result = ADC_result - /*(10^i);*/ (pow(10,i));
//        }
//    }
//
//    USART_puts("\nSending ADC result \n");
//    USART_putc(ADC_string);
}

int main(int argc, char** argv)
{
    CLOCK_setup();

    USART_init();
    USART_puts("Init complete!\n");

    TRISCbits.TRISC2 = 0;

    PORTCbits.RC2 = 1;
    delay(1);

    ADC_setup();
    USART_puts("ADC configured!\n");

    while(1)
    {
        USART_puts("Sending...\n");
//        ADC_convert_and_send();
//        PORTCbits.RC2 = 1;
//        delay(2);
//        PORTCbits.RC2 = 0;
    }

    return (EXIT_SUCCESS);
}
