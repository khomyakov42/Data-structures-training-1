#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "linear_sequence.h"

#define TEST { test_line = __LINE__; test_init(); } {
#define ENDTEST } { test_teardown(); test_line = 0; }

#define test_assert(expr) { test_line = __LINE__; test_assert_impl(expr); }
#define test_assert_seq { test_line = __LINE__; } test_assert_seq_impl
#define ITER_VAL(iter) (*LSQ_DereferenceIterator(iter))

int test_line;

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

void test_fail()
{
    fprintf(stderr, "Test failed! Line %d\n", test_line);
    exit(EXIT_FAILURE);
}

void test_assert_impl(int value)
{
    if (!value) test_fail();
}

void test_assert_seq_impl(LSQ_HandleT seq, int count, ...)
{
    va_list vl;
    LSQ_IteratorT it;

    if (LSQ_GetSize(seq) != count) test_fail();
    va_start(vl, count);

    for (it = LSQ_GetFrontElement(seq); !LSQ_IsIteratorPastRear(it); LSQ_AdvanceOneElement(it))
    {
        if (count == 0) test_fail();
        if (*LSQ_DereferenceIterator(it) != va_arg(vl, int)) test_fail();
        count--;
    }
    va_end(vl);
    if (count != 0) test_fail();
    LSQ_DestroyIterator(it);
}

void seq_push(LSQ_HandleT seq, int count, ...)
{
    int i;
    va_list vl;
    va_start(vl, count);
    
    for (i = 0; i < count; i++)
    {
        LSQ_InsertRearElement(seq, va_arg(vl, int));
    }
    
    va_end(vl);
}

void dump(LSQ_HandleT seq)
{
    LSQ_IteratorT it;
    for (it = LSQ_GetFrontElement(seq); !LSQ_IsIteratorPastRear(it); LSQ_AdvanceOneElement(it))
    {
        printf("%d\n", *LSQ_DereferenceIterator(it));
    }
    LSQ_DestroyIterator(it);
}

