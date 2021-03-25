#include "io430.h"
#include "BIBLIOTECA_LCM.h"
#include "BIBLIOTECA_TECLADO.h"

#define MUX        P2OUT
#define BitCtlMUX  BIT2
#define Bit0MUX    BIT0
#define Bit1MUX    BIT1

  // Declaração das variáveis que armazenam os valores de data e hora para o Relógio

char  VetorTransforma[] = {0,0};

char  AnoRLG       = 21,    
      MesRLG       = 3,
      DiaRLG       = 11,  
      HoraRLG      = 23,
      MinutoRLG    = 58,
      SegundosRLG  = 00;
  // Declaração das variáveis que armazenam os valores de data e hora para o Calendário
char  AnoDES       = 21,    
      MesDES       = 3  ,
      DiaDES       = 11 ,  
      HoraDES      = 23 ,
      MinutoDES    = 59 ,
      SegundosDES  = 00 ;

  // Declaração 

char  SegundoAUX    = 0,
      MinutoAUX     = 0,
      HorasAUX      = 0;
  
  // Declaração das variáveis que armazenam os flags de controle do relógico

char   SelecionaModo = 1,
       Cursor        = 0x80,
       AnulaModo     = 0,
       VarHabDesp    = 0,
       VarDesperta   = 0,
       Tecla            ,
       ValDia           ,
       ValDiaDES        ,
       FlagPausa        ,
       FlagZerar        ,
       FlagSalvamento   ,
       FlagSecBaseT   = 0,
       SegundoTEMP    = 0,
       MinutoTEMP     = 0,
       HorasTEMP      = 0;
  
  // Declaração do vetor que armazena string de ImprimeString
  unsigned const char DadoSTR[]={"   WAKE UP JK   "};
  
  const char TAB_L[]={0x0E,0x0D,0x0B,0x07,0x00};
  const char TAB_T[]={0x00,'7','8','9','C','4','5','6','T','1','2','3','D','<','0','>','A'};
  
  void ImprimeEsteticaAUX();
  void SelModo();
  void TrataTecla();
  void PosCursor(char POS);
  void configuraLCM();
  void ContaDES();
  void ImprimeString(unsigned const char* AptSTR);
  void programaLCM(); 
  void TransformaBCD(char valor);
  void ImprimeDES();
  void ImprimeEstetica();       
  void ContaRLG();              
  void imprimeCampo(unsigned char , unsigned char , unsigned char );
  void EnviaDado(unsigned char);
  void ImprimeRLG();
  void enviaComando(unsigned char);
  void ConfigCLK();
  void ConfigCCA();
  void ConfigIO();
  char FctTCL(const char* Apt_T,const char* Apt_L);
  char LeColunas();

void main()
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  
  ConfigIO();                           // Realiza a chamada de ConfigIO
  ConfigCLK();                          // Realiza a chamada de ConfigCLK
  ConfigCCA();                          // Realiza a chamada de ConfigCCA
  TA0CCR0 = 32768;                         
  __bis_SR_register(GIE);   	        // Habilita interrupção geral
  
  configuraLCM();          // Configura as portas do MSP.
  programaLCM();           // Programa o LCM 
  ImprimeEstetica();         // Configura os : e / para separar as variaveis mostradas
  ContaRLG();              // Realiza a  verificação das variáveis no LCM

  while(1)
  {
    ContaRLG();
    SelModo();
    Tecla = FctTCL(TAB_T,TAB_L);
    if(Tecla)
    {
      __delay_cycles(320000);
      TrataTecla();
      Tecla = 0x00;
    }
  }
  
}

