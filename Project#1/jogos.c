/*****************************************************************/
/*         Trabalho pratico 1 | PROG2 | MIEEC | 2018/19          */
/*****************************************************************/
/*                    FUNCOES A IMPLEMENTAR                      */
/*****************************************************************/

#include "jogos.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


vetor* jogos_load(const char *nomef){
    
    vetor* V;
    jogo aux;
    
    V = vetor_novo();
    FILE *f= fopen(nomef, "r");
    
    if( f == NULL || V == NULL){    
        return NULL;
    }
    
    while( fscanf( f, "%s %s %s %d %d %d %d", aux.epoca, aux.nome_casa, aux.nome_fora, &aux.golos_casa, &aux.golos_fora, &aux.vermelhos_casa, &aux.vermelhos_fora) == 7){
        vetor_insere( V, aux, -1 );
    }
    
    fclose(f);
    return V;
    
}


int jogos_save(vetor *vec, const char *nomef){
    
    int i;
    int tamanho = vetor_tamanho(vec);
    jogo* aux;
    
    FILE *f= fopen(nomef, "w");

    if( f == NULL || vec == NULL ){                       
        return -1;
    }

    for(i=0; i<tamanho; i++){
        aux=vetor_elemento( vec, i);
        fprintf( f, "%s %s %s %d %d %d %d\n", aux->epoca, aux->nome_casa, aux->nome_fora, aux->golos_casa, aux->golos_fora, aux->vermelhos_casa, aux->vermelhos_fora);
    }

    fclose(f);
    return i;
}


vetor_equipas *stats_equipa(vetor *vec){
    int i;
    int j;
    int n=0;
    int ano=-1;
    int tamanho = vetor_tamanho(vec);

    
    vetor_equipas* V;
    V = vetor_equipas_novo();

    equipa aux[50];

    if( V == NULL){    
        return NULL;
    }

    for(i=0; i<tamanho; i++){
                                                                            
        if( strcmp(vec->elementos[i].epoca, "15/16")==0 ){
            ano=0;
            
        }
        if( strcmp(vec->elementos[i].epoca, "16/17")==0 ){
            ano=1;
            
        }
        if( strcmp(vec->elementos[i].epoca, "17/18")==0 ){
            ano=2;

        }
                                    //O método utilizado para verificar se a equipa já existe incrementar e
                                    //se não existe acrescentar foi adaptado de um exercício abordado em programação 1                        
        for(j=0; j<n; j++){                                                         //Equipa que está a jogar em casa
            if(strcmp(vec->elementos[i].nome_casa, aux[j].nome_equipa)==0){
                break;
            }
        }
        if(j==n){
            strcpy(aux[j].nome_equipa, vec->elementos[i].nome_casa);
            aux[j].diff_golos = vec->elementos[i].golos_casa - vec->elementos[i].golos_fora;
            aux[j].vermelhos[ano] = vec->elementos[i].vermelhos_casa;
            n++;
        }
        else{
            aux[j].diff_golos += vec->elementos[i].golos_casa - vec->elementos[i].golos_fora;
            aux[j].vermelhos[ano] += vec->elementos[i].vermelhos_casa;
        }



        for(j=0; j<n; j++){                                                       //Equipa que está a jogar fora
            if(strcmp(vec->elementos[i].nome_fora, aux[j].nome_equipa)==0){
                break;
            }
        }
        if(j==n){
            strcpy(aux[j].nome_equipa, vec->elementos[i].nome_fora);
            aux[j].diff_golos = vec->elementos[i].golos_fora - vec->elementos[i].golos_casa;
            aux[j].vermelhos[ano] = vec->elementos[i].vermelhos_fora;
            n++;
        }
        else{
            aux[j].diff_golos += vec->elementos[i].golos_fora - vec->elementos[i].golos_casa;
            aux[j].vermelhos[ano] += vec->elementos[i].vermelhos_fora;
        }

       
    }
    for(ano=0; ano<3; ano++){
        for(i=0; i<n; i++){
            aux[i].vermelhos[ano]= aux[i].vermelhos[ano] / 38;
        }
    }
    for(i=0; i<n; i++){
        vetor_equipas_insere(V, aux[i], i);
    }

    return V;

}

//INICIO DAS FUNCOES DE ORDENACAO

void swapE(equipa *a, equipa *b){
    equipa c= *a;
    *a = *b;
    *b=c;
    
}

