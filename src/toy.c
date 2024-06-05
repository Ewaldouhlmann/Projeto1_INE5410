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
    // Pega os argumentos do brinquedo e define o tempo de execução e espera
    toy_t *toy = (toy_t *) args;
    toy->tempo_exec = rand() % 5 + 1;
    toy->tempo_espera = 5; 

    // Inicializa os semáforos e mutex do brinquedo
    sem_init(&toy->sem_entrar, 0, toy->capacity); 
    sem_init(&toy->sem_sair, 0, 0); 
    pthread_mutex_init(&toy->mtx_clients, NULL);

    debug("[START] - Brinquedo [%d] iniciado\n", toy->id);

    // Enquanto houver clientes no parque, executa o brinquedo
    while (1) {

        // Verifica se há clientes no brinquedo, e executa o brinquedo caso tenha
        pthread_mutex_lock(&toy->mtx_clients);
        if (toy->clients > 0) 
        {
            executa_brinquedo(toy); 
            pthread_mutex_unlock(&toy->mtx_clients); 
        } 
        else 
        {
            pthread_mutex_unlock(&toy->mtx_clients); 

            // Espera para a thread não ficar em loop infinito
            sleep(toy->tempo_espera); 
        }

        // Verifica o número total de clientes no parque e sai do loop caso não tenha mais clientes
        if (total_clientes == 0) {
            break;
        }
    }

    debug("[END] - Brinquedo [%d] encerrado\n", toy->id);
    pthread_exit(NULL);
}


// Essa função executa o brinquedo
void executa_brinquedo(toy_t *toy) {
    // Brinquedo inicia e espera até o fim da execução
    debug("[ON] - Brinquedo [%d] foi ligado\n", toy->id);

    sleep(toy->tempo_exec);

    debug("[OFF] - Brinquedo [%d] foi desligado\n", toy->id);

    // Libera os clientes que estão no brinquedo
    liberar_clientes(toy);

    // Permitir que novos clientes entrem no brinquedo
    liberar_vagas(toy);

}


// Essa função libera as vagas restantes do brinquedo
void liberar_vagas(toy_t *toy) {
    // Guarda o número de vagas para serem liberadas e reseta o número de clientes
    int vagas = toy->clients;
    toy->clients = 0;

    // Libera as novas vagas para o brinquedo
    for (int i = 0; i < vagas; i++) {
        sem_post(&toy->sem_entrar);
    }
}

// Essa função libera os clientes do brinquedo
void liberar_clientes(toy_t *toy) {
    // Libera os clientes que estavam no brinquedo para procurarem outro brinquedo ou sair do parque
    for (int i = 0; i < toy->clients; i++) {
        sem_post(&toy->sem_sair);
    }
}


void open_toys(toy_args *args) {
    // Guarda o total de brinquedos e inicializa o array de brinquedos
    total_brinquedos = args->n; 
    brinquedos = malloc(sizeof(toy_t *) * total_brinquedos);

    // Inicializa as threads dos brinquedos
    for (int i = 0; i < total_brinquedos; i++) {
        brinquedos[i] = args->toys[i];
        pthread_create(&brinquedos[i]->thread, NULL, turn_on, (void *)brinquedos[i]);
    }

    // Sincroniza para liberar a bilheteria
    pthread_mutex_lock(&sync_mutex);
    sync_count += total_brinquedos;
    if (sync_count >= total_brinquedos + total_func) {
        brinquedos_abertos = 1;
        // Acorda a condição de broadcast para liberar a bilheteria
        pthread_cond_broadcast(&sync_cond); 
    }
    pthread_mutex_unlock(&sync_mutex);

}

// Desligando os brinquedos
void close_toys(){
    // Espera a finalização dos brinquedos
    for (int i = 0; i < total_brinquedos; i++)
    {
        // Dá join nas threads dos brinquedos e destroi os semáforos e mutex
        pthread_join(brinquedos[i]->thread, NULL);
        sem_destroy(&brinquedos[i]->sem_sair);
        sem_destroy(&brinquedos[i]->sem_entrar);
        pthread_mutex_destroy(&brinquedos[i]->mtx_clients);
    }

    // Liberando os brinquedos da memória
    free(brinquedos);
}