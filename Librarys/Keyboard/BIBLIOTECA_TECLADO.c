#include "io430.h"

#define Colunas    P2IN
#define MUX        P2OUT
#define Linhas     P1OUT
#define BitCtlMUX  BIT2
#define Bit0MUX    BIT0
#define Bit1MUX    BIT1

/*------------------------------------------------------------------------------ 
Função: LeColunas
Descrição: Informa em qual coluna a tecla pressionada está.
Entrada: -
Saída: colunas
------------------------------------------------------------------------------*/
char LeColunas()
{
  char colunas = 0x00;
  MUX |= BitCtlMUX;
  colunas = ((~Colunas & (Bit0MUX + Bit1MUX)))<<2;  // Verifica as colunas 3 e 4
  MUX &= ~BitCtlMUX;
  colunas |= ((~Colunas & (Bit0MUX + Bit1MUX)));  // Verifica as colunas 1 e 2
  return(colunas);
}

/*------------------------------------------------------------------------------ 
Função: FctTCL
Descrição: Retorna qual tecla foi pressionada.
Entrada: const char* Apt_T, const char* Apt_L
Saída: *Apt_T
------------------------------------------------------------------------------*/
char FctTCL(const char* Apt_T,const char* Apt_L)
{
  char VarColunas;
  const char *Apt_TINI = Apt_T;
  
  Linhas &= ~0x0F;                           // Ativa as linhas
  VarColunas=LeColunas();                     
  if(VarColunas)                         // Verifica se Colunas != 0
  {                              
    __delay_cycles(300);
    Linhas  |= 0x0F;                         // Desativa as linhas
    Linhas  &= (*(Apt_L));                  // Ativa a primeira linha( Ativa em nivel lógico baixo)
    VarColunas=LeColunas();
    while( (VarColunas == 0x00) && (*(Apt_L) != 0x00) )
    {
      Apt_T += 4;
      Apt_L ++;
      Linhas  |= 0x0F;                      // Desativa para em seguida ativar somente uma linha para verificação
      Linhas  &= (*(Apt_L));                //Ativa a linha correspondente em Apt_L( Ativa em nivel lógico baixo)
      VarColunas=LeColunas();
    }
   }
  
  if( ( Apt_L != 0x00) && (VarColunas != 0x00))
  {
    switch( VarColunas  )
     {
      case 1:
        Apt_T += 1;
        break;
      case 2:
        Apt_T += 2;
        break;
      case 4:
        Apt_T += 3;
        break;
      default:
        Apt_T += 4;
     }
    }
  else                             
   Apt_T = Apt_TINI;
  
  return(*(Apt_T));
}