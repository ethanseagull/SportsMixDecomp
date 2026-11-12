// Runtime support: registers the program's exception tables (the linker's
// _eti_init_info) at start-up and removes them at exit. Both functions are
// reached only through the .ctors/.dtors entries at the end of this file.

typedef struct __eti_init_info __eti_init_info;

extern "C" {
extern __eti_init_info _eti_init_info[];

int __register_fragment(__eti_init_info *info, char *TOC);
void __unregister_fragment(int fragmentID);

void __init_cpp_exceptions(void);
void __fini_cpp_exceptions(void);
void __destroy_global_chain(void);
}

static int fragmentID = -2;

// The table of contents pointer (r2, the small data 2 base) the exception
// tables are relative to. The original reads the register directly
// (`mr r4, r2` inside __init_cpp_exceptions); C has no way to name it.
static inline char *GetR2(void)
{
#ifdef __MWERKS__
    register char *pTOC;
    asm { mr pTOC, r2 }
    return pTOC;
#else
    return 0;
#endif
}

extern "C" void __init_cpp_exceptions(void)
{
    if (fragmentID == -2) {
        fragmentID = __register_fragment(_eti_init_info, GetR2());
    }
}

extern "C" void __fini_cpp_exceptions(void)
{
    if (fragmentID != -2) {
        __unregister_fragment(fragmentID);
        fragmentID = -2;
    }
}

// Start-up and exit entries. The linker sorts the .ctors$/.dtors$ sections
// by suffix, so these run first at start-up and the global object chain is
// unwound before the exception tables go away.
#pragma section ".ctors$10"
#pragma section ".dtors$10"
#pragma section ".dtors$15"
__declspec(section ".ctors$10") extern void *const __init_cpp_exceptions_reference = __init_cpp_exceptions;
__declspec(section ".dtors$10") extern void *const __destroy_global_chain_reference = __destroy_global_chain;
__declspec(section ".dtors$15") extern void *const __fini_cpp_exceptions_reference = __fini_cpp_exceptions;
