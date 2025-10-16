#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

uint16_t TI = 0; // 2 bytes starting at 0

int Send_Modbus_Request (char *server_add, unsigned int port, uint8_t*APDU,int APDUlen,uint8_t* APDU_R){
    TI++;           // incrementa TI a cada request
    int length = APDUlen + 1;   // length of REMAINING bytes, including UNIT IDENTIFIER!
    uint8_t UnitID = 1;
    int PDUlen = APDUlen + 7;
    // assembles PDU = APDU(SDU) + MBAP
    uint8_t PDU[PDUlen];

    PDU[0] = TI >> 8; 
    PDU[1] = (uint8_t) TI;     //high byte TI
    PDU[2] = 0X00;
    PDU[3] = 0X00;     // byte 2 e 3 -> Protocol identifier = 0 -> Modbus Protocol
    PDU[4] = length >> 8;
    PDU[5] = (uint8_t) length;
    PDU[6] = UnitID;

    for (int i=0; i<APDUlen;i++){
        PDU[7+i]=APDU[i];
    }

    // opens TCP client socket and connects to server 
    
    //Creating Socket    
    int sd;

    sd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);   // PF_INET,SOCK_STREAM E IPPROTO_TCP são int's definidos em socket.h
    
    if(sd<0){
        //printf("Socket creation failed.\n")
        return -1;
    }
        //printf("Socket created succesfully.\n");
    
    //Connecting socket to the server that is listening()
    struct sockaddr_in s_addr;

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    
    if (inet_aton(server_add, &s_addr.sin_addr) == 0) {
        //printf("Invalid server address.\n");
        close(sd);
        return -2;
    }

    int con;
    socklen_t addlen = sizeof(s_addr);
    con = connect(sd,(struct sockaddr *) &s_addr, addlen);

    if (con == -1){
        //printf("Socket connection failed.\n");
        close(sd);
        return -3;
    }

    //printf("Connected to server.\n");
    
    // sends Modbus TCP PDU
    int written=write(sd,PDU,PDUlen);
    if(written < 0){
        //printf("Couldn't write!\n");
        close(sd);
        return -4;
    }

    //printf("Written %d bytes",PDULEN)
    

    // read response MBAP header first (7 bytes)
    uint8_t MBAP_R[7];
    int read_bytes = read(sd, MBAP_R, 7);
    if (read_bytes < 0) {
        //printf("Error reading MBAP header.\n");
        close(sd);
        return -5;
    }

    // verifica Transaction ID
    if (MBAP_R[0] != PDU[0] || MBAP_R[1] != PDU[1]) {
        //printf("Transaction ID mismatch.\n");
        close(sd);
        return -6;
    }

    // calcula quantos bytes faltam = length field - 1 byte (UnitID)
    uint16_t remaining = (MBAP_R[4] << 8) | MBAP_R[5];
    remaining = remaining - 1;
    
    int in = read(sd, APDU_R, remaining);
    if (in < 0) {
        //printf("Error reading APDU response.\n");
        close(sd);
        return -7;
    }
    close(sd);
    return in;      //numero de bytes lidos
}
