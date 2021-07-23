/*****************************************************************/
/*           Movies | PROG2 | MIEEC | 2018/19                */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "movies.h"



//Declaracao funcoes auxiliares//

// -------------------------------------------------------------- HEAP   -----------------------------------------------------------------------
#define RAIZ 		(1)
#define PAI(x) 		(x/2)
#define FILHO_ESQ(x) 	(x*2)
#define FILHO_DIR(x) 	(x*2+1)

int maior_que(filme * e1, filme * e2);
int avl_altura(no_avl *no);
int calc_balanceamento(no_avl *N);
no_avl* roda_direita(no_avl *y);
no_avl* roda_esquerda(no_avl *x);
no_avl* avl_no_valormin(no_avl* no);
no_avl* avl_no_valormax(no_avl* no);
int max(int a, int b);
int min(int a, int b);
void avlNoApaga(no_avl* node);
void avlNoCopia(no_avl* nodeDest, no_avl* nodeSource);
void avlNoValorCopia(no_avl* nodeDest, no_avl* nodeSrc);
vetor_filmes* vfilmes_novo(int capacidade);
void vfilmes_apaga(vetor_filmes *v);
int vfilmes_insere(vetor_filmes * v, char* titulo, char* categoria, unsigned int filmId, float rating);


int maior_que(filme * e1, filme * e2)
{
	if (e1 == NULL || e2 == NULL)
	{
		return 0;
	}
    
    if(e1->rating == e2->rating) return e1->filmId > e2->filmId;

	return e1->rating > e2->rating;
}

heap* heap_nova(int capacidade)
{
	heap * h = (heap *) malloc(sizeof(heap));

	if (!h)
		return NULL;

	h->tamanho = 0;
	h->capacidade = capacidade;
	h->elementos = (filme **) calloc(capacidade+1, sizeof(filme *));

	if (!h->elementos) {
		free(h);
		return NULL;
	}

	return h;
}

void heap_apaga(heap *h)
{
	int i;

	if(!h)
		return;

	/* apaga todos os elementos e respetivas strings */
	for(i=RAIZ; i<=h->tamanho; i++)
	{
		free(h->elementos[i]->titulo);
        free(h->elementos[i]->categoria);
		free(h->elementos[i]);
		h->elementos[i]=NULL;
	}

	free(h->elementos);
	free(h);
}

filme * elemento_novo(char* titulo, char* categoria, unsigned int filmId, float rating)
{
	filme * elem = (filme *) malloc(sizeof(filme));

	if (!elem)
		return NULL;

	elem->titulo = (char*)calloc(strlen(titulo)+1, sizeof(char));
    elem->categoria = (char*)calloc(strlen(categoria)+1, sizeof(char));

	if (!elem->titulo || !elem->categoria)
	{
		free(elem);
		return NULL;
	}

	strcpy(elem->titulo, titulo);
    strcpy(elem->categoria, categoria);
	elem->filmId = filmId;
    elem->rating = rating;

	return elem;
}

int heap_insere(heap * h, filme* elem)
{
	filme * aux;
	int i;

	/* se heap esta' cheia, aumenta capacidade */
	if (h->tamanho >= h->capacidade){
		if(h->capacidade == 0)
		h->capacidade = 1;
		else
		h->capacidade *= 2;

		h->elementos = (filme**)realloc(h->elementos, (h->capacidade+1) * sizeof(filme*));
		if(h->elementos == NULL)
		return -1;
    }


	if (!elem)
		return 0;

	/* coloca elemento no fim da heap */
	h->tamanho++;
	i = h->tamanho;
	h->elementos[i] = elem;

 	/* enquanto elemento for mais prioritario do que o respetivo pai, troca-os */
	while (i != RAIZ && maior_que(h->elementos[i], h->elementos[PAI(i)]))
	{
		aux = h->elementos[PAI(i)];
		h->elementos[PAI(i)] = h->elementos[i];
		h->elementos[i] = aux;
		i = PAI(i);
	}
	return 1;
}

filme * heap_remove(heap * h)
{                         
	int i, filho_maior;
	filme * aux;
	filme * ret = &(*h->elementos[RAIZ]);

	/* se heap estiver vazia, nao remove elemento */
	if (!h || h->tamanho <= 0)
		return NULL;

	//Bloco de código causa erro no valgrind pois dá free ao elemento que é suposto ser retornado
    /*ret = h->elementos[RAIZ];
	free(h->elementos[RAIZ]);*/


	/* coloca ultimo elemento da heap na raiz */
	h->elementos[RAIZ] = h->elementos[h->tamanho];
	h->elementos[h->tamanho] = NULL;
	h->tamanho--;

 	i = RAIZ;

	/* enquanto nao chegar 'a base da heap */
	while(FILHO_ESQ(i) <= h->tamanho)
	{
		filho_maior = FILHO_ESQ(i);

		/* verifica se existe filho 'a direita e se este e' mais prioritario do que 'a esquerda */
		if (FILHO_DIR(i) <= h->tamanho && maior_que(h->elementos[FILHO_DIR(i)], h->elementos[FILHO_ESQ(i)]))
			filho_maior = FILHO_DIR(i);

		/* enquanto elemento for mais prioritario do que o respetivo pai, troca-os */
		if (maior_que(h->elementos[filho_maior], h->elementos[i]))
		{
			aux = h->elementos[filho_maior];
			h->elementos[filho_maior] = h->elementos[i];
			h->elementos[i] = aux;
			i = filho_maior;
		}
		else
			break;
	}

	return ret;
}

