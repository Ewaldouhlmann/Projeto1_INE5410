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

// Variáveis do arquivo
static client_t **clientes; // Array de clientes 
static pthread_mutex_t mtx_enqueue = PTHREAD_MUTEX_INITIALIZER; // Mutex para a fila
static pthread_mutex_t mtx_people = PTHREAD_MUTEX_INITIALIZER; // Mutex para controlar as threads funcionarios

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){
    // Recupera os argumentos do cliente
    client_t *self = (client_t *) arg;

    // Entra na fila da bilheteria
    queue_enter(self); 
    
    // Entra no parque e começa a brincar até ficar sem moedas
    debug("[CLIENT] - Cliente %d entrou no parque\n", self->id); // Debug
    while(self->coins > 0){
        enjoy_toy(self);
    }
    // Sai do parque, garantindo que apenas um cliente decremente do total de clientes
    pthread_mutex_lock(&mtx_people);
    total_clientes--; 
    pthread_mutex_unlock(&mtx_people);
    debug("[OUT] - Cliente %d saiu do parque\n", self->id);
    pthread_exit(NULL);
}


void enjoy_toy(client_t *self) {
    // Pega um brinquedo aleatório
    int toy_id = rand() % self->number_toys + 1; // Id do brinquedo aleatório
    toy_t *toy = brinquedos[toy_id - 1]; // Pega o brinquedo no array de brinquedos(id - 1)
    
    // Sinaliza que deseja entrar no brinquedo
    sem_wait(&toy->sem_entrar); 
    debug("[ENTER] - Cliente %d entrou no brinquedo %d\n", self->id, toy_id);

    // Cliente entra no brinquedo e da uma moeda
    pthread_mutex_lock(&toy->mtx_clients); 
    toy->clients++; 
    pthread_mutex_unlock(&toy->mtx_clients); 
    self->coins--; 

    // Espera a finalização do brinquedo
    sem_wait(&toy->sem_sair);
    debug("[OUT] - Cliente [%d] saiu do brinquedo [%d]\n", self->id, toy_id);
}


// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){

    // Espera até sua vez de comprar as moedas
    sem_wait(&sem_buy_coins);
    self->coins = rand() % MAX_COINS + 1;
    debug("[CASH] - Cliente [%d] comprou [%d] moedas.\n", self->id, self->coins);
}


void wait_ticket(client_t *self) {
    // Espera até que a bilheteria e os brinquedos estejam abertos
    pthread_mutex_lock(&sync_mutex);
    while (!bilheteria_aberta || !brinquedos_abertos) {
        pthread_cond_wait(&sync_cond, &sync_mutex);
    }
    pthread_mutex_unlock(&sync_mutex);

    // Vai realizar a compra de moedas
    buy_coins(self);
}


// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){
    // Garante que apenas um cliente entre por vez na fila
    pthread_mutex_lock(&mtx_enqueue);
    enqueue(gate_queue, self->id); 
    pthread_mutex_unlock(&mtx_enqueue);
    debug("[WAITING] - Cliente [%d] entrou na fila do portao principal\n", self->id); 
    
    // Espera a inicialização da bilheteria
    wait_ticket(self);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    // Alocando memória para cada cliente e guardando o total de clientes
    total_clientes = args->n; 
    clientes = malloc(sizeof(client_t *) * total_clientes); 
    
    // Criando as threads dos clientes
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

    // Liberando memória
    free(clientes);
    pthread_mutex_destroy(&mtx_enqueue);
    pthread_mutex_destroy(&mtx_people);
}
