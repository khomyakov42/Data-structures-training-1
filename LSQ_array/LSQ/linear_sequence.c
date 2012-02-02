#include"linear_sequence.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SEQ(T) ((LSQ_TypeSequence*)(T))
#define ITR(T) ((LSQ_TypeIterator*)(T))

typedef struct {
	LSQ_BaseTypeT *head;
	LSQ_IntegerIndexT size;
} LSQ_TypeSequence;

typedef struct{
	LSQ_IntegerIndexT index;
	LSQ_TypeSequence *SeqHandle;
} LSQ_TypeIterator;

/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
LSQ_HandleT LSQ_CreateSequence(){
	LSQ_TypeSequence *sequence = NULL;
	sequence = (LSQ_TypeSequence*)malloc(sizeof(LSQ_TypeSequence));
	sequence->size = 0;
	sequence->head = NULL;
	return ((LSQ_HandleT)sequence);
}

/* Фyнкция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
void LSQ_DestroySequence(LSQ_HandleT handle){
	if (handle == LSQ_HandleInvalid) return;
	free(SEQ(handle)->head);
	free(SEQ(handle));
}

/* Функция, возвращающая текущее количество элементов в контейнере */
LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
	if (handle == LSQ_HandleInvalid) return (0);
	return (SEQ(handle)->size);
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return (iterator != NULL && ITR(iterator)->index >= 0 && ITR(iterator)->index <= ITR(iterator)->SeqHandle->size - 1);
}

/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return((iterator != NULL && ITR(iterator)->index >= ITR(iterator)->SeqHandle->size));
}

/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return(iterator != NULL && ITR(iterator)->index < 0);
}

/* Функция разыменовывающая итератор. Возвращает указатель на элемент, на который ссылается данный итератор */
LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	return (iterator==NULL)? NULL : (ITR(iterator)->SeqHandle->head + ITR(iterator)->index);
}

/* Функция, возвращающая итератор, ссылающийся на элемент с указанным индексом */
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	LSQ_TypeIterator *iterator = NULL;
	
	if (handle == LSQ_HandleInvalid) return (NULL);
	iterator = (LSQ_TypeIterator*)malloc(sizeof(LSQ_TypeIterator));
   iterator->index = index;
	iterator->SeqHandle = SEQ(handle);
	return ((LSQ_IteratorT)iterator);
}

/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	LSQ_TypeIterator *iterator = NULL;
	if (handle == LSQ_HandleInvalid) return (LSQ_HandleInvalid);
	iterator = (LSQ_TypeIterator*)malloc(sizeof(LSQ_TypeIterator));
	iterator->index = 0;
	iterator->SeqHandle = SEQ(handle);
	return ((LSQ_IteratorT)iterator);
}

/* Функция, возвращающая итератор, ссылающийся на следущий после последнего элемент контейнера */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	LSQ_TypeIterator *iterator = NULL;
	if (SEQ(handle) == LSQ_HandleInvalid) return (LSQ_HandleInvalid);
	iterator = (LSQ_TypeIterator*)malloc(sizeof(LSQ_TypeIterator));
	iterator->index = SEQ(handle)->size;
	iterator->SeqHandle = SEQ(handle);
	iterator->index = SEQ(handle)->size;
	return ((LSQ_IteratorT)iterator);
}

/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
void LSQ_DestroyIterator(LSQ_IteratorT iterator){
    free(ITR(iterator));
}

/* Функция, перемещающая итератор на один элемент вперед */
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
	if (ITR(iterator) == NULL) return;
	ITR(iterator)->index++;
}

/* Функция, перемещающая итератор на один элемент назад */
void LSQ_RewindOneElement(LSQ_IteratorT iterator){
	if (ITR(iterator) == NULL) return;
	ITR(iterator)->index--;
}

/* Функция, перемещающая итератор на заданное смещение со знаком */
void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
	if (ITR(iterator) == NULL) return;
	ITR(iterator)->index += shift;
}

/* Функция, устанавливающая итератор на элемент с указанным номером */
void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
	if (ITR(iterator) == NULL) return;
	ITR(iterator)->index = pos;
}

