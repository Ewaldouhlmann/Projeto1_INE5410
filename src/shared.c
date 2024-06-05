#include "shared.h"

#include <queue.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui

int total_clientes = 0; // Total de clientes
int total_clientes_const = 0; // Total de clientes
int total_brinquedos = 0; // Total de brinquedos
int total_func = 0; // Total de funcionarios
int bilheteria_aberta = 0;  // Se a bilheteria está aberta
int brinquedos_abertos = 0; // Quantos brinquedos estão abertos
int sync_count = 0; // Contador para sincronização
int sync_fechamento = 0; // sincronizar o fechamento do parque

pthread_mutex_t sync_mutex; // Mutex para avisar que a bilheteria e os brinquedos estão abertos
pthread_cond_t sync_cond;   // Variável de condição para avisar que a bilheteria e os brinquedos estão abertos

toy_t **brinquedos = NULL; // Array de brinquedos
ticket_t **funcionarios = NULL; // Array de funcionarios

sem_t sem_buy_coins; // Semaforo para acesso a bilheteria

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;