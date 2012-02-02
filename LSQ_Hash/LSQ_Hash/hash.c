#include <assert.h>
#include <string.h>
#include <math.h>
#include "linear_sequence_assoc_hash.h"

#define SEQ(T) ((TypeHash*)(T))
#define ITR(T) ((TypeIterator*)(T))
#define TypeHashIndex unsigned short

const int HASH_SIZE = 65535;
const unsigned short Crc32Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};


typedef struct Node{
	struct Node *next;
	LSQ_KeyT key;
	LSQ_BaseTypeT value;
} TypeNode;

typedef struct{
	LSQ_SizeT size;
	TypeNode **buckets;
	LSQ_Callback_CloneFuncT *keyCloneFunc;
	LSQ_Callback_CompareFuncT *keyCompFunc;
	LSQ_Callback_SizeFuncT *keySizeFunc;
	LSQ_Callback_CloneFuncT *valCloneFunc;
} TypeHash;

typedef enum{
	NORMAL,
	BEFOR_FIRST,
	PAST_REAR
} TypeStateIterator;

typedef struct{
	TypeNode *node;
	TypeHash *hash;
	TypeHashIndex bucket;
	TypeStateIterator state;
} TypeIterator;

static TypeHashIndex ComputeHash(TypeHash *hash, LSQ_KeyT key){
	unsigned short crc = 0xFFFF; 
	unsigned char x = *(unsigned char*)key;
	int i, size = hash->keySizeFunc(key);

	for(i = 0; i < size; i++)
		crc = (crc << 8) ^ Crc32Table[(crc >> 8) ^ x++];
	return(crc);
}

extern LSQ_IteratorT CreateIterator(TypeStateIterator state, TypeHash* handle, TypeNode *node, TypeHashIndex bucket){
	TypeIterator *iter = (TypeIterator*)malloc(sizeof(TypeIterator));
	if(iter == NULL)
		return(LSQ_HandleInvalid);
	
	iter->bucket = bucket;
	iter->hash = handle;
	iter->node = node;
	iter->state = state;
	return((LSQ_IteratorT)iter);
}

/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
LSQ_HandleT LSQ_CreateSequence(LSQ_Callback_CloneFuncT keyCloneFunc, LSQ_Callback_SizeFuncT keySizeFunc, LSQ_Callback_CompareFuncT keyCompFunc,
						                    LSQ_Callback_CloneFuncT valCloneFunc){
	TypeHash *hash = (TypeHash*)malloc(sizeof(TypeHash));
	if(hash == NULL)
		return(LSQ_HandleInvalid);

	hash->buckets = (TypeNode**)calloc(HASH_SIZE, sizeof(TypeNode*));

	hash->keyCloneFunc = keyCloneFunc;
	hash->keyCompFunc = keyCompFunc;
	hash->keySizeFunc = keySizeFunc;
	hash->valCloneFunc = valCloneFunc;
	hash->size = 0;
	return((LSQ_HandleT)hash);
}
/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
void LSQ_DestroySequence(LSQ_HandleT handle){
	TypeNode *node = NULL, *pnode = NULL;
	TypeHashIndex i;

	if(handle == LSQ_HandleInvalid)
		return;

	for(i = 0; i < HASH_SIZE; i++){
		node = SEQ(handle)->buckets[i];
		while(node != NULL){
			pnode = node;
			node = node->next;

			free(pnode->value);
			free(pnode->key);
			free(pnode);
		}
	}

	free(SEQ(handle)->buckets);
	free(SEQ(handle));
}
/* Функция, возвращающая текущее количество элементов в контейнере */
LSQ_SizeT LSQ_GetSize(LSQ_HandleT handle){
	return(handle == LSQ_HandleInvalid)? 0: SEQ(handle)->size;
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return(iterator == NULL)? 0: (ITR(iterator)->state == NORMAL);
}

/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return(iterator == NULL)? 0: (ITR(iterator)->state == PAST_REAR);
}

/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return(0);
}

/* Функция разыменовывающая итератор. Возвращает указатель на значение элемента, на который ссылается данный итератор */
LSQ_BaseTypeT LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	return(!LSQ_IsIteratorDereferencable(iterator))? LSQ_HandleInvalid: ITR(iterator)->node->value;
}

/* Функция разыменовывающая итератор. Возвращает указатель на ключ элемента, на который ссылается данный итератор */
LSQ_KeyT LSQ_GetIteratorKey(LSQ_IteratorT iterator){
	return(!LSQ_IsIteratorDereferencable(iterator))? LSQ_HandleInvalid: ITR(iterator)->node->key;
}

