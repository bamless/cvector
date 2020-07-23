# cvector
![test](https://github.com/bamless/cvector/workflows/test/badge.svg)

This is a dynamic growable array implementation that makes an effort to mimic the C++ `std::vector` interface. The vector uses `void*` to store data internally, sacrificing type safety in order to
make it possible to store data of any type.

## How to use

The following example shows a typical usage of `Vector`:

```c
#include "vector.h"

int main() {
    // Create a vector of ints
    Vector vec = vecNew(sizeof(int));

    // Or alternatively:
    Vector vec;
    vecInit(&vec, sizeof(int))

    for(int i = 0; i < 10; i++) {
        vecPush(&vec, &i); // Copy `i` to the end of the vector
    }

    // Print the vector's elements
    vecForeach(int* it, vec) {
        printf("%d\n", *it);
    }

    // Free the vector's data
    vecFree(&vec);
}
```

The interace tries to emulate copy/move constructors and assignments using functions:

```c
#include "vector.h"

int main() {
    Vector vec = vecNew(sizeof(int));
    for(int i = 0; i < 5; i++) {
        vecPush(&vec, &i);
    }

    // 'Copy constructor', copies the contents of `vec` into `vecCpy`
    Vector vecCpy = vecCopy(&vec);

    // Copy assignments works similarly, but operates on a vector already 
    // initialized, freeing its element before copying the other vector's data
    Vector vec2 = vecNew(sizeof(int));
   
    // ... Optionally fill the vector ...

    // Copy the the contents of `vec` into `vec2`, releasing `vec2` data
    vecCopyAssign(&vec2, &vec);

    // Move the contents of `vec` into `vecMov`. Now `vec` is in
    // an 'uninitialized state' and shouldn't be sued anymore
    Vector vecMov = vecMove(&vec);

    // Move assignment
    Vector vec3 = vecNew(sizeof(int));
    
    // ... Optionally fill the vector ...

    // Similar to the above, but releases `vec3` data
    vecMoveAssign(&vec3, &vec);
}
```

Beware that assignment 'operators' should be used only on previously initialized vectors, else 
undefined behaviour is invoked. A vector is initialized after a call to `vecNew` or `vecInit` and
unninitialized after the corresponding `vecFree` call. You can even query the vector's state
using the `vecIsInitialized(Vector *vec)` function.

Iterators are also supported in the form of a pointer to the underlying data:
```c
#include "vector.h"

int main() {
    Vector vec = vecNew(sizeof(int));
    // ... Fill the vector ... 

    // A pointer to the first element of the vector
    int* iteratorToStart = vecBegin(&vec);

    // A pointer to one past the end of the vector
    int* iteratorToEnd = vecBegin(&vec);

    // A pointer to the fifth element of the vector
    int* iterator = vecIterator(&vec, 5);

    // You can even retrieve an index from an iterator
    size_t index = vecIteratorIndex(&vec, iterator);

    // You can then iterate over the vector like this:
    for(int *it = vecBegin(&vec), it != vecEnd(&vec); it++) {
        printf("%d\n", *it);
    }

    // Alternatively you can use the vecForeach helper macro
    vecForeach(int *it, vec) {
        printf("%d\n", *it);
    }
}
```

## Integration

You can simply copy `vector.h` and `vector.c` inside your project and you're done!

## Compilation

A simple Makefile is provided for building a static library if you prefer linking instead of copying 
the files in your project. Simply input this in the terminal:

`make config=Release cvector`

supported `config` values are `Debug` and `Release`.
