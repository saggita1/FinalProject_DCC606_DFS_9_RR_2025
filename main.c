#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define N 13
#define MAX_PATHS 1000
#define MAX_COMPONENTS 50
#define MAX_SEPARATORS 20

// ===== MATRIZ DE ADJACÊNCIA =====
int matriz[N][N] = {
    {0,3,4,0,0,0,0,0,0,0,0,0,0},
    {0,0,5,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,2,5,0,0,0,0,0,0,0,0},
    {0,0,0,0,6,0,6,0,0,0,0,0,0},
    {0,0,0,0,0,4,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,3,0,2,0,0,0},
    {0,0,0,0,0,0,0,0,4,0,0,3,0},
    {0,0,0,0,0,0,0,0,0,2,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,6,0,6},
    {0,0,0,0,0,0,0,0,0,0,0,4,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1},
    {9,0,0,0,0,0,0,0,0,0,0,0,0}
};

pthread_mutex_t global_lock;

// ===== ESTRUTURAS DE DADOS =====

// Estrutura para representar um caminho no separador
typedef struct {
    int vertices[N];
    int length;
} CyclePath;

// Estrutura para representar um separador de ciclo
typedef struct {
    CyclePath paths[MAX_SEPARATORS];
    int num_paths;
    int separator_vertices[N];  // Vértices que fazem parte do separador
    bool is_separator[N];       // Marca se vértice é separador
} CycleSeparator;

// Estrutura para componentes após remoção do separador
typedef struct {
    int vertices[N];
    int size;
    int component_id;
} Component;

// Estrutura para árvore DFS
typedef struct {
    int parent[N];
    int discovery_time[N];
    int finish_time[N];
    int dfs_number[N];
    bool visited[N];
    int time_counter;
} DFSTree;

// Estrutura para threading
typedef struct {
    Component* component;
    DFSTree* local_tree;
    int thread_id;
} ThreadData;

// ===== VARIÁVEIS GLOBAIS =====
DFSTree global_dfs_tree;
Component components[MAX_COMPONENTS];
int num_components = 0;
int global_time = 0;

// ===== FUNÇÕES AUXILIARES =====

void init_dfs_tree(DFSTree* tree) {
    for (int i = 0; i < N; i++) {
        tree->parent[i] = -1;
        tree->discovery_time[i] = -1;
        tree->finish_time[i] = -1;
        tree->dfs_number[i] = -1;
        tree->visited[i] = false;
    }
    tree->time_counter = 0;
}

// Encontra componentes fortemente conexas (para construir separadores)
void tarjan_scc(int v, int* indices, int* lowlinks, int* stack, int* stack_size, 
                bool* on_stack, int* index_counter, Component sccs[], int* scc_count) {
    indices[v] = lowlinks[v] = (*index_counter)++;
    stack[(*stack_size)++] = v;
    on_stack[v] = true;
    
    for (int w = 0; w < N; w++) {
        if (matriz[v][w] > 0) {
            if (indices[w] == -1) {
                tarjan_scc(w, indices, lowlinks, stack, stack_size, on_stack, 
                          index_counter, sccs, scc_count);
                lowlinks[v] = (lowlinks[w] < lowlinks[v]) ? lowlinks[w] : lowlinks[v];
            } else if (on_stack[w]) {
                lowlinks[v] = (indices[w] < lowlinks[v]) ? indices[w] : lowlinks[v];
            }
        }
    }
    
    if (lowlinks[v] == indices[v]) {
        Component* scc = &sccs[*scc_count];
        scc->size = 0;
        scc->component_id = *scc_count;
        
        int w;
        do {
            w = stack[--(*stack_size)];
            on_stack[w] = false;
            scc->vertices[scc->size++] = w;
        } while (w != v);
        
        (*scc_count)++;
    }
}

// ===== IMPLEMENTAÇÃO DOS SEPARADORES DE CICLO =====

