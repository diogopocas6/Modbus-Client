#include <stdint.h>

int Send_Modbus_Request (char *server_add, unsigned int port, uint8_t*APDU,int APDUlen,uint8_t* APDU_R);
// generates TI (trans.ID →sequence number)
// assembles PDU = APDU(SDU) + MBAP
// opens TCP client socket and connects to server 
// sends Modbus TCP PDU
// response o timeout
// if response, remove MBAP, PDU_R → APDU_R
// closes TCP client socket with server (*)
// returns: APDU_R and 0 – ok, <0 – error (timeout)