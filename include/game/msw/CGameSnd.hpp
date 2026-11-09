#ifndef MSW_CGAMESND_HPP_
#define MSW_CGAMESND_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // The sound interface of the games. Lives in the DOL. Only the member
    // used so far; it takes no object.
    class CGameSnd {
        public:
            // Plays sound nSe, at *pPos when pPos is not null. What the other
            // arguments mean is not known; the callers seen so far pass
            // (nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f). (return type guessed: every
            // caller seen ends in this call)
            static void Play(long nSe, const bcs::FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                             float fArg5, float fArg6);
    };
}

#endif /* !MSW_CGAMESND_HPP_ */
