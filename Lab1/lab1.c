#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definição da estrutura evento_t
struct evento_t {
    double tempo;
    int alvo;
    int tipo;
};
typedef struct evento_t evento_t;

// Definição da estrutura lista_eventos_t
struct lista_eventos_t {
    evento_t *evento;
    struct lista_eventos_t *proximo;
};
typedef struct lista_eventos_t lista_eventos_t;

// Função para adicionar um evento no início da lista (da Questão 1)
bool lista_eventos_adicionar_inicio(evento_t *evento, lista_eventos_t **lista) {
    lista_eventos_t *item_novo = malloc(sizeof(lista_eventos_t));
    if (item_novo == NULL) return false;
    
    item_novo->evento = evento;
    item_novo->proximo = *lista;
    *lista = item_novo;
    
    return true;
}

// Função para adicionar um evento no final da lista (Questão 2)
bool lista_eventos_adicionar_fim(evento_t *evento, lista_eventos_t **lista) {
    lista_eventos_t *item_novo = malloc(sizeof(lista_eventos_t));
    if (item_novo == NULL) return false;

    item_novo->evento = evento;
    item_novo->proximo = NULL;

    if (*lista == NULL) {
        *lista = item_novo;
    } else {
        lista_eventos_t *atual = *lista;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = item_novo;
    }
    return true;
}

// Função para adicionar um evento de forma ordenada (Questão 3)
bool lista_eventos_adicionar_ordenado(evento_t *evento, lista_eventos_t **lista) {
    lista_eventos_t *item_novo = malloc(sizeof(lista_eventos_t));
    if (item_novo == NULL) return false;
    
    item_novo->evento = evento;
    item_novo->proximo = NULL;

    // Caso 1: Lista vazia ou novo evento tem tempo menor que o primeiro
    if (*lista == NULL || evento->tempo < (*lista)->evento->tempo) {
        item_novo->proximo = *lista;
        *lista = item_novo;
        return true;
    }

    // Caso 2: Inserção no meio ou final da lista
    lista_eventos_t *atual = *lista;
    while (atual->proximo != NULL && atual->proximo->evento->tempo < evento->tempo) {
        atual = atual->proximo;
    }
    
    item_novo->proximo = atual->proximo;
    atual->proximo = item_novo;
    return true;
}

// Função para listar os eventos
void lista_eventos_listar(lista_eventos_t *lista) {
    lista_eventos_t *atual = lista;
    while (atual != NULL) {
        printf("Tempo: %3.6f, Alvo: %d, Tipo: %d\n", 
               atual->evento->tempo, 
               atual->evento->alvo, 
               atual->evento->tipo);
        atual = atual->proximo;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    lista_eventos_t *lista = NULL;
    double tempo;
    int alvo, tipo;

    while (!feof(arquivo)) {
        if (fscanf(arquivo, "%lf\t%d\t%d\n", &tempo, &alvo, &tipo) == 3) {
            evento_t *novo_evento = malloc(sizeof(evento_t));
            if (novo_evento == NULL) {
                printf("Erro ao alocar memória para evento\n");
                break;
            }
            
            novo_evento->tempo = tempo;
            novo_evento->alvo = alvo;
            novo_evento->tipo = tipo;
            
            if (!lista_eventos_adicionar_fim(novo_evento, &lista)) {
                printf("Erro ao adicionar evento à lista\n");
                free(novo_evento);
                break;
            }
        }
    }

    fclose(arquivo);
    lista_eventos_listar(lista);

    // Liberar memória alocada
    lista_eventos_t *atual = lista;
    while (atual != NULL) {
        lista_eventos_t *proximo = atual->proximo;
        free(atual->evento);
        free(atual);
        atual = proximo;
    }

    return 0;
}