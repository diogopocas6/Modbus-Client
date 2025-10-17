#include <stdio.h>
#include <stdlib.h>
#include "modbusAP.h"

#define SERVER1 "127.0.0.1"
#define SERVER2 "10.227.113.1"
#define PORT 502

int check_modbus_error(int result) {
    if (result >= -4 && result <= -1) {
        printf("result = %d\n", -result);  // Erro Modbus (exceção)
        return 0;
    } else if (result < 0) {
        printf("result = -1\n");           // Erro não Modbus
        return 0;
    }
    return 1; // sucesso
}

int main(void) {

    int result;
    uint16_t A[4];
    uint16_t B, C;
    uint16_t val_41 = 0x41;

    printf("Primeiro passo: escrever 0x41 no registo 121\n");
    result = Write_multiple_regs(SERVER1, PORT, 121, 1, &val_41);
    if (!check_modbus_error(result)) return 0;
    printf("Escreveu %d registos\n", result);

    printf("Segundo passo: ler registos 122 a 125\n");
    result = Read_h_regs(SERVER1, PORT, 122, 4, A);
    if (!check_modbus_error(result)) return 0;
    for (int i = 0; i < 4; i++)
        printf("A[%d] = %u\n", i, A[i]);

    printf("Terceiro passo: ler registo 126\n");
    result = Read_h_regs(SERVER1, PORT, 126, 1, &B);
    if (!check_modbus_error(result)) return 0;
    printf("B = %u\n", B);

    if (B == 0)
        C = 9999;
    else
        C = A[0] + A[3];

    printf("Quarto passo: escrever C=%u no registo 127\n", C);
    result = Write_multiple_regs(SERVER1, PORT, 127, 1, &C);
    if (!check_modbus_error(result)) return 0;
    printf("Escreveu %d registos\n", result);

    printf("Quinto passo: escrever C=%u no registo 128 (servidor remoto)\n", C);
    result = Write_multiple_regs(SERVER2, PORT, 128, 1, &C);
    if (!check_modbus_error(result)) return 0;
    printf("Escreveu %d registos\n", result);

    printf("result = 0\n");
    return 0;
}
