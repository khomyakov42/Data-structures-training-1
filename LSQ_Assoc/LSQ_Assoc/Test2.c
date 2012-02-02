#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <Windows.h>
#include "linear_sequence_assoc.h"

#define TEST { test_line = __LINE__; test_init(); } {
#define ENDTEST } { test_teardown(); test_line = 0; }

#define test_assert(expr) { test_line = __LINE__; test_assert_impl(expr); }
#define test_assert_seq { test_line = __LINE__; } test_assert_seq_impl
#define ITER_VAL(iter) (*LSQ_DereferenceIterator(iter))

#define ITR(T) ((LSQ_TypeIterator*)(T))
#define SEQ(T) ((LSQ_TypeTree*)(T))
unsigned long R=0;
#define Random(Max) ((R=(R*9301L+49267L)%233280L)%(long)Max)

typedef struct Node{
	struct Node* parent;
	struct Node* left;
	struct Node* right;
	LSQ_IntegerIndexT key;
	LSQ_BaseTypeT value;
	int height;
} LSQ_TypeNode;

typedef struct{
	LSQ_TypeNode *root;
	int size;
} LSQ_TypeTree;

typedef enum{
	IS_BEFOR_FIRST,
	IS_PAST_REAR,
	IS_NORMAL
}LSQ_TypeStateIterator;

typedef struct{
	LSQ_TypeStateIterator state;
	LSQ_TypeNode *node;
	LSQ_TypeTree *tree;
}LSQ_TypeIterator;

typedef enum{
	KEY,
	VALUE,
	BALANCE
} TypeWriteTree;

int test_line, depth;

LSQ_HandleT seq;
LSQ_IteratorT iter;

void test_init()
{
    seq = LSQ_CreateSequence();
}

void test_teardown()
{
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
	if (LSQ_GetSize(seq) != count) test_fail();
   va_start(vl, count);
	for (it = LSQ_GetFrontElement(seq); !LSQ_IsIteratorPastRear(it); LSQ_AdvanceOneElement(it)){
      if (count == 0) test_fail();
      if (*LSQ_DereferenceIterator(it) != va_arg(vl, int)) test_fail();
      count--;
   }
   va_end(vl);
   if (count != 0) test_fail();
   LSQ_DestroyIterator(it);
}

void seq_push(LSQ_HandleT seq, int count, ...){
   int i;
	int k;
   va_list vl;
   va_start(vl, count);
   
   for (i = 0; i < count; i++){
	   k = va_arg(vl, int);
      LSQ_InsertElement(seq, k, k);
   }
  
   va_end(vl);
}

void dump(LSQ_HandleT seq)
{
   LSQ_IteratorT it;
	for (it = LSQ_GetFrontElement(seq); !LSQ_IsIteratorPastRear(it); LSQ_AdvanceOneElement(it)){
       printf("%d\n", *LSQ_DereferenceIterator(it));
   }
   LSQ_DestroyIterator(it);
}
	
extern int GetHeight(LSQ_TypeNode *node){
	return(node == NULL)? 0: node->height;
}

void TreeOut(LSQ_TypeNode* node,TypeWriteTree statewrite, int x, int y ,int depth){
	COORD position = {0,0}; //позиция x и y 79 символов длина консоли 3*2 *h
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int s;
	if(node == NULL)
		return;
	if(depth <= 1)
		system("cls");
	if(x == 0)
		x = 40; 
	position.X = x;
	position.Y = y;
	SetConsoleCursorPosition(hConsole, position);
	
	switch (statewrite) {
	case KEY:s = node->key;break;
	case VALUE:s = node->value;break;
	case BALANCE:s = GetHeight(node->right) - GetHeight(node->left);break;
	}
	
	printf("%d\n",s);
	
	position.X = x - 1;//(40/(depth*2));
	position.Y = y + 1;
	SetConsoleCursorPosition(hConsole, position);
	if(node->left != NULL)
		printf("%c",47);
	
	if(node->right != NULL)
		printf(" %c",92);

	TreeOut(node->left, statewrite, x - 3 - depth, y + 2, depth + 1);
	TreeOut(node->right, statewrite, x + 3 + depth, y + 2, depth + 1);
}


