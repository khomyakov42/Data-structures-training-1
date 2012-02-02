#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "linear_sequence_assoc_hash.h"

#define SEQ(T) ((TypeHash*)(T))
#define ITR(T) ((TypeIterator*)(T))
#define TypeHashIndex int
const int BUCKETS_COUNT = 5;


typedef struct Node{
	struct Node *next;
	LSQ_KeyT key;
	LSQ_BaseTypeT value;
} TypeNode;

typedef struct{
	LSQ_SizeT size;
	TypeNode **buckets;
	LSQ_Callback_CloneFuncT *keyCloneFunc;
	LSQ_Callback_SizeFuncT *keySizeFunc;
	LSQ_Callback_CompareFuncT *keyCompFunc;
   LSQ_Callback_CloneFuncT *valCloneFunc;
} TypeHash;

typedef enum{
	NORMAL,
	BEFOR_FIRST,
	PAST_REAR
} TypeStateIterator;

typedef struct{
	TypeNode *node;
	TypeHash *Hash;
	TypeHashIndex bucket;
	TypeStateIterator state;
} TypeIterator;

int GetKeyInHash(TypeHash *hash, LSQ_KeyT key){
	 int size = hash->keySizeFunc(key);
    double intpart, fracpart;
    int i;

    unsigned int hcode = 0;
	 for (i = 0; i < size; i++)
		 hcode += ((char *)key)[i] * (i+1);

    fracpart = modf(0.618033989 * hcode, &intpart);
	 return (TypeHashIndex)(fracpart * (BUCKETS_COUNT - 1));
}
/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
LSQ_HandleT LSQ_CreateSequence(LSQ_Callback_CloneFuncT keyCloneFunc, LSQ_Callback_SizeFuncT keySizeFunc,
											LSQ_Callback_CompareFuncT keyCompFunc, LSQ_Callback_CloneFuncT valCloneFunc){
	TypeHash *hash = (TypeHash*)malloc(sizeof(TypeHash));
	if(hash == NULL)
		return;
	hash->buckets = (TypeNode**)calloc(BUCKETS_COUNT, sizeof(TypeNode*));
	if(hash->buckets == NULL){
		free(hash);
		return(LSQ_HandleInvalid);
	}
	hash->keyCloneFunc = keyCloneFunc;
	hash->keyCompFunc = keyCompFunc;
	hash->keySizeFunc = keySizeFunc;
	hash->valCloneFunc = valCloneFunc;
	hash->size = 0;
	return(hash);
}

/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
void LSQ_DestroySequence(LSQ_HandleT handle){
	int i;
	TypeNode *node = NULL, *pnode = NULL;

	if(handle == NULL)
		return;
	if(SEQ(handle)->buckets != NULL)
		for(i = 0; i < BUCKETS_COUNT; i++){
			node = SEQ(handle)->buckets[i];
			while(node != NULL){
				pnode = node;
				node = node->next;
				free(pnode);
			}
		}

	free(SEQ(handle)->buckets);
	free(handle);
}
			
/* Функция, возвращающая текущее количество элементов в контейнере */
LSQ_SizeT LSQ_GetSize(LSQ_HandleT handle){
	return(handle == LSQ_HandleInvalid)? 0: SEQ(handle)->size;
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	if(iterator == NULL)
		return(0);
	else
		return(ITR(iterator)->state == NORMAL);
}
/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	if(iterator == NULL)
		return(0);
	else
		return(ITR(iterator)->state == PAST_REAR);
}

