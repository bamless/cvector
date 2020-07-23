#undef __STRICT_ANSI__
#define CTEST_MAIN
#define CTEST_SEGFAULT
#define CTEST_COLOR_OK
#include "ctest.h"
#include "vector.h"

int main(int argc, const char** argv) {
    return ctest_main(argc, argv);
}

CTEST(vector, constructor) {
    Vector vec1 = vecNew(sizeof(int));
    ASSERT_TRUE(vecIsInitialized(&vec1));

    Vector vec2;
    vecInit(&vec2, sizeof(int));
    ASSERT_TRUE(vecIsInitialized(&vec2));

    vecFree(&vec1);
    vecFree(&vec2);
    ASSERT_FALSE(vecIsInitialized(&vec1));
    ASSERT_FALSE(vecIsInitialized(&vec2));
}

CTEST(vector, push) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 20; i++) {
        ASSERT_EQUAL(vecPush(&vec, &i), (size_t)i);
    }

    // Don't use foreach or vecGet to minimize test dependency
    for(size_t i = 0; i < vec.size; i++) {
        ASSERT_EQUAL(((int*)vec.data)[i], (int)i);
    }

    vecFree(&vec);
}

CTEST(vector, get) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    ASSERT_EQUAL(*(int*)vecGet(&vec, 3), 3);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 0), 0);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 9), 9);

    vecFree(&vec);
}

CTEST(vector, set) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    int x = 100;
    vecSet(&vec, 4, &x);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 4), x);
    vecSet(&vec, 0, &x);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 0), x);
    vecSet(&vec, 9, &x);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 9), x);

    vecFree(&vec);
}

CTEST(vector, copy_construct) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    Vector cpy = vecCopy(&vec);
    for(size_t i = 0; i < vecSize(&vec); i++) {
        ASSERT_EQUAL(*(int*)vecGet(&vec, i), *(int*)vecGet(&cpy, i));
    }

    Vector cpyAssign = vecNew(sizeof(int));
    for(size_t i = 0; i < 5; i++) {
        vecPush(&cpyAssign, &i);
    }

    vecCopyAssign(&cpyAssign, &vec);
    for(size_t i = 0; i < vecSize(&vec); i++) {
        ASSERT_EQUAL(*(int*)vecGet(&vec, i), *(int*)vecGet(&cpyAssign, i));
    }

    vecFree(&vec);
    vecFree(&cpy);
    vecFree(&cpyAssign);
}

CTEST(vector, move_construct) {
    const size_t vecSize = 10;

    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < (int)vecSize; i++) {
        vecPush(&vec, &i);
    }

    Vector mov = vecMove(&vec);
    ASSERT_FALSE(vecIsInitialized(&vec));

    for(size_t i = 0; i < vecSize; i++) {
        ASSERT_EQUAL(*(int*)vecGet(&mov, i), (int)i);
    }

    Vector movAssign = vecNew(sizeof(int));
    for(size_t i = 0; i < 5; i++) {
        vecPush(&movAssign, &i);
    }

    vecMoveAssign(&movAssign, &mov);
    ASSERT_FALSE(vecIsInitialized(&mov));

    for(size_t i = 0; i < vecSize; i++) {
        ASSERT_EQUAL(*(int*)vecGet(&movAssign, i), (int)i);
    }

    vecFree(&vec);
    vecFree(&mov);
    vecFree(&movAssign);
}

CTEST(vector, foreach) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    {
        int i = 0;
        for(int* it = vecBegin(&vec); it != vecEnd(&vec); it++) {
            ASSERT_EQUAL(*it, i++);
        }
    }

    {
        int i = 0;
        vecForeach(int* it, vec) { ASSERT_EQUAL(*it, i++); }
    }

    vecFree(&vec);
}

CTEST(vector, front_back) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    ASSERT_EQUAL(*(int*)vecFront(&vec), *(int*)vecGet(&vec, 0));
    ASSERT_EQUAL(*(int*)vecBack(&vec), *(int*)vecGet(&vec, vecSize(&vec) - 1));

    vecFree(&vec);
}

CTEST(vector, clear) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }
    vecClear(&vec);
    ASSERT_EQUAL_U(vecSize(&vec), 0);
    vecFree(&vec);
}

CTEST(vector, insert) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    int a = 10, b = 20, c = 30;
    ASSERT_EQUAL(*(int*)vecInsert(&vec, 0, &a), a);
    ASSERT_EQUAL(*(int*)vecInsert(&vec, 4, &b), b);
    ASSERT_EQUAL(*(int*)vecInsert(&vec, 10, &c), c);

    ASSERT_EQUAL(*(int*)vecGet(&vec, 0), a);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 4), b);
    ASSERT_EQUAL(*(int*)vecGet(&vec, 10), c);

    ASSERT_EQUAL(vecSize(&vec), 13);

    vecFree(&vec);
}

CTEST(vector, erase) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    ASSERT_EQUAL(*(int*)vecErase(&vec, 0), 1);
    ASSERT_EQUAL(*(int*)vecErase(&vec, 4), 6);
    ASSERT_TRUE(vecErase(&vec, 7) == vecEnd(&vec));
    ASSERT_EQUAL(vecSize(&vec), 7);

    vecFree(&vec);
}

CTEST(vector, pop) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    vecPop(&vec);
    ASSERT_EQUAL(*(int*)vecGet(&vec, vecSize(&vec) - 1), 8);
    ASSERT_EQUAL(vecSize(&vec), 9);

    vecFree(&vec);
}

CTEST(vector, reserve) {
    const size_t required = 20;

    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    size_t oldSize = vecSize(&vec);
    ASSERT_TRUE(vecCapacity(&vec) - vecSize(&vec) < required);
    vecReserve(&vec, required);
    ASSERT_TRUE(oldSize + required <= vecCapacity(&vec));

    // check for validity
    int i = 0;
    vecForeach(int* it, vec) { ASSERT_EQUAL(*it, i++); }

    size_t oldCapacity = vecCapacity(&vec);
    vecReserve(&vec, 0);
    ASSERT_EQUAL_U(vecCapacity(&vec), oldCapacity);

    vecFree(&vec);
}

CTEST(vector, shrink) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    ASSERT_NOT_EQUAL(vecSize(&vec), vecCapacity(&vec));
    vecShrinkToFit(&vec);
    ASSERT_EQUAL(vecSize(&vec), vecCapacity(&vec));

    vecFree(&vec);
}

CTEST(vector, iter_index) {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i);
    }

    void* it1 = vecIterator(&vec, 4);
    ASSERT_EQUAL(vecIteratorIndex(&vec, it1), 4);
    void* it2 = vecIterator(&vec, 0);
    ASSERT_EQUAL(vecIteratorIndex(&vec, it2), 0);

    vecFree(&vec);
}
