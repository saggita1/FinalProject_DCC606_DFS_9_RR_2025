#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_VERTICES 20
#define MAX_NOME 30
#define MAX_THREADS 20
#define MAX_CAMINHO 100
#define MAX_CAMINHOS_POR_THREAD 100

// Estrutura para uma aresta
typedef struct {
    int destino;
    int tempo;
} Aresta;

// Estrutura para um grafo
typedef struct {
    int num_vertices;
    int num_arestas[MAX_VERTICES];
    Aresta* lista_arestas[MAX_VERTICES];
} Grafo;

// Estrutura para armazenar um caminho
typedef struct {
    int vertices[MAX_CAMINHO];
    int tamanho;
    int tempo_total;
} Caminho;

// Caminhos por thread
typedef struct {
    Caminho caminhos[MAX_CAMINHOS_POR_THREAD];
    int num_caminhos;
} CaminhosThread;

CaminhosThread caminhos_por_thread[MAX_THREADS];

// Estrutura de argumentos para a thread
typedef struct {
    int origem;
    int destino;
    int inicio_thread;
    int thread_id;
} ThreadArgs;

// Lista de nomes dos locais
char *NOMES[] = {
    "Estacao Central", "Aeroporto", "Shopping", "Hospital", "Universidade",
    "Rodoviaria", "Correios", "Biblioteca", "Praca", "Mercado",
    "Aquario", "Zoologico", "Estadio", "Centro Comercial", "Terminal",
    "Museu", "Parque", "Banco", "Teatro", "Praia"
};

Grafo cidade;

int obter_indice(char *nome) {
    for (int i = 0; i < MAX_VERTICES; i++) {
        if (strcmp(NOMES[i], nome) == 0) return i;
    }
    return -1;
}

void adicionar_aresta(char *origem, char *destino, int tempo) {
    int o = obter_indice(origem);
    int d = obter_indice(destino);
    if (o == -1 || d == -1) return;

    cidade.lista_arestas[o] = realloc(cidade.lista_arestas[o], (cidade.num_arestas[o] + 1) * sizeof(Aresta));
    cidade.lista_arestas[o][cidade.num_arestas[o]].destino = d;
    cidade.lista_arestas[o][cidade.num_arestas[o]].tempo = tempo;
    cidade.num_arestas[o]++;
}