// Encontra um ciclo fundamental em uma SCC
bool find_fundamental_cycle(Component* scc, CyclePath* cycle) {
    if (scc->size <= 1) return false;
    
    int start = scc->vertices[0];
    bool visited[N] = {false};
    int path[N];
    int path_len = 0;
    
    // DFS para encontrar um ciclo
    int stack[N], stack_top = 0;
    int parent[N];
    
    for (int i = 0; i < N; i++) parent[i] = -1;
    
    stack[stack_top++] = start;
    visited[start] = true;
    
    while (stack_top > 0) {
        int v = stack[--stack_top];
        path[path_len++] = v;
        
        for (int i = 0; i < scc->size; i++) {
            int w = scc->vertices[i];
            if (matriz[v][w] > 0) {
                if (!visited[w]) {
                    visited[w] = true;
                    parent[w] = v;
                    stack[stack_top++] = w;
                } else if (parent[v] != w && w == start && path_len > 2) {
                    // Encontrou um ciclo de volta ao início
                    cycle->length = path_len;
                    for (int j = 0; j < path_len; j++) {
                        cycle->vertices[j] = path[j];
                    }
                    return true;
                }
            }
        }
    }
    
    // Se não encontrou ciclo completo, cria um caminho
    if (path_len > 1) {
        cycle->length = path_len;
        for (int j = 0; j < path_len; j++) {
            cycle->vertices[j] = path[j];
        }
        return true;
    }
    
    return false;
}

// Implementação da rotina REDUCE (simplificada)
void reduce_separator_paths(CycleSeparator* separator) {
    if (separator->num_paths <= 1) return;
    
    printf("REDUCE: Reduzindo %d caminhos...\n", separator->num_paths);
    
    // Combina pares de caminhos (simulação da redução)
    int new_count = 0;
    for (int i = 0; i < separator->num_paths - 1; i += 2) {
        CyclePath* path1 = &separator->paths[i];
        CyclePath* path2 = &separator->paths[i + 1];
        
        // Combina os dois caminhos em um novo caminho
        CyclePath* new_path = &separator->paths[new_count];
        new_path->length = 0;
        
        // Adiciona vértices do primeiro caminho
        for (int j = 0; j < path1->length && new_path->length < N; j++) {
            new_path->vertices[new_path->length++] = path1->vertices[j];
        }
        
        // Adiciona vértices do segundo caminho (evitando duplicatas)
        for (int j = 0; j < path2->length && new_path->length < N; j++) {
            bool exists = false;
            for (int k = 0; k < new_path->length; k++) {
                if (new_path->vertices[k] == path2->vertices[j]) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                new_path->vertices[new_path->length++] = path2->vertices[j];
            }
        }
        
        new_count++;
    }
    
    // Se sobrou um caminho ímpar
    if (separator->num_paths % 2 == 1) {
        separator->paths[new_count] = separator->paths[separator->num_paths - 1];
        new_count++;
    }
    
    separator->num_paths = new_count;
    printf("REDUCE: Resultado com %d caminhos\n", new_count);
}

// Implementação de JOIN_PATHS_TO_CYCLE_SEPARATOR
void join_paths_to_cycle_separator(CycleSeparator* separator) {
    printf("JOIN_PATHS: Unindo caminhos em ciclos...\n");
    
    // Marca todos os vértices dos caminhos como separadores
    for (int i = 0; i < separator->num_paths; i++) {
        for (int j = 0; j < separator->paths[i].length; j++) {
            int v = separator->paths[i].vertices[j];
            if (!separator->is_separator[v]) {
                separator->separator_vertices[0]++; // Conta total
                separator->is_separator[v] = true;
            }
        }
    }
}

