#ifndef BIBLIOTECA_LCM_H

    #define LCM_DADO_DIR    P1DIR
    #define LCM_DADO_OUT    P1OUT
    #define LCM_CTL_DIR     P2DIR
    #define LCM_CTL_OUT     P2OUT

    #define RS  BIT3
    #define EN  BIT5 
    #define RW  BIT4
    
    void configuraLCM(void);
    void enviaComando(unsigned char cmd);
    void EnviaDado(unsigned char Dado);
    void programaLCM(void);
    void PosCursor(char POS);
    void ImprimeString(unsigned const char* AptSTR);
    void imprimeCampo(unsigned char valor, unsigned char lin, unsigned char col);

#endif