int compara_equipas(equipa a, equipa b, int criterio){
    if( criterio == 0 ){
        if( strcmp( a.nome_equipa, b.nome_equipa) <= 0 ) return 1;
        if( strcmp( a.nome_equipa, b.nome_equipa) > 0 ) return 0;
    }
    if( criterio ==1 ){
        if( a.diff_golos <= b.diff_golos ) return 1;
        else return 0;
    }
    return 0;
}

void quickSortIter(vetor_equipas *v, int left, int right, int criterio) {
    int i, j, tamanho = right-left+1;
    if(tamanho<2) /* com tamanho 0 ou 1 esta ordenado */
        return;
    else {
        int pos = rand()%tamanho + left; /* determina pivot */
        swapE(&(v->elementos[pos]), &(v->elementos[right])); /* coloca pivot no fim */
        i = left; j = right-1; /* passo de partição */
        while(1) {
            while (i < right && compara_equipas(v->elementos[i], v->elementos[right], criterio) ) i++;
            while (j >= 0 && compara_equipas(v->elementos[right], v->elementos[j], criterio) ) j--;
            if (i < j) swapE(&(v->elementos[i]), &(v->elementos[j]));
                else break;
        }
        swapE(&(v->elementos[i]), &(v->elementos[right])); /* repoe pivot */
        quickSortIter(v, left, i-1, criterio);
        quickSortIter(v, i+1, right, criterio);
    }
}


int equipas_ordena(vetor_equipas *v, int criterio){
    int tamanho=vetor_equipas_tamanho(v);
    if( v == NULL || (criterio!=1 && criterio!=0) ){
        return -1;
    }
    quickSortIter(v, 0, tamanho-1, criterio);

    return 0;
}


// FIM DAS FUNCOES PARA ORDENACAO

int corrige_jogo(vetor *vec, const char *nomef){
    int tamanho = vetor_tamanho(vec);
    FILE *f;
    f = fopen (nomef, "r");
    if( f == NULL || vec == NULL ){                        
        return -1;
    }
    
    jogo aux;
    while (fscanf(f, "%s %s %s %d %d %d %d", aux.epoca, aux.nome_casa, aux.nome_fora, &aux.golos_casa, &aux.golos_fora, &aux.vermelhos_casa, &aux.vermelhos_fora) == 7){
        for(int i=0 ; i<tamanho ; i++){
            if(( strcmp(aux.epoca , vec->elementos[i].epoca) == 0 ) && ( strcmp(aux.nome_casa , vec->elementos[i].nome_casa) == 0 ) && ( strcmp(aux.nome_fora , vec->elementos[i].nome_fora) == 0 ) ){
               
               vetor_atribui(vec, i, aux);
            }
        }
    }
    
    fclose(f);
    return 0;
}


int pontos_de_equipa(vetor *vec, char *nome_equipa, char *epoca){
    int tamanho = vetor_tamanho(vec);
    int pontos = 0;
    int aux1 = 0;
    int aux2 = 0;
    int i;
    if(vec == NULL){
        return -1;
    }
    for(i=0; i<tamanho; i++){                           //Verificar se nome_equipa e epoca existem em vec
        if( (strcmp(vec->elementos[i].nome_casa, nome_equipa) == 0) || (strcmp(vec->elementos[i].nome_fora, nome_equipa) == 0) ){
            aux1=1;
        }
        if( strcmp(vec->elementos[i].epoca, epoca) == 0 ){
            aux2=1;
        }
    }
    if(aux1!=1 || aux2!=1){
        return -1;
    } 

    for(int i=0;i<tamanho;i++)
    {
        if(strcmp(vec->elementos[i].epoca, epoca) == 0){
            if( strcmp(vec->elementos[i].nome_casa , nome_equipa) == 0 ){
                if(vec->elementos[i].golos_casa > vec->elementos[i].golos_fora)
                pontos = pontos + 3;
                if(vec->elementos[i].golos_casa == vec->elementos[i].golos_fora)
                pontos = pontos + 1;
            }
            if( strcmp( vec->elementos[i].nome_fora , nome_equipa ) == 0 ){
                if(vec->elementos[i].golos_fora > vec->elementos[i].golos_casa)
                pontos = pontos + 3;
                if(vec->elementos[i].golos_fora == vec->elementos[i].golos_casa)
                pontos = pontos + 1;
            }

        }
    }
    return pontos;
}






