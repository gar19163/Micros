/*
 * File:        Proyecto02.c
 * Dispositivo:	PIC16f887
 * Autor:		Alejandro Garcia Aguirre
 * Compilador:	XC8 (v2.31), MPLABX V5.50
 * 
 * Programa:	Robot OTTO
 * Hardware:    Entradas
 *              Poteciometro de control lado izquieredo A0-1 por canales
 *              Potenciometro pie izquierdo             A0   canal 0
 *              Potenciometro pierna izquierdo          A1   canal 1
 *              Poteciometro de control lado derecho    A2-3 por canales
 *              Potenciometro pie derecho               A2   canal 2
 *              Potenciometro pierna derecha            A3   canal 3
 *              Push Button para grabar en              B7
 *              Push Button para reproducir en          B6
 * 
 *              Salidas
 *              Servo motor pie izquierda               C2   CCP1
 *              Servo motor pierna izquierda            C1   CCP2
 *              Servo motor pie derecho                 C0   bit-banged
 *              Servo motor pierna derecha              C3   bit-banged
 *              Comunicaión UART                        C6   TX/CK
 *              Comunicaión UART                        C7   RX/DT
 *              Luz led que indica que esta grabando    D2
 *              Luz led  que esta reproduciendo         D3
 *              
 * Created on 16 de mayo de 2021, 05:06 PM
 * Última modificación:  4 de junio de 2021
 */

//-------------------------------------------------------
//   Palabras de configuración
//-------------------------------------------------------

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO
                                //  oscillator: I/O function on RA6/OSC2/CLKOUT
                                // pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit 
                                // (WDT disabled and can be enabled by SWDTEN 
                                //  bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit 
                                // (RE3/MCLR pin function is digital input, 
                                // MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code 
                                // protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit 
                                // (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
                                // (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit 
                                // (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON        // Low Voltage Programming Enable bit 
                                // (RB3/PGM pin has PGM function, low voltage 
                                // programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit 
                                // (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits 
                                // (Write protection off)

//-------------------------------------------------------
//   Librerias incluidas
//-------------------------------------------------------
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

//-------------------------------------------------------
//  Directivas del compilador
//-------------------------------------------------------
#define _XTAL_FREQ 8000000  //Delay
#define addresseeprom 0x10  //Dirección de almacenamiento 

//-------------------------------------------------------
//  Variables
//-------------------------------------------------------
int rec = 0;                //Estado del boton
int play = 0;               //Estado del boton
int op   = 0;
int ban  = 1;
char adc1;               //Valor de ADC
char adc2;               //Valor de ADC
char adc3;               //Valor de ADC
char adc4;               //Valor de ADC

//-------------------------------------------------------
//  arreglos
//-------------------------------------------------------

//-------------------------------------------------------
//   Prototipo
//------------------------------------------------------
void setup(void);
void send_char (char dato);
void send_str  (char st[]);
//uint8_t WRTEEPROM(uint8_t data, uint8_t address);
//char RDEEPROM(unsigned address);
void servo1Rotate0(void);
void servo1Rotate90(void);
void servo2Rotate0(void);
void servo2Rotate90(void);
void servo3Rotate0(void);
void servo3Rotate90(void);
void servo4Rotate0(void);
void servo4Rotate90(void);
//-------------------------------------------------------
//   Interrupciones
//------------------------------------------------------
void __interrupt()isr(void){
    
    if(PIR1bits.RCIF){                  //Interrupción de recepción
        op = RCREG; 
    }
    
    if (RBIF == 1){  //interrupción push button
        if (PORTBbits.RB7 == 0){    //play
            if (play==0){
                play++;
                rec=0;
                PORTDbits.RD2   = 1;
                PORTDbits.RD3   = 0;
//                RDEEPROM(addresseeprom);
                if (ADRESH <128)            //Seleciono el rago para posición
                    servo3Rotate0();        //Servo3 a 0 grados
                else
                    servo3Rotate90();       //Servo3 a 90 grados
                __delay_ms(30);
                        
                ADRESH = adc2;
                if (ADRESH <128)
                    servo4Rotate0();        //Servo4 a 0 grados 
                else
                    servo4Rotate90();       //Servo a 90 grados
                __delay_ms(30);
                        
                ADRESH = adc3;
                if (ADRESH <128)
                    servo1Rotate0();        //Servo1 a 0 grados
                else
                    servo1Rotate90();       //Servo a 90 grados
                __delay_ms(30);

                if (ADRESH <128)
                    servo2Rotate0();        //Servo2 a 0 grados
                else
                    servo2Rotate90();       //Servo a 90 grados
                __delay_ms(30);
            }
            else if (play==1){
                play = 0;                   //Apago modulo
                PORTDbits.RD2   = 0;        //Apago led
            }            
        }
        
        if (PORTBbits.RB6 == 0){            //Rec
            if (rec==0){
                play = 0;                   //Aseguro que play sea 0
                rec++;                      //Sumii contador 
                PORTDbits.RD2   = 0;        //Lez del modulo
                PORTDbits.RD3   = 1;
//                WRTEEPROM(adc1, addresseeprom);//Llamo funcion
                
               ADRESH = adc1;               //Guardo dato
                __delay_ms(30);
                ADRESH = adc2;
                 __delay_ms(30);
                 ADRESH = adc3;
                __delay_ms(30);
                ADRESH = adc4;
                 __delay_ms(30);
                
            }
            else if (rec==1){
                rec = 0;                //apago modulo
                PORTDbits.RD3   = 0;    //apago led
            }            
        }
    }
        
    
	if(PIR1bits.ADIF == 1){         //Interrupción ADC Lado Izquierdo
        
        PIR1bits.ADIF = 0;          //Bajo bandera
        
        if(ADCON0bits.CHS == 0){    //PWM 1 
            CCPR1L = (ADRESH >> 1) + 60;     //Rota de 90 a 0
            CCP1CONbits.DC1B1 = ADRESH & 0b01;
            CCP1CONbits.DC1B0 = (ADRESL>>7);         
        }
        else if (ADCON0bits.CHS == 1){        //PWM 2
            CCPR2L = (ADRESH >> 1) + 60;   //Se rota 1 bit de 0 a 90
            CCP2CONbits.DC2B1 = ADRESH & 0b01;//LSB
            CCP2CONbits.DC2B0 = (ADRESL>>7);  //MSB analogico 
        }        
    }
}

