/**
 * Esse arquivo tem como objetivo a implementação do cliente em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>

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
    client_t *self = (client_t *) arg;

    queue_enter(self); // Entra na fila da bilheteria e espera comprar as moedas
    
    debug("Moedas do cliente %d - %d\n", self->id, self->coins);
    while (self->coins > 0)
    {
        enjoy_toy(self); // Brinca
    }

    pthread_exit(NULL);
}

void enjoy_toy(client_t *self)
{
    debug("[INFO] - Cliente esta brincando!\n");
    self->coins--;
}

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    wait_ticket(self); // Espera até ser o primeiro da fila
    
    // Após virar o primeiro da fila, ele sinaliza que está esperando
    sem_wait(&sem_buy_coins);
    self->coins = rand() % MAX_COINS + 1;
    debug("[INFO] - Cliente [%d] comprou %d moedas\n", self->id, self->coins);
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    // Espera ser o primeiro da fila
    while (1)
    {
        pthread_mutex_lock(&mtx_dequeue);
        if (self->id == gate_queue->front->data)
        {
            pthread_mutex_unlock(&mtx_dequeue);
            break;
        }
        pthread_mutex_unlock(&mtx_dequeue);
    }
}   


// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){

    pthread_mutex_lock(&mtx_enqueue);
    enqueue(gate_queue, self->id);
    pthread_mutex_unlock(&mtx_enqueue);
    buy_coins(self);

    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);

}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    // Alocando memória para cada cliente e guardando o total de clientes
    total_clientes = args->n; 
    clientes = malloc(sizeof(client_t *) * total_clientes); 
    

    for(int i = 0; i < total_clientes; i++)
    {
        clientes[i] = args->clients[i];
        pthread_create(&clientes[i]->thread, NULL, enjoy, (void *)clientes[i]);
    }
}

// Essa função deve finalizar os clientes
void close_gate(){
    for (int i = 0; i < total_clientes; i++)
    {
        pthread_join(clientes[i]->thread, NULL);
    }
}