void adicionar_todas_arestas()
{
    adicionar_aresta("Estacao Central", "Banco", 25);
    adicionar_aresta("Estacao Central", "Aquario", 20);
    adicionar_aresta("Estacao Central", "Praia", 44);
    adicionar_aresta("Aeroporto", "Praca", 35);
    adicionar_aresta("Aeroporto", "Praia", 38);
    adicionar_aresta("Aeroporto", "Universidade", 14);
    adicionar_aresta("Shopping", "Aeroporto", 11);
    adicionar_aresta("Shopping", "Correios", 5);
    adicionar_aresta("Shopping", "Biblioteca", 43);
    adicionar_aresta("Hospital", "Praca", 23);
    adicionar_aresta("Hospital", "Centro Comercial", 34);
    adicionar_aresta("Hospital", "Museu", 9);
    adicionar_aresta("Universidade", "Praca", 42);
    adicionar_aresta("Universidade", "Rodoviaria", 9);
    adicionar_aresta("Universidade", "Correios", 27);
    adicionar_aresta("Rodoviaria", "Estacao Central", 8);
    adicionar_aresta("Rodoviaria", "Teatro", 15);
    adicionar_aresta("Rodoviaria", "Centro Comercial", 14);
    adicionar_aresta("Correios", "Aeroporto", 38);
    adicionar_aresta("Correios", "Aquario", 12);
    adicionar_aresta("Correios", "Rodoviaria", 28);
    adicionar_aresta("Biblioteca", "Hospital", 9);
    adicionar_aresta("Biblioteca", "Correios", 40);
    adicionar_aresta("Biblioteca", "Museu", 34);
    adicionar_aresta("Praca", "Estacao Central", 5);
    adicionar_aresta("Praca", "Estadio", 5);
    adicionar_aresta("Praca", "Museu", 18);
    adicionar_aresta("Mercado", "Shopping", 18);
    adicionar_aresta("Mercado", "Centro Comercial", 33);
    adicionar_aresta("Mercado", "Terminal", 28);
    adicionar_aresta("Aquario", "Centro Comercial", 30);
    adicionar_aresta("Aquario", "Estadio", 41);
    adicionar_aresta("Aquario", "Universidade", 14);
    adicionar_aresta("Zoológico", "Estacao Central", 34);
    adicionar_aresta("Zoológico", "Aeroporto", 32);
    adicionar_aresta("Zoológico", "Mercado", 8);
    adicionar_aresta("Estadio", "Mercado", 8);
    adicionar_aresta("Estadio", "Teatro", 32);
    adicionar_aresta("Estadio", "Biblioteca", 35);
    adicionar_aresta("Centro Comercial", "Aeroporto", 11);
    adicionar_aresta("Centro Comercial", "Shopping", 38);
    adicionar_aresta("Centro Comercial", "Aquario", 27);
    adicionar_aresta("Terminal", "Parque", 34);
    adicionar_aresta("Terminal", "Banco", 44);
    adicionar_aresta("Terminal", "Estadio", 18);
    adicionar_aresta("Museu", "Mercado", 28);
    adicionar_aresta("Museu", "Aquario", 34);
    adicionar_aresta("Museu", "Universidade", 13);
    adicionar_aresta("Parque", "Banco", 10);
    adicionar_aresta("Parque", "Aquario", 9);
    adicionar_aresta("Parque", "Centro Comercial", 44);
    adicionar_aresta("Banco", "Parque", 18);
    adicionar_aresta("Banco", "Aeroporto", 11);
    adicionar_aresta("Banco", "Terminal", 13);
    adicionar_aresta("Teatro", "Aeroporto", 23);
    adicionar_aresta("Teatro", "Centro Comercial", 42);
    adicionar_aresta("Teatro", "Biblioteca", 10);
    adicionar_aresta("Praia", "Teatro", 6);
    adicionar_aresta("Praia", "Centro Comercial", 38);
    adicionar_aresta("Praia", "Biblioteca", 21);
    adicionar_aresta("Rodoviaria", "Shopping", 17);
    adicionar_aresta("Museu", "Aeroporto", 12);
    adicionar_aresta("Museu", "Praca", 33);
    adicionar_aresta("Centro Comercial", "Estacao Central", 19);
    adicionar_aresta("Parque", "Aeroporto", 5);
    adicionar_aresta("Biblioteca", "Shopping", 40);
    adicionar_aresta("Hospital", "Banco", 42);
    adicionar_aresta("Biblioteca", "Terminal", 7);
    adicionar_aresta("Mercado", "Praia", 7);
    adicionar_aresta("Banco", "Parque", 43);
    adicionar_aresta("Teatro", "Centro Comercial", 40);
    adicionar_aresta("Correios", "Universidade", 28);
    adicionar_aresta("Estadio", "Terminal", 45);
    adicionar_aresta("Banco", "Universidade", 41);
    adicionar_aresta("Banco", "Teatro", 20);
    adicionar_aresta("Aquario", "Estacao Central", 35);
    adicionar_aresta("Shopping", "Aquario", 22);
    adicionar_aresta("Universidade", "Biblioteca", 21);
    adicionar_aresta("Aeroporto", "Praia", 9);
    adicionar_aresta("Rodoviaria", "Aeroporto", 27);
    adicionar_aresta("Praca", "Shopping", 13);
    adicionar_aresta("Mercado", "Praia", 7);
    adicionar_aresta("Museu", "Mercado", 23);
    adicionar_aresta("Correios", "Universidade", 14);
    adicionar_aresta("Mercado", "Terminal", 35);
    adicionar_aresta("Terminal", "Rodoviaria", 15);
    adicionar_aresta("Aquario", "Praca", 39);
    adicionar_aresta("Correios", "Museu", 10);
    adicionar_aresta("Aquario", "Estadio", 45);
    adicionar_aresta("Correios", "Terminal", 32);
    adicionar_aresta("Zoológico", "Mercado", 21);
    adicionar_aresta("Universidade", "Correios", 33);
    adicionar_aresta("Correios", "Terminal", 35);
    adicionar_aresta("Centro Comercial", "Museu", 15);
    adicionar_aresta("Museu", "Parque", 34);
    adicionar_aresta("Centro Comercial", "Banco", 15);
    adicionar_aresta("Estacao Central", "Shopping", 37);
    adicionar_aresta("Hospital", "Universidade", 14);
}

void inicializar_grafo() {
    cidade.num_vertices = MAX_VERTICES;
    for (int i = 0; i < MAX_VERTICES; i++) {
        cidade.num_arestas[i] = 0;
        cidade.lista_arestas[i] = NULL;
    }
    adicionar_todas_arestas();
}