void mostraHeap(heap *h)
{
    if(!h)
        return;

    for(int indice =1; indice<=(h->tamanho); indice++)
    {
        printf("%s %s %u %f\n",h->elementos[indice]->titulo, h->elementos[indice]->categoria,
                        h->elementos[indice]->filmId,h->elementos[indice]->rating);
    }
}
//--------------------------------------------------------------- ARVORE -----------------------------------------------------------------------

category_t* novaCategoria(heap* itemTree, char* categName)
{
    category_t* retCat = (category_t*)malloc(sizeof(category_t));

    if(retCat == NULL)
    {
        return NULL;
    }

    retCat->itemTree = itemTree;

    retCat->categName = (char*)malloc((strlen(categName)+1)*sizeof(char));

    if(retCat->categName==NULL)
    {
        return NULL;
    }

    strcpy(retCat->categName,categName);

    return retCat;

    //Default
    return NULL;
}

void categoriaApaga(category_t* categ)
{
    if(categ==NULL)
        return;
    
    heap_apaga(categ->itemTree);
    free(categ->categName);
    categ->categName = NULL;
    free(categ);
    categ = NULL;
}

colecaoFilmes* avl_nova()
{
    // Implementacao exercicio 5.4.1
    colecaoFilmes *avl = (colecaoFilmes*) malloc(sizeof(colecaoFilmes));
    if(avl == NULL)
        return NULL;

    avl->raiz = NULL; /* arvore vazia */
    avl->vec = vfilmes_novo(50);
    return avl;

    // Default
    return NULL;
}

no_avl* avl_novo_no(category_t* categ)
{
    // Implementacao exercicio 5.4.2
    no_avl *no = (no_avl*) malloc(sizeof(no_avl));

    if(no==NULL)
        return NULL;

    no->categ = categ;
    no->esquerda = NULL;
    no->direita  = NULL;
    no->altura = 0;  /* novo no e' inicialmente uma folha */
    return no;

    // Default
    return NULL;
}

no_avl* avl_insere(no_avl *no, category_t* categ)
{
    //Código adaptado do ficheiro auxiliar para implementação de árvores avl disponível no MOODLE
    
    /* 1.  efetua insercao normal de arvore binaria de pesquisa */
    if (no == NULL)
        return avl_novo_no(categ);

    if (strcmp(categ->categName, no->categ->categName) < 0)
        no->esquerda  = avl_insere(no->esquerda, categ);
    else if(strcmp(categ->categName, no->categ->categName) > 0)
        no->direita = avl_insere(no->direita, categ);
    else {
        return no;
    }

    /* 2. atualiza a altura deste no ancestral */
    no->altura = max(avl_altura(no->esquerda), avl_altura(no->direita)) + 1;

    /* 3. calcula o fator de balanceamento deste no ancestral para verificar
       se deixou de estar balanceado */
    int balance = calc_balanceamento(no);

    /* se o no deixou de estar balanceado, existem 4 casos */

    if (balance > 1) {
        /* Arvore e' right-heavy */
    	if (calc_balanceamento(no->direita) < 0) {
    		/* Sub-arvore direita é left-heavy */
            /* Rotacao RL */
			no->direita = roda_direita(no->direita);
			return roda_esquerda(no);
    	} else {
    		/* Rotacao L */
    		return roda_esquerda(no);
    	}
    }
    else if (balance < -1) {
        /* Arvore e' left-heavy */
    	if (calc_balanceamento(no->esquerda) > 0) {
            /* Sub-arvore esquerda é right-heavy */
    		/* Rotacao LR */
			no->esquerda = roda_esquerda(no->esquerda);
			return roda_direita(no);
    	} else {
    		/* Rotacao R */
    		return roda_direita(no);
    	}
    }
    /* caso esteja balanceada retorna o apontador para o no (inalterado) */
    return no;
}

