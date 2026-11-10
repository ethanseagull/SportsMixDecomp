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
            // (nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f). Returns the id of the sound
            // task (msw::menu::CSndEff::Play keeps it for CTaskMan::GetTask).
            static unsigned long Play(long nSe, const bcs::FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                                      float fArg5, float fArg6);
            // The same with the position by reference (a second function in
            // the DOL). (return type guessed)
            static void Play(long nSe, const bcs::FVEC3 &rPos, unsigned long nArg2, long nArg3, float fArg4,
                             float fArg5, float fArg6);
            // Starts music nSeq; returns the id of its sound task.
            // msw::menu::CSndBGM passes (nSeq, 1, 1.0f, 0.0f). (argument
            // meanings not known)
            static unsigned long PlayBGM(long nSeq, unsigned long nArg1, float fArg2, float fArg3);
            static void MenuSaveHeapState();
            // (name guessed; fn_80147D98, no symbol) Loads menu music
            // 0xBB7..0xBB9 into the sound heap; other numbers do nothing.
            static void MenuLoadSeq(unsigned long nSeq);
    };
}

#endif /* !MSW_CGAMESND_HPP_ */
