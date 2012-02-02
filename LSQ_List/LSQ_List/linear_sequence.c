#include "linear_sequence.h"
#include <stdlib.h>
#include <string.h>

#define SEQ(T) ((TypeList*)(T))
#define ITR(T) ((TypeIterator*)(T))

typedef struct NodeList{
   struct NodeList *next;
   struct NodeList *prev;
	LSQ_BaseTypeT data;
} TypeListNode;

typedef struct{
	LSQ_IntegerIndexT size;
	TypeListNode *BeforFirst;
	TypeListNode *PastRear;
} TypeList;

typedef struct{
	TypeListNode *node;
	TypeList *list;
}TypeIterator;

/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
LSQ_HandleT LSQ_CreateSequence(void){
	TypeList *list = (TypeList*)malloc(sizeof(TypeList));

	if(list == NULL)
		return(LSQ_HandleInvalid);

	list->BeforFirst = (TypeListNode*)malloc(sizeof(TypeListNode));
	list->PastRear = (TypeListNode*)malloc(sizeof(TypeListNode));
	if(list->BeforFirst == NULL || list->PastRear == NULL){
		free(list->BeforFirst);
		free(list->PastRear);
		free(list);
	}

	list->BeforFirst->prev = NULL;
	list->BeforFirst->next = list->PastRear;
	list->PastRear->prev = list->BeforFirst;
	list->PastRear->next = NULL;
	list->size = 0;
	return(list);
}

/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
void LSQ_DestroySequence(LSQ_HandleT handle){
	TypeListNode *node = NULL;

	if(handle == NULL)
		return;

	node = SEQ(handle)->BeforFirst;
	while(node->next != NULL){
		node = node->next;
		free(node->prev);
	}
	free(node);
	free(handle);
}


/* Функция, возвращающая текущее количество элементов в контейнере */
LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
	if(handle == NULL)
		return(0);
	
	return(SEQ(handle)->size);
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return(iterator == NULL || ITR(iterator)->list == NULL)? 0: (ITR(iterator)->node != ITR(iterator)->list->BeforFirst && ITR(iterator)->node != ITR(iterator)->list->PastRear);
//проверка на нуллл контейнера
}

/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return(iterator == NULL || ITR(iterator)->list == NULL)? 0:(ITR(iterator)->node == ITR(iterator)->list->PastRear);
//проверка на нуллл контейнера
}
/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return(iterator == NULL || ITR(iterator)->list == NULL)? 0:(ITR(iterator)->node == ITR(iterator)->list->BeforFirst);
//проверка на нуллл контейнера
}


/* Функция разыменовывающая итератор. Возвращает указатель на элемент, на который ссылается данный итератор */
LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	return(!LSQ_IsIteratorDereferencable(iterator))? NULL: (&(ITR(iterator)->node->data));
}
/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным индексом */
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	TypeIterator *iterator = NULL;

	if(handle == NULL)
		return(LSQ_HandleInvalid);

	iterator = (TypeIterator*)malloc(sizeof(TypeIterator));
	iterator->list = SEQ(handle);
	LSQ_SetPosition(iterator, index);
	
	return(iterator);
}
/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	return(LSQ_GetElementByIndex(handle, 0));
}
/* Функция, возвращающая итератор, ссылающийся на последний элемент контейнера */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	if(handle == NULL)
		return(LSQ_HandleInvalid);
	return(LSQ_GetElementByIndex(handle, SEQ(handle)->size));
}
/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
void LSQ_DestroyIterator(LSQ_IteratorT iterator){
	if(iterator != NULL)
		free(iterator);
}