no_avl* avl_remove(no_avl* no, const char *categStr)
{
    /* 1. efetua remocao normal de arvore binaria de pesquisa */

    if (no == NULL)
        return no;
    /* se a str a ser removida é menor do que a str da raiz,
       entao esta' na sub-arvore esquerda */
    if ( strcmp(categStr, no->categ->categName) < 0 )
        no->esquerda = avl_remove(no->esquerda, categStr);

    /* se a str a ser removida é maior do que a str da raiz,
       entao esta' na sub-arvore direita */
    else if( strcmp(categStr, no->categ->categName) > 0 )
        no->direita = avl_remove(no->direita, categStr);

    /* se a str a ser removida é igual a str da raiz,
       entao e' este no a remover */
    else
    {
        /* no' com apenas um filho ou sem filhos */
        if( (no->esquerda == NULL) || (no->direita == NULL) )
        {
            no_avl *temp = no->esquerda ? no->esquerda : no->direita;

            /* caso sem filhos */
            if(temp == NULL)
            {
                temp = no;
                no = NULL;
            }
            else /* caso de um filho */
            {
                /* copia os conteudos do filho que não está vazio */
                avlNoCopia(no,temp);
				//
            }
            avlNoApaga(temp);
        }
        else
        {
            /* no' com dois filhos: obtem sucessor em-ordem (menor da arvore direita) */
            no_avl* temp = avl_no_valormin(no->direita);

            /* copia o valor em.ordem do sucessor para este no' */
            avlNoValorCopia(no,temp);
            /* apaga o sucessor em-ordem */
            no->direita = avl_remove(no->direita, temp->categ->categName);
        }
    }
	
    /* se a arvore tinha apenas um no, então retorna */
    if (no == NULL)
      return no;
	
    /* 2. atualiza a altura do no corrente */
    no->altura = max(avl_altura(no->esquerda), avl_altura(no->direita)) + 1;

    /* 3. calcula o fator de balanceamento deste no ancestral para verificar
       se deixou de estar balanceado */
    int balance = calc_balanceamento(no);

    /* se o no deixou de estar balanceado, existem 4 casos */
    if (balance > 1) {
        /* Arvore e' right-heavy */
        if (calc_balanceamento(no->direita) < 0) {
            /* Sub-arvore direita é left-heavy */
            /* Rotacao RL */
            no->direita = roda_direita(no->direita);
            return roda_esquerda(no);
        } else {
            /* Rotacao L */
            return roda_esquerda(no);
        }
    }
    else if (balance < -1) {
        /* Arvore e' left-heavy */
        if (calc_balanceamento(no->esquerda) > 0) {
            /* Sub-arvore esquerda é right-heavy */
            /* Rotacao LR */
            no->esquerda = roda_esquerda(no->esquerda);
            return roda_direita(no);
        } else {
            /* Rotacao R */
            return roda_direita(no);
        }
    }
    /* caso esteja balanceada retorna o apontador para o no (inalterado) */
    return no;
}

no_avl* avl_pesquisa(no_avl *no, const char* categStr)
{
    //Código adaptado do ficheiro auxiliar para implementação de árvores avl disponível no MOODLE

    if(no == NULL || categStr == NULL)
        return NULL;

    if(strcmp(categStr, no->categ->categName) < 0)
        return avl_pesquisa(no->esquerda, categStr);

    else if(strcmp(categStr, no->categ->categName) > 0)
        return avl_pesquisa(no->direita, categStr);

    else
        return no;
}

void avl_apaga_i(no_avl *no)
{
	if (!no)
		return;
	avl_apaga_i(no->esquerda);
	avl_apaga_i(no->direita);
	avlNoApaga(no);
}

void avl_apaga(colecaoFilmes* avl)
{
	if (!avl) return;
	
	avl_apaga_i(avl->raiz);
	
/*    while(avl->raiz != NULL)
    {
        avl->raiz = avl_remove(avl->raiz, avl->raiz->categ->categName);
    }*/
    vfilmes_apaga(avl->vec);

    free(avl);
}

int avl_altura(no_avl *no)
{
    if (no == NULL)
        return -1;
    return no->altura;
}

int calc_balanceamento(no_avl *N)
{
    if (N == NULL)
        return 0;
    return avl_altura(N->direita) - avl_altura(N->esquerda);
}

no_avl* roda_direita(no_avl *y)
{
    no_avl *x = y->esquerda;
    no_avl *T2 = x->direita;

    /* efetua rotacao */
    x->direita = y;
    y->esquerda = T2;

    /* atualiza alturas */
    y->altura = max(avl_altura(y->esquerda), avl_altura(y->direita))+1;
    x->altura = max(avl_altura(x->esquerda), avl_altura(x->direita))+1;

    /* retorna novo no' */
    return x;
}

no_avl* roda_esquerda(no_avl *x)
{
    no_avl *y = x->direita;
    no_avl *T2 = y->esquerda;

    /* efetua rotacao */
    y->esquerda = x;
    x->direita = T2;

    /*  atualiza alturas */
    x->altura = max(avl_altura(x->esquerda), avl_altura(x->direita))+1;
    y->altura = max(avl_altura(y->esquerda), avl_altura(y->direita))+1;

    /* retorna novo no' */
    return y;
}

