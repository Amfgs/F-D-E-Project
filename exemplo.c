#include <stdio.h>
#include <stdint.h>

uint8_t mem[128] = {0};
uint8_t reg[4]   = {0};
uint8_t pc = 0, zf = 0, running = 1;
int ciclo = 0;

void fetch(uint8_t *op, uint8_t *a, uint8_t *b) {
    *op = mem[pc];
    *a  = mem[pc + 1];
    *b  = mem[pc + 2];
    pc += 3;
}

void decode_execute(uint8_t op, uint8_t a, uint8_t b) {
    switch (op) {
        case 0x01: reg[a] = mem[b];                          break; /* LOAD  */
        case 0x02: mem[b] = reg[a];                          break; /* STORE */
        case 0x03: reg[a] = reg[a] + reg[b];                 break; /* ADD   */
        case 0x04: reg[a] = reg[a] - reg[b];                 break; /* SUB   */
        case 0x05: reg[a] = b;                               break; /* MOV   */
        case 0x06: zf = (reg[a] == reg[b]) ? 1 : 0;         break; /* CMP   */
        case 0x07: pc = a;                                   break; /* JMP   */
        case 0x08: if ( zf) pc = a;                          break; /* JZ    */
        case 0x09: if (!zf) pc = a;                          break; /* JNZ   */
        case 0x0A: running = 0;                              break; /* HALT  */
    }
}

void trace(uint8_t op, uint8_t a, uint8_t b) {
    const char *nomes[] = {
        "", "LOAD", "STORE", "ADD", "SUB",
        "MOV", "CMP", "JMP", "JZ", "JNZ", "HALT"
    };
    printf("Ciclo %2d: %-5s %3d,%3d  |  R0=%3d  R1=%3d  R2=%3d  R3=%3d  |  PC=%3d  ZF=%d\n",
           ciclo, nomes[op], a, b,
           reg[0], reg[1], reg[2], reg[3], pc, zf);
}

int main() {

    /* ── 1. DADOS: 8 valores em 0x39–0x40 (após o programa) ── */
    mem[0x39] = 3;
    mem[0x3A] = 7;
    mem[0x3B] = 2;
    mem[0x3C] = 5;
    mem[0x3D] = 1;
    mem[0x3E] = 8;
    mem[0x3F] = 4;
    mem[0x40] = 6;
    /* soma esperada: 3+7+2+5+1+8+4+6 = 36 */

    /* ── 2. PROGRAMA a partir de 0x00 ─────────────────────── */
    int i = 0;

    /* MOV R0, 0 – zera acumulador */
    mem[i++] = 0x05; mem[i++] = 0x00; mem[i++] = 0x00;

    /* LOAD R1, [0x39] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x39;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x3A] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x3A;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x3B] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x3B;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x3C] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x3C;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x3D] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x3D;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x3E] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x3E;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x3F] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x3F;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* LOAD R1, [0x40] + ADD R0, R1 */
    mem[i++] = 0x01; mem[i++] = 0x01; mem[i++] = 0x40;
    mem[i++] = 0x03; mem[i++] = 0x00; mem[i++] = 0x01;

    /* STORE R0, [0x50] – resultado em 0x50 (após os dados) */
    mem[i++] = 0x02; mem[i++] = 0x00; mem[i++] = 0x50;

    /* HALT */
    mem[i++] = 0x0A; mem[i++] = 0x00; mem[i++] = 0x00;

    /* ── 3. CICLO F-D-E ───────────────────────────────────── */
    printf("=== MiniCPU – Grupo 2: Somatorio de Array ===\n\n");

    while (running && pc < 128) {
        uint8_t op, a, b;
        ciclo++;
        fetch(&op, &a, &b);
        decode_execute(op, a, b);
        trace(op, a, b);
    }

    /* ── 4. VALIDAÇÃO ─────────────────────────────────────── */
    printf("\n--- Validacao ---\n");
    printf("mem[0x50] = %d\n", mem[0x50]);
    printf("%s\n", mem[0x50] == 36 ? "CORRETO! Soma = 36 ✓" : "ERRO! Valor inesperado ✗");

    return 0;
}