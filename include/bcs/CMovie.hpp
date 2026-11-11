#ifndef BCS_CMOVIE_HPP_
#define BCS_CMOVIE_HPP_

namespace bcs {

    // Lives in the DOL. Plays a THP movie. Only the members used so far;
    // return types are guesses read from how the callers test the results
    // (MenuF game/menu/CGameSelect.cpp).
    class CMovie {
        public:
            static CMovie *GetInstance();

            // 1 when a movie may be opened.
            int IsWait() const;
            // Non-zero once the opened movie can start.
            int IsReady() const;
            // Non-zero while a movie is playing.
            int IsPlay() const;
            // 1 on success. The meaning of the last two arguments is not
            // known (called with 1, 0).
            int Open(const char *pFileName, int nArg0, long nArg1);
            // Non-zero on success; the argument is called with 0.
            int Start(unsigned long nArg);
            void Stop();
            // Added for MenuF game/menu/CPartySelect.cpp. Non-zero once a
            // frame of the movie can be shown.
            int CanbeDraw() const;
            // Added for MenuF game/menu/CTitle.cpp (return types and
            // argument names guessed). Whether the last frame was shown,
            // and where and how the frames are drawn.
            int IsFrameEnd() const;
            void SetDrawEnv(long nUnk0, long nUnk1, float fUnk0, float fUnk1);
    };
}

#endif /* !BCS_CMOVIE_HPP_ */