no_avl* avl_no_valormin(no_avl* no)
{
    no_avl* curr = no;

    /* percorre a arvore para encontrar o filho mais 'a esquerda */
    while (curr->esquerda != NULL)
        curr = curr->esquerda;

    return curr;
}

no_avl* avl_no_valormax(no_avl* no)
{
    no_avl* curr = no;

    /* percorre a arvore para encontrar o filho mais 'a direita */
    while (curr->direita != NULL)
        curr = curr->direita;

    return curr;
}

int max(int a, int b)
{
    return (a > b)? a : b;
}

int min(int a, int b)
{
    return (a < b)? a : b;
}

void avlNoApaga(no_avl* node)
{

    if(!node)
        return;

    categoriaApaga(node->categ);
    node->esquerda = NULL;
    node->direita = NULL;
    node->altura = 0;
    free(node);
    node = NULL;
}

void avlNoCopia(no_avl* nodeDest, no_avl* nodeSrc)
{

    if(!nodeSrc || !nodeDest)
        return;

    nodeDest->categ->categName = realloc(nodeDest->categ->categName, (strlen(nodeSrc->categ->categName)+1)*sizeof(char));
    strcpy(nodeDest->categ->categName, nodeSrc->categ->categName);
    heap_apaga(nodeDest->categ->itemTree);
    nodeDest->categ->itemTree = heap_nova(nodeSrc->categ->itemTree->capacidade);

    for(int i=RAIZ; i<=(nodeSrc->categ->itemTree->tamanho); i++)
    {
        nodeDest->categ->itemTree->elementos[i] = elemento_novo(nodeSrc->categ->itemTree->elementos[i]->titulo,
        nodeSrc->categ->itemTree->elementos[i]->categoria,nodeSrc->categ->itemTree->elementos[i]->filmId,
        nodeSrc->categ->itemTree->elementos[i]->rating);
    }
	nodeDest->categ->itemTree->tamanho = nodeSrc->categ->itemTree->tamanho;
    nodeDest->esquerda = nodeSrc->esquerda;
    nodeDest->direita = nodeSrc->direita;
    nodeDest->altura = nodeSrc->altura;

}

void avlNoValorCopia(no_avl* nodeDest, no_avl* nodeSrc)
{

    if(!nodeSrc || !nodeDest)
        return;

    nodeDest->categ->categName = realloc(nodeDest->categ->categName, (strlen(nodeSrc->categ->categName)+1)*sizeof(char));
    strcpy(nodeDest->categ->categName, nodeSrc->categ->categName);
    heap_apaga(nodeDest->categ->itemTree);
    nodeDest->categ->itemTree = heap_nova(nodeSrc->categ->itemTree->capacidade);

    for(int i=RAIZ; i<=(nodeSrc->categ->itemTree->tamanho); i++)
    {
        nodeDest->categ->itemTree->elementos[i] = elemento_novo(nodeSrc->categ->itemTree->elementos[i]->titulo,
        nodeSrc->categ->itemTree->elementos[i]->categoria,nodeSrc->categ->itemTree->elementos[i]->filmId,
        nodeSrc->categ->itemTree->elementos[i]->rating);
    }
	nodeDest->categ->itemTree->tamanho = nodeSrc->categ->itemTree->tamanho;
}
//---------------------------------------------------------------- VETOR FILMES TODOS ---------------------------------------------------------------

vetor_filmes* vfilmes_novo(int capacidade)
{
	vetor_filmes * v = (vetor_filmes *) malloc(sizeof(vetor_filmes));

	if (!v)
		return NULL;

	v->tamanho = 0;
	v->capacidade = capacidade;
	v->elementos = (filme **) calloc(capacidade+1, sizeof(filme *));

	if (!v->elementos) {
		free(v);
		return NULL;
	}

	return v;
}

void vfilmes_apaga(vetor_filmes *v)
{
	int i;
    int nvezes = v->tamanho;
	if(!v)
		return;

	/* apaga todos os elementos e respetivas strings */
	for(i=RAIZ; i<=nvezes; i++)
	{
        if(!v->elementos[i]){

            continue;
        }
		free(v->elementos[i]->titulo);
        free(v->elementos[i]->categoria);
		free(v->elementos[i]);
		v->elementos[i]=NULL;
	}

	free(v->elementos);
	free(v);
}

