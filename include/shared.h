#ifndef __SHARED_H__
#define __SHARED_H__

#include <defs.h>
#include <pthread.h>
// Você pode declarar novas funções (ou variaveis compartilhadas) aqui
extern int total_clientes;  // Total de clientes
extern int total_clientes_const;  // Total de clientes
extern int total_brinquedos;    // Total de brinquedos
extern int total_func;       // Total de funcionarios
extern int bilheteria_aberta; // Flag que indica se a bilheteria está aberta
extern int brinquedos_abertos;  // Flag que indica se os brinquedos estão abertos
extern int sync_count; // Contador para sincronização
extern int sync_fechamento; // sincronizar o fechamento do parque

extern pthread_mutex_t sync_mutex; // Mutex para avisar que a bilheteria e os brinquedos estão abertos
extern pthread_cond_t sync_cond;    // Variável de condição para avisar que a bilheteria e os brinquedos estão abertos

extern toy_t **brinquedos;  // Array de brinquedos
extern ticket_t **funcionarios; // Array de funcionarios

extern sem_t sem_buy_coins; // Semaforo para a bilheteria
/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
extern Queue *gate_queue;

#endif