//-------------------------------------------------------
//   Main
//-------------------------------------------------------
void main(void){
    setup();                            //Llamamos a nuestras configuraciones
    ban=1;                              //Bandera de menu
    
//-------------------------------------------------------
//   Loop
//-------------------------------------------------------    
    while (1) {                         //Loop            
        
        if(ADCON0bits.GO == 0){         //Selección de canal

            if(ADCON0bits.CHS == 0){     //Canal 0
                ADCON0bits.CHS = 1;     //Cambio a canal 1
                adc1 = ADRESH;          
            }
            else if (ADCON0bits.CHS == 1){ //Canal 1 
                ADCON0bits.CHS = 2;     //Cambio a canal 2
                adc2 = ADRESH;
                   
            }
            else if (ADCON0bits.CHS == 2){ //Canal 1
                ADCON0bits.CHS = 3;     //Cambio a canal 2
                adc3 = ADRESH;          //Guardo DAtos
                if (ADRESH<128)         //Rango Adresh
                    servo1Rotate0();    //0 grados
                else
                    servo1Rotate90();   //90 grados   
                
            }
            else{                        //Canal 3
                ADCON0bits.CHS = 0;      //Cambio a canal 0
                adc4 = ADRESH;           //Guardo DAtos
                if (ADRESH <128)         //Rango ADRESH
                    servo2Rotate0();     //0 grados
                else
                    servo2Rotate90();    //90 grados 
            }            
            __delay_us(50);             //Delay
            ADCON0bits.GO = 1;          //GO
        }
//-------------------------------------------------------
//   Comunicación UART
//-------------------------------------------------------
        __delay_ms(500);
        if (ban==1){                    //Desplegar menu
            send_str("Que accion desas ejecutar?\r");
            send_str("Lado izquierdo\r");
            send_str("(1) Mover pie izquierdo a 0\r");
            send_str("(2) Mover pie izquierdo a 90 \r");
            send_str("(3) Mover pierna izquierdo a 0 \r");
            send_str("(4) Mover pierna izquierdo a 90 \r \r");
            send_str("Lado Derecho\r");
            send_str("(5) Mover pie derecho a 0\r");
            send_str("(6) Mover pie derecho a 90\r");
            send_str("(7) Mover pierna derecho a 0\r");
            send_str("(8) Mover pierna derecho a 90\r\r");
            send_str("Datos\r");
            send_str("(9) Leer los datos");
            ban = 0;                     //Bajo bendera de menu
            op  = 0;                     //Coloco en 0 opciones
        } 
        
        switch(op){                      //Opciones
            case ('1'):
                TRISA = 0x00;           //Apago puerto A
                TXREG = '\f';           //Limpio
                send_str("Girando pierna Izq a 0");     //Despliego cadena
                servo1Rotate0();        //Llamo mi función 
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                TRISA = 0xFF;           //Enciendo Puerto A
                break;
            
            case ('2'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pierna Izq a 90");//Despliego cadena
                servo1Rotate90();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                break;
                
            case ('3'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pier Izq a 0");//Despliego cadena
                servo2Rotate0();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                TRISA = 0xFF;
                break;
            
            case ('4'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pier Izq a 90");//Despliego cadena
                servo2Rotate90();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                TRISA = 0xFF;
                break;
                
            case ('5'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pierna der a 0");     //Despliego cadena
                servo3Rotate0();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                break;
            
            case ('6'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pierna der a 90");//Despliego cadena
                servo3Rotate90();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                TRISA = 0xFF;
                break;
                
            case ('7'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pier der a 0");//Despliego cadena
                servo4Rotate0();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio               
                ban = 1;                //Muestro menu
                TRISA = 0xFF;
                break;
            
            case ('8'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("Girando pier der a 90");//Despliego cadena
                servo4Rotate90();
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio                
                ban = 1;                //Muestro menu
                TRISA = 0xFF;
                break;
                
            case ('9'):
                TRISA = 0x00;
                TXREG = '\f';           //Limpio
                send_str("datos");      //Despliego cadena
                __delay_ms(3000);       //Delay para limpiar
                TXREG = '\f';           //Limpio
                servo2Rotate90();
                ban = 1;                //Muestro menu
                TRISA = 0xFF;
                break;
        }
   }
}
//-------------------------------------------------------
//   Conf PIC
//-------------------------------------------------------
void setup(void){               //Configuraciones del PIC
    //Entradas digitales y analogicas
    ANSEL   = 0b00001111;       //Se utiliza canal del 0 al 4
    ANSELH  = 0x00;          
    
    //Entradas y Salidas
    TRISA   = 0xFF;
    TRISB   = 0xFF;
    TRISE   = 0xFF;
    
    TRISC   = 0x00;             //salidas
    TRISD   = 0x00;
    
    TRISCbits.TRISC7= 1;
    //Inicialización de los puertos
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    //Configuracion del oscilador
    OSCCONbits.IRCF2     = 1;    //8MHZ
    OSCCONbits.IRCF1    = 1;
    OSCCONbits.IRCF0    = 1;
    OSCCONbits.SCS      = 1;     //Oscilador interno
    
    //TMR0
    OPTION_REGbits.T0CS = 0;     //Fosc/4
    OPTION_REGbits.PSA  = 0;     //Prescaler assigned TMR0
    OPTION_REGbits.PS2  = 1;     //1:128
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS0  = 0;
    TMR0 = 247;                  //2ms
    
    //Configuracion de ADC
    ADCON1bits.ADFM     = 0;     //Justificado a la izquierda
    ADCON1bits.VCFG0    = 0;     //voltaje VSS
    ADCON1bits.VCFG1    = 0;     //voltaje VDD
    ADCON0bits.ADCS     = 0b10;  //Fosc/32
    ADCON0bits.CHS      = 0;     //canal 0
    __delay_us(50);
    ADCON0bits.ADON     = 1;     //Enciendo
    
    //Configuracion de PWM
    TRISCbits.TRISC2    = 1;     //CCP1
    TRISCbits.TRISC1    = 1;     //CCP2    
    PR2 = 255;                   //PWM
    CCP1CONbits.P1M     = 0;     //Modo PWM
    CCP1CONbits.CCP1M   = 0b1100;//Modo     
    CCP2CONbits.CCP2M   = 0b1100; 
    CCPR1L              = 0x0f;  //Periodo incial
    CCPR2L              = 0x0f; 
    CCP1CONbits.DC1B    = 0;     //Bits menos significativos
    CCP2CONbits.DC2B0   = 0;        
    CCP2CONbits.DC2B1   = 0;
    
    //TMR2
    PIR1bits.TMR2IF     = 0;    //Bajo bandera
    T2CONbits.T2CKPS    = 0b11; //1:16
    T2CONbits.TMR2ON    = 1;    //Enciendo TMR2
    
    while(PIR1bits.TMR2IF == 0);//Ciclo TMR2 Para levantar bandera
    PIR1bits.TMR2IF     = 0;    //Bajo bandera 
    TRISCbits.TRISC2    = 0;    //Salida PWM
    TRISCbits.TRISC1    = 0;    //Salida PWM    

    
    //Config TX y RX
    TXSTAbits.SYNC      = 0;    //Asincrono
    TXSTAbits.BRGH      = 0;    //High Speed 
    BAUDCTLbits.BRG16   = 1;    //usamos 16 bits
    SPBRG              = 51;    //Baud rate
    SPBRGH              = 0;    //9600
    RCSTAbits.SPEN      = 1;    //Enciendo modulo 
    RCSTAbits.RX9       = 0;    //8bits de recepción
    RCSTAbits.CREN      = 1;    //Recepción  
    TXSTAbits.TXEN      = 1;    //TRansmisión   
    
    //Pull-up internos
    OPTION_REGbits.nRBPU = 0;   //Activo
    WPUBbits.WPUB7 = 1;             
    WPUBbits.WPUB6 = 1;
    IOCBbits.IOCB7 = 1;
    IOCBbits.IOCB6 = 1;
    
    //Configuraciones de interrupciones 
    INTCONbits.GIE      = 1;    //Interrupciones generales
    INTCONbits.PEIE     = 1;    //Int de perifericos
    INTCONbits.RBIF     = 0;    //Push Button      
    INTCONbits.RBIE     = 1;
    PIR1bits.RCIF       = 0;    //UART
    PIE1bits.RCIE       = 1;
    PIR1bits.ADIF       = 0;    //ADC
    PIE1bits.ADIE       = 1;
    
}
//-------------------------------------------------------
//   Subrutinas
//-------------------------------------------------------
void send_char (char dato){
    while(!TXIF);           //Transmisión
    TXREG = dato;           //Muestro caracter
}

void send_str(char st[]){
    int i = 0;              
    while (st[i] != 0){
        send_char(st[i]);   //Envio valor a send char
        i++;                //Aumento contador 
    }
}
//uint8_t WRTEEPROM(uint8_t data, uint8_t address){//Escritura de EEPROM
//    EEADR = address;             
//    EEDATA= data;                 
//    
//    EECON1bits.EEPGD = 0;       
//    EECON1bits.WREN = 1;        
//    INTCONbits.GIE =0;         
//    
//    EECON2 = 0x55;              
//    EECON2 = 0x0AA;                 
//    EECON1bits.WR =1;           //escritura
//    INTCONbits.GIE =1;          //se habilitar interrupciones    
//    PIR2bits.EEIF=0;
//    EECON1bits.WREN = 0;        //Deshabilita escritura
//    
//    while(PIR2bits.EEIF == 0);  //espero a que escriba
//    PIR2bits.EEIF = 0;
//    
//    EECON1bits.WREN = 0;
//    INTCONbits.GIE = 1;
//    return;
//}

//char RDEEPROM(unsigned address){//Lectura EPROM
//    EEADR = address;            //Dirección
//    EECON1bits.EEPGD = 0;       //memoria EEPROm
//    EECON1bits.RD=1;            //Lectura
//    return EEDATA;
//}

//-------------------------------------------------------
//   Movimientos de servo
//-------------------------------------------------------
void servo1Rotate0(void){        //0 Degree pierna izquierda
  unsigned int i;
  for (i = 0; i <50; i ++){      //Rango de int
    PORTCbits.RC0 = 1;           //Enciendo pin
    __delay_us(800);             //Tiempo en alto
    PORTCbits.RC0 = 0;           //Apago pin
    __delay_us(19200);           //Tiempo en bajo
  }
}

void servo1Rotate90(void){ //90 Degree pierna izquierda
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC0 = 1;
    __delay_us(1500);       //Tiempo en alto
    PORTCbits.RC0 = 0;
    __delay_us(18500);
  }
}

void servo2Rotate0(void){  //0 Degree pierna izquierda
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC3 = 1;
    __delay_us(800);        //Tiempo en alto
    PORTCbits.RC3 = 0;
    __delay_us(19200);
  }
}

void servo2Rotate90(void){ //90 Degree pierna izquierda
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC3 = 1;
    __delay_us(1500);       //Tiempo en alto    
    PORTCbits.RC3 = 0;
    __delay_us(18500);
  }
}

void servo3Rotate0(void){        //0 Degree pierna der
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC2 = 1;
    __delay_us(800);        //Tiempo en alto
    PORTCbits.RC2 = 0;
    __delay_us(19200);
  }
}

void servo3Rotate90(void){ //90 Degree pierna der
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC2 = 1;
    __delay_us(1500);       //Tiempo en alto
    PORTCbits.RC2 = 0;
    __delay_us(18500);
  }
}

void servo4Rotate0(void){  //0 Degree pierna izquierda
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC1 = 1;
    __delay_us(800);        //Tiempo en alto
    PORTCbits.RC1 = 0;
    __delay_us(19200);
  }
}

void servo4Rotate90(void){ //90 Degree pierna izquierda
  unsigned int i;
  for (i = 0; i <50; i ++){//Rango de int
    PORTCbits.RC1 = 1;
    __delay_us(1500);       //Tiempo en alto
    PORTCbits.RC1 = 0;
    __delay_us(18500);
  }
}

