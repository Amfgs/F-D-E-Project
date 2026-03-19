#include <stdio.h>
#include <stdint.h>


uint8_t mem[256] = {0};   /* memória unificada (instruções + dados) */
uint8_t reg[4]   = {0};   /* R0, R1, R2, R3                         */
uint8_t pc = 0, zf = 0, running = 1;
int ciclo = 0;

/* ── Fetch ───────────────────────────────────────────────── */
void fetch(uint8_t *op, uint8_t *a, uint8_t *b) {
    *op = mem[pc];
    *a  = mem[pc + 1];
    *b  = mem[pc + 2];
    pc += 3;
}

/* ── Decode / Execute ────────────────────────────────────── */
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

/* ── Trace ───────────────────────────────────────────────── */
void trace(uint8_t op, uint8_t a, uint8_t b) {
    const char *nomes[] = {
        "", 
        "LOAD", 
        "STORE", 
        "ADD", 
        "SUB",
        "MOV", 
        "CMP", 
        "JMP", 
        "JZ", 
        "JNZ", 
        "HALT"
    };
    printf("Ciclo %2d: %-5s %3d,%3d  |  R0=%3d  R1=%3d  R2=%3d  R3=%3d  |  PC=%3d  ZF=%d\n",ciclo, nomes[op], a, b, reg[0], reg[1], reg[2], reg[3], pc, zf);
}

/* ── Main ────────────────────────────────────────────────── */
int main() {

    /* ── 1. DADOS: 8 valores nos endereços 0x10–0x17 ──────── */
    mem[0x10] = 3;
    mem[0x11] = 7;
    mem[0x12] = 2;
    mem[0x13] = 5;
    mem[0x14] = 1;
    mem[0x15] = 8;
    mem[0x16] = 4;
    mem[0x17] = 6;

    int i = 0; /* cursor de escrita na memória */

    /* MOV R0, 0  – zera acumulador */
    mem[i++] = 0x05;
    mem[i++] = 0x00;
    mem[i++] = 0x00;

    /* LOAD R1, [0x10] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x10;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x11] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x11;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x12] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x12;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x13] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x13;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x14] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01;
    mem[i++] = 0x14;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x15] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x15;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x16] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x16;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* LOAD R1, [0x17] + ADD R0, R1 */
    mem[i++] = 0x01; 
    mem[i++] = 0x01; 
    mem[i++] = 0x17;
    mem[i++] = 0x03; 
    mem[i++] = 0x00; 
    mem[i++] = 0x01;

    /* STORE R0, [0x20] – grava resultado */
    mem[i++] = 0x02; 
    mem[i++] = 0x00; 
    mem[i++] = 0x20;

    /* HALT */
    mem[i++] = 0x0A; 
    mem[i++] = 0x00; 
    mem[i++] = 0x00;

    /* ── 3. CICLO F-D-E ───────────────────────────────────── */
    printf("=== MiniCPU – Grupo 2: Somatorio de Array ===\n\n");

    while (running && pc < 256) {
        uint8_t op, a, b;
        ciclo++;
        fetch(&op, &a, &b);
        decode_execute(op, a, b);
        trace(op, a, b);
    }

    /* ── 4. VALIDAÇÃO ─────────────────────────────────────── */
    printf("\n--- Validacao ---\n");
    printf("mem[0x20] = %d\n", mem[0x20]);
    printf("%s\n", mem[0x20] == 36 ? "CORRETO! Soma = 36 ✓" : "ERRO! Valor inesperado ✗");

    return 0;
}