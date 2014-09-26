/* 
 * File:   usart_eco1_main.c
 * Author: aguivone
 *
 * Fuente http://microcontroladores-c.blogspot.com.ar/2013/03/rs232-com-o-mplab-xc8.html
 * Hace eco de lo que recibe por usart (rs-232)
 * Created on 19 de abril de 2014, 15:16
 *
 * La unica modificacion al codigo base original fue agregar un delay de 1 ms
 * cuando se le pasa cada caracter del string para enviar.
 */
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
// CONFIG

#pragma config FOSC = XT      // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF     // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON     // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON     // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF    // Brown-out Detect Enable bit (BOD enabled)
#pragma config LVP = OFF      // Low-Voltage Programming Enable bit (RB4/PGM pin has PGM function, low-voltage programming enabled)
#pragma config CPD = OFF      // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF       // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _XTAL_FREQ 4000000 // Freq XTAL 4Mhz

/*
 * 
 */

char caracter;
bit flag_interrupcao = 0;
///////////////////////////////////////////////////interrup��o//////////////////////////////////////////////////////////////
void interrupt RS232(void)//vetor de interrup��o
 {
     caracter = RCREG;
     flag_interrupcao = 1;
     RCIF = 0;//  limpa flag de interrup��o de recep��o
 }

/////////////////////////////////fun�oes usadas pela uart //////////////////////////////////////////////////////
void inicializa_RS232(long velocidade,int modo)
{   /*  Por padr�o � usado o modo 8 bits e sem paridade, mas se necessario ajuste
     *  aqui a configura��o desejada.
     *  verifique datasheet para ver a porcentagem de erro e se a velocidade �
     *  possivel para o cristal utilizado.
    */
    RCSTA = 0x90;  //habilita porta serial (RCSTA.SPEN=1),recep��o de 8 bit em
                   // modo continuo (RCSTA.CREN = 1), asincronico.

    int valor;
        if(modo == 1)
        {//modo = 1 ,modo alta velocidade
         TXSTA = 0x24;//modo assincrono,trasmissao 8 bits.
         valor =(int)(((_XTAL_FREQ/velocidade)-16)/16);//calculo do valor do gerador de baud rate
        }
        else
        {//modo = 0 ,modo baixa velocidade
         TXSTA = 0x20;//modo assincrono,trasmissao 8 bits.
         valor =(int)(((_XTAL_FREQ/velocidade)-64)/64);//calculo do valor do gerador de baud rate
        }
    SPBRG = valor;
    RCIE = 1; //habilita interrup��o de recep��o
    TXIE = 0; //deixa interrup��o de transmiss�o desligado(pois corre se o risco de ter
              //uma interrup��o escrita e leitura ao mesmo tempo)
}
void escreve(char valor)
{
    TXIF = 0;//limpa flag que sinaliza envio completo.
    TXREG = valor;
    while(TXIF ==0);//espera enviar caracter
}
void imprime(const char frase[])
{
     char indice = 0;
     char tamanho = strlen(frase);
      while(indice < tamanho ) 
       {
           escreve(frase[indice]);
           __delay_ms(1);   // @retux: Agregado sin este delay se envia demasiado rapido.
           indice++;
       }
}

///////// Funcion main ///////////////////////



void main(void)
{
    TRISB = 0X02;//configura portB  B1 (pino RX) como entrada
    PORTB = 0;  // limpar as portas que est�o configuradas como saidas
    inicializa_RS232(9600,1);//modo de alta velocidade
    PEIE = 1;//habilita interrup��o de perifericos do pic
    GIE = 1; //GIE: Global Interrupt Enable bit
    imprime("Usando serial MPlab xc8 \n\r");
    imprime("En Minicom habilite newline con <ctrl-a> a. \n\r");
    imprime("Digite, yo le voy a dar eco. \n\r");
    for(;;)
    {
        if(flag_interrupcao ==  1)
        {   //tem dados para ler
            //imprime("\n\r Digitaste:");
            escreve(caracter);
            flag_interrupcao = 0;
        }

    }//loop infinito
}

