#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "modbusAP.h"
#include "modbusTCP.h"
#define APDU_max 252 // 1+2+2+1+2*123 = 252    Slide 30 ModbusProtocol - Write multiple registers
// 1 + 1 + 2 * 125 = 252      Slide 15 Modbus Protocol - Read Multiple registers

int Write_multiple_regs (
    char* server_add, // server address
    unsigned int port, //port 
    uint16_t st_r, // starting register
    uint16_t n_r, //number of registers
    uint16_t* val) // data 
{
    uint8_t APDU[APDU_max], APDU_R[5]; // resposta tem sempre 5 bytes
    int APDUlen;

    // check consistency of parameters
    if( st_r == 0){                     // PORQUÊ????????????
        //printf("Modbus start address cannot be 0\n");
        return -2;
    }

    //number of registers
    if( (n_r > 123) || (n_r < 1)){      // Podemos escrever 1 a 123 registos
        //printf("Modbus number of registers ouyt of range\n");
        return -3;
    }
    if (n_r + st_r > 65535){
        //printf("Too many registers to write for this starting address, must not exceed 65535."); // Muito raro porque apenas podemos escrever 123...
        return -4;
    }
    // assembles APDU (MODBUS PDU)
    
    //Byte code
    APDU[0] = 0X10; // Write multiple registers code
    
    //Start address
    APDU[1] = st_r>>8; //pega no high byte
    APDU[2] = (uint8_t) st_r; //evita erros ao fazermos cast para 8 bits
    
    //Number of registers
    APDU[3] = n_r>>8;
    APDU[4] = (uint8_t) n_r;
    
    //Byte count
    APDU[5]= (uint8_t) (2*n_r);
    
    //Register values
    for ( int i=0; i<n_r ;i++){
        APDU[6 + 2*i] = val[i]>>8;
        APDU[7 + 2*i] = (uint8_t) val[i];
    }
    APDUlen = 6 + (2*n_r);
    /*
    for (int i=0; i<APDUlen;i++){
        printf("%2x ",APDU[i]);
    }
    printf("\n");
    */
    int ret = Send_Modbus_Request (server_add,port,APDU,APDUlen,APDU_R);
    if ( ret<0){
        //printf("Error sending Modbus Request.\n");
        return -1;
    }
    
    
    // checks the reponse (APDU_R or error_code)
    if (APDU_R[0] == 0X90){       //Resposta devolve 0x90 em caso de erro e 0x10 em caso de resposta certa (function code)
        //printf("Response error.");
        return -APDU_R[1];        //Tipo de erro vai de 1 a 4, por isso retornamos ou -1 ou -2 ou -3 ou -4
    }

    // returns: number of written registers – ok, <0 – error
    if (APDU_R[0] == 0X10){
        return n_r;
    }
return -1;

}
int Read_h_regs(
    char* server_add, //server address
    unsigned int port, // port
    uint16_t st_r, // starting register
    uint16_t n_r, //number of registers to read
    uint16_t* val) // values
{
uint8_t APDU[5],APDU_R[APDU_max];
int APDUlen, response; 

// check consistency of parameters
if (n_r < 1 || n_r > 125){
    //printf("Error - Invalid number of registers to read");
    return -1;
}

if (n_r + st_r > 65535){
        //printf("Too many registers to write for this starting address, must not exceed 65535."); // Muito raro porque apenas podemos ler 125
        return -2;
    }

// assembles APDU (MODBUS PDU)
APDU[0] = 0X03; // Function code 0x03 - Read Multiple Registers
APDU[1] = st_r >> 8;
APDU[2] = (uint8_t)st_r;
APDU[3] = n_r >> 8; // Podiamos escrever 0x00? já que nunca vamos escrever mais do que 125 registos, entao o high byte será sempre 00
APDU[4] = (uint8_t)n_r;
APDUlen = 5; // pedido tem sempre 5 bytes

response = Send_Modbus_Request (server_add,port,APDU,APDUlen,APDU_R);

// checks the reponse (APDU_R or error_code)
// returns: number of read registers – ok, <0 – error

// Cases of errors
    if (response < 0){
        //printf("Modbus Request fail.")
        return -1;
    }

    if (APDU_R[0] == 0x83 ){    // resposta normalmente é o function code, porém 0x83 é um error code
        return -APDU_R[1];  // Exception code pode ser 1 2 3 ou 4. Retornamos valor negativo para cada um desses erros
    }

//Success cases
    if (APDU_R[0] == 0X03){
        for (int i=0; i<n_r;i++){
            val[i] = ((uint16_t)APDU_R[2 + 2*i] << 8) | APDU_R[2 + 2*i + 1];

        }
        return n_r;
    }
return -1;
}