int vfilmes_insere(vetor_filmes * v, char* titulo, char* categoria, unsigned int filmId, float rating)
{
	filme * elem;

	/* se vetor esta' cheio, aumenta capacidade */
	if (filmId >= v->capacidade){
		if(v->capacidade == 0)
		v->capacidade = 1;
		else                                                            //se o filmId for maior que a capacidade do vetor
		v->capacidade *= filmId;

		v->elementos = (filme**)realloc(v->elementos, (v->capacidade+1) * sizeof(filme*));
		if(v->elementos == NULL)
		return -1;
    }

	elem = elemento_novo(titulo, categoria, filmId, rating);
	if (!elem)
		return 0;

	/* coloca elemento na posiçao filmId */
	if(filmId > v->tamanho){
        v->tamanho = filmId;
    }
    

	v->elementos[filmId] = elem;

	return 1;
}

//---------------------------------------------------------------- HEAP PARA EXERCICIO 9  -----------------------------------------------------------

int maior9_que(categorias * e1, categorias * e2)
{
	if (e1 == NULL || e2 == NULL)
	{
		return 0;
	}
    
    if(e1->contador == e2->contador) return (e1->somarating/ e1->contador) > (e2->somarating/e2->contador);

	return e1->contador > e2->contador;
}

heap9* heap9_nova(int capacidade)
{
	heap9 * h = (heap9 *) malloc(sizeof(heap9));

	if (!h)
		return NULL;

	h->tamanho = 0;
	h->capacidade = capacidade;
	h->elementos = (categorias **) calloc(capacidade+1, sizeof(categorias *));

	if (!h->elementos) {
		free(h);
		return NULL;
	}

	return h;
}

void heap9_apaga(heap9 *h)
{
	int i;

	if(!h)
		return;

	/* apaga todos os elementos e respetivas strings */
	for(i=RAIZ; i<=h->tamanho; i++)
	{
		free(h->elementos[i]->nome);
        vetor_apaga(h->elementos[i]->ids);

		free(h->elementos[i]);
		h->elementos[i]=NULL;
	}

	free(h->elementos);
	free(h);
}

int heap9_insere(heap9 * h, categorias* categ)
{
	categorias * aux;
	int i;

	/* se heap esta' cheia, aumenta capacidade */
	if (h->tamanho >= h->capacidade){
		if(h->capacidade == 0)
		h->capacidade = 1;
		else
		h->capacidade *= 2;

		h->elementos = (categorias**)realloc(h->elementos, (h->capacidade+1) * sizeof(categorias*));
		if(h->elementos == NULL)
		return -1;
    }


	/* coloca elemento no fim da heap */
	h->tamanho++;
	i = h->tamanho;
	h->elementos[i] = categ;

 	/* enquanto elemento for mais prioritario do que o respetivo pai, troca-os */
	while (i != RAIZ && maior9_que(h->elementos[i], h->elementos[PAI(i)]))
	{
		aux = h->elementos[PAI(i)];
		h->elementos[PAI(i)] = h->elementos[i];
		h->elementos[i] = aux;
		i = PAI(i);
	}
	return 1;
}

categorias * heap9_remove(heap9 * h)
{                         
	int i, filho_maior;
	categorias * aux;
	categorias * ret = &(*h->elementos[RAIZ]);

	/* se heap estiver vazia, nao remove elemento */
	if (!h || h->tamanho <= 0)
		return NULL;

	//Bloco de código causa erro no valgrind pois dá free ao elemento que é suposto ser retornado
    /*ret = h->elementos[RAIZ];
	free(h->elementos[RAIZ]);*/


	/* coloca ultimo elemento da heap na raiz */
	h->elementos[RAIZ] = h->elementos[h->tamanho];
	h->elementos[h->tamanho] = NULL;
	h->tamanho--;

 	i = RAIZ;

	/* enquanto nao chegar 'a base da heap */
	while(FILHO_ESQ(i) <= h->tamanho)
	{
		filho_maior = FILHO_ESQ(i);

		/* verifica se existe filho 'a direita e se este e' mais prioritario do que 'a esquerda */
		if (FILHO_DIR(i) <= h->tamanho && maior9_que(h->elementos[FILHO_DIR(i)], h->elementos[FILHO_ESQ(i)]))
			filho_maior = FILHO_DIR(i);

		/* enquanto elemento for mais prioritario do que o respetivo pai, troca-os */
		if (maior9_que(h->elementos[filho_maior], h->elementos[i]))
		{
			aux = h->elementos[filho_maior];
			h->elementos[filho_maior] = h->elementos[i];
			h->elementos[i] = aux;
			i = filho_maior;
		}
		else
			break;
	}

	return ret;
}
// --------------------------------------------------------------- FIM FUNÇOES AUXILIARES -----------------------------------------------------------



//////   Implementacao Tabela de Dispersão - Clientes  ///////
colecaoClientes* colecaoClientesNova(int tamanho)
{
    /* aloca memoria para a estrutura tabela_dispersao */

    colecaoClientes *t = (colecaoClientes*) malloc(sizeof (colecaoClientes));
    if (t == NULL)
        return NULL;

    /* aloca memoria para os elementos */
    t->elementos = (elementoCliente **) calloc(tamanho, sizeof (elementoCliente*));
    if (t->elementos == NULL) {
        free(t);
        return NULL;
    }

    t->tamanho = tamanho;

    return t;
}

