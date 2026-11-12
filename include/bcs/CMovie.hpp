#ifndef BCS_CMOVIE_HPP_
#define BCS_CMOVIE_HPP_

namespace bcs {

    // The THP player's description of the video. Added for MenuF
    // game/menu/CBgDisplay.cpp (names of the public THP player's
    // THPVideoInfo).
    struct THPVideoInfo {
        unsigned long xSize;     // 0x00
        unsigned long ySize;     // 0x04
        unsigned long videoType; // 0x08
    };

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
            // Added for game/msw/menu/CMovieScnObj.cpp (names guessed, DOL
            // fn_8006A190, fn_8006A328, fn_8006A434): msw::menu::CMovieScnObj
            // draws the current frame with these three, in this order, on
            // the screen rectangle (nX, nY, nW, nH) with texture coordinates
            // (fS0, fT0) to (fS1, fT1).
            void BeginDraw();
            void Draw(long nX, long nY, long nW, long nH, float fS0, float fT0, float fS1, float fT1);
            void EndDraw();
            // Added for game/msw/menu/CMovieScnObj.cpp (DOL fn_80069DCC,
            // name guessed): bit 0x8000 of the state word at 0x34. The menu
            // movie player stops the movie whenever it is set.
            int IsError() const;

            // Added for MenuF game/menu/CBgDisplay.cpp (inline: emitted out
            // of line there as GetTHPVideoInfo__Q23bcs6CMovieCFv).
            const THPVideoInfo *GetTHPVideoInfo() const { return &m_VideoInfo; }

        private:
            unsigned char m_Unk00[0x88];

        public:
            // 0x88 to 0xB0 added for MenuF game/menu/CBgDisplay.cpp: the
            // video's description and the Y, U and V planes of the frame
            // being shown (names guessed; the file's own helpers read the
            // planes directly).
            THPVideoInfo m_VideoInfo;      // 0x88
            unsigned char m_Unk94[0x10];
            void *m_pTextureY;             // 0xA4
            void *m_pTextureU;             // 0xA8
            void *m_pTextureV;             // 0xAC
    };
}

#endif /* !BCS_CMOVIE_HPP_ */