int main()
{
   int i,j, count, a[10];

	TEST
		test_assert(LSQ_GetSize(seq) == 0);
		LSQ_InsertElement(seq, 2, 2);
		test_assert_seq(seq, 1, 2);

		LSQ_InsertElement(seq, 1, 1);
		test_assert_seq(seq, 2, 1, 2);

		LSQ_InsertElement(seq, 3, 3);
		test_assert_seq(seq, 3, 1, 2, 3);

		LSQ_InsertElement(seq, 5, 5);
		test_assert_seq(seq, 4, 1, 2, 3, 5);

		LSQ_InsertElement(seq, 4, 4);
		test_assert_seq(seq, 5, 1, 2, 3, 4, 5);
		TreeOut(SEQ(seq)->root,KEY,0,0,1); 
	ENDTEST
	
	TEST
		seq_push(seq,7, 7, 8, 3, 5, 4, 2, 9);
		LSQ_DeleteFrontElement(seq);
		test_assert_seq(seq, 6, 3, 4, 5, 7, 8, 9);

		LSQ_DeleteRearElement(seq);
		test_assert_seq(seq, 5, 3, 4, 5, 7, 8);

		LSQ_DeleteElement(seq, 4);
		test_assert_seq(seq, 4, 3, 5, 7, 8);

		LSQ_DeleteElement(seq, 7);
		test_assert_seq(seq, 3, 3, 5, 8);

		LSQ_DeleteElement(seq, 5);
		test_assert_seq(seq, 2, 3, 8);

		LSQ_DeleteRearElement(seq);
		test_assert_seq(seq, 1, 3);

		LSQ_DeleteFrontElement(seq);
		test_assert(LSQ_GetSize(seq) == 0);


		seq_push(seq,7, -7, -8, -3, -5, -4, -2, -9);
		LSQ_DeleteFrontElement(seq);
		test_assert_seq(seq, 6, -8, -7, -5, -4, -3, -2);

		LSQ_DeleteRearElement(seq);
		test_assert_seq(seq, 5, -8, -7, -5, -4, -3);

		LSQ_DeleteElement(seq, -4);
		test_assert_seq(seq, 4, -8, -7, -5, -3);

		LSQ_DeleteElement(seq, -7);
		test_assert_seq(seq, 3, -8, -5, -3);

		LSQ_DeleteElement(seq, -5);
		test_assert_seq(seq, 2, -8, -3);

		LSQ_DeleteRearElement(seq);
		test_assert_seq(seq, 1, -8);

		LSQ_DeleteFrontElement(seq);
		test_assert(LSQ_GetSize(seq) == 0);
	ENDTEST
	
	TEST
		seq_push(seq, 6, 0, 1 , 2, 3, 4, 5);

		iter = LSQ_GetFrontElement(seq);

		test_assert(*LSQ_DereferenceIterator(iter) == 0);
		test_assert(LSQ_GetIteratorKey(iter) == 0);

		for(i = 0; i < 5; i++)
			LSQ_AdvanceOneElement(iter);

		test_assert(LSQ_GetIteratorKey(iter) == 5);
		test_assert(*LSQ_DereferenceIterator(iter) == 5);

		LSQ_AdvanceOneElement(iter);
		test_assert(LSQ_IsIteratorPastRear(iter));

		LSQ_AdvanceOneElement(iter);
		test_assert(LSQ_IsIteratorPastRear(iter));

		for(i = 0; i < 7; i++)
			LSQ_RewindOneElement(iter);

		test_assert(LSQ_IsIteratorBeforeFirst(iter));
		LSQ_DestroyIterator(iter);
	ENDTEST

	TEST
		seq_push(seq ,4, 0, 2, 4, 7);
		iter = LSQ_GetPastRearElement(seq);

		test_assert(LSQ_IsIteratorPastRear(iter));

		LSQ_RewindOneElement(iter);
		test_assert(LSQ_GetIteratorKey(iter) == 7);
		test_assert(*LSQ_DereferenceIterator(iter) == 7);

		LSQ_AdvanceOneElement(iter);
		test_assert(LSQ_IsIteratorPastRear(iter));

		for(i = 0; i < 4; i++)
			LSQ_RewindOneElement(iter);

		test_assert(LSQ_GetIteratorKey(iter) == 0);
		test_assert(*LSQ_DereferenceIterator(iter) == 0);

		LSQ_RewindOneElement(iter);
		test_assert(LSQ_IsIteratorBeforeFirst(iter));
		
		LSQ_RewindOneElement(iter);
		test_assert(LSQ_IsIteratorBeforeFirst(iter));
		LSQ_DestroyIterator(iter);
	ENDTEST

	TEST
		seq_push(seq, 4, 0, 1, 2, 3);
		iter = LSQ_GetFrontElement(seq);

		test_assert(LSQ_GetIteratorKey(iter) == 0);
		LSQ_ShiftPosition(iter,1);
		test_assert(LSQ_GetIteratorKey(iter) == 1);

		LSQ_ShiftPosition(iter, 3);
		test_assert(LSQ_IsIteratorPastRear(iter));

		LSQ_ShiftPosition(iter, 10);
		test_assert(LSQ_IsIteratorPastRear(iter));
		test_assert(LSQ_DereferenceIterator(iter) == NULL);

		LSQ_ShiftPosition(iter, -1);
		test_assert(LSQ_GetIteratorKey(iter) == 3);

		LSQ_ShiftPosition(iter, -3);
		test_assert(LSQ_GetIteratorKey(iter) == 0);

		LSQ_ShiftPosition(iter, -1);
		test_assert(LSQ_IsIteratorBeforeFirst(iter));

		LSQ_ShiftPosition(iter, -10);
		test_assert(LSQ_IsIteratorBeforeFirst(iter));

		test_assert(LSQ_DereferenceIterator(iter) == NULL);
	ENDTEST

	TEST
		seq_push(seq, 7, 7, 4, 2, 0, 1, 3, 9);
		TreeOut(SEQ(seq)->root,KEY,0,0,1);
		iter = LSQ_GetElementByIndex(seq, 2);

		LSQ_DeleteElement(seq, LSQ_GetIteratorKey(iter));
		test_assert_seq(seq, 6, 0, 1, 3, 4, 7, 9);
		TreeOut(SEQ(seq)->root,KEY,0,0,1);
		LSQ_SetPosition(iter, 3);
		test_assert(*LSQ_DereferenceIterator(iter) == 3);
		test_assert_seq(seq, 6, 0, 1, 4, 7, 9);

		LSQ_SetPosition(iter, 0);
		LSQ_DeleteElement(seq, LSQ_GetIteratorKey(iter));
		test_assert_seq(seq, 6, 1, 3, 4, 7, 9);

		LSQ_SetPosition(iter, 4);
		LSQ_DeleteFrontElement(seq);
		TreeOut(SEQ(seq)->root,KEY,0,0,1);
		LSQ_DeleteRearElement(seq);
		TreeOut(SEQ(seq)->root,KEY,0,0,1);
		LSQ_DeleteElement(seq, LSQ_GetIteratorKey(iter));
		TreeOut(SEQ(seq)->root,KEY,0,0,1);
		test_assert_seq(seq, 3, 1, 3, 7);

		LSQ_SetPosition(iter, 1);
		LSQ_ShiftPosition(iter, 0);
		test_assert(*LSQ_DereferenceIterator(iter) == 1);
		
		LSQ_DeleteElement(seq, LSQ_GetIteratorKey(iter));
		test_assert_seq(seq, 2, 3, 7);

		LSQ_SetPosition(iter, 3);
		LSQ_ShiftPosition(iter, 1000);
		LSQ_RewindOneElement(iter);
		LSQ_DeleteElement(seq, LSQ_GetIteratorKey(iter));
		LSQ_DeleteFrontElement(seq);
		test_assert(LSQ_GetSize == 0);

		LSQ_InsertElement(seq, 6, 6);
		LSQ_DeleteRearElement(seq);
		test_assert(LSQ_GetSize == 0);

		LSQ_DestroyIterator(iter);
	ENDTEST

	TEST
		for(i = 0; i <= 1000; i++)
			LSQ_InsertElement(seq,i,i);
		system("cls");
		for(iter = LSQ_GetFrontElement(seq), i = 0; !LSQ_IsIteratorPastRear(iter); i++, LSQ_AdvanceOneElement(iter)){
			if(LSQ_GetIteratorKey(iter) != i)
				test_fail;
		}
		LSQ_DestroyIterator(iter);
	ENDTEST

   TEST
      LSQ_DestroySequence(LSQ_HandleInvalid);
      LSQ_GetSize(LSQ_HandleInvalid);
      LSQ_IsIteratorDereferencable(LSQ_HandleInvalid);
      LSQ_IsIteratorPastRear(LSQ_HandleInvalid);
      LSQ_IsIteratorBeforeFirst(LSQ_HandleInvalid);
      LSQ_DereferenceIterator(LSQ_HandleInvalid);
      test_assert(LSQ_GetElementByIndex(LSQ_HandleInvalid, 0) == LSQ_HandleInvalid);
      test_assert(LSQ_GetFrontElement(LSQ_HandleInvalid) == LSQ_HandleInvalid);
      test_assert(LSQ_GetPastRearElement(LSQ_HandleInvalid) == LSQ_HandleInvalid);
      
      LSQ_DestroyIterator(LSQ_HandleInvalid);
      LSQ_AdvanceOneElement(LSQ_HandleInvalid);
      LSQ_RewindOneElement(LSQ_HandleInvalid);
      LSQ_ShiftPosition(LSQ_HandleInvalid, 0);
      LSQ_SetPosition(LSQ_HandleInvalid, 0);
      
      LSQ_DeleteFrontElement(LSQ_HandleInvalid);
      LSQ_DeleteRearElement(LSQ_HandleInvalid);
   ENDTEST

	TEST
		for(i = 0; i < 10; i++){
			for(j = 0; j < 10; j++)
				a[j] = Random(100);

			for(j = 0; j < 10; j++)
				LSQ_InsertElement(seq, a[j], a[j]);

			for(i = 0; i < 9; i++)
				for(j = 0; j < 9; j++)
					if(a[j]>a[j+1]){
						count = a[j];
						a[j] = a[j+1];
						a[j+1] = count;
					}

			TreeOut(SEQ(seq)->root, KEY, 0, 0, 1);

			for(iter = LSQ_GetFrontElement(seq), j = 0; !LSQ_IsIteratorPastRear(iter); LSQ_AdvanceOneElement(iter), j++)
				test_assert(*LSQ_DereferenceIterator(iter) == a[j]);
			LSQ_DestroyIterator(iter);
		}
	ENDTEST
	printf("All tests passed!\n");
}

