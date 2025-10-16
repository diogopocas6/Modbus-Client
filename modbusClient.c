#include <stdio.h>
#include <stdint.h>
#include "modbusAP.h"

int main(void) {
    char *server_ip = "127.0.0.1";  // endereço do servidor Modbus TCP
    unsigned int port = 502;        // porto padrão do Modbus TCP
    
    //  Testar leitura de 3 registos a partir do endereço 1
    uint16_t read_values[10];
    int ret = Read_h_regs(server_ip, port, 1, 3, read_values);
    
    if (ret > 0) {
        printf("Read %d registers successfully:\n", ret);
        for (int i = 0; i < ret; i++) {
            printf("Register[%d] = %u (0x%04X)\n", i, read_values[i], read_values[i]);
        }
    } else {
        printf("Error reading registers (code %d)\n", ret);
    }

    // Testar escrita de 2 registos
    uint16_t write_values[2] = {0x1234, 0x5678};
    ret = Write_multiple_regs(server_ip, port, 10, 2, write_values);
    
    if (ret > 0) {
        printf("Successfully wrote %d registers starting at 10.\n", ret);
    } else {
        printf("Error writing registers (code %d)\n", ret);
    }

    return 0;
}