void dfs(int atual, int destino, int visitado[], Caminho* caminho_atual, int thread_id) {
    if (atual == destino) {
        if (caminhos_por_thread[thread_id].num_caminhos < MAX_CAMINHOS_POR_THREAD) {
            Caminho novo;
            novo.tamanho = caminho_atual->tamanho;
            novo.tempo_total = caminho_atual->tempo_total;
            memcpy(novo.vertices, caminho_atual->vertices, sizeof(int) * caminho_atual->tamanho);
            caminhos_por_thread[thread_id].caminhos[caminhos_por_thread[thread_id].num_caminhos++] = novo;
        }
        return;
    }

    for (int i = 0; i < cidade.num_arestas[atual]; i++) {
        Aresta a = cidade.lista_arestas[atual][i];
        if (!visitado[a.destino]) {
            visitado[a.destino] = 1;
            caminho_atual->vertices[caminho_atual->tamanho++] = a.destino;
            caminho_atual->tempo_total += a.tempo;

            dfs(a.destino, destino, visitado, caminho_atual, thread_id);

            caminho_atual->tempo_total -= a.tempo;
            caminho_atual->tamanho--;
            visitado[a.destino] = 0;
        }
    }
}

void* thread_dfs(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int visitado[MAX_VERTICES] = {0};

    Caminho caminho_atual;
    caminho_atual.tamanho = 0;
    caminho_atual.tempo_total = 0;

    caminho_atual.vertices[caminho_atual.tamanho++] = args->origem;
    caminho_atual.vertices[caminho_atual.tamanho++] = args->inicio_thread;
    visitado[args->origem] = 1;
    visitado[args->inicio_thread] = 1;

    dfs(args->inicio_thread, args->destino, visitado, &caminho_atual, args->thread_id);

    pthread_exit(NULL);
}

int main() {
    inicializar_grafo();

    int origem = obter_indice("Estacao Central");
    int destino = obter_indice("Aeroporto");

    int thread_count = cidade.num_arestas[origem];
    pthread_t threads[MAX_THREADS];
    ThreadArgs args[MAX_THREADS];

    printf("Buscando caminhos de '%s' para '%s' com paralelismo particionado...\n", NOMES[origem], NOMES[destino]);

    for (int i = 0; i < thread_count; i++) {
        args[i].origem = origem;
        args[i].destino = destino;
        args[i].inicio_thread = cidade.lista_arestas[origem][i].destino;
        args[i].thread_id = i;
        printf("Criando thread %d para vizinho: %s\n", i, NOMES[args[i].inicio_thread]);
        pthread_create(&threads[i], NULL, thread_dfs, &args[i]);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n================ CAMINHOS ENCONTRADOS POR THREAD ================\n");

for (int i = 0; i < thread_count; i++) {
    if (caminhos_por_thread[i].num_caminhos > 0) {
        printf("\n🔹 Thread %d (vizinho: %s):\n", i, NOMES[caminhos_por_thread[i].caminhos[0].vertices[1]]);
    } else {
        printf("\n🔹 Thread %d:\n", i);
    }

    if (caminhos_por_thread[i].num_caminhos == 0) {
        printf("   Nenhum caminho encontrado.\n");
        continue;
    }

    for (int j = 0; j < caminhos_por_thread[i].num_caminhos; j++) {
        Caminho c = caminhos_por_thread[i].caminhos[j];
        printf("   Caminho %d: ", j + 1);
        for (int k = 0; k < c.tamanho; k++) {
            printf("%s", NOMES[c.vertices[k]]);
            if (k < c.tamanho - 1) printf(" -> ");
        }
        printf("   | Tempo total: %d min\n", c.tempo_total);
    }
}

    printf("\n================ MELHOR CAMINHO POR THREAD =====================\n");

    for (int i = 0; i < thread_count; i++) {
        printf("\n🔹 Thread %d (vizinho: %s)\n", i, NOMES[args[i].inicio_thread]);

        if (caminhos_por_thread[i].num_caminhos == 0) {
            printf("   Nenhum caminho encontrado.\n");
            continue;
        }

        int melhor_tempo = __INT_MAX__;
        int indice_melhor = -1;

        for (int j = 0; j < caminhos_por_thread[i].num_caminhos; j++) {
            if (caminhos_por_thread[i].caminhos[j].tempo_total < melhor_tempo) {
                melhor_tempo = caminhos_por_thread[i].caminhos[j].tempo_total;
                indice_melhor = j;
            }
        }

        Caminho melhor = caminhos_por_thread[i].caminhos[indice_melhor];
        printf("   ");
        for (int k = 0; k < melhor.tamanho; k++) {
            printf("%s", NOMES[melhor.vertices[k]]);
            if (k < melhor.tamanho - 1) printf(" -> ");
        }
        printf("   | Tempo total: %d min\n", melhor.tempo_total);
    }



    return 0;
}
