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
    toy->tempo_exec = rand() % 5 + 1; // Tempo de execução do brinquedo
    toy->tempo_espera = rand() % 5 + 1; // Tempo de espera do brinquedo
    sem_init(&toy->sem_entrar, 0, toy->capacity); // Inicializa o semáforo de entrada com capacidade total

    while (1) {
        if (total_clientes == 0) {
            break; // Sai do loop quando não há mais clientes
        }

        pthread_mutex_lock(&toy->mtx_clients); // Bloqueia o contador de clientes no brinquedo
        if (toy->clients > 0 || toy->cronometro > toy->tempo_espera) {
            debug("[TOY] - Brinquedo [%d] iniciou\n", toy->id);
            pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo

            sleep(toy->tempo_exec); // Brinquedo aberto por um tempo aleatório

            pthread_mutex_lock(&toy->mtx_clients); // Bloqueia o contador de clientes no brinquedo novamente
            liberar_clientes(toy); // Libera os clientes do brinquedo
            liberar_vagas(toy); // Libera as vagas do brinquedo
            toy->clients = 0; // Reseta o contador de clientes
            toy->cronometro = 0; // Reseta o cronômetro
            pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo novamente

            debug("[TOY] - Brinquedo [%d] encerrou\n", toy->id);

        } else {
            pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo
        }
        sleep(1); // Incrementa o cronômetro a cada segundo
    }

    pthread_exit(NULL);
}


// Executa o brinquedo
void executa_brinquedo(toy_t *toy)
{
    debug("[TOY] - Brinquedo [%d] iniciou\n", toy->id);
    pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo

    sleep(toy->tempo_exec); // Brinquedo aberto por um tempo aleatório

    pthread_mutex_lock(&toy->mtx_clients); // Bloqueia o contador de clientes no brinquedo novamente
    liberar_clientes(toy); // Libera os clientes do brinquedo
    liberar_vagas(toy); // Libera as vagas do brinquedo
    toy->clients = 0; // Reseta o contador de clientes
    pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo novamente

    debug("[TOY] - Brinquedo [%d] encerrou\n", toy->id);

    toy->cronometro = 0; // Reseta o cronômetro
}

// Libera os clientes do brinquedo
void liberar_clientes(toy_t *toy) {
    for (int i = 0; i < toy->clients; i++) {
        sem_post(&toy->sem_sair); // Libera a saída de um cliente
    }
}

// Libera as vagas do brinquedo
void liberar_vagas(toy_t *toy) {
    for (int i = 0; i < toy->capacity; i++) {
        sem_post(&toy->sem_entrar); // Libera a entrada de um cliente
    }
}

// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args){
    total_brinquedos = args->n;   // recupera o total de 
    brinquedos = malloc(sizeof(toy_t *) * total_brinquedos);

    for(int i = 0; i < total_brinquedos; i++)
    {
        brinquedos[i] = args->toys[i];
        pthread_create(&brinquedos[i]->thread, NULL, turn_on, (void *)brinquedos[i]);
    }

}

// Desligando os brinquedos
void close_toys(){
    for (int i = 0; i < total_brinquedos; i++)
    {
        pthread_join(brinquedos[i]->thread, NULL);
    }

    // Liberando da memória
    free(brinquedos);
}