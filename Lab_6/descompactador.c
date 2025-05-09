#include <stdio.h>
#include <stdlib.h>

struct zip_file_hdr {
    int signature;
    short version;
    short flags;
    short compression;
    short mod_time;
    short mod_date;
    int crc;
    int compressed_size;
    int uncompressed_size;
    short name_length;
    short extra_field_length;
} __attribute__((packed));

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_zip>\n", argv[0]);
        return 1;
    }

    FILE *zip_file = fopen(argv[1], "rb");
    if (!zip_file) {
        perror("Erro ao abrir o arquivo ZIP");
        return 1;
    }

    struct zip_file_hdr *hdr = malloc(sizeof(struct zip_file_hdr));
    if (!hdr) {
        perror("Erro ao alocar memória");
        fclose(zip_file);
        return 1;
    }

    int contador_arquivo = 0;
    while (1) {
        size_t lidos = fread(hdr, sizeof(struct zip_file_hdr), 1, zip_file);
        if (lidos != 1 || hdr->signature != 0x04034b50) {
            break;
        }

        contador_arquivo++;
        printf("Arquivo %d ..\n", contador_arquivo);

        char *nome_arquivo = malloc(hdr->name_length + 1);
        if (!nome_arquivo) {
            perror("Erro ao alocar memória para o nome do arquivo");
            break;
        }

        lidos = fread(nome_arquivo, hdr->name_length, 1, zip_file);
        if (lidos != 1) {
            free(nome_arquivo);
            break;
        }

        nome_arquivo[hdr->name_length] = '\0';
        printf("---> Nome do Arquivo: %s\n", nome_arquivo);
        printf("---> Tamanho Compactado: %d\n", hdr->compressed_size);
        printf("---> Tamanho Descompactado: %d\n", hdr->uncompressed_size);

        free(nome_arquivo);

        long deslocamento = hdr->extra_field_length + hdr->compressed_size;
        if (fseek(zip_file, deslocamento, SEEK_CUR) != 0) {
            perror("Erro ao buscar próximo arquivo");
            break;
        }
    }

    free(hdr);
    fclose(zip_file);
    return 0;
}