void colecaoClientesApaga(colecaoClientes *td)
{
    int i;
    elementoCliente *elem, *aux;

    if (td == NULL) return ;

    /* para cada entrada na tabela */
    for (i = 0; i < td->tamanho; i++)
    {
        /* e enquanto existirem elementos nessa entrada */
        elem = td->elementos[i];
        while (elem != NULL)
        {
            /* liberta cada elemento */
            aux = elem->proximo;
            vetor_apaga(elem->clien->vistos);
            free(elem->clien->username);
            elem->clien->username=NULL;
            free(elem->clien);
            free(elem);
            elem = aux;
        }
    }

    /* liberta vector e estrutura */
    free(td->elementos);
    free(td);
}

int clienteAdiciona(colecaoClientes *td, const char *username, unsigned int filmId)
{
   // Resolução do exercicio 1
   //Código adaptado da função tabela_adiciona do ficheiro tabdispersao.c disponibilizado na aula prática 10

    unsigned long index;
    elementoCliente * elem;
    int i;

    if (!td || !username || filmId < 0 ) return -1;

    /* calcula hash para a string a adicionar */
    index = hash_cliente(username, td->tamanho );

    /* verifica se chave ja' existe na lista */
    elem = td->elementos[index];
    while (elem != NULL && strcmp(elem->clien->username, username) != 0)
        elem = elem->proximo;
    if (elem == NULL)
    {
        /* novo elemento, chave nao existe na lista */

        /* aloca memoria para o elemento */
        elem = (elementoCliente*) malloc(sizeof (elementoCliente) );
        if (elem == NULL)
            return -1;

        /* aloca memoria para o valoreto */
        elem->clien = (cliente*) malloc(sizeof (cliente));
        if (elem->clien == NULL)
            return -1;

        /*copia chave e valor */
        elem->clien->username = (char*) malloc( sizeof(char)*(strlen(username)+1) );
        strcpy(elem->clien->username, username);
        if(filmId == 0){
            elem->clien->vistos = vetor_novo();
        }
        else{
            elem->clien->vistos = vetor_novo();
            vetor_insere(elem->clien->vistos, filmId, -1);
        }

        /* insere no inicio da lista */
        elem->proximo = td->elementos[index];
        td->elementos[index] = elem;
    } else {
        /* verificar se o cliente que já existe viu o filme */
        for (i = 0; i < vetor_tamanho(elem->clien->vistos) ; i++){
		    if( vetor_elemento(elem->clien->vistos, i) == filmId ) return 0;
	    }

        /* se não viu o filme, acrescenta ao vetor vistos */
        vetor_insere(elem->clien->vistos, filmId, -1);

    }
    
    return 1;
}




int colecaoNumClientes(colecaoClientes *td)
{
	// resolução do exercicio 2
    //Código adaptado da função tabela_numelementos do ficheiro tabdispersao.c disponibilizado na aula prática 10

    int i, count = 0;
    elementoCliente * elem;

    if (!td) return -1;

    /* percorre todos os elementos da tabela */
    for (i = 0; i < td->tamanho; i++)
    {
        elem = td->elementos[i];
        while(elem != NULL)
        {
            elem = elem->proximo;
            count++;
        }
    }
    return count;
}

int clienteExiste(colecaoClientes *td, const char *username)
{
    // resolução do exercicio 3
    
    if(!td || !username){
        return -1;
    }

    unsigned long index = hash_cliente(username, td->tamanho );
    elementoCliente * elem = td->elementos[index];

    while( elem!= NULL && strcmp(elem->clien->username, username) != 0){
        elem = elem->proximo;
    }

    if(elem == NULL){
        return 0;
    }

	return 1;

}

unsigned long hash_cliente(const char* username, int tamanho)
{
    int c, t = strlen(username);
    unsigned long hash = 7;

    for(c = 0; c < t; c++)
    {
        hash += (int) username[c]*powf(2,c);
    }

    return hash % tamanho;

}

/*================================================================================*/
void mostraTabela(colecaoClientes *td)
{
    int i;
    elementoCliente * elem;


    printf("TABELA DE CLIENTES (%d Clientes)\n", colecaoNumClientes(td));
    for (i = 0; i < td->tamanho; i++)
    {
        if (td->elementos[i])
        {
            printf("\t[%2d]", i);
            elem = td->elementos[i];
            while (elem)
            {
                printf(" : [\"%s\" Filmes: ", elem->clien->username);
                if (elem->clien->vistos->tamanho>0)
                {

                    for (int j=0; j<elem->clien->vistos->tamanho; j++)
                    {
                        printf(" (%d)", elem->clien->vistos->elementos[j]);
                    }

                }
                printf("]");
                elem = elem->proximo;
            }
            printf("\n");
        }
    }
    printf("\n");
}



