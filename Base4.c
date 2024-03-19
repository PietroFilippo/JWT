#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para codificar uma string para Base4
char* codificarBase4(const char* str) {
    int len = strlen(str);
    char* codificada = (char*)malloc((len * 2 + 1) * sizeof(char));
    
    if (codificada == NULL) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }

    for (int i = 0, j = 0; i < len; i++, j += 2) {
        int valor = (int)str[i];
        codificada[j] = (char)(valor / 4 + '0');
        codificada[j + 1] = (char)(valor % 4 + '0');
    }
    codificada[len * 2] = '\0';
    
    return codificada;
}

// Função para decodificar uma string de Base4
char* decodificarBase4(const char* codificada) {
    int len = strlen(codificada) / 2;
    char* decodificada = (char*)malloc((len + 1) * sizeof(char));
    
    if (decodificada == NULL) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }

    for (int i = 0, j = 0; i < len; i++, j += 2) {
        int valor = (codificada[j] - '0') * 4 + (codificada[j + 1] - '0');
        decodificada[i] = (char)valor;
    }
    decodificada[len] = '\0';
    
    return decodificada;
}

int main() {
    char str[] = "Hello, World!";
    
    // Codificar
    char* codificada = codificarBase4(str);
    printf("Codificada: %s\n", codificada);

    // Decodificar
    char* decodificada = decodificarBase4(codificada);
    printf("Decodificada: %s\n", decodificada);

    free(codificada);
    free(decodificada);

    return 0;
}
