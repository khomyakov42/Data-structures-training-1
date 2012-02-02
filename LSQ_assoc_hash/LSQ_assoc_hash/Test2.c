#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <Windows.h>
#include <math.h>
#include "linear_sequence_assoc_hash.h"

#define TEST { test_line = __LINE__; test_init(); } {
#define ENDTEST } { test_teardown(); test_line = 0; }
#define SEQ(T) ((TypeHash*)(T))
#define ITR(T) ((TypeIterator*)(T))
#define test_assert(expr) { test_line = __LINE__; test_assert_impl(expr); }
#define test_assert_seq { test_line = __LINE__; } test_assert_seq_impl
#define ITER_VAL(iter) (*LSQ_DereferenceIterator(iter))
#define TypeBucket int



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
	TypeBucket bucket;
	TypeStateIterator state;
} TypeIterator;

LSQ_HandleT seq;
LSQ_IteratorT iter;

void* keyCloneFunc(int x){
	return(x);
}
unsigned int keySizeFunc(void *x){
	return(4);
}
int keyCompFunc(void *x, void *y){
	return(x == y);
}
void test_init(){
	seq = LSQ_CreateSequence(&keyCloneFunc,&keySizeFunc,&keyCompFunc,&keyCloneFunc);
}

void test_teardown(){
	int i;
	for(i = 0; i < 10000; i++)
		a[i] = 0;
    LSQ_DestroySequence(seq);
}

void test_fail(){
	char s;
   printf("Test failed! Line %d\n", test_line);
	scanf("%c",&s);
	exit(0);
}

void test_assert_impl(int value){
   if (!value) test_fail();
}

void test_assert_seq_impl(LSQ_HandleT seq, int count, ...){
   va_list vl;
   LSQ_IteratorT it;
	int i, OK, x;
	int b[1000];
	va_start(vl, count);

	for(i = 0; i < count; i++)
		b[i] = va_arg(vl, int);
	va_end(vl);

	if (LSQ_GetSize(seq) != count)
		test_fail();

	for (it = LSQ_GetFrontElement(seq); !LSQ_IsIteratorPastRear(it); LSQ_AdvanceOneElement(it)){
      if (count == 0)
			test_fail();
		x = *(int*)LSQ_DereferenceIterator(it);
		OK = 0;

		for(i = 0; i < count; i++)
			if(x == a[i]){
				OK = 1;
				b[i] = - 999;
				break;     //привет корова^^   
			}
		if(OK = 0)
			test_fail();
		count--;
   }
   if (count != 0) test_fail();
   LSQ_DestroyIterator(it);
}

void seq_push(LSQ_HandleT seq, int count, ...){
   int i;
   va_list vl;

	va_start(vl, count);

	for(i = 0; i < count; i++)
		a[i] = va_arg(vl, int);
	va_end(vl);
   
	for (i = 0; i < count; i++)
      LSQ_InsertElement(seq, &a[i], &a[i]);

	size = count;
}

void dump(LSQ_HandleT seq)
{
   LSQ_IteratorT it;

	printf("\n");
	printf("\n");
	for (it = LSQ_GetFrontElement(seq); !LSQ_IsIteratorPastRear(it); LSQ_AdvanceOneElement(it))
       printf("%d\n", *(int*)LSQ_DereferenceIterator(it));
   LSQ_DestroyIterator(it);
}

void seq_add(LSQ_HandleT seq, int count, ...){
	int i;
	va_list vl;

	va_start(vl, count);

	for(i = size; i < count + size; i++)
		a[i] = va_arg(vl, int);

	for(i = size; i < count + size; i++)
		LSQ_InsertElement(seq, &a[i], &a[i]);

	size += count;
}	

void seq_del(LSQ_HandleT seq, int x){
	int i, j;
	int* key = NULL;

	for(i = 0; i < size; i++)
	   if(a[i] == x){
			key = &a[i];
			break;
		}

	LSQ_DeleteElement(seq, key);

	size--;
}
int* GetKey(int key){
	return(&key);
}
int main(){
   int i, count;
	
	TEST
		seq_push(seq, 5, 1, 2, 3, 4, 5);
		test_assert_seq(seq, 5, 1, 2, 3, 4, 5);
		dump(seq);

		seq_del(seq, 4);
		dump(seq);
	ENDTEST
	


	TEST
		count = 1;
		iter = LSQ_GetFrontElement(seq);
		printf("%d",(int)LSQ_DereferenceIterator(iter));

		LSQ_AdvanceOneElement(iter);
		printf("%d",(int)LSQ_DereferenceIterator(iter));

		LSQ_AdvanceOneElement(iter);
		printf("%d",(int)LSQ_DereferenceIterator(iter));

		LSQ_AdvanceOneElement(iter);
		test_assert(LSQ_IsIteratorPastRear(iter) == 1);
		LSQ_DestroyIterator(iter);
	ENDTEST

	

	printf("All tests passed!\n");
}

