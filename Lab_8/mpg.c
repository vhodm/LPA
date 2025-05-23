// Aluno: Victor Hugo Oliveira de Melo
// Matrícula: 22352937

#include <stdio.h>
#include <stdlib.h>

const unsigned char START_CODE_PREFIX[3] = {0x00, 0x00, 0x01};

const char *frame_rate_table[] = {
    NULL,    // 0
    "23.976", "24.000", "25.000", "29.970",
    "30.000", "50.000", "59.940", "60.000"  // 1-8
};

void print_stream_info(unsigned char code) {
    printf("--> código: %.2x ", code);
    if (code == 0xb3) {
        printf("-- Sequence Header");
    } else if (code == 0xb8) {
        printf("-- Group of Pictures");
    } else if (code == 0x00) {
        printf("-- Picture");
    } else if (code >= 0x01 && code <= 0xaf) {
        printf("-- Slice");
    } else if (code >= 0xe0 && code <= 0xef) {
        printf("-- Packet Audio");
    } else {
        printf("-- Tipo de stream não implementado");
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_mpeg>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    unsigned char buffer[3] = {0, 0, 0};
    int c;
    while ((c = fgetc(file)) != EOF) {
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = (unsigned char)c;

        if (buffer[0] == START_CODE_PREFIX[0] &&
            buffer[1] == START_CODE_PREFIX[1] &&
            buffer[2] == START_CODE_PREFIX[2]) {

            int stream_code_int = fgetc(file);
            if (stream_code_int == EOF) break;
            unsigned char stream_code = (unsigned char)stream_code_int;

            print_stream_info(stream_code);

            if (stream_code == 0xb3) { // Sequence Header
                int bytes[4];
                for (int i = 0; i < 4; i++) {
                    bytes[i] = fgetc(file);
                    if (bytes[i] == EOF) {
                        fprintf(stderr, "\nErro: Arquivo incompleto no cabeçalho Sequence\n");
                        fclose(file);
                        return 1;
                    }
                }
                unsigned char byte1 = bytes[0];
                unsigned char byte2 = bytes[1];
                unsigned char byte3 = bytes[2];
                unsigned char byte4 = bytes[3];

                unsigned int largura = byte1 * 16 + (byte2 >> 4);
                unsigned int altura = ((byte2 & 0x0F) << 8) | byte3;
                unsigned int frame_rate_code = byte4 & 0x0F;

                const char *frame_rate = "desconhecido";
                if (frame_rate_code >= 1 && frame_rate_code <= 8) {
                    frame_rate = frame_rate_table[frame_rate_code];
                }

                printf(" -- Width = %u, Height = %u -- Frame rate = %sfps", largura, altura, frame_rate);
            } else if (stream_code == 0x00) { // Picture
                // Lê dois bytes após o start code
                int byte1 = fgetc(file);
                int byte2 = fgetc(file);
                
                if (byte1 == EOF || byte2 == EOF) {
                    fprintf(stderr, "\nErro: Arquivo incompleto no cabeçalho Picture\n");
                    fclose(file);
                    return 1;
                }
                
                // Extrai o tipo do SEGUNDO byte (byte2)
                unsigned char tipo_code = ((unsigned char)byte2 >> 3) & 0x07; // 3 bits mais significativos
                
                const char *tipo = "?";
                switch (tipo_code) {
                    case 1: tipo = "I"; break;
                    case 2: tipo = "P"; break;
                    case 3: tipo = "B"; break;
                }
                printf(" -- Tipo = %s", tipo);
            }

            printf("\n");
        }
    }

    fclose(file);
    return 0;
}