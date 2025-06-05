#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#pragma pack(push, 1)
struct eth_header {
    uint8_t dest[6];
    uint8_t src[6];
    uint16_t ethertype;
};

struct ip_header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};

struct tcp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t data_offset_res;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urg_ptr;
};
#pragma pack(pop)

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    // Ler cabeçalho Ethernet
    struct eth_header eth;
    if (fread(&eth, 1, sizeof(eth), f) != sizeof(eth)) {
        perror("Erro ao ler cabeçalho Ethernet");
        fclose(f);
        return 1;
    }

    printf("Lendo Ethernet ...\n");
    printf(" --> MAC de Origem: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
           eth.src[0], eth.src[1], eth.src[2], eth.src[3], eth.src[4], eth.src[5]);
    printf(" --> MAC de Destino: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
           eth.dest[0], eth.dest[1], eth.dest[2], eth.dest[3], eth.dest[4], eth.dest[5]);

    // Ler cabeçalho IP
    struct ip_header ip;
    if (fread(&ip, 1, sizeof(ip), f) != sizeof(ip)) {
        perror("Erro ao ler cabeçalho IP");
        fclose(f);
        return 1;
    }

    int ip_ihl = ip.version_ihl & 0x0F;
    int ip_header_length = ip_ihl * 4;

    if (ip_header_length > sizeof(ip)) {
        fseek(f, ip_header_length - sizeof(ip), SEEK_CUR);
    }

    printf("Lendo IP ...\n");
    printf(" --> Versão do IP: %d\n", ip.version_ihl >> 4);
    printf(" --> Tamanho do cabeçalho: %d bytes\n", ip_header_length);
    printf(" --> Tamanho do pacote: %d bytes\n", ntohs(ip.tot_len));
    uint8_t *saddr = (uint8_t *)&ip.saddr;
    uint8_t *daddr = (uint8_t *)&ip.daddr;
    printf(" --> Endereço IP de Origem: %d.%d.%d.%d\n", saddr[0], saddr[1], saddr[2], saddr[3]);
    printf(" --> Endereço IP de Destino: %d.%d.%d.%d\n", daddr[0], daddr[1], daddr[2], daddr[3]);

    // Ler cabeçalho TCP
    struct tcp_header tcp;
    if (fread(&tcp, 1, 20, f) != 20) {
        perror("Erro ao ler cabeçalho TCP");
        fclose(f);
        return 1;
    }

    int tcp_data_offset = (tcp.data_offset_res >> 4) & 0x0F;
    int tcp_header_length = tcp_data_offset * 4;

    if (tcp_header_length > 20) {
        fseek(f, tcp_header_length - 20, SEEK_CUR);
    }

    printf("Lendo TCP ...\n");
    printf(" --> Porta de Origem: %d\n", ntohs(tcp.src_port));
    printf(" --> Porta de Destino: %d\n", ntohs(tcp.dest_port));
    printf(" --> Tamanho do cabeçalho: %d bytes\n", tcp_header_length);

    // Calcular tamanho dos dados HTTP
    int total_ip_length = ntohs(ip.tot_len);
    int http_data_length = total_ip_length - ip_header_length - tcp_header_length;

    printf("Lendo Dados (HTTP) ...\n");
    printf(" --> Tamanho dos dados: %d bytes\n", http_data_length);
    printf(" --> Dados:\n");

    // Imprimir dados HTTP
    for (int i = 0; i < http_data_length; i++) {
        int c = fgetc(f);
        if (c == EOF) break;
        putchar(c);
    }

    fclose(f);
    return 0;
}