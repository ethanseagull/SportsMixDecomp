// Entry points of the BossF module: the loader calls _prolog after linking
// the module and _epilog before unloading it.

typedef void (*VoidFunc)(void);

extern "C" {

extern VoidFunc _ctors[];
extern VoidFunc _dtors[];

void _prolog(void);
void _epilog(void);
void _unresolved(void);
void BossModuleMain(void);

// Runs the module's static initialisers.
void _prolog(void) {
    VoidFunc* ctor;

    for (ctor = _ctors; *ctor != 0; ctor++) {
        (*ctor)();
    }
    BossModuleMain();
}

// Runs the module's static destructors.
void _epilog(void) {
    VoidFunc* dtor;

    for (dtor = _dtors; *dtor != 0; dtor++) {
        (*dtor)();
    }
}

// Target of every call the loader could not resolve.
void _unresolved(void) {
}

void BossModuleMain(void) {
}

}
