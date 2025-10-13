#include <stdint.h>

int Write_multiple_regs (char* server_add, unsigned int port, uint16_t st_r, uint16_t n_r, uint16_t* val);
// check consistency of parameters
// assembles APDU (MODBUS PDU)
// checks the reponse (APDU_R or error_code)
// returns: number of written registers – ok, <0 – error

int Read_h_regs (char* server_add,unsigned int port,uint16_t st_r,uint16_t n_r,uint16_t* val);
// check consistency of parameters
// assembles APDU (MODBUS PDU)
// checks the reponse (APDU_R or error_code)
// returns: number of read registers – ok, <0 – error
