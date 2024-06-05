/**
 * Esse arquivo tem como objetivo a implementação do cliente em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "defs.h"
#include "client.h"
#include "queue.h"
#include "shared.h"

void queue_enter(client_t *self);
void buy_coins(client_t *self);
void wait_ticket(client_t *self);
void enjoy_toy(client_t *self);


// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){
    // Recupera os argumentos do cliente
    client_t *self = (client_t *) arg;
    
    // Entra na fila da bilheteria e espera até comprar moedas
    queue_enter(self); 

    // Entra no parque e começa a brincar até ficar sem moedas
    debug("[CLIENT] - Cliente %d entrou no parque\n", self->id);
    while(self->coins > 0){
        enjoy_toy(self);
    }

    // Sai do parque e finaliza a thread
    pthread_mutex_lock(&mtx_people);
    total_clientes--;
    // Se não houver mais clientes, sinaliza para os brinquedos encerrarem
    if (total_clientes == 0) {
        // Liberando cada um dos brinquedos para finalizarem
        for (int i = 0; i < self->number_toys; i++) {
            pthread_mutex_lock(&brinquedos[i]->mtx_cond);
            pthread_cond_signal(&brinquedos[i]->cond);
            pthread_mutex_unlock(&brinquedos[i]->mtx_cond);
        }
    }
    pthread_mutex_unlock(&mtx_people);
    debug("[CLIENT] - Cliente %d saiu do parque\n", self->id);
    pthread_exit(NULL);
}

// Essa função o cliente espera um brinquedo liberar e brinca nele
void enjoy_toy(client_t *self) {
    // Pega um brinquedo aleatório e vai se divertir
    int toy_id = rand() % self->number_toys + 1; 
    toy_t *toy = brinquedos[toy_id - 1];
    
    sem_wait(&toy->sem_entrar); // Espera entrar no brinquedo
    debug("[ENJOYING] - Cliente %d entrou no brinquedo %d\n", self->id, toy_id); // Debug

    // Entrega uma moeda
    self->coins--;

    pthread_mutex_lock(&toy->mtx_clients); // Bloqueia o contador de clientes no brinquedo
    toy->clients++; // Incrementa o contador de clientes no brinquedo
    pthread_mutex_unlock(&toy->mtx_clients); // Libera o contador de clientes no brinquedo
    
    // Sinaliza para iniciar o brinquedo
    pthread_mutex_lock(&toy->mtx_cond);
    pthread_cond_signal(&toy->cond);
    pthread_mutex_unlock(&toy->mtx_cond);
    
    // Espera o brinquedo terminar
    sem_wait(&toy->sem_sair); 
    debug("[ENJOYED] - Cliente %d saiu do brinquedo %d\n", self->id, toy_id); // Debug
}


// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    // Espera até sua vez de comprar moedas
    sem_wait(&sem_buy_coins);
    self->coins = rand() % MAX_COINS + 1;
}

void wait_ticket(client_t *self) {
    // Espera até que a bilheteria e os brinquedos estejam abertos
    pthread_mutex_lock(&sync_mutex);
    pthread_cond_wait(&sync_cond, &sync_mutex);
    pthread_mutex_unlock(&sync_mutex);

    // Se a bilheteria estiver aberta o cliente entra, espera sua vez de comprar
    buy_coins(self);
}


// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){
    // Garante que apenas um cliente entre por vez na fila
    pthread_mutex_lock(&mtx_enqueue);
    enqueue(gate_queue, self->id); 
    pthread_mutex_unlock(&mtx_enqueue);
    debug("[WAITING] - Cliente %d entrou na fila do portão principal\n", self->id); 
    
    // Após entrar na fila, o cliente vai paracomprar moedas
    wait_ticket(self);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    // Alocando memória para cada cliente e guardando o total de clientes
    total_clientes = args->n; 
    clientes = malloc(sizeof(client_t *) * total_clientes); 
    
    // Iniciando os clientes
    for(int i = 0; i < total_clientes; i++)
    {
        clientes[i] = args->clients[i];
        pthread_create(&clientes[i]->thread, NULL, enjoy, (void *)clientes[i]);
    }

}

// Essa função deve finalizar os clientes
void close_gate(){
    // Espera todos os clientes finalizarem
    for (int i = 0; i < total_clientes; i++)
    {
        pthread_join(clientes[i]->thread, NULL);
    }

    debug("[CLOSE] - Parque fechado\n");
    // Liberando memória
    free(clientes);
    pthread_mutex_destroy(&mtx_enqueue);
    pthread_mutex_destroy(&mtx_people);
}

