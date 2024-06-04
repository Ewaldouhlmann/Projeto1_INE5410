/**
 * Esse arquivo tem como objetivo a implementação de um brinquedo em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "toy.h"
#include "shared.h"

void liberar_clientes(toy_t *toy);
void liberar_vagas(toy_t *toy);
void executa_brinquedo(toy_t *toy);

void *turn_on(void *args) {
    toy_t *toy = (toy_t *) args;
    toy->tempo_exec = 0; // Tempo de execução do brinquedo
    toy->tempo_espera = rand() % 5 + 1; // Tempo de espera do brinquedo
    sem_init(&toy->sem_entrar, 0, toy->capacity); // Inicializa o semáforo de entrada com capacidade total
    sem_init(&toy->sem_sair, 0, 0); // Inicializa o semáforo de saída com 0
    debug("[INFO] - Brinquedo [%d] iniciado\n", toy->id);

    while (1) {
        pthread_mutex_lock(&toy->mtx_clients); // Bloqueia o contador de clientes no brinquedo
        if (toy->clients > 0) {
            debug("[TOY] - Brinquedo [%d] com %d clientes\n", toy->id, toy->clients);
            executa_brinquedo(toy); // Executa o brinquedo
            pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo
        } else {
            debug("[TOY] - Brinquedo [%d] sem clientes\n", toy->id);
            pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo
            sleep(toy->tempo_espera); // Espera um tempo aleatório
        }

        if (total_clientes == 0) {
            break; // Sai do loop quando não há mais clientes
        }
    }

    pthread_exit(NULL);
}

void executa_brinquedo(toy_t *toy) {
    debug("[TOY] - Brinquedo [%d] ligou\n", toy->id);

    sleep(toy->tempo_exec); // Brinquedo aberto por um tempo aleatório

    liberar_clientes(toy); // Libera os clientes do brinquedo
    liberar_vagas(toy); // Libera as vagas do brinquedo

    debug("[TOY] - Brinquedo [%d] encerrou\n", toy->id);
}

void liberar_vagas(toy_t *toy) {
    int vagas = toy->clients; // Recupera o número de vagas
    toy->clients = 0; // Reseta o contador de clientes
    debug("[TOY] - Brinquedo [%d] liberando %d vagas\n", toy->id, vagas);
    for (int i = 0; i < vagas; i++) {
        sem_post(&toy->sem_entrar); // Libera a entrada de um cliente
    }
}

void liberar_clientes(toy_t *toy) {
    debug("[TOY] - Brinquedo [%d] liberando %d clientes\n", toy->id, toy->clients);
    for (int i = 0; i < toy->clients; i++) {
        sem_post(&toy->sem_sair); // Libera a saída de um cliente
    }
}


void open_toys(toy_args *args) {
    total_brinquedos = args->n;   // Recupera o total de brinquedos
    brinquedos = malloc(sizeof(toy_t *) * total_brinquedos);

    for (int i = 0; i < total_brinquedos; i++) {
        brinquedos[i] = args->toys[i];
        pthread_create(&brinquedos[i]->thread, NULL, turn_on, (void *)brinquedos[i]);
    }

    // Sincroniza com os funcionários
    pthread_mutex_lock(&sync_mutex);
    sync_count += total_brinquedos;
    if (sync_count >= total_brinquedos + total_func) {
        brinquedos_abertos = 1;
        pthread_cond_broadcast(&sync_cond);
    }
    pthread_mutex_unlock(&sync_mutex);
}

// Desligando os brinquedos
void close_toys(){
    for (int i = 0; i < total_brinquedos; i++)
    {
        sem_destroy(&brinquedos[i]->sem_sair);
        sem_destroy(&brinquedos[i]->sem_entrar);
        pthread_mutex_destroy(&brinquedos[i]->mtx_clients);
        pthread_join(brinquedos[i]->thread, NULL);
    }

    // Liberando da memórias

    free(brinquedos);
}