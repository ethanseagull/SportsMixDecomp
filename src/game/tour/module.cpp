// Entry points of the TourF module.

typedef void (*VoidFunc)();

extern "C" {

extern VoidFunc _ctors[];
extern VoidFunc _dtors[];

void _prolog();
void _epilog();
void _unresolved();
void TournamentModuleMain();

// Runs the module's static initialisers when it is loaded.
void _prolog() {
    for (VoidFunc* ctor = _ctors; *ctor != 0; ctor++) {
        (*ctor)();
    }
}

// Runs the module's static destructors when it is unloaded.
void _epilog() {
    for (VoidFunc* dtor = _dtors; *dtor != 0; dtor++) {
        (*dtor)();
    }
}

// Target of every import that could not be resolved.
void _unresolved() {
}

void TournamentModuleMain() {
}

} // extern "C"
