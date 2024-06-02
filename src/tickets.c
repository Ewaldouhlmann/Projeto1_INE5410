/**
 * Esse arquivo tem como objetivo a implementação da bilheteria em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <tickets.h>
#include <unistd.h>

#include <queue.h>
#include "shared.h"


// Thread que implementa uma bilheteria
void *sell(void *args){
    ticket_t *funcionario = (ticket_t *) args;
    debug("[INFO] - Funcionário %d iniciou\n", funcionario->id);
    debug("[INFO] - Funcionário %d esperando clientes\n", funcionario->id);
    sem_post(&sem_buy_coins);

    
    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){
    total_func = args->n;
    funcionarios = malloc(sizeof(ticket_t *) * total_func);

    // Inicializa o semáforo
    sem_init(&sem_buy_coins, 0, 0);


    /*Se o número de funcionários for maior que o número de clientes,  
    somente será necessário que tenha um funcionário *por cliente*/
    if (total_clientes < total_func) 
    {
        total_func = total_clientes;
    }

    for(int i = 0; i < total_func; i++)
    {
        //recupera as threads e adicionando no array de threads funcionarios
        funcionarios[i] = args->tickets[i]; 
        // criação das thread 
        pthread_create(&funcionarios[i]->thread, NULL, sell, (void *)funcionarios[i]);
    }
    
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < total_func; i++) {
        pthread_join(funcionarios[i]->thread, NULL);
    }
}