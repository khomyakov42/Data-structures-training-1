#ifndef LINEAR_SEQUENCE_H
#define LINEAR_SEQUENCE_H

#include <stdlib.h>

/* Тип хранимых в контейнере значений */
typedef int LSQ_BaseTypeT;

/* Дескриптор контейнера */
typedef void* LSQ_HandleT;

/* Неинициализированное значение дескриптора контейнера */
#define LSQ_HandleInvalid NULL

/* Дескриптор итератора */
typedef void* LSQ_IteratorT;

/* Тип целочисленного индекса контейнера */
typedef int LSQ_IntegerIndexT;
/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
extern LSQ_HandleT LSQ_CreateSequence(void);
/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
extern void LSQ_DestroySequence(LSQ_HandleT handle);

/* Функция, возвращающая текущее количество элементов в контейнере */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle);

/* Функция, определяющая, может ли данный итератор быть разыменован */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator);
/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator);
/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator);

/* Функция разыменовывающая итератор. Возвращает указатель на значение элемента, на который ссылается данный итератор */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator);
/* Функция разыменовывающая итератор. Возвращает указатель на ключ элемента, на который ссылается данный итератор */
extern LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator);

/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным ключом. Если элемент с данным ключом  *
 * отсутствует в контейнере, должен быть возвращен итератор PastRear.                                       */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index);
/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle);
/* Функция, возвращающая итератор, ссылающийся на фиктивный элемент, следующий за последним элементом контейнера */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle);

/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator);

/* Следующие функции позволяют реализовать итерацию по элементам. При этом осуществляется проход только  *
 * по тем ключам, которые есть в контейнере.                                                             */
/* Функция, перемещающая итератор на один элемент вперед */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator);
/* Функция, перемещающая итератор на один элемент назад */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator);
/* Функция, перемещающая итератор на заданное смещение со знаком */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift);
/* Функция, устанавливающая итератор на элемент с указанным номером */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos);

/* Функция, добавляющая новую пару ключ-значение в контейнер. Если элемент с данным ключом существует,  *
 * его значение обновляется указанным.                                                                  */
extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value);

/* Функция, удаляющая первый элемент контейнера */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle);
/* Функция, удаляющая последний элемент контейнера */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle);
/* Функция, удаляющая элемент контейнера, указываемый заданным ключом. */
extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key);
#endif

