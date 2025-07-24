#include <stdio.h>

#define MAX 1000

int adj[MAX][MAX];
int visited[MAX];
int n = 6;

void dfs(int u) {
    visited[u] = 1;
    for (int v = 0; v < n; v++) {
        if (adj[u][v] && !visited[v]) {
            dfs(v);
        }
    }
}

int find_cycle_util(int u, int *stack, int *on_stack) {
    visited[u] = 1;
    stack[u] = 1;
    on_stack[u] = 1;

    for (int v = 0; v < n; v++) {
        if (!adj[u][v]) continue;
        if (!visited[v]) {
            if (find_cycle_util(v, stack, on_stack)) return 1;
        } else if (on_stack[v]) {
            return 1;
        }
    }

    stack[u] = 0;
    on_stack[u] = 0;
    return 0;
}

int tem_ciclo() {
    int stack[MAX] = {0};
    int on_stack[MAX] = {0};
    for (int i = 0; i < n; i++) visited[i] = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            if (find_cycle_util(i, stack, on_stack)) return 1;
        }
    }
    return 0;
}

int main() {
    int i;
    int arestas[][2] = {
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 1},  // ciclo: 1 → 2 → 3 → 1
        {2, 4},
        {4, 5}
    };
    int m = sizeof(arestas) / sizeof(arestas[0]);

    for (i = 0; i < m; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        adj[u][v] = 1;
    }

    for (i = 0; i < n; i++) visited[i] = 0;
    dfs(0);

    int completo = 1;
    for (i = 0; i < n; i++) {
        if (!visited[i]) {
            completo = 0;
            break;
        }
    }

    if (completo) {
        printf("Todos os nós são alcançáveis a partir do nó 0.\n");
    } else {
        printf("Nem todos os nós são alcançáveis a partir do nó 0.\n");
    }

    if (tem_ciclo()) {
        printf("O grafo possui um ciclo (potencial separador cíclico).\n");
    } else {
        printf("O grafo não possui ciclo.\n");
    }

    return 0;
}