int main()
{
    int i, count;


    TEST
        test_assert(LSQ_GetSize(seq) == 0);

        LSQ_InsertFrontElement(seq, 1);
        test_assert_seq(seq, 1, 1);

        LSQ_InsertFrontElement(seq, 2);
        test_assert_seq(seq, 2, 2,1);

        LSQ_InsertRearElement(seq, 3);
        test_assert_seq(seq, 3, 2,1,3);
    ENDTEST

    TEST
        seq_push(seq, 4, 1,2,3,4);
        iter = LSQ_GetFrontElement(seq);
        test_assert(LSQ_IsIteratorDereferencable(iter));
        test_assert(!LSQ_IsIteratorBeforeFirst(iter));
        test_assert(!LSQ_IsIteratorPastRear(iter));
        test_assert(ITER_VAL(iter) == 1);
                                         
        LSQ_RewindOneElement(iter);
        test_assert(LSQ_IsIteratorBeforeFirst(iter));
        test_assert(!LSQ_IsIteratorDereferencable(iter));
        
        LSQ_AdvanceOneElement(iter);
        test_assert(LSQ_IsIteratorDereferencable(iter));
        test_assert(ITER_VAL(iter) == 1);
        
        LSQ_ShiftPosition(iter, 3);
        test_assert(ITER_VAL(iter) == 4);
        
        LSQ_AdvanceOneElement(iter);
        test_assert(LSQ_IsIteratorPastRear(iter));
        
        LSQ_ShiftPosition(iter, -1);
        test_assert(LSQ_IsIteratorDereferencable(iter));
        
        LSQ_SetPosition(iter, 1);
        test_assert(ITER_VAL(iter) == 2);
        
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 3, 1,2,3);
        
        iter = LSQ_GetPastRearElement(seq);
        test_assert(LSQ_IsIteratorPastRear(iter));
        LSQ_DestroyIterator(iter);
        
        iter = LSQ_GetElementByIndex(seq, 1);
        test_assert(ITER_VAL(iter) == 2);
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 2, 3,4);
        test_assert_seq(seq, 2, 3,4);
        
        LSQ_InsertFrontElement(seq, 1);
        test_assert_seq(seq, 3, 1,3,4);
        
        LSQ_InsertRearElement(seq, 5);
        test_assert_seq(seq, 4, 1,3,4,5);
        
        iter = LSQ_GetElementByIndex(seq, 1);
        LSQ_InsertElementBeforeGiven(iter, 2);
        test_assert_seq(seq, 5, 1,2,3,4,5);
        
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 5, 1,2,3,4,5);
        
        LSQ_DeleteFrontElement(seq);
        test_assert_seq(seq, 4, 2,3,4,5);
        
        LSQ_DeleteRearElement(seq);
        test_assert_seq(seq, 3, 2,3,4);
        
        iter = LSQ_GetElementByIndex(seq, 1);
        LSQ_DeleteGivenElement(iter);
        LSQ_DestroyIterator(iter);
        test_assert_seq(seq, 2, 2,4);
        
        iter = LSQ_GetElementByIndex(seq, 1);
        LSQ_DeleteGivenElement(iter);
        LSQ_DestroyIterator(iter);
        test_assert_seq(seq, 1, 2);
    ENDTEST

    TEST
        iter = LSQ_GetFrontElement(seq);
        test_assert(LSQ_IsIteratorPastRear(iter));
        LSQ_DestroyIterator(iter);
        
        iter = LSQ_GetPastRearElement(seq);
        test_assert(LSQ_IsIteratorPastRear(iter));
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 1, 1);
        LSQ_DeleteFrontElement(seq);
        test_assert_seq(seq, 0);
        
        seq_push(seq, 1, 1);
        LSQ_DeleteRearElement(seq);
        test_assert_seq(seq, 0);
    ENDTEST
    
    TEST
        seq_push(seq, 5, 1,2,3,4,5);
        
        count = 5;
        iter = LSQ_GetElementByIndex(seq, LSQ_GetSize(seq) - 1);
        for (; !LSQ_IsIteratorBeforeFirst(iter); LSQ_RewindOneElement(iter), count--)
        {
            if (count < 0) test_fail();
        }
        if (count != 0) test_fail();
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 3, 1,2,3);
        iter = LSQ_GetFrontElement(seq);
        
        LSQ_ShiftPosition(iter, -10);
        test_assert(LSQ_IsIteratorBeforeFirst(iter));
        
        LSQ_ShiftPosition(iter, 20);
        test_assert(LSQ_IsIteratorPastRear(iter));
        
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 3, 1,2,3);
        iter = LSQ_GetFrontElement(seq);
        
        LSQ_SetPosition(iter, -10);
        test_assert(LSQ_IsIteratorBeforeFirst(iter));
        
        LSQ_SetPosition(iter, 20);
        test_assert(LSQ_IsIteratorPastRear(iter));
        
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 3, 3,2,1);
        iter = LSQ_GetElementByIndex(seq, 1000);
        test_assert(LSQ_IsIteratorPastRear(iter));
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        for (i = 0; i < 1000; i++)
        {
            LSQ_InsertRearElement(seq, 1000 - i);
        }
        
        for (i = 0; i < 1000; i++)
        {
            iter = LSQ_GetElementByIndex(seq, i);
            test_assert(ITER_VAL(iter) == 1000 - i);
            LSQ_DestroyIterator(iter);
        }
    ENDTEST
    
    TEST
        LSQ_DeleteFrontElement(seq);
        LSQ_DeleteRearElement(seq);
        test_assert_seq(seq, 0);
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
        
        LSQ_InsertFrontElement(LSQ_HandleInvalid, 0);
        LSQ_InsertRearElement(LSQ_HandleInvalid, 0);
        LSQ_InsertElementBeforeGiven(LSQ_HandleInvalid, 0);
        LSQ_DeleteFrontElement(LSQ_HandleInvalid);
        LSQ_DeleteRearElement(LSQ_HandleInvalid);
        LSQ_DeleteGivenElement(LSQ_HandleInvalid);
    ENDTEST
    
    TEST
        seq_push(seq, 0);
        iter = LSQ_GetFrontElement(seq);
        LSQ_SetPosition(iter, 10);
        LSQ_DestroyIterator(iter);
        
        iter = LSQ_GetFrontElement(seq);
        LSQ_SetPosition(iter, -10);
        LSQ_DestroyIterator(iter);
    ENDTEST
    
    TEST
        seq_push(seq, 1, 1);
        iter = LSQ_GetElementByIndex(seq, -1);
        LSQ_DeleteGivenElement(iter);
        LSQ_DestroyIterator(iter);
        
        iter = LSQ_GetElementByIndex(seq, 1);
        LSQ_DeleteGivenElement(iter);
        LSQ_DestroyIterator(iter);
    ENDTEST

    TEST
        seq_push(seq, 5, 10,20,30,40,50);
        iter = LSQ_GetElementByIndex(seq, 3);
        LSQ_InsertElementBeforeGiven(iter, 35);
        LSQ_DestroyIterator(iter);
        iter = LSQ_GetPastRearElement(seq);
        LSQ_ShiftPosition(iter, -6);
        test_assert(ITER_VAL(iter) == 10);
    ENDTEST
    
    TEST /* RE 19 */
        seq_push(seq, 5, 1,2,3,4,5);
        iter = LSQ_GetElementByIndex(seq, 2);
        LSQ_DeleteGivenElement(iter);
        test_assert(ITER_VAL(iter) == 4);
    ENDTEST
    
    TEST /* RE 35 */
        seq_push(seq, 5, 1,2,3,4,5);
        iter = LSQ_GetElementByIndex(seq, 2);
        LSQ_InsertElementBeforeGiven(iter, 9);
        test_assert(ITER_VAL(iter) == 9);
    ENDTEST
    
    printf("All tests passed!\n");
    return EXIT_SUCCESS;
}

