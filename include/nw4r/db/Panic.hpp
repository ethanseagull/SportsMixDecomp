#ifndef NW4R_DB_PANIC_HPP_
#define NW4R_DB_PANIC_HPP_

namespace nw4r {
    namespace db {

        // Reports a failed check and stops. Signature from the DOL's mangled
        // name; the parameter names are guessed from the callers.
        void Panic(const char *pFile, int line, const char *pFormat, ...);
    }
}

// The library's assertion, written with the file name and line the original
// passes (they are part of the binary). The condition is evaluated as a
// value, which is how the original tests a compound condition (a bool in r0,
// then one compare). Added for MenuF game/menu/CBgDisplay.cpp.
#define NW4R_ASSERT_AT(exp, file, line, msg) ((void)((exp) || (nw4r::db::Panic(file, line, msg), 0)))

#endif /* !NW4R_DB_PANIC_HPP_ */
