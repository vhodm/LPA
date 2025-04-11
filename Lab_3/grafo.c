// Laboratório 03: Grafos
// Aluno: Victor Hugo Oliveira de Melo
// Professor: Horacio Fernandes

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// Estrutura que representa um nó na lista de vizinhos (lista encadeada)
typedef struct lista_vizinhos {
    int vizinho; // ID do nó vizinho
    struct lista_vizinhos *prox; // Ponteiro para o próximo vizinho na lista
} lista_vizinhos_t;

// Estrutura que representa um nó do grafo (dispositivo da rede)
typedef struct no {
    int id; // Identificador único do nó
    double pos_x; // Posição X do nó no plano
    double pos_y; // Posição Y do nó no plano
    lista_vizinhos_t *lista_vizinhos; // Lista encadeada de vizinhos
} no_t;

// Tipo grafo: vetor de nós (ponteiro para no_t)
typedef no_t* grafo_t;

// Função: Adiciona um vizinho ao início da lista de vizinhos
// Parâmetros:
// - vizinho: ID do nó a ser adicionado como vizinho
// - lista: ponteiro para a lista de vizinhos (passado por referência)
// Retorno: true em caso de sucesso, false em caso de falha na alocação
bool lista_vizinhos_adicionar(int vizinho, lista_vizinhos_t **lista) {
    lista_vizinhos_t *novo = malloc(sizeof(lista_vizinhos_t));
    if (novo == NULL) return false;
    novo->vizinho = vizinho;
    novo->prox = *lista; // Insere no início da lista
    *lista = novo; // Atualiza o ponteiro da lista
    return true;
}

// Função: Imprime todos os vizinhos de um nó no formato especificado
// Parâmetros:
// - lista: ponteiro para o início da lista de vizinhos
void lista_vizinhos_imprimir(lista_vizinhos_t *lista) {
    lista_vizinhos_t *atual = lista;
    while (atual != NULL) {
        printf(" %d", atual->vizinho); // Imprime cada ID separado por espaço
        atual = atual->prox;
    }
    printf("\n"); // Nova linha após a lista
}

// Função: Aloca memória para o grafo (vetor de nós)
// Parâmetros:
// - tam: número de nós no grafo
// Retorno: ponteiro para o vetor alocado
grafo_t grafo_criar(int tam) {
    return (grafo_t)malloc(tam * sizeof(no_t)); // Aloca tam elementos do tipo no_t
}

// Função: Atualiza as listas de vizinhos de todos os nós com base no raio de comunicação
// Parâmetros:
// - tam: número total de nós
// - raio_comunicacao: distância máxima para comunicação entre nós
// - grafo: vetor de nós do grafo
void grafo_atualizar_vizinhos(int tam, double raio_comunicacao, grafo_t grafo) {
    for (int i = 0; i < tam; i++) { // Para cada nó i
        for (int j = 0; j < tam; j++) { // Compara com cada nó j
            if (i == j) continue; // Ignora o próprio nó
            
            // Calcula a distância euclidiana entre os nós i e j
            double dx = grafo[i].pos_x - grafo[j].pos_x;
            double dy = grafo[i].pos_y - grafo[j].pos_y;
            double distancia = sqrt(dx*dx + dy*dy);
            
            // Se a distância for menor que o raio, adiciona i como vizinho de j
            if (distancia < raio_comunicacao) {
                lista_vizinhos_adicionar(i, &grafo[j].lista_vizinhos);
            }
        }
    }
}

// Função: Imprime o grafo completo no formato especificado
// Parâmetros:
// - tam: número total de nós
// - grafo: vetor de nós do grafo
void grafo_imprimir(int tam, grafo_t grafo) {
    for (int i = 0; i < tam; i++) { // Para cada nó no grafo
        printf("NÓ %d:", grafo[i].id); // Cabeçalho com o ID do nó
        lista_vizinhos_imprimir(grafo[i].lista_vizinhos); // Lista de vizinhos
    }
}

// Função principal: Lê o arquivo de entrada, constrói o grafo e imprime os resultados
// Parâmetros:
// - argc: número de argumentos da linha de comando
// - argv: vetor de argumentos (argv[1] = nome do arquivo de entrada)
int main(int argc, char **argv) {
    // Validação da entrada
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo de entrada
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // Lê a primeira linha (número de nós e raio de comunicação)
    int tam;
    double raio;
    if (fscanf(file, "%d\t%lf\n", &tam, &raio) != 2) {
        fprintf(stderr, "Erro ao ler a primeira linha\n");
        fclose(file);
        return 1;
    }

    // Aloca memória para o grafo
    grafo_t grafo = grafo_criar(tam);
    if (!grafo) {
        fprintf(stderr, "Erro ao alocar o grafo\n");
        fclose(file);
        return 1;
    }

    // Lê os dados de cada nó do arquivo
    int i = 0;
    while (i < tam && !feof(file)) {
        int id;
        double x, y;
        if (fscanf(file, "%d\t%lf\t%lf\n", &id, &x, &y) != 3) {
            fprintf(stderr, "Erro ao ler a linha do nó %d\n", i + 1);
            fclose(file);
            free(grafo);
            return 1;
        }
        grafo[i].id = id;
        grafo[i].pos_x = x;
        grafo[i].pos_y = y;
        grafo[i].lista_vizinhos = NULL; // Inicializa lista vazia
        i++;
    }

    fclose(file);

    // Atualiza as listas de vizinhos
    grafo_atualizar_vizinhos(tam, raio, grafo);

    // Imprime o grafo no formato especificado
    grafo_imprimir(tam, grafo);

    // Libera a memória alocada para as listas de vizinhos e o grafo
    for (int j = 0; j < tam; j++) {
        lista_vizinhos_t *atual = grafo[j].lista_vizinhos;
        while (atual != NULL) {
            lista_vizinhos_t *prox = atual->prox;
            free(atual); // Libera cada elemento da lista
            atual = prox;
        }
    }
    free(grafo); // Libera o vetor de nós

    return 0;
}