// Runtime support for destroying global objects at exit.
// Each constructed global registers a node; the chain is unwound in reverse.

typedef void (*DestructorFunc)(void* object, int mode);

typedef struct DestructorChain {
    struct DestructorChain* next;
    DestructorFunc destructor;
    void* object;
} DestructorChain;

DestructorChain* __global_destructor_chain;

void* __register_global_object(void* object, void* destructor, void* regmem) {
    DestructorChain* node = (DestructorChain*)regmem;

    node->next = __global_destructor_chain;
    node->destructor = (DestructorFunc)destructor;
    node->object = object;
    __global_destructor_chain = node;
    return object;
}

void __destroy_global_chain(void) {
    DestructorChain* node;

    while ((node = __global_destructor_chain) != 0) {
        __global_destructor_chain = node->next;
        node->destructor(node->object, -1);
    }
}

// The linker walks .dtors at exit; this entry makes it unwind the chain.
// It must be weak and live in .dtors$15 so the linker merges it with the
// entry it generates itself instead of adding a second one.
#pragma section ".dtors$15"
__declspec(section ".dtors$15") __declspec(weak)
    extern void* const __destroy_global_chain_reference = __destroy_global_chain;