// Constrói separador de ciclo direcionado
bool construct_cycle_separator(Component sccs[], int scc_count, CycleSeparator* separator) {
    printf("\n=== CONSTRUINDO SEPARADOR DE CICLO ===\n");
    
    separator->num_paths = 0;
    for (int i = 0; i < N; i++) {
        separator->is_separator[i] = false;
    }
    separator->separator_vertices[0] = 0;
    
    // Para cada SCC não trivial, encontra um ciclo fundamental
    for (int i = 0; i < scc_count; i++) {
        if (sccs[i].size > 1) {
            CyclePath cycle;
            if (find_fundamental_cycle(&sccs[i], &cycle)) {
                separator->paths[separator->num_paths] = cycle;
                separator->num_paths++;
                
                printf("Ciclo encontrado na SCC %d: ", i);
                for (int j = 0; j < cycle.length; j++) {
                    printf("%d ", cycle.vertices[j]);
                }
                printf("(tamanho: %d)\n", cycle.length);
            }
        }
    }
    
    if (separator->num_paths == 0) {
        printf("Nenhum separador não-trivial encontrado\n");
        return false;
    }
    
    // Aplica REDUCE enquanto há muitos caminhos
    while (separator->num_paths > 3) {
        reduce_separator_paths(separator);
    }
    
    // Aplica JOIN_PATHS_TO_CYCLE_SEPARATOR
    join_paths_to_cycle_separator(separator);
    
    printf("Separador final construído com %d caminhos\n", separator->num_paths);
    return true;
}

// ===== DECOMPOSIÇÃO BASEADA EM SEPARADORES =====

void decompose_graph(CycleSeparator* separator) {
    printf("\n=== DECOMPONDO GRAFO ===\n");
    num_components = 0;
    
    // Cria componentes removendo vértices do separador
    bool used[N] = {false};
    
    // Marca vértices do separador como usados
    for (int i = 0; i < N; i++) {
        if (separator->is_separator[i]) {
            used[i] = true;
        }
    }
    
    // Para cada vértice não usado, cria uma componente por DFS
    for (int start = 0; start < N; start++) {
        if (!used[start]) {
            Component* comp = &components[num_components];
            comp->size = 0;
            comp->component_id = num_components;
            
            // DFS para encontrar componente conexa
            int stack[N], stack_top = 0;
            stack[stack_top++] = start;
            used[start] = true;
            
            while (stack_top > 0) {
                int v = stack[--stack_top];
                comp->vertices[comp->size++] = v;
                
                for (int w = 0; w < N; w++) {
                    if (matriz[v][w] > 0 && !used[w] && !separator->is_separator[w]) {
                        used[w] = true;
                        stack[stack_top++] = w;
                    }
                }
            }
            
            if (comp->size > 0) {
                printf("Componente %d: ", num_components);
                for (int i = 0; i < comp->size; i++) {
                    printf("%d ", comp->vertices[i]);
                }
                printf("(tamanho: %d)\n", comp->size);
                num_components++;
            }
        }
    }
    
    printf("Total de componentes após decomposição: %d\n", num_components);
}

// ===== DFS PARALELO NAS COMPONENTES =====

void dfs_component(int v, Component* comp, DFSTree* tree, int thread_id) {
    pthread_mutex_lock(&global_lock);
    tree->discovery_time[v] = tree->time_counter++;
    tree->visited[v] = true;
    printf("Thread %d: Descobrindo vértice %d (tempo %d)\n", 
           thread_id, v, tree->discovery_time[v]);
    pthread_mutex_unlock(&global_lock);
    
    // Explora vizinhos dentro da componente
    for (int i = 0; i < comp->size; i++) {
        int w = comp->vertices[i];
        if (matriz[v][w] > 0 && !tree->visited[w]) {
            tree->parent[w] = v;
            dfs_component(w, comp, tree, thread_id);
        }
    }
    
    pthread_mutex_lock(&global_lock);
    tree->finish_time[v] = tree->time_counter++;
    printf("Thread %d: Finalizando vértice %d (tempo %d)\n", 
           thread_id, v, tree->finish_time[v]);
    pthread_mutex_unlock(&global_lock);
}

void* parallel_dfs_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    Component* comp = data->component;
    DFSTree* tree = data->local_tree;
    int thread_id = data->thread_id;
    
    printf("Thread %d iniciada para componente %d\n", thread_id, comp->component_id);
    
    init_dfs_tree(tree);
    
    // Executa DFS em cada vértice não visitado da componente
    for (int i = 0; i < comp->size; i++) {
        int v = comp->vertices[i];
        if (!tree->visited[v]) {
            printf("Thread %d: Iniciando DFS do vértice %d\n", thread_id, v);
            dfs_component(v, comp, tree, thread_id);
        }
    }
    
    printf("Thread %d finalizada\n", thread_id);
    return NULL;
}