///////////////////////////////////////////////////



/* inserir um Novo Filme*/
int inserirNovoFilme(colecaoFilmes* colecFilmes, char* titulo, char* categoria, int filmId, float rating)
{
    // resolução do exercicio 5

    if(!colecFilmes || !titulo || !categoria || filmId < 0 || rating < 0 || rating > 10)
        return -1;

    int existe = 0;
    heap* h;
    
    if(avl_pesquisa(colecFilmes->raiz, categoria) != NULL){    
        existe = 1;
    }

    filme* f = elemento_novo(titulo, categoria, filmId, rating);

    if(existe == 0){
        h = heap_nova(100);     //capacidade da heap_nova não irá interferir pois o heap_insere foi modificado de modo a
                                //incrementar a capacidade caso necessario
        heap_insere(h, f);
        category_t* cat = novaCategoria(h, categoria);
        colecFilmes->raiz = avl_insere(colecFilmes->raiz, cat);
        
        vfilmes_insere(colecFilmes->vec, titulo, categoria, filmId, rating);
        return 1;
    }
    
    if(existe==1){
        h = avl_pesquisa(colecFilmes->raiz, categoria)->categ->itemTree;
        
        for(int i=RAIZ; i< h->tamanho+RAIZ; i++){
            if( h->elementos[i]->filmId == filmId){
                return 0;
            }
        }

        heap_insere(h, f);
        vfilmes_insere(colecFilmes->vec, titulo, categoria, filmId, rating);
        return 1;
    }
    
    return -1;
}

colecaoFilmes* filmesCarrega(const char *nomeFicheiro)
{
   // resolução do exercicio 6
    FILE *f = fopen(nomeFicheiro, "r");
    if(!f){
        return NULL;
    }

    colecaoFilmes* colecNova = avl_nova();
    if(!colecNova){
        return NULL;
    }
    
    char string[100];
    char* titulo;
    char* categoria;
    char* cfilmId;
    char* crating;
    
    unsigned int filmId;
    float rating;

    while( fgets(string, 100, f ) != NULL ){

        titulo = strtok(string, "|");
        categoria = strtok(NULL, "|");
        cfilmId = strtok(NULL, "|");
        crating = strtok(NULL, "|");
    
        filmId = atoi(cfilmId);
        rating = atof(crating);

        inserirNovoFilme(colecNova, titulo, categoria, filmId, rating);
    }

    fclose(f);

    return colecNova;
}


// Remover um filme///
int removerFilme(colecaoFilmes* colecFilmes, colecaoClientes *td, int filmId)
{
    // resolução do exercicio 7
    if(!colecFilmes || !td || filmId < 1) return -1;
    
    filme** aux = (filme**)malloc (sizeof(filme*));
    filme* ret;
    filme* f;

    int i, j;
    heap* h;

    //verificar se o filme existe em colecFilmes

    //se o filmId é maior que o tamanho que o vetor
    if(filmId > colecFilmes->vec->tamanho){
        free(aux);
        return 0;
    }
    //se a posição do vetor está vazia (nao existe esse filme)
    if(!colecFilmes->vec->elementos[filmId]){
        free(aux);
        return 0;
    }
    
    //existe
    h = avl_pesquisa(colecFilmes->raiz, colecFilmes->vec->elementos[filmId]->categoria)->categ->itemTree;

    aux = (filme**)realloc(aux, sizeof(filme*) * h->tamanho);
    
    //retirar da heap

    for(i=0; i<h->tamanho; i++){
        ret = heap_remove(h);
        if(ret->filmId == filmId) break;
        aux[i] = ret;
    }
    for(j=0; j<i; j++){
        f = elemento_novo(aux[j]->titulo, aux[j]->categoria, aux[j]->filmId, aux[j]->rating);
        heap_insere(h, f );
    }

    free(ret->categoria);
    free(ret->titulo);
    free(ret);


    //retirar da colecaoClientes* td

    elementoCliente* elem;

    for(i=0; i < td->tamanho; i++){

        for(elem = td->elementos[i]; elem != NULL; elem=elem->proximo){

            for(j=0; j < elem->clien->vistos->tamanho; j++){
                if(vetor_elemento(elem->clien->vistos, j) == filmId){
                    vetor_remove(elem->clien->vistos, j);
                    break;
                }
            }

        }

    }

    free(aux);

    free(colecFilmes->vec->elementos[filmId]->categoria);
    free(colecFilmes->vec->elementos[filmId]->titulo);
    free(colecFilmes->vec->elementos[filmId]);
    colecFilmes->vec->elementos[filmId] = NULL;
    
    if(filmId == colecFilmes->vec->tamanho ){
        colecFilmes->vec->tamanho--;
    }

    return 1;
}

