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
    ticket_t *funcionario = (ticket_t *) args;  // recupera os argumentos do funcionario
    debug("[INFO] - Funcionário %d iniciou\n", funcionario->id);  // debug
    //debug("[INFO] - Funcionário %d iniciou\n", funcionario->id);  // debug
    while (1) {  // enquanto a fila não estiver vazia
        pthread_mutex_lock(&mtx_clientes_entraram);
        if (clientes_entraram == total_clientes){
            pthread_mutex_unlock(&mtx_clientes_entraram);
            break;
        }
        pthread_mutex_unlock(&mtx_clientes_entraram);
        pthread_mutex_lock(&mtx_dequeue);  // mutex para controlar a fila
        int cliente = dequeue(gate_queue);  // desenfileirar o cliente
        if (cliente != -1) // Se a fila é diferente de -1
        {
            sem_post(&sem_buy_coins);  // liberar para os clientes comprarem os tickets
            //debug("[SELL] Funcinário %d atendeu o cliente %d\n", funcionario->id, cliente);
            clientes_entraram++;
        }
        pthread_mutex_unlock(&mtx_dequeue);  //fim do mutex para controlar a fila
    }
    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){
    total_func = args->n;
    bilheteria_aberta = 0;
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
    // Sincroniza com os brinquedos
    pthread_mutex_lock(&sync_mutex);
    sync_count += total_func;
    if (sync_count >= total_brinquedos + total_func) {
        bilheteria_aberta = 1;
        pthread_cond_broadcast(&sync_cond);
    }
    pthread_mutex_unlock(&sync_mutex);

}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < total_func; i++) {
        pthread_join(funcionarios[i]->thread, NULL);
    }

    // Liberando memória
    free(funcionarios);
    sem_destroy(&sem_buy_coins);
    pthread_mutex_destroy(&mtx_clientes_entraram);
}