/* Функция, перемещающая итератор на один элемент вперед */
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
	LSQ_ShiftPosition(iterator, 1);
}
/* Функция, перемещающая итератор на один элемент назад */
void LSQ_RewindOneElement(LSQ_IteratorT iterator){
	LSQ_ShiftPosition(iterator, -1);
}
/* Функция, перемещающая итератор на заданное смещение со знаком */
void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
	if(iterator == NULL)
		return;

	if(shift > 0)
		while(shift > 0 && !LSQ_IsIteratorPastRear(iterator)){
			shift--;
			ITR(iterator)->node = ITR(iterator)->node->next;
		}
	else
		while(shift < 0 && !LSQ_IsIteratorBeforeFirst(iterator)){
			ITR(iterator)->node = ITR(iterator)->node->prev;
			shift++;
		}
}
/* Функция, устанавливающая итератор на элемент с указанным номером */
void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
	if(iterator == NULL || ITR(iterator)->list == NULL)
		return;
	if(pos <= (ITR(iterator)->list->size / 2)){
		ITR(iterator)->node = ITR(iterator)->list->BeforFirst;
		LSQ_ShiftPosition(iterator, pos + 1);
	}else{
		ITR(iterator)->node = ITR(iterator)->list->PastRear;
		LSQ_ShiftPosition(iterator, pos - ITR(iterator)->list->size);

	}
}
/* Функция, добавляющая элемент в начало контейнера */
void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	TypeIterator *iterator = ITR(LSQ_GetFrontElement(handle));

	if(iterator != LSQ_HandleInvalid){
		LSQ_InsertElementBeforeGiven( iterator, element);
		LSQ_DestroyIterator(iterator);
	}
}
/* Функция, добавляющая элемент в конец контейнера */
void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	TypeIterator *iterator = ITR(LSQ_GetPastRearElement(handle));

	if(iterator != LSQ_HandleInvalid){
		LSQ_InsertElementBeforeGiven( iterator, element);
		LSQ_DestroyIterator(iterator);
	}
}

/* Функция, добавляющая элемент в контейнер на позицию, указываемую в данный момент итератором. Элемент, на который  *
 * указывает итератор, а также все последующие, сдвигается на одну позицию в конец.                                  */
void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement){
	TypeListNode *node = NULL;

	if(iterator == NULL)
		return;

	node = (TypeListNode*)malloc(sizeof(TypeListNode));
	if(node == NULL){
		free(node);
		return;
	}
	node->data = newElement;
	node->prev = ITR(iterator)->node->prev;
	node->next = ITR(iterator)->node;
	ITR(iterator)->node->prev->next = node;
	ITR(iterator)->node->prev = node;
	ITR(iterator)->node = ITR(iterator)->node->prev;
	ITR(iterator)->list->size++;
}

/* Функция, удаляющая первый элемент контейнера */
void LSQ_DeleteFrontElement(LSQ_HandleT handle){
	TypeIterator *iterator = ITR(LSQ_GetFrontElement(handle));
	if(iterator != LSQ_HandleInvalid){
		LSQ_DeleteGivenElement(iterator);
		LSQ_DestroyIterator(iterator);
	}
}

/* Функция, удаляющая последний элемент контейнера */
void LSQ_DeleteRearElement(LSQ_HandleT handle){
	TypeIterator *iterator = ITR(LSQ_GetPastRearElement(handle));
	if(iterator != LSQ_HandleInvalid){
		iterator->node = iterator->node->prev;
		LSQ_DeleteGivenElement(iterator);
		LSQ_DestroyIterator(iterator);
	}
}
/* Функция, удаляющая элемент контейнера, указываемый заданным итератором. Все последующие элементы смещаются на     *
 * одну позицию в сторону начала.                                                                                    */
void LSQ_DeleteGivenElement(LSQ_IteratorT iterator){
	TypeListNode *node = NULL;
	if(iterator == NULL || !LSQ_IsIteratorDereferencable(iterator))
		return;
	ITR(iterator)->list->size--;
   ITR(iterator)->node->prev->next = ITR(iterator)->node->next;
   ITR(iterator)->node->next->prev = ITR(iterator)->node->prev;
	node = ITR(iterator)->node;
	ITR(iterator)->node = ITR(iterator)->node->next;
	free(node);
}