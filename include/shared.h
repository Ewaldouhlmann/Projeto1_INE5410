#ifndef __SHARED_H__
#define __SHARED_H__

#include <defs.h>
#include <pthread.h>
// Você pode declarar novas funções (ou variaveis compartilhadas) aqui
extern int total_clientes;  // Total de clientes
extern int total_brinquedos;    // Total de brinquedos
extern int total_func;       // Total de funcionarios
extern int clientes_entraram; // Quantos clientes entraram no parque
extern int bilheteria_aberta; 
extern int brinquedos_abertos;

extern pthread_mutex_t sync_mutex;
extern pthread_cond_t sync_cond;
extern int sync_count;

extern toy_t **brinquedos;  // Array de brinquedos
extern ticket_t **funcionarios; // Array de funcionarios
extern client_t **clientes; // Array de clientes

extern pthread_mutex_t mtx_enqueue; // Mutex para o portão principal
extern pthread_mutex_t mtx_dequeue; // Mutex para a bilheteria
extern pthread_mutex_t mtx_bilheteria_iniciada; // Mutex para a bilheteria
extern pthread_mutex_t mtx_clientes_entraram; // Mutex para a bilheteria
extern pthread_mutex_t mtx_people; // Mutex para controlar as threads funcionarios

extern sem_t sem_buy_coins;
extern sem_t sem_func_control;
/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
extern Queue *gate_queue;

#endif
