#include "shared.h"

#include <queue.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui

int total_clientes = 0; // Total de clientes
int total_brinquedos = 0; // Total de brinquedos
int total_func = 0; // Total de funcionarios
int clientes_entraram = 0; // Quantos clientes entraram no parque
int bilheteria_aberta = 0;  // Se a bilheteria está aberta
int brinquedos_abertos = 0; // Quantos brinquedos estão abertos
int sync_count = 0; // Contador para sincronização

pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;


toy_t **brinquedos = NULL; // Array de brinquedos
ticket_t **funcionarios = NULL; // Array de funcionarios
client_t **clientes = NULL; // Array de clientes


pthread_mutex_t sync_mutex;
pthread_cond_t sync_cond;

pthread_mutex_t mtx_enqueue = PTHREAD_MUTEX_INITIALIZER; // Mutex para o portão principal
pthread_mutex_t mtx_dequeue = PTHREAD_MUTEX_INITIALIZER; // Mutex para a bilheteria
pthread_mutex_t mtx_bilheteria_iniciada = PTHREAD_MUTEX_INITIALIZER; // Mutex para a bilheteria
pthread_mutex_t mtx_clientes_entraram = PTHREAD_MUTEX_INITIALIZER; // Mutex para a bilheteria
pthread_mutex_t mtx_people = PTHREAD_MUTEX_INITIALIZER; // Mutex para controlar as threads funcionarios

sem_t sem_buy_coins; // Semaforo para a bilheteria
sem_t sem_func_control;  // Semaforo para controlar as threads funcionarios

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;