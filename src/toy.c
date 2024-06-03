/**
 * Esse arquivo tem como objetivo a implementação de um brinquedo em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "toy.h"
#include "shared.h"


// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args){
    toy_t *toy = (toy_t *) args;

    debug("[ON] - O brinquedo  [%d] foi ligado.\n", toy->id); 

    debug("[OFF] - O brinquedo [%d] foi desligado.\n", toy->id);

    pthread_exit(NULL);
}


// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args){
    total_brinquedos = args->n;   // recupera o total de 
    brinquedos = malloc(sizeof(toy_t *) * total_brinquedos);

    for(int i = 0; i < total_brinquedos; i++)
    {
        brinquedos[i] = args->toys[i];
        pthread_create(&brinquedos[i]->thread, NULL, turn_on, (void *)brinquedos[i]);
    }

}

// Desligando os brinquedos
void close_toys(){
    for (int i = 0; i < total_brinquedos; i++)
    {
        pthread_join(brinquedos[i]->thread, NULL);
    }

    // Liberando da memória
    free(brinquedos);
}