/* Функция, добавляющая элемент в начало контейнера */
void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	if (SEQ(handle) == LSQ_HandleInvalid) return;
	SEQ(handle)->size++; 
	SEQ(handle)->head = (LSQ_BaseTypeT*)realloc(SEQ(handle)->head, sizeof(LSQ_BaseTypeT)*(SEQ(handle)->size));
	memmove(SEQ(handle)->head + 1, SEQ(handle)->head, sizeof(LSQ_BaseTypeT)*(SEQ(handle)->size - 1));
	*(SEQ(handle)->head) = element;
}

/* Функция, добавляющая элемент в конец контейнера */
void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
	if (SEQ(handle) == LSQ_HandleInvalid) return; 
	SEQ(handle)->size = SEQ(handle)->size + 1;
	SEQ(handle)->head = (LSQ_BaseTypeT*)realloc(SEQ(handle)->head, sizeof(LSQ_BaseTypeT)*(SEQ(handle)->size));
   *(SEQ(handle)->head + SEQ(handle)->size - 1) = element;
}

/* Функция, добавляющая элемент в контейнер на позицию, указываемую в данный момент итератором. Элемент, на который  *
 * указывает итератор, а также все последующие, сдвигается на одну позицию в конец.                                  */
void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement){
	if (ITR(iterator) == NULL) return;
	ITR(iterator)->SeqHandle->size++;
	
	ITR(iterator)->SeqHandle->head = (LSQ_BaseTypeT*)realloc(ITR(iterator)->SeqHandle->head, sizeof(LSQ_BaseTypeT)*(ITR(iterator)->SeqHandle->size));
	memmove(ITR(iterator)->SeqHandle->head + ITR(iterator)->index + 1, ITR(iterator)->SeqHandle->head + ITR(iterator)->index,sizeof(LSQ_BaseTypeT)*(ITR(iterator)->SeqHandle->size - ITR(iterator)->index - 1));
	*(ITR(iterator)->SeqHandle->head + ITR(iterator)->index) = newElement;
}

/* Функция, удаляющая первый элемент контейнера */
void LSQ_DeleteFrontElement(LSQ_HandleT handle){
	if (SEQ(handle) == LSQ_HandleInvalid || SEQ(handle)->size == 0) return;
	SEQ(handle)->size--;
	memmove(SEQ(handle)->head, SEQ(handle)->head + 1, sizeof(LSQ_BaseTypeT)*(SEQ(handle)->size));
	SEQ(handle)->head = (LSQ_BaseTypeT*)realloc(SEQ(handle)->head, sizeof(LSQ_BaseTypeT)*(SEQ(handle)->size));
}

/* Функция, удаляющая последний элемент контейнера */
void LSQ_DeleteRearElement(LSQ_HandleT handle){
	if (SEQ(handle) == LSQ_HandleInvalid || SEQ(handle)->size == 0) return;
	SEQ(handle)->size--;
	SEQ(handle)->head = (LSQ_BaseTypeT*)realloc(SEQ(handle)->head, sizeof(LSQ_BaseTypeT)*(SEQ(handle)->size));
}

/* Функция, удаляющая элемент контейнера, указываемый заданным итератором. Все последующие элементы смещаются на     *
 * одну позицию в сторону начала.                                                                                    */
void LSQ_DeleteGivenElement(LSQ_IteratorT iterator){
	if (ITR(iterator) == NULL || !LSQ_IsIteratorDereferencable(iterator)) return;
	ITR(iterator)->SeqHandle->size--;
	memmove(ITR(iterator)->SeqHandle->head + ITR(iterator)->index , ITR(iterator)->SeqHandle->head + ITR(iterator)->index + 1, sizeof(LSQ_BaseTypeT)*(ITR(iterator)->SeqHandle->size - ITR(iterator)->index));
	ITR(iterator)->SeqHandle->head = (LSQ_BaseTypeT*)realloc(ITR(iterator)->SeqHandle->head,sizeof(LSQ_BaseTypeT)*(ITR(iterator)->SeqHandle->size));
}