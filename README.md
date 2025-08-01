# Sistema de Navegação Urbana - DFS Paralelo

Um sistema avançado de busca de rotas urbanas implementado em C, utilizando algoritmo DFS (Depth-First Search) paralelo com múltiplas threads para encontrar rotas otimizadas entre pontos de interesse de uma cidade.

## 🚀 Características Principais

- **Busca Paralela**: Utiliza até 8 threads simultâneas para exploração eficiente do grafo
- **Múltiplos Critérios**: Otimização por tempo, distância, custo ou número de transferências
- **Interface Interativa**: Menu intuitivo para diferentes tipos de consulta
- **Grafo Realístico**: 20 pontos de interesse conectados por diferentes meios de transporte
- **Análise Estatística**: Relatórios detalhados dos resultados encontrados

## 🏙️ Pontos de Interesse

O sistema modela uma cidade com os seguintes locais:

| ID | Local | ID | Local |
|----|-------|-------|-------|
| 0 | Estação Central | 10 | Banco Central |
| 1 | Shopping Center | 11 | Correios |
| 2 | Universidade | 12 | Aeroporto |
| 3 | Hospital | 13 | Rodoviária |
| 4 | Parque Central | 14 | Praia |
| 5 | Museu | 15 | Estádio |
| 6 | Biblioteca | 16 | Zoológico |
| 7 | Teatro | 17 | Aquário |
| 8 | Praça Principal | 18 | Centro Comercial |
| 9 | Mercado Municipal | 19 | Terminal Metrô |

## 🚌 Meios de Transporte

- **Metrô**: Rápido e com poucas transferências
- **Ônibus**: Cobertura ampla da cidade
- **Trem**: Conexões de longa distância
- **Táxi**: Opção mais cara mas direta

## 🛠️ Compilação e Execução

### Pré-requisitos
- Compilador GCC com suporte a pthread
- Sistema operacional Unix/Linux ou Windows com MinGW

### Compilação
```bash
gcc -o navegacao_urbana paste.c -lpthread -lm
```

### Execução
```bash
./navegacao_urbana
```

## 📋 Menu de Opções

1. **Buscar rotas (Estação Central → Aeroporto)**: Busca padrão pré-configurada
2. **Buscar rotas personalizadas**: Escolha origem e destino
3. **Exibir por menor tempo**: Ordena resultados por tempo de viagem
4. **Exibir por menor distância**: Ordena por distância percorrida
5. **Exibir por menor custo**: Ordena por custo da viagem
6. **Exibir por menos transferências**: Ordena por número de baldeações
7. **Exibir estatísticas**: Análise comparativa dos resultados
8. **Exibir mapa da cidade**: Visualiza todas as conexões disponíveis

## 🧵 Implementação Paralela

### Arquitetura de Threads
- **Thread Principal**: Coordena a execução e interface
- **Threads de Busca**: Cada thread explora diferentes caminhos iniciais
- **Sincronização**: Mutex protege a lista global de resultados

### Algoritmo DFS
```c
// Cada thread executa DFS independente
void dfs_recursivo(int vertice_atual, int destino, ...)
{
    // Verifica se chegou ao destino
    if (vertice_atual == destino) {
        adicionar_caminho(&novo_caminho);  // Thread-safe
        return;
    }
    
    // Explora vizinhos recursivamente
    for (int i = 0; i < num_arestas[vertice_atual]; i++) {
        // Evita ciclos e continua busca
        dfs_recursivo(proximo_vertice, ...);
    }
}
```

## 📊 Exemplo de Saída

```
=== ROTAS ENCONTRADAS (ordenadas por: Menor Tempo) ===

Rota 1:
  Caminho: Estação Central -[trem]-> Aeroporto
  Tempo total: 45 minutos
  Distância total: 25 km
  Custo total: R$ 15
  Transferências: 1

Rota 2:
  Caminho: Estação Central -[metrô]-> Rodoviária -[ônibus]-> Aeroporto
  Tempo total: 50 minutos
  Distância total: 27 km
  Custo total: R$ 16
  Transferências: 1

*** MELHOR ROTA RECOMENDADA ***
Rota: Estação Central -> Aeroporto
Detalhes: 45 min, 25 km, R$ 15, 1 transferências
```

## 🔧 Configurações Técnicas

### Constantes do Sistema
```c
#define MAX_VERTICES 20        // Máximo de pontos de interesse
#define MAX_THREADS 8          // Número de threads paralelas
#define MAX_PATHS 100         // Máximo de rotas armazenadas
#define MAX_PATH_LENGTH 50    // Comprimento máximo de uma rota
```

### Estruturas de Dados
- **Aresta**: Representa conexão entre dois pontos
- **GrafoCidade**: Matriz de adjacência com pesos múltiplos
- **Caminho**: Armazena uma rota completa com métricas
- **ResultadoBusca**: Lista thread-safe de todos os caminhos

## 📈 Análise de Desempenho

O sistema fornece métricas detalhadas:
- Tempo de execução da busca paralela
- Número de threads utilizadas
- Quantidade total de rotas encontradas
- Estatísticas comparativas (min, max, média) para todos os critérios

## 🔒 Sincronização e Thread Safety

### Mecanismos Utilizados
- **pthread_mutex_t**: Protege acesso à lista global de resultados
- **Verificação de Finalização**: Coordena término das threads
- **Detecção de Ciclos**: Evita loops infinitos na busca

### Exemplo de Sincronização
```c
void adicionar_caminho(Caminho* caminho) {
    pthread_mutex_lock(&resultado_global.mutex);
    // Adiciona caminho de forma thread-safe
    resultado_global.caminhos[resultado_global.num_caminhos++] = *caminho;
    pthread_mutex_unlock(&resultado_global.mutex);
}
```

## 🎯 Casos de Uso

### Planejamento de Viagens
- Encontrar a rota mais rápida para o aeroporto
- Planejar passeios turísticos econômicos
- Otimizar deslocamentos urbanos diários

### Análise de Mobilidade
- Comparar diferentes meios de transporte
- Identificar gargalos no sistema de transporte
- Avaliar impacto de transferências na viagem

## 🚀 Extensões Possíveis

- **Algoritmos Adicionais**: Implementar Dijkstra, A* ou Floyd-Warshall
- **Dados Dinâmicos**: Integrar APIs de trânsito em tempo real
- **Interface Gráfica**: Visualização do mapa e rotas
- **Persistência**: Salvar/carregar configurações e histórico
- **Otimização Multi-objetivo**: Combinar múltiplos critérios simultaneamente

## 📝 Notas Técnicas

### Complexidade
- **Temporal**: O(V^P) onde V é o número de vértices e P a profundidade máxima
- **Espacial**: O(P * T) onde T é o número de threads

### Limitações
- Profundidade de busca limitada para evitar explosão combinatorial
- Número máximo de caminhos armazenados para controle de memória
- Grafo estático (não considera mudanças em tempo real)

## 👥 Contribuição

Este sistema demonstra conceitos avançados de:
- Programação concorrente em C
- Algoritmos de busca em grafos
- Sincronização de threads
- Estruturas de dados complexas
- Otimização multi-critério
