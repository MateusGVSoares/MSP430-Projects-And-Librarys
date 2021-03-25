#include "io430.h"

#define LCM_DADO_DIR    P1DIR
#define LCM_DADO_OUT    P1OUT
#define LCM_CTL_DIR     P2DIR
#define LCM_CTL_OUT     P2OUT

#define RS  BIT3
#define EN  BIT5 
#define RW  BIT4

void configuraLCM(void)
{
  LCM_DADO_DIR  =  0XFF;
  LCM_DADO_OUT  =  0X00;
  LCM_CTL_DIR  |=  (RS + EN + RW);
  LCM_CTL_OUT  &= ~(RS +EN + RW);
}


void enviaComando(unsigned char cmd)
{
	    
  LCM_DADO_OUT = cmd;
  LCM_CTL_OUT &= ~(EN + RS);
  __delay_cycles(200);
  __delay_cycles(400);
  LCM_CTL_OUT |= EN;
  __delay_cycles(400);
  LCM_CTL_OUT &= ~EN;
}

void EnviaDado(unsigned char Dado)
{
  LCM_CTL_OUT |= RS;
  __delay_cycles(200);
  LCM_DADO_OUT = Dado;
  __delay_cycles(200);
  LCM_CTL_OUT |= EN;
  __delay_cycles(200);
  LCM_CTL_OUT &= ~EN;
}

void programaLCM(void)
{
  enviaComando(0x38);
  enviaComando(0x06);
  enviaComando(0x0C);
  enviaComando(0x01);
}

void PosCursor(char POS)
{
  enviaComando(POS);
}

void ImprimeString(unsigned const char* AptSTR)           
{
  LCM_DADO_OUT = 0x00;
  LCM_CTL_OUT |= RS;
  LCM_CTL_OUT &= ~EN;
  while (*AptSTR != 0x00)
  {
    EnviaDado(*AptSTR);
    AptSTR++;    
  }
}

void imprimeCampo(unsigned char valor, unsigned char lin, unsigned char col)
{
  unsigned char Posiciona;
  unsigned char ValorT[]={0,0};
  ValorT[1]= 0;
  ValorT[0]= 0;
  if(valor >= 10)
  {
    while(valor >= 10)
    {
      valor -= 10;
      ValorT[0]++;
    }
    ValorT[1] = valor;
  }
  else
  ValorT[1]= valor;
  if(lin == 0)
    Posiciona = 0x80;
  else
    Posiciona = 0xC0;
  Posiciona += col;
  PosCursor(Posiciona);
  EnviaDado(0x30 + ValorT[0]);
  PosCursor(Posiciona + 1);
  EnviaDado(0x30 + ValorT[1]); 
}
