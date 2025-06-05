#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

int tamanhoMaximo = 4;
char caracteres[] = "0123456789";
char *senhaCriptografada = "$6$Byk0oaCjQMb9ApCS242m.GP6/I/t21JROFazxXEwTiVzdDJZ2FdyPdSaRnkPmH9RXdT7xjr30.uy484yx61WsgEUWuvpcqa7x/0oN1"; // Hash de "2024"



/**
 * Função recursiva para gerar e testar combinações
 * @param buffer Array para armazenar a combinação atual
 * @param pos Posição atual no array
 * @param tamanho Tamanho da senha sendo gerada
 */
void gerar_combinacoes(char *buffer, int pos, int tamanho) {
    if (pos == tamanho) {
        // Finaliza a string e testa
        buffer[pos] = '\0';
        
        // Gera hash para a senha teste
        char *hash = crypt(buffer, senhaCriptografada);
        
        // Compara com o hash alvo
        if (strcmp(hash, senhaCriptografada) == 0) {
            printf("Senha encontrada: %s\n", buffer);
            exit(0);
        }
        return;
    }

    // Gera combinações para cada caractere
    for (int i = 0; i < strlen(caracteres); i++) {
        buffer[pos] = caracteres[i];
        gerar_combinacoes(buffer, pos + 1, tamanho);
    }
}

int main(int argc, char *argv[]) {
    // Aloca memória para a senha sendo testada
    char *buffer = malloc(tamanhoMaximo + 1); // +1 para o terminador
    
    if (!buffer) {
        perror("Erro de alocação");
        return 1;
    }

    // Testa todos os tamanhos de 1 até o máximo
    for (int t = 1; t <= tamanhoMaximo; t++) {
        gerar_combinacoes(buffer, 0, t);
    }

    // Libera recursos se não encontrou
    free(buffer);
    printf("Senha não encontrada!\n");
    return 1;
}