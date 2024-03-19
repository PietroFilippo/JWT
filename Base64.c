#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* encodeBase64(const unsigned char *input, int length) {
    char *output = (char*)malloc(((length + 2) / 3) * 4 + 1); // Tamanho de saída, mais espaço para o terminador nulo
    if (output == NULL) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }

    int i = 0, j = 0;
    while (i < length) {
        unsigned char octet_a = i < length ? input[i++] : 0;
        unsigned char octet_b = i < length ? input[i++] : 0;
        unsigned char octet_c = i < length ? input[i++] : 0;

        unsigned char triple[3] = {octet_a, octet_b, octet_c};

        output[j++] = base64chars[triple[0] >> 2];
        output[j++] = base64chars[((triple[0] & 0x03) << 4) | ((triple[1] & 0xf0) >> 4)];
        output[j++] = base64chars[((triple[1] & 0x0f) << 2) | ((triple[2] & 0xc0) >> 6)];
        output[j++] = base64chars[triple[2] & 0x3f];
    }

    // Adiciona o padding se necessário
    for (int p = 0; p < (3 - (length % 3)) % 3; p++) {
        output[j++] = '=';
    }

    output[j] = '\0';

    return output;
}

// Função para encontrar o índice de um caractere no array base64chars
int findIndex(char c) {
    if (c == '=') return 0;
    for (int i = 0; i < 64; i++) {
        if (base64chars[i] == c) return i;
    }
    return -1;
}

char* decodeBase64(const char* input) {
    int length = strlen(input);
    int padding = 0;

    // Conta o padding no final
    if (input[length - 1] == '=') padding++;
    if (input[length - 2] == '=') padding++;

    // Calcula o tamanho do resultado decodificado
    int resultSize = (length * 3) / 4 - padding;
    char* result = (char*)malloc(resultSize + 1); // +1 para o caractere nulo
    if (result == NULL) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }

    for (int i = 0, j = 0; i < length; i += 4) {
        // Converte cada grupo de 4 caracteres para 24 bits
        int sextet_a = findIndex(input[i]);
        int sextet_b = findIndex(input[i + 1]);
        int sextet_c = findIndex(input[i + 2]);
        int sextet_d = findIndex(input[i + 3]);

        // Combina os 24 bits e extrai como 3 bytes
        int triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;

        if (j < resultSize) result[j++] = (triple >> 16) & 0xFF; // Primeiro byte
        if (j < resultSize) result[j++] = (triple >> 8) & 0xFF;  // Segundo byte
        if (j < resultSize) result[j++] = triple & 0xFF;         // Terceiro byte
    }

    result[resultSize] = '\0'; // Termina a string

    return result;
}

int main() {
    char str[] = "Hello, World!";
    int len = strlen(str);

    unsigned char *input = (unsigned char*)str;
    char *encoded = encodeBase64(input, len);

    printf("Encoded: %s\n", encoded);

    char* decoded = decodeBase64(encoded);

    printf("Decoded: %s\n", decoded);

    free(encoded);
    free(decoded);

    return 0;
}