// Remover a estrutura colecaoFilmes
void colecaoFilmesApaga(colecaoFilmes* colecFilmes, colecaoClientes *td)
{
    // resolução do exercicio 8

    if(!colecFilmes || !td) return;

    int i, j;
    int currID;

    elementoCliente* elem;

    for(i=0; i < td->tamanho; i++){

        for(elem = td->elementos[i]; elem != NULL; elem=elem->proximo){

            for(j=0; j < elem->clien->vistos->tamanho; j++){
                currID = vetor_elemento(elem->clien->vistos, j);
                if( colecFilmes->vec->elementos[currID] ){
                    vetor_remove(elem->clien->vistos, j);
                    j--;
                }

            }

        }

    }
    avl_apaga(colecFilmes);
}

//Sugestões //////////////////////////////


vetor* sugestoes(colecaoFilmes* colecFilmes, colecaoClientes *td,char* username, int nFilmes, float limiar)
{
	// resolução do exercicio 9

    if(!colecFilmes || !td || !username) return NULL;

    //Encontrar cliente
    int i, j;
    unsigned long index = hash_cliente(username, td->tamanho );
    elementoCliente * elem = td->elementos[index];

    while( elem!= NULL && strcmp(elem->clien->username, username) != 0){
        elem = elem->proximo;
    }
    
    vetor* vistos = elem->clien->vistos;

    //Determinar categoria mais vista do utilizador
    
    categorias** listacat = (categorias**) malloc(sizeof(categorias*) );

    int n=0;
    for(i=0; i < vetor_tamanho(vistos) ; i++){
        for(j=0; j < n; j++ ){

            if(!listacat[j]->nome) break;

            if( strcmp(listacat[j]->nome, colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->categoria ) == 0){
                break;
            }
        }
        
        if(j==n){
            listacat = (categorias**)realloc(listacat, (n+1) * sizeof(categorias*));
            
            listacat[n] = (categorias*)malloc(sizeof(categorias) );
            
            listacat[n]->ids = vetor_novo();
            vetor_insere(listacat[n]->ids, colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->filmId, -1); 
            
            listacat[n]->nome = (char*)calloc( strlen(colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->categoria) +1, sizeof(char) );
            strcpy( listacat[n]->nome, colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->categoria);
            
            listacat[n]->contador = 1;
            listacat[n]->somarating = colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->rating;
            n++;
        }
        else{
            vetor_insere(listacat[j]->ids, colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->filmId, -1); 
            listacat[j]->contador++;
            listacat[j]->somarating = listacat[j]->somarating + colecFilmes->vec->elementos[ vetor_elemento(vistos, i) ]->rating; 
        }
    }

    //Heap organizada por: mais vistas, caso de empate, maior media de rating dos vistos

    heap9* h9 = heap9_nova(n);

    for(i=0; i<n ; i++ ){
        heap9_insere( h9, listacat[i] );
    }
    free(listacat);

    
    //Vetor de ids com sugestoes

    vetor* sugestoes = vetor_novo();
    
    categorias* currcateg;

    heap* h;
    heap* aux;

    filme* f;

    int javiu = 0;
    int tamanho;


    while(vetor_tamanho(sugestoes) < nFilmes && h9->tamanho > 0){
        
        currcateg = heap9_remove(h9);

        
        h = avl_pesquisa(colecFilmes->raiz, currcateg->nome)->categ->itemTree;

        aux = heap_nova(h->tamanho);

        while( vetor_tamanho(sugestoes) < nFilmes && h->tamanho > 0){

            javiu = 0;

            f = heap_remove(h);
            heap_insere(aux, f);
            
            if(f->rating >= limiar){
                
                //Verificar se o utilizador já viu o filme
                for(i=0; i < vetor_tamanho(currcateg->ids); i++){
                    if( f->filmId == vetor_elemento(currcateg->ids, i) ){
                        javiu = 1;
                        break;
                    }
                }
                //Se nao viu, sugere o filme
                if(javiu==0) vetor_insere(sugestoes, f->filmId, -1);

            }
            //Se o rating do filme atual é menor do que o limiar, então passa para a proxima categoria
            else{
                break;
            }

    
        }

        tamanho = aux->tamanho;
        for(i=0; i < tamanho; i++){
            f = heap_remove(aux);

            heap_insere(h, f);

        }
        free(currcateg->nome);
        vetor_apaga(currcateg->ids);
        free(currcateg);
        heap_apaga(aux);

    }
    
        
    //Se sugestoes nao tiver completo, então preenche o resto dos espaços com 0
    for(i=vetor_tamanho(sugestoes); vetor_tamanho(sugestoes) < nFilmes; i++){
        vetor_insere(sugestoes, 0, -1);
    }


    heap9_apaga(h9);
    return sugestoes;
}



///////////////////////////////////////