/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным ключом. Если элемент с данным ключом  *
 * отсутствует в контейнере, должен быть возвращен итератор PastRear.                                       */
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_KeyT key){
	TypeHashIndex bucket = 0;
	TypeNode *node = NULL;

	if(handle == NULL)
		return(NULL);

	bucket = ComputeHash(SEQ(handle), key);
	node = SEQ(handle)->buckets[bucket];
	while(node != NULL && SEQ(handle)->keyCompFunc(node->key, key) != 0){
		node = node->next;
	}

	return(node == NULL)? LSQ_GetPastRearElement(handle): CreateIterator(NORMAL, SEQ(handle), node, bucket);
}

/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	TypeHashIndex i = 0;

	if(handle == NULL)
		return(LSQ_HandleInvalid);

	for(i = 0; i < HASH_SIZE; i++){
		if(SEQ(handle)->buckets[i] != NULL)
			return(CreateIterator(NORMAL,SEQ(handle), SEQ(handle)->buckets[i], i));
	}

	return(LSQ_GetPastRearElement(handle));
}

/* Функция, возвращающая итератор, ссылающийся на фиктивный элемент, следующий за последним элементом контейнера */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	return(handle == LSQ_HandleInvalid)? LSQ_HandleInvalid: CreateIterator(PAST_REAR, SEQ(handle), NULL, 0);
}

/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
void LSQ_DestroyIterator(LSQ_IteratorT iterator){
//	if(iterator == NULL)
//		return;

	free(iterator);
}

/* Следующие функции позволяют реализовать итерацию по элементам. При этом осуществляется проход только  *
 * по тем ключам, которые есть в контейнере.                                                             */
/* Функция, перемещающая итератор на один элемент вперед */
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
	TypeHashIndex i = 0;

	if(iterator == NULL || ITR(iterator)->state == PAST_REAR)
		return;

	if(ITR(iterator)->node->next != NULL){
		ITR(iterator)->node = ITR(iterator)->node->next;
		return;
	}

	for(i = ITR(iterator)->bucket + 1; i < HASH_SIZE; i++){
		if(ITR(iterator)->hash->buckets[i] != NULL){
			ITR(iterator)->node = ITR(iterator)->hash->buckets[i];
			ITR(iterator)->bucket = i;
			return;
		}
	}

	ITR(iterator)->node = NULL;
	ITR(iterator)->state = PAST_REAR;
}

/* Функция, добавляющая новую пару ключ-значение в контейнер. Если элемент с данным ключом существует,  *
 * его значение обновляется указанным.                                                                  */
void LSQ_InsertElement(LSQ_HandleT handle, LSQ_KeyT key, LSQ_BaseTypeT value){
	TypeHashIndex bucket;
	TypeNode *node = NULL;
	TypeIterator *iter = (TypeIterator*)LSQ_GetElementByIndex(handle, key);

	if(iter != NULL && iter->state == NORMAL){
		free(iter->node->value);
		iter->node->value = SEQ(handle)->valCloneFunc(value);
		LSQ_DestroyIterator(iter);
		return;
	}
	LSQ_DestroyIterator(iter);

	bucket = ComputeHash(SEQ(handle), key);

	node = (TypeNode*)malloc(sizeof(TypeNode));

	node->key = SEQ(handle)->keyCloneFunc(key);
	node->value = SEQ(handle)->valCloneFunc(value);
	node->next = SEQ(handle)->buckets[bucket];

	SEQ(handle)->buckets[bucket] = node;
	SEQ(handle)->size++;
}

/* Функция, удаляющая элемент контейнера, указываемый заданным ключом. */
void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_KeyT key){
	TypeNode *node = NULL, *pnode = NULL;
	TypeHashIndex bucket;

	if(handle == LSQ_HandleInvalid)
		return;

	bucket = ComputeHash(SEQ(handle), key);
	node = SEQ(handle)->buckets[bucket];

	while(node != NULL && SEQ(handle)->keyCompFunc(node->key, key) != 0){
		pnode = node;
		node = node->next;
	}

	if(node != NULL){
		if(pnode != NULL)
			pnode->next = node->next;
		else
			SEQ(handle)->buckets[bucket] = node->next;
		
		free(node->key);
		free(node->value);
		free(node);
		SEQ(handle)->size--;
	}
}