/*------------------------------------------------------------------------------ 
Função: SelModo
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void SelModo()
{
  switch(SelecionaModo)
  {
    case 0:
      TA0CCTL0 &= ~(CCIE);
      ContaRLG();
      ImprimeRLG();
      break;
        
    case 1: // REL/CAL
      if( (VarHabDesp) && (AnoDES == AnoRLG) && (MesDES == MesRLG) && (DiaDES == DiaRLG) && (HoraDES == HoraRLG) && (MinutoDES == MinutoRLG) )
      {
        __delay_cycles(300000);
        enviaComando(0x01);
        enviaComando(0x0C);
        PosCursor(0x82);
        ImprimeString(DadoSTR);
        __delay_cycles(800000);
        enviaComando(0x01);
        enviaComando(0x0C);
      }
      ImprimeEstetica();
      ImprimeRLG();
      break;
          
    case 2: // TEMPORIZADOR
      if(AnulaModo)
      {
        SelecionaModo = 1;
        AnulaModo     = 0;
        FlagPausa     = 1;
        FlagZerar     = 1;
        FlagSecBaseT  = 0;
        SegundoTEMP   = 0; 
        MinutoTEMP    = 0;
        HorasTEMP     = 0;
        SegundoAUX    = 0;
        MinutoAUX     = 0;
        HorasAUX      = 0;
        break;
      }
      if(FlagPausa && FlagZerar && FlagSalvamento)
      {
        FlagSalvamento = 0;
        SegundoTEMP    = SegundoAUX;
        MinutoTEMP     = MinutoAUX;
        HorasTEMP      = HorasAUX;
      }
      if(FlagZerar)
      {
        FlagPausa     = 1;
        SegundoAUX    = SegundoTEMP;
        MinutoAUX     = MinutoTEMP;
        HorasAUX      = HorasTEMP;
      }
      if(FlagPausa)
      {
        FlagSecBaseT = 0;
      }
      else
      {
        FlagSecBaseT = 1;
        if(SegundoAUX == 0)
        {
          if(MinutoAUX == 0)
          {
            if(HorasAUX == 0)
            {
              FlagZerar = 1;
              break;
            }
          MinutoAUX = 59;
          HorasAUX --;
          }
        SegundoAUX = 59;
        MinutoAUX --;
        }
      }
        
      enviaComando(0x0C);
      __delay_cycles(2000);
      imprimeCampo(SegundoAUX,0,10);
      imprimeCampo(MinutoAUX,0,7);
      imprimeCampo(HorasAUX,0,4);
      enviaComando(0x0F);
      PosCursor(Cursor);
      break;
      
    case 3: // CRONÔMETRO
      if(AnulaModo)
      {
        enviaComando(0x0C);
        SelecionaModo = 1;
        AnulaModo     = 0;
        FlagPausa     = 1;
        FlagZerar     = 1;
        SegundoAUX    = 0;
        MinutoAUX     = 0;
        HorasAUX      = 0;
        break;
      }
      if(FlagZerar)
      {
        Zerar:
        enviaComando(0x0F);
        FlagPausa     = 1;
        FlagZerar     = 0; 
        SegundoAUX    = 0;
        MinutoAUX     = 0;
        HorasAUX      = 0;
      }
      if(FlagPausa)
      {
        FlagSecBaseT   = 0;
      }
      else
      {
        FlagSecBaseT   = 1;
        if(SegundoAUX > 60)
        {
          SegundoAUX = 0;
          MinutoAUX++;
        }
        else if( MinutoAUX > 60)
        {
          HorasAUX = 0;
          MinutoAUX =0;
        }
        else if(HorasAUX == 24)
        {
          goto Zerar;
        } 
      }
      enviaComando(0x0C);
      imprimeCampo(SegundoAUX,0,10);
      imprimeCampo(MinutoAUX,0,7);
      imprimeCampo(HorasAUX,0,4);
      break;
         
      
    default: // DESPERTADOR
      ContaDES();
      if(AnulaModo)
      {
        SelecionaModo = 1;
        AnulaModo     = 0;
        break;
      }
      if(VarHabDesp)
      {
        ImprimeDES();
        ImprimeEstetica();
        __delay_cycles(250000);
        enviaComando(0x01);
        enviaComando(0x0C);
        break;
      }
      else
      {
        ImprimeEstetica();
        ImprimeDES();
      }
  }
}
/*------------------------------------------------------------------------------ 
Função: TrataTecla
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void TrataTecla()
{
  char Numeral;
  
  switch(Tecla)
  {
    case 0x00:
      break;
      
    case 'C':
      switch(SelecionaModo)
      {
        case 0:                  // AJUSTE
          break;
          
        case 1:                  // RELOGIO CALENDARIO
          enviaComando(0x01);
          ImprimeEsteticaAUX();
          SelecionaModo = 3;
          FlagZerar = 1;
          Cursor = 0x84;
          break;
          
        case 2:                  // TEMPORIZADOR
          FlagZerar = 1;
          break;
          
        case 3:                  // CRONOMETRO
          FlagZerar = 1;
          break;
          
        default:                 // DESPERTADOR
          VarHabDesp = 1;
          break;
      }
    case 'T':
      if(SelecionaModo == 1)
      {
        enviaComando(0x01);
        ImprimeEsteticaAUX();
        SelecionaModo = 2;
        FlagZerar = 1;
        Cursor = 0x84;
      }
      else if(SelecionaModo == 3 || SelecionaModo == 2)
      {
        FlagPausa = 1;
      }
      break;
      
    case 'D':
      if(SelecionaModo == 1)
      {
        enviaComando(0x01);
        ImprimeEstetica();
        SelecionaModo = 4;
      }
      else if(SelecionaModo == 3 || SelecionaModo == 2)
      {
        FlagPausa = 0;
        FlagZerar = 0;
      }
      else if(SelecionaModo == 4)
      {
        VarHabDesp = 0;
      }
      break;
     
    case 'A':
      if(SelecionaModo == 1)
      {
        SelecionaModo = 0;
        Cursor = 0x80;
      }
      else if(SelecionaModo == 0)
      {
        TA0CCTL0 |= CCIE;
        SelecionaModo = 1;
      }
      else
      {
        enviaComando(0x01);
        AnulaModo = 1;
      }
      break;
       
    case '<':
      if(SelecionaModo == 2 || SelecionaModo == 3)
      {
        if(Cursor > 0x84)
        {
          Cursor--;
          if(Cursor == 0x86 || Cursor == 0x89)
            Cursor--;
        }
      }
      else if(Cursor > 0x80)
      {
        Cursor--;
        if(Cursor == 0x82 || Cursor == 0x85 || Cursor == 0x8A || Cursor == 0x8D)
          Cursor--;
      }
      break;
       
    case '>':
      if(SelecionaModo == 2 || SelecionaModo == 3)
      {
        if(Cursor < 0x8B)
        {
          Cursor++;
          if(Cursor == 0x86 || Cursor == 0x89)
          Cursor++;
        }
      }
      else if(Cursor < 0x8F)
      {
        Cursor++;
        if(Cursor == 0x82 || Cursor == 0x85 || Cursor == 0x8A || Cursor == 0x8D)
        Cursor++;
      }
      PosCursor(Cursor);
      break;
       
    default:
      Numeral = Tecla - 0x30;
       
      VetorTransforma[1] = 0;
      VetorTransforma[0] = 0;
       
      if(SelecionaModo == 2 )
      {
        FlagSalvamento = 1;
        if(Cursor == 0x84 || Cursor == 0x85)
        {
          TransformaBCD(HorasAUX);
          if(Cursor == 0x84)
              VetorTransforma[0] = Numeral;
          
          else
            VetorTransforma[1] = Numeral;
          
          if( (10*VetorTransforma[0] + VetorTransforma[1]) <= 23)
            HorasAUX = 10*VetorTransforma[0] + VetorTransforma[1];
        }
        
        else if(Cursor == 0x87 || Cursor == 0x88)
        {
          TransformaBCD(MinutoAUX);
          if(Cursor == 0x87)
            VetorTransforma[0] = Numeral;
        
          else
            VetorTransforma[1] = Numeral;
        
          if( (10*VetorTransforma[0] + VetorTransforma[1]) <= 59)
            MinutoAUX = 10*VetorTransforma[0] + VetorTransforma[1];
        }
      
        else if(Cursor == 0x8A || Cursor == 0x8B)
        {
          TransformaBCD(SegundoAUX);
        
          if(Cursor == 0x8A)
            VetorTransforma[0] = Numeral;
        
          else
            VetorTransforma[1] = Numeral;
        
          if( (10*VetorTransforma[0] + VetorTransforma[1]) <= 59)
            SegundoAUX = 10*VetorTransforma[0] + VetorTransforma[1];
        }
      }
      else if (SelecionaModo == 0  || SelecionaModo == 4 )
      {
        if(Cursor == 0x80 || Cursor == 0x81)
        {
             if(SelecionaModo)
              TransformaBCD(HoraDES);
             else
              TransformaBCD(HoraRLG);
             
             if(Cursor == 0x80)
              VetorTransforma[0] = Numeral;
             else
              VetorTransforma[1] = Numeral;
             
              if( (10*VetorTransforma[0] + VetorTransforma[1]) < 24)
              {
               if(SelecionaModo)
                HoraDES = 10*VetorTransforma[0] + VetorTransforma[1];
               else
                HoraRLG = 10*VetorTransforma[0] + VetorTransforma[1];
              }
          }
          else if(Cursor == 0x83 || Cursor == 0x84)
          {
             if(SelecionaModo)
              TransformaBCD(MinutoDES);
             else
              TransformaBCD(MinutoRLG);
             
             if(Cursor == 0x83)
              VetorTransforma[0] = Numeral;
             else
              VetorTransforma[1] = Numeral;
             
             if( (10*VetorTransforma[0] + VetorTransforma[1]) <= 59)
             {
              if(SelecionaModo)
                MinutoDES = 10*VetorTransforma[0] + VetorTransforma[1];
          else
            MinutoRLG = 10*VetorTransforma[0] + VetorTransforma[1];
          }
         }
         
         else if(Cursor == 0x86 || Cursor == 0x87)
         {
             if(SelecionaModo)
              TransformaBCD(SegundosDES);
             else
              TransformaBCD(SegundosRLG);
             
             if(Cursor == 0x86)
              VetorTransforma[0] = Numeral;
             else
              VetorTransforma[1] = Numeral;
             
             if( (10*VetorTransforma[0] + VetorTransforma[1]) <= 59)
             {
              if(SelecionaModo)
                SegundosDES = 10*VetorTransforma[0] + VetorTransforma[1];
              else
                SegundosRLG = 10*VetorTransforma[0] + VetorTransforma[1];
          }
         }
         
         else if(Cursor == 0x88 || Cursor == 0x89)
         {      
          if(SelecionaModo)
            TransformaBCD(DiaDES);
          
          else
            TransformaBCD(DiaRLG);
             
          if(Cursor == 0x88)
            VetorTransforma[0] = Numeral;
          
          else
            VetorTransforma[1] = Numeral;
          
          switch(SelecionaModo)
          {
            case 4 :
              if( (((10*VetorTransforma[0] + VetorTransforma[1]) <= ValDiaDES) && ((10*VetorTransforma[0] + VetorTransforma[1]) != 0)) ) 
                DiaDES = 10*VetorTransforma[0] + VetorTransforma[1];
              break;
            default:
              if( ((10*VetorTransforma[0] + VetorTransforma[1]) <= ValDia) && ((10*VetorTransforma[0] + VetorTransforma[1]) != 0) )  
                DiaRLG = 10*VetorTransforma[0] + VetorTransforma[1];
          }
         }
         
         else if(Cursor == 0x8B || Cursor == 0x8C)
         {
          if(SelecionaModo)
            TransformaBCD(MesDES);
          
          else
            TransformaBCD(MesRLG);
             
          if(Cursor == 0x8B)
            VetorTransforma[0] = Numeral;
          
          else
            VetorTransforma[1] = Numeral;
          
          if( (10*VetorTransforma[0] + VetorTransforma[1]) <= 12 && ((10*VetorTransforma[0] + VetorTransforma[1]) != 0))
          {
            if(SelecionaModo)
              MesDES = 10*VetorTransforma[0] + VetorTransforma[1];
            
            else
              MesRLG = 10*VetorTransforma[0] + VetorTransforma[1];
          }
        }
        
        else if(Cursor == 0x8E || Cursor == 0x8F)
        {
          if(SelecionaModo)
            TransformaBCD(AnoDES);
          
          else
            TransformaBCD(AnoRLG);
             
          if(Cursor == 0x8E)
            VetorTransforma[0] = Numeral;
          
          else
            VetorTransforma[1] = Numeral;
             
          if(SelecionaModo)
            AnoDES = 10*VetorTransforma[0] + VetorTransforma[1];
          
          else
            AnoRLG = 10*VetorTransforma[0] + VetorTransforma[1];
        }
      }
    break;
  }
}

/*------------------------------------------------------------------------------ 
Função: ContaRLG
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ContaRLG()           
{
  // VERIFICA RELÓGIO
  unsigned char AnoBi, ValAno;
  
  
     AnoBi = AnoRLG%4;
     if(AnoBi == 0)                             // Verifica se Ano é bissexto
      ValAno = 1;
     else
      ValAno = 0;
     if(MesRLG == 1 || MesRLG == 3 || MesRLG == 5 || MesRLG == 7 || MesRLG == 8 || MesRLG == 10 || MesRLG == 12) // Verifica qual é o mês contado
       ValDia = 31;
     else if( MesRLG == 2 && ValAno)              // Verifica se Fev possui 28 ou 29 dias
       ValDia = 29;
     else if( MesRLG == 2 && (~ValAno))
       ValDia = 28; 
     else
       ValDia = 30;
     
     if((DiaRLG > ValDia))
     {
        if( ValDia == 29)
          DiaRLG = 29; 
        else if( ValDia ==28)              
          DiaRLG = 28;
        else
          DiaRLG = 30;
     }
}

/*------------------------------------------------------------------------------ 
Função: ContaDES
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ContaDES()           
{
  // VERIFICA RELÓGIO
  unsigned char AnoBi, ValAno;
  
  
     AnoBi = AnoDES%4;
     if(AnoBi == 0)                             // Verifica se Ano é bissexto
      ValAno = 1;
     else
      ValAno = 0;
     if(MesDES == 1 || MesDES == 3 || MesDES == 5 || MesDES == 7 || MesDES == 8 || MesDES == 10 || MesDES == 12) // Verifica qual é o mês contado
       ValDiaDES = 31;
     else if( MesDES == 2 && ValAno)              // Verifica se Fev possui 28 ou 29 dias
       ValDiaDES = 29;
     else if( MesDES == 2 && (~ValAno))
       ValDiaDES = 28; 
     else
       ValDiaDES = 30;
     
     if((DiaDES > ValDiaDES))
     {
        if( ValDiaDES == 29)
          DiaDES = 29; 
        else if( ValDiaDES ==28)              
          DiaDES = 28;
        else
          DiaDES = 30;
     }
}

/*------------------------------------------------------------------------------ 
Função: TransformaBCD
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void TransformaBCD(char valor)
{
  if(valor >= 10)
  {
    while(valor >= 10)
    {
      valor -= 10;
      VetorTransforma[0]++;
    }
    VetorTransforma[1] = valor;
  }
  else
  VetorTransforma[1]= valor;
}

/*------------------------------------------------------------------------------
Função: ConfigIO
Descrição: Realiza a configuração das entradas e saídas I/0
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ConfigIO()
{
  P2DIR &= ~(Bit0MUX + Bit1MUX);        // Configura P2.0 e P2.1 como entradas
  P2DIR |=  BitCtlMUX;
  MUX   &= ~BitCtlMUX;
}

/*------------------------------------------------------------------------------
Função: ConfigCLK
Descrição: Realiza a configuração do sistema de clock
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ConfigCLK()
{
  // Seta RSEL e DCO para nível lógico baixo
  BCSCTL1 &= ~(RSEL2+RSEL1+RSEL0);	 
  DCOCTL  &= ~(DCO2+DCO1+DCO0);    	
  // Configuração de DCO para frequência aproximada de 1,05MHz
  BCSCTL1 |= (RSEL2);      		// RSEL = 4 = 100b	
  DCOCTL  |= (DCO2+DCO1);        	// DCO = 6 =  110b (Proteus => 1.05MHz)
  
  //ACLK possui fator de divisão 1 por definição
  
}

/*------------------------------------------------------------------------------
Função: ConfigCCA
Descrição: Realiza a configuração do Timer_A
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ConfigCCA()
{
  // Configuração do Timer_A 
    //Núcleo do Contador
    TA0R=0;                     	// Reseta contador 
    TACTL = TASSEL_1 +          	// Fonte do clock   : ACLK ( 1MHz )
            MC_1 +               	// Modo de contagem : Crescente, conta até atingir o TA1CCR0
            ID_0;			// Divisão por 1
  
    // Bloco Capture/Compare
    TA0CCTL0|=CCIE;                     //Habilita int para o modo capture/compare
}

/*------------------------------------------------------------------------------ 
Função: ImprimeDES
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ImprimeDES()
{
  enviaComando(0x0C);
  imprimeCampo(SegundosDES,0,6);
  imprimeCampo(MinutoDES,0,3);
  imprimeCampo(HoraDES,0,0);
  imprimeCampo(DiaDES,0,8) ;
  imprimeCampo(MesDES,0,11);
  imprimeCampo(AnoDES,0,14);
  enviaComando(0x0F);
  PosCursor(Cursor);
}

/*------------------------------------------------------------------------------ 
Função: ImprimeRLG
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ImprimeRLG()
{
  enviaComando(0x0C);
  imprimeCampo(SegundosRLG,0,6);
  imprimeCampo(MinutoRLG,0,3);
  imprimeCampo(HoraRLG,0,0);
  imprimeCampo(DiaRLG,0,8);
  imprimeCampo(MesRLG,0,11);
  imprimeCampo(AnoRLG,0,14);
  enviaComando(0x0F);
  PosCursor(Cursor); 
}

/*------------------------------------------------------------------------------ 
Função: ImprimeEstetica
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ImprimeEstetica()
{
  enviaComando(0x0C);
  PosCursor(0x82);
  EnviaDado(':');
  PosCursor(0x85);
  EnviaDado(':');
  PosCursor(0x8A);
  EnviaDado('/');
  PosCursor(0x8D);
  EnviaDado('/');
  enviaComando(0x0F);
  PosCursor(Cursor);
}

/*------------------------------------------------------------------------------ 
Função: ImprimeEsteticaAUX
Descrição: -
Entrada: -
Saída: -
------------------------------------------------------------------------------*/
void ImprimeEsteticaAUX()
{
  enviaComando(0x0C);
  PosCursor(0x86);
  EnviaDado(':');
  PosCursor(0x89);
  EnviaDado(':');
  PosCursor(0x84);
  enviaComando(0x0F);
  PosCursor(Cursor);
}


// Interrupção do Timer0
#pragma vector= TIMER0_A0_VECTOR        
__interrupt void BaseRelogio()
{
  SegundosRLG++;
  if(FlagSecBaseT && (SelecionaModo == 3) )
    SegundoAUX ++;
  if(FlagSecBaseT && (SelecionaModo == 2) )
    SegundoAUX --;
  
  if((SegundosRLG > 59))
  {
    SegundosRLG = 0;
    MinutoRLG++;
    if((MinutoRLG > 59))
    {
      MinutoRLG = 0;
      HoraRLG++;
      if((HoraRLG > 23))
      {
        HoraRLG = 0;
        DiaRLG++;
        if( (DiaRLG > ValDia) )
        {
          DiaRLG = 1;
          MesRLG++;
          if( (MesRLG > 12))
          {
            MesRLG = 0;
            AnoRLG++;
          }
        }
      }
    }        
  }
}
