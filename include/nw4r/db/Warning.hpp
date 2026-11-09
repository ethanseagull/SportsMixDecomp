#ifndef NW4R_DB_WARNING_HPP_
#define NW4R_DB_WARNING_HPP_

namespace nw4r {
    namespace db {

        // Reports a failed check and goes on. Signature from the DOL's
        // mangled name; the parameter names are guessed from the callers.
        void Warning(const char *pFile, int line, const char *pFormat, ...);
    }
}

// (name guessed) Warns when `exp` is false. The inline nw4r::math::FSqrt the
// modules emit shows the call: file name, line number, then the message.
#define NW4R_WARNING(exp, msg) ((exp) ? (void)0 : nw4r::db::Warning(__FILE__, __LINE__, msg))

#endif /* !NW4R_DB_WARNING_HPP_ */
