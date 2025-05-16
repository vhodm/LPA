#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main (int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file){
        perror("Erro de abertura do arquivo");
        return 1;
    }

    // ignorar a assinatura de 8 bytes do png
    fseek(file, 8, SEEK_SET);

    int chunk_num = 0;

    while(1){
        chunk_num++;
        uint32_t length;
        char type[4];
        size_t bytes_lidos;

        // ler o tamanho do chunk
        bytes_lidos = fread(&length, 1, 4, file);
        if (bytes_lidos != 4){
            fprintf(stderr, "Erro ao ler o tamanho do chunk %d\n", chunk_num);
            fclose(file);
            return 1;
        }

        length = ntohl(length);

        // ler o tipo do chunk de 4 bytes
        bytes_lidos = fread(type, 1, 4, file);
        if (bytes_lidos != 4){
            perror("Erro ao ler o tipo de chunk");
            fclose(file);
            return 1;
        }

        printf("Lendo chunk %d:\n", chunk_num);
        printf(" --> Tamanho: %u\n", length);
        printf(" --> Tipo: %.4s\n", type);

        if (memcmp(type, "IHDR", 4) == 0){
            // processar os chunks do tipo IHDR
            unsigned char ihdr_data[13];
            bytes_lidos = fread(ihdr_data, 1, 13, file);
            if (bytes_lidos != 13){
                perror("Erro ao ler dados dos chunks IHDR");
                fclose(file);
                return 1;
            }
            
            uint32_t largura, altura;
            memcpy(&largura, ihdr_data, 4);
            largura = ntohl(largura);
            memcpy(&altura, ihdr_data+4, 4);
            altura = ntohl(altura);

            printf(" --> Largura: %u\n", largura);
            printf(" --> Altura: %u\n", altura);

            // pular os 4 bytes do crc
            fseek(file, 4, SEEK_CUR);
        } else if (memcmp(type, "IEND", 4) == 0){
            //pular crc e sair
            fseek(file, 4, SEEK_CUR);
            break;
        } else {
            // pular crc e os dados do png
            fseek(file, length+4, SEEK_CUR);
        }
    }

    fclose(file);
    return 0;
}