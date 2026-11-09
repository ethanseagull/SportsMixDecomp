#ifndef NW4R_DB_PANIC_HPP_
#define NW4R_DB_PANIC_HPP_

namespace nw4r {
    namespace db {

        // Reports a failed check and stops. Signature from the DOL's mangled
        // name; the parameter names are guessed from the callers.
        void Panic(const char *pFile, int line, const char *pFormat, ...);
    }
}

#endif /* !NW4R_DB_PANIC_HPP_ */