/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	if(iterator == NULL)
		return(0);
	else
		return(ITR(iterator)->state == BEFOR_FIRST);
}
/* Функция разыменовывающая итератор. Возвращает указатель на значение элемента, на который ссылается данный итератор */
LSQ_BaseTypeT LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	if(LSQ_IsIteratorDereferencable(iterator)){
		assert(ITR(iterator)->node != NULL);
		return(ITR(iterator)->node->value);
	}
	else
		return(LSQ_HandleInvalid);
}
/* Функция разыменовывающая итератор. Возвращает указатель на ключ элемента, на который ссылается данный итератор */
LSQ_KeyT LSQ_GetIteratorKey(LSQ_IteratorT iterator){
	if(LSQ_IsIteratorDereferencable(iterator)){
		assert(ITR(iterator)->node != NULL);
		return((ITR(iterator)->node->key));
	}
	else
		return(LSQ_HandleInvalid);
}
/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным ключом. Если элемент с данным ключом  *
 * отсутствует в контейнере, должен быть возвращен итератор PastRear.                                       */
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_KeyT key){
	TypeIterator *iterator = NULL;
	TypeNode *node = NULL;
	TypeHashIndex bucket;

	if(handle == NULL)
		return(LSQ_HandleInvalid);

	bucket = GetKeyInHash(SEQ(handle),key);
	node = SEQ(handle)->buckets[bucket];

	if(node == NULL)
		return(LSQ_GetPastRearElement(handle));

	while(node->next != NULL && SEQ(handle)->keyCompFunc(node->key, key) != 0)
		node = node->next;
	
	iterator = (TypeIterator*)malloc(sizeof(TypeIterator));
	iterator->Hash = SEQ(handle);
	iterator->node = node;
	iterator->state = NORMAL;
	iterator->bucket = bucket;
	return(iterator);
}
/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	TypeIterator *iterator = NULL;
	TypeHashIndex bucket;

	if(handle == NULL)
		return(LSQ_HandleInvalid);

	iterator = (TypeIterator*)malloc(sizeof(TypeIterator));
	if(iterator == NULL)
		return(LSQ_HandleInvalid);
	
	iterator->Hash = SEQ(handle);
	for(bucket = 0; bucket < BUCKETS_COUNT; bucket++)
		if(SEQ(handle)->buckets[bucket] != NULL){
			iterator->node = SEQ(handle)->buckets[bucket];
			iterator->bucket = bucket;
			iterator->state = NORMAL;
			break;
		}
	
	if(iterator->state != NORMAL){
		iterator->node = NULL;
		iterator->state = PAST_REAR;
	}
	return(iterator);
}
/* Функция, возвращающая итератор, ссылающийся на фиктивный элемент, следующий за последним элементом контейнера */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	TypeIterator *iterator = NULL;

	if(handle == NULL)
		return(LSQ_HandleInvalid);

	iterator = (TypeIterator*)malloc(sizeof(TypeIterator));
	if(iterator == NULL)
		return(LSQ_HandleInvalid);

	iterator->Hash = SEQ(handle);
	iterator->node = NULL;
	iterator->state = PAST_REAR;
	return(iterator);
}
/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
void LSQ_DestroyIterator(LSQ_IteratorT iterator){
	if(iterator == NULL)
		return;
	free(iterator);
}

/* Следующие функции позволяют  итерацию по элементам. При этом осуществляется проход только  *
 * по тем ключам, которые есть в контейнере.                                                             */
/* Функция, перемещающая итератор на один элемент вперед */
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
	int bucket;

	if(iterator == NULL || ITR(iterator)->state == PAST_REAR)
		return;

	if(ITR(iterator)->node != NULL && ITR(iterator)->node->next != NULL){
		ITR(iterator)->node = ITR(iterator)->node->next;
		return;
	}

	for(bucket = ITR(iterator)->bucket + 1; bucket < BUCKETS_COUNT; bucket++)
		if(ITR(iterator)->Hash->buckets[bucket] != NULL){
			ITR(iterator)->node = ITR(iterator)->Hash->buckets[bucket];
			ITR(iterator)->state = NORMAL;
			ITR(iterator)->bucket = bucket;
			return;
		}
	ITR(iterator)->state = PAST_REAR;
	ITR(iterator)->node = NULL;
}
/* Функция, добавляющая новую пару ключ-значение в контейнер. Если элемент с данным ключом существует,  *
 * его значение обновляется указанным.                                                                  */
void LSQ_InsertElement(LSQ_HandleT handle, LSQ_KeyT key, LSQ_BaseTypeT value){
	TypeNode *node = NULL, *pnode = NULL;
	TypeHashIndex bucket;

	if(handle == LSQ_HandleInvalid)
		return;

	bucket = GetKeyInHash(SEQ(handle), key);
	
	if(SEQ(handle)->buckets[bucket] == NULL){
		node = (TypeNode*)malloc(sizeof(TypeNode));
		if(node == NULL)
			return;

		node->next = NULL;
		node->key = SEQ(handle)->keyCloneFunc(key);
		node->value = SEQ(handle)->valCloneFunc(value);
		SEQ(handle)->buckets[bucket] = node;
	}
	else{
		node = SEQ(handle)->buckets[bucket];
		pnode = node;	

 		while(node != NULL && SEQ(handle)->keyCompFunc(node->key, key) == 0)
			node = node->next;

		if(node != NULL){
			node->value = SEQ(handle)->keyCloneFunc(value);
			return;
		}
		else
			node = (TypeNode*)malloc(sizeof(TypeNode));
		
		node->key = SEQ(handle)->keyCloneFunc(key);
		node->value = SEQ(handle)->valCloneFunc(value);
		node->next = pnode;
		SEQ(handle)->buckets[bucket] = node;
	}
	SEQ(handle)->size++;
}
/* Функция, удаляющая элемент контейнера, указываемый заданным ключом. */
void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_KeyT key){
	TypeNode *node = NULL, *pnode = NULL;
	TypeHashIndex bucket;

	if(handle == LSQ_HandleInvalid)
		return;
	bucket = GetKeyInHash(SEQ(handle), key);
	node = SEQ(handle)->buckets[bucket];
	pnode = node;

	while(node != NULL && SEQ(handle)->keyCompFunc(node->key, key) == 0){
		pnode = node;
		node = node->next;
	}
	if(node == NULL)
		return;
	if(node == SEQ(handle)->buckets[bucket])
		SEQ(handle)->buckets[bucket] = node->next;
	else
		pnode->next = node->next;

	free(node);
	SEQ(handle)->size--;
}