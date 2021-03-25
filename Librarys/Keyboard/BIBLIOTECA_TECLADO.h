#ifndef BIBLIOTECA_TECLADO_H
    
  #define Colunas    P2IN
  #define MUX        P2OUT
  #define Linhas     P1OUT
  #define BitCtlMUX  BIT2
  #define Bit0MUX    BIT0
  #define Bit1MUX    BIT1

  char FctTCL(const char* Apt_T,const char* Apt_L);
  char LeColunas();

#endif