#include "vector.h"

#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define VEC_GROW_RATE 2
#define VEC_DEF_SIZE  1

static void* offset(const Vector* vec, size_t i) {
    assert(i <= vec->capacity);
    return ((unsigned char*)vec->data) + vec->elemSize * i;
}

static void reallocVector(Vector* vec, size_t capacity) {
    vec->capacity = capacity;
    vec->data = realloc(vec->data, vec->elemSize * capacity);
    assert(vec->data);
}

static void growVector(Vector* vec) {
    size_t newCapacity = vec->capacity ? vec->capacity * VEC_GROW_RATE : VEC_DEF_SIZE;
    reallocVector(vec, newCapacity);
}

static bool shouldGrow(Vector* vec, size_t required) {
    return vec->size + required > vec->capacity;
}

static void reset(Vector* vec) {
    vec->size = 0;
    vec->capacity = 0;
    vec->elemSize = 0;
    vec->data = NULL;
}

void vecInit(Vector* vec, size_t elemSize) {
    reset(vec);
    vec->elemSize = elemSize;
}

Vector vecNew(size_t elemSize) {
    Vector vec;
    vecInit(&vec, elemSize);
    return vec;
}

Vector vecCopy(const Vector* vec) {
    Vector copy;
    vecInit(&copy, vec->elemSize);
    vecReserve(&copy, vec->size);
    memcpy(copy.data, vec->data, vec->size * vec->elemSize);
    copy.size = vec->size;
    return copy;
}

void vecCopyAssign(Vector* dest, const Vector* src) {
    assert(vecIsInitialized(dest));
    if(dest->data) free(dest->data);
    vecInit(dest, src->elemSize);
    vecReserve(dest, src->size);
    memcpy(dest->data, src->data, src->size * src->elemSize);
    dest->size = src->size;
}

Vector vecMove(Vector* vec) {
    Vector moved = *vec;
    reset(vec);
    return moved;
}

void vecMoveAssign(Vector* dest, Vector* src) {
    assert(vecIsInitialized(dest));
    vecFree(dest);
    *dest = *src;
    reset(src);
}

void vecFree(Vector* vec) {
    if(vec->data) free(vec->data);
    reset(vec);
}

void* vecGet(Vector* vec, size_t i) {
    assert(i < vec->size);
    return offset(vec, i);
}

const void* vecConstGet(const Vector* vec, size_t i) {
    assert(i < vec->size);
    return offset(vec, i);
}

void* vecData(Vector* vec) {
    return vec->data;
}

const void* vecConstData(Vector* vec) {
    return vec->data;
}

void* vecFront(Vector* vec) {
    return vec->data;
}

const void* vecConstFront(const Vector* vec) {
    return vec->data;
}

void* vecBack(Vector* vec) {
    return offset(vec, vec->size - 1);
}

const void* vecConstBack(const Vector* vec) {
    return offset(vec, vec->size - 1);
}

void vecClear(Vector* vec) {
    vec->size = 0;
}

size_t vecPush(Vector* vec, void* elem) {
    if(shouldGrow(vec, 1)) growVector(vec);
    memcpy(offset(vec, vec->size), elem, vec->elemSize);
    return vec->size++;
}

void vecSet(Vector* vec, size_t i, void* elem) {
    assert(i < vec->size);
    memcpy(offset(vec, i), elem, vec->elemSize);
}

void* vecInsert(Vector* vec, size_t i, void* elem) {
    assert(i <= vec->size);
    if(shouldGrow(vec, 1)) growVector(vec);
    size_t shiftRight = (vec->size - i) * vec->elemSize;
    void* insertIt = offset(vec, i);
    memmove(offset(vec, i + 1), insertIt, shiftRight);
    memcpy(insertIt, elem, vec->elemSize);
    vec->size++;
    return insertIt;
}

void* vecErase(Vector* vec, size_t i) {
    assert(i <= vec->size);
    size_t shiftLeft = (vec->size - i - 1) * vec->elemSize;
    void* removeIt = offset(vec, i);
    memmove(removeIt, offset(vec, i + 1), shiftLeft);
    vec->size--;
    return removeIt;
}

void vecPop(Vector* vec) {
    assert(vec->size);
    vec->size--;
}

void vecSwap(Vector* v1, Vector* v2) {
    Vector tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

bool vecEmpty(const Vector* vec) {
    return vec->size == 0;
}

size_t vecSize(const Vector* vec) {
    return vec->size;
}

size_t vecCapacity(const Vector* vec) {
    return vec->capacity;
}

bool vecIsInitialized(const Vector* vec) {
    return vec->elemSize != 0;
}

void vecReserve(Vector* vec, size_t required) {
    if(!shouldGrow(vec, required)) return;
    size_t newCapacity = vec->capacity ? vec->capacity * VEC_GROW_RATE : VEC_DEF_SIZE;
    while(newCapacity < vec->size + required) {
        newCapacity <<= 1;
    }
    reallocVector(vec, newCapacity);
}

void vecShrinkToFit(Vector* vec) {
    if(vec->capacity > vec->size) {
        reallocVector(vec, vec->size);
    }
}

void* vecBegin(Vector* vec) {
    return vec->data;
}

void* vecEnd(Vector* vec) {
    return offset(vec, vec->size);
}

void* vecIterator(Vector* vec, size_t i) {
    assert(i <= vec->size);
    return offset(vec, i);
}

size_t vecIteratorIndex(const Vector* vec, void* it) {
    ptrdiff_t i = (intptr_t)it - (intptr_t)vec->data;
    assert(i >= 0);
    return i / vec->elemSize;
}
