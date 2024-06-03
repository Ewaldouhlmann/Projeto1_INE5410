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
    client_t *self = (client_t *) arg;
    
    wait_ticket(self); // Espera a bilheteria abrir e obter os tickets


    // Enquanto o cliente tiver moedas ele pode brincar
    while(self->coins > 0){
        enjoy_toy(self);
    }

    pthread_mutex_lock(&mtx_people);
    total_clientes--; // Decrementa o total de clientes
    pthread_mutex_unlock(&mtx_people);
    pthread_exit(NULL);
}

void enjoy_toy(client_t *self) {
    int toy_id = rand() % self->number_toys + 1; // Id do brinquedo que o cliente vai brincar
    toy_t *toy = brinquedos[toy_id - 1]; // Indice do brinquedo é o id - 1

    sem_wait(&toy->sem_entrar); // Espera entrar no brinquedo
    
    
    pthread_mutex_lock(&toy->mtx_clients); // Bloqueia o acesso de outros clientes ao brinquedo
    debug("[TOY] - Cliente [%d] entrou no brinquedo [%d]\n", self->id, toy_id);
    toy->clients++; // Incrementa o número de clientes no brinquedo
    pthread_mutex_unlock(&toy->mtx_clients); // Libera o acesso de outros clientes ao brinquedo
    
    
    self->coins--; // Decrementa a quantidade de moedas do cliente
    sem_wait(&toy->sem_sair); // Espera sair do brinquedo
    debug("[TOY] - Cliente [%d] saiu do brinquedo [%d]\n", self->id, toy_id);

}
// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    // Se o cliente não for o primeiro da fila ele espera
    sem_wait(&sem_buy_coins); // Sinalizando que deseja comprar moedas
    self->coins = rand() % MAX_COINS + 1;
    debug("[CASH] - Cliente [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    // Se a bilheteria estiver aberta o cliente entra na fila
    while(!bilheteria_aberta){
        sleep(1);
    }
    queue_enter(self);
}


// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){
    // Garante que apenas um cliente entre por vez na fila
    pthread_mutex_lock(&mtx_enqueue);
    debug("[WAITING] - Cliente [%d] entrou na fila da bilheteria\n",self->id);
    enqueue(gate_queue, self->id); 
    pthread_mutex_unlock(&mtx_enqueue);
    
    // Após entrar na fila, o cliente vai paracomprar moedas
    buy_coins(self);
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

    // Liberando memória
    free(clientes);
}

