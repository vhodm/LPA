// Laboratório 04: Simulador de Eventos Discretos
// Aluno: Victor Hugo Oliveira de Melo
// Professor: Horacio Fernandes

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct lista_vizinhos {
    int vizinho;
    struct lista_vizinhos *prox;
} lista_vizinhos_t;

typedef struct no {
    int id;
    double x;
    double y;
    lista_vizinhos_t *vizinhos;
    bool pacote_enviado;
} no_t;

typedef struct grafo {
    int num_nos;
    no_t *nos;
} grafo_t;

typedef struct evento {
    double tempo;
    int alvo;
    int tipo;
    struct evento *prox;
} evento_t;

grafo_t grafo_criar(int tam) {
    grafo_t g;
    g.num_nos = tam;
    g.nos = (no_t *)malloc(tam * sizeof(no_t));
    for (int i = 0; i < tam; i++) {
        g.nos[i].id = i;
        g.nos[i].vizinhos = NULL;
        g.nos[i].pacote_enviado = false;
    }
    return g;
}

bool lista_vizinhos_adicionar(int vizinho, lista_vizinhos_t **lista) {
    lista_vizinhos_t *novo = (lista_vizinhos_t *)malloc(sizeof(lista_vizinhos_t));
    if (novo == NULL) return false;
    novo->vizinho = vizinho;
    novo->prox = *lista;
    *lista = novo;
    return true;
}

void grafo_atualizar_vizinhos(int tam, double raio_comunicacao, grafo_t grafo) {
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam; j++) {
            if (i == j) continue;
            double dx = grafo.nos[i].x - grafo.nos[j].x;
            double dy = grafo.nos[i].y - grafo.nos[j].y;
            double distancia = sqrt(dx*dx + dy*dy);
            if (distancia <= raio_comunicacao) {
                lista_vizinhos_adicionar(j, &grafo.nos[i].vizinhos);
            }
        }
    }
}

bool lista_eventos_adicionar_ordenado(evento_t *evento, evento_t **lista) {
    if (*lista == NULL || evento->tempo < (*lista)->tempo) {
        evento->prox = *lista;
        *lista = evento;
        return true;
    }
    evento_t *atual = *lista;
    while (atual->prox != NULL && atual->prox->tempo <= evento->tempo) {
        atual = atual->prox;
    }
    evento->prox = atual->prox;
    atual->prox = evento;
    return true;
}

void simulacao_iniciar(evento_t **lista, grafo_t grafo) {
    while (*lista != NULL) {
        evento_t *evento_atual = *lista;
        *lista = evento_atual->prox;
        
        int alvo = evento_atual->alvo;
        double tempo = evento_atual->tempo;
        printf("[%3.6f] Nó %d recebeu pacote.\n", tempo, alvo);
        
        no_t *no_atual = &grafo.nos[alvo];
        if (!no_atual->pacote_enviado) {
            lista_vizinhos_t *vizinho = no_atual->vizinhos;
            while (vizinho != NULL) {
                int id_vizinho = vizinho->vizinho;
                printf("\t--> Repassando pacote para o nó %d ...\n", id_vizinho);
                
                evento_t *novo_evento = (evento_t *)malloc(sizeof(evento_t));
                novo_evento->tempo = tempo + 0.1 + (id_vizinho * 0.01);
                novo_evento->alvo = id_vizinho;
                novo_evento->tipo = 1;
                novo_evento->prox = NULL;
                
                lista_eventos_adicionar_ordenado(novo_evento, lista);
                vizinho = vizinho->prox;
            }
            no_atual->pacote_enviado = true;
        }
        free(evento_atual);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada>\n", argv[0]);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Erro ao abrir arquivo");
        return 1;
    }
    
    int num_nos;
    double raio;
    if (fscanf(fp, "%d %lf", &num_nos, &raio) != 2) {
        fprintf(stderr, "Formato inválido\n");
        fclose(fp);
        return 1;
    }
    
    grafo_t grafo = grafo_criar(num_nos);
    for (int i = 0; i < num_nos; i++) {
        int id;
        double x, y;
        if (fscanf(fp, "%d %lf %lf", &id, &x, &y) != 3) {
            fprintf(stderr, "Erro ao ler nó %d\n", i);
            fclose(fp);
            return 1;
        }
        grafo.nos[id].x = x;
        grafo.nos[id].y = y;
    }
    fclose(fp);
    
    grafo_atualizar_vizinhos(num_nos, raio, grafo);
    
    evento_t *lista_eventos = NULL;
    evento_t *evento_inicial = (evento_t *)malloc(sizeof(evento_t));
    evento_inicial->tempo = 1.0;
    evento_inicial->alvo = 0;
    evento_inicial->tipo = 1;
    evento_inicial->prox = NULL;
    lista_eventos_adicionar_ordenado(evento_inicial, &lista_eventos);
    
    simulacao_iniciar(&lista_eventos, grafo);
    
    // Liberar memória (simplificado)
    for (int i = 0; i < grafo.num_nos; i++) {
        lista_vizinhos_t *v = grafo.nos[i].vizinhos;
        while (v != NULL) {
            lista_vizinhos_t *temp = v;
            v = v->prox;
            free(temp);
        }
    }
    free(grafo.nos);
    
    return 0;
}