// ===== FUNÇÃO PRINCIPAL =====

int main() {
    printf("=== DFS PARALELO COM SEPARADORES DE CICLO DIRECIONADO ===\n");
    printf("Baseado no artigo de Aggarwal, Anderson e Kao (1989)\n\n");
    
    pthread_mutex_init(&global_lock, NULL);
    init_dfs_tree(&global_dfs_tree);
    
    // Passo 1: Encontrar SCCs usando Tarjan
    printf("=== PASSO 1: ENCONTRANDO SCCs ===\n");
    Component sccs[MAX_COMPONENTS];
    int scc_count = 0;
    int indices[N], lowlinks[N], stack[N], stack_size = 0;
    bool on_stack[N];
    int index_counter = 0;
    
    for (int i = 0; i < N; i++) {
        indices[i] = -1;
        lowlinks[i] = -1;
        on_stack[i] = false;
    }
    
    for (int v = 0; v < N; v++) {
        if (indices[v] == -1) {
            tarjan_scc(v, indices, lowlinks, stack, &stack_size, on_stack, 
                      &index_counter, sccs, &scc_count);
        }
    }
    
    printf("Encontradas %d SCCs\n", scc_count);
    for (int i = 0; i < scc_count; i++) {
        printf("SCC %d: ", i);
        for (int j = 0; j < sccs[i].size; j++) {
            printf("%d ", sccs[i].vertices[j]);
        }
        printf("(tamanho: %d)\n", sccs[i].size);
    }
    
    // Passo 2: Construir separador de ciclo direcionado
    printf("\n=== PASSO 2: CONSTRUINDO SEPARADORES ===\n");
    CycleSeparator separator;
    if (!construct_cycle_separator(sccs, scc_count, &separator)) {
        printf("Falha ao construir separador. Executando DFS sequencial...\n");
        // Fallback para DFS sequencial
        for (int v = 0; v < N; v++) {
            if (!global_dfs_tree.visited[v]) {
                dfs_component(v, NULL, &global_dfs_tree, 0);
            }
        }
    } else {
        // Passo 3: Decompor grafo usando separadores
        printf("\n=== PASSO 3: DECOMPOSIÇÃO ===\n");
        decompose_graph(&separator);
        
        // Passo 4: DFS paralelo nas componentes
        printf("\n=== PASSO 4: DFS PARALELO ===\n");
        pthread_t threads[MAX_COMPONENTS];
        ThreadData thread_data[MAX_COMPONENTS];
        DFSTree local_trees[MAX_COMPONENTS];
        
        // Criar threads para cada componente
        for (int i = 0; i < num_components; i++) {
            thread_data[i].component = &components[i];
            thread_data[i].local_tree = &local_trees[i];
            thread_data[i].thread_id = i;
            
            pthread_create(&threads[i], NULL, parallel_dfs_thread, &thread_data[i]);
        }
        
        // Aguardar conclusão das threads
        for (int i = 0; i < num_components; i++) {
            pthread_join(threads[i], NULL);
        }
        
        // Passo 5: Consolidar resultados
        printf("\n=== PASSO 5: CONSOLIDAÇÃO ===\n");
        printf("Árvore DFS consolidada:\n");
        for (int i = 0; i < N; i++) {
            // Busca informações nas árvores locais
            for (int j = 0; j < num_components; j++) {
                if (local_trees[j].visited[i]) {
                    printf("Vértice %d: pai=%d, descoberta=%d, término=%d (Thread %d)\n",
                           i, local_trees[j].parent[i], 
                           local_trees[j].discovery_time[i],
                           local_trees[j].finish_time[i], j);
                    break;
                }
            }
        }
    }
    
    printf("\n=== DFS PARALELO CONCLUÍDO ===\n");
    pthread_mutex_destroy(&global_lock);
    return 0;
}
