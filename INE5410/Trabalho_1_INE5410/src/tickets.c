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
    // Recupera o funcionário
    ticket_t *funcionario = (ticket_t *) args;  
    
    // Sincroniza com os brinquedos
    pthread_mutex_lock(&sync_mutex);
    sync_count ++;
    if (sync_count >= total_brinquedos + total_func) {
        pthread_cond_broadcast(&sync_cond);
    }
    pthread_mutex_unlock(&sync_mutex);

    // Inicia a bilheteria
    debug("[INFO] - Funcionário %d iniciou\n", funcionario->id); 
    while (1) { 
         // Verifica se todos os clientes já entraram e sai do loop, caso sim
        pthread_mutex_lock(&mtx_clientes_entraram);
        if (clientes_entraram == total_clientes)
        {
            pthread_mutex_unlock(&mtx_clientes_entraram);
            break;
        }
        pthread_mutex_unlock(&mtx_clientes_entraram);
        
        // Bloqueia o dequeue para controlar a fila
        pthread_mutex_lock(&mtx_dequeue); 
        // Se a fila estiver vazia, libera o mutex e continua
        if (is_queue_empty(gate_queue)) 
        {
            pthread_mutex_unlock(&mtx_dequeue);
            continue;
        }
        else
        {
            // Se a fila não estiver vazia, atende o cliente
            int cliente = dequeue(gate_queue);
            
            // Atende o cliente 
            debug("[INFO] - Funcionário %d atendendo cliente %d\n", funcionario->id, cliente);
            sleep(rand()%3 + 1); // Tempo de atendimento aleatório

            // Libera o semáforo para próximo cliente
            sem_post(&sem_buy_coins);

            // Adiciona ao contador de clientes que entraram
            pthread_mutex_lock(&mtx_clientes_entraram);
            clientes_entraram++;
            pthread_mutex_unlock(&mtx_clientes_entraram);

            // Libera o mutex de dequeue
            pthread_mutex_unlock(&mtx_dequeue);
        }
        // Descansa um pouco para outras threads terem a chance de atender
        sleep(3); 
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

    // Inicializa as threads dos funcionários
    for(int i = 0; i < total_func; i++)
    {
        funcionarios[i] = args->tickets[i]; 
        pthread_create(&funcionarios[i]->thread, NULL, sell, (void *)funcionarios[i]);
    }
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < total_func; i++) {
        pthread_join(funcionarios[i]->thread, NULL);
    }

    // Liberando memória
    free(funcionarios);
    
    // Destruindo o semáforo, pois todos os clientes já compraram os tickets
    sem_destroy(&sem_buy_coins);

    // Destruindo os mutexes utilizados na fila e na contagem de clientes
    pthread_mutex_destroy(&mtx_clientes_entraram);
    pthread_mutex_destroy(&mtx_dequeue);

    // Destruindo o mutex e cond de sincronização, como só é necessário até os clientes entrarem, pode ser destruído aqui
    pthread_mutex_destroy(&sync_mutex);
    pthread_cond_destroy(&sync_cond);
}