#ifndef BCS_CMOVIE_HPP_
#define BCS_CMOVIE_HPP_

#include "bcs/CTask.hpp"
#include "revolution/pad.h"

namespace bcs {

    // The THP file's header (the file format's fields, in file order).
    // Added for DOL game/bcs/CMovie.cpp.
    struct THPHeader {
        char magic[4];                        // 0x00
        unsigned long version;                // 0x04
        unsigned long bufSize;                // 0x08
        unsigned long audioMaxSamples;        // 0x0C
        float frameRate;                      // 0x10
        unsigned long numFrames;              // 0x14
        unsigned long firstFrameSize;         // 0x18
        unsigned long movieDataSize;          // 0x1C
        unsigned long compInfoDataOffsets;    // 0x20
        unsigned long offsetDataOffsets;      // 0x24
        unsigned long movieDataOffsets;       // 0x28
        unsigned long finalFrameDataOffsets;  // 0x2C
    };

    // Which components each frame has (THP file format).
    struct THPFrameCompInfo {
        unsigned long numComponents;          // 0x00
        unsigned char frameComp[16];          // 0x04
    };

    // The THP player's description of the video. Added for MenuF
    // game/menu/CBgDisplay.cpp (names of the public THP player's
    // THPVideoInfo).
    struct THPVideoInfo {
        unsigned long xSize;     // 0x00
        unsigned long ySize;     // 0x04
        unsigned long videoType; // 0x08
    };

    // The THP file's description of the sound (THP file format).
    struct THPAudioInfo {
        unsigned long sndChannels;            // 0x00
        unsigned long sndFrequency;           // 0x04
        unsigned long sndNumSamples;          // 0x08
        unsigned long sndNumTracks;           // 0x0C
    };

    class CMovieThread;

    // Lives in the DOL (game/bcs/CMovie.cpp). Plays a THP movie: a task
    // (vtable: destructor, Main, Disp, CTask's GetFrameRateF) with a
    // reading thread (CMovieThread at 0x148) that loads the file. 0x150
    // bytes (CreateInstance allocates it). The names of the DOL's fn_
    // functions and of most members are guessed from what they do.
    class CMovie : public CTask {
        public:
            // (fn_8006960C) Starts the reading thread.
            CMovie(CTask *pParent = 0, short nPriority = -1);
            // (fn_80069844)
            virtual ~CMovie();
            // (fn_80069DD8) Steps the playback: decodes the frames due.
            virtual void Main();
            // (fn_80069F70) Draws the current frame on the screen
            // rectangle set by SetDrawEnv when Start was given 0x20.
            virtual void Disp();

            // (fn_800695C8)
            static void CreateInstance();
            static CMovie *GetInstance();

            // 1 when a movie may be opened.
            int IsWait() const;
            // Non-zero once the opened movie can start.
            int IsReady() const;
            // Non-zero while a movie is playing.
            int IsPlay() const;
            // (fn_80069A20, fn_80069A2C) Bit 0x80 of the state word: the
            // frames stop advancing while it is set.
            int IsPause() const;
            void SetPause(int bPause);
            // 1 when the reading thread took the file. nStartFrame is where
            // Start begins (wrapped to the movie's length); nArg0 is handed
            // to the thread (called with 1, meaning not known).
            int Open(const char *pFileName, int nArg0, long nStartFrame);
            // Non-zero on success. nFlags is or-ed into the state word
            // (0x20 lets Disp draw the frames itself; callers pass 0).
            int Start(unsigned long nFlags);
            // Non-zero when a movie was open.
            int Stop();
            // Added for MenuF game/menu/CPartySelect.cpp. Non-zero once a
            // frame of the movie can be shown.
            int CanbeDraw() const;
            // Added for MenuF game/menu/CTitle.cpp (return types and
            // argument names guessed). Whether the last frame was shown,
            // and where and how Disp draws the frames: a negative scale
            // stands for the frame buffer's size.
            int IsFrameEnd() const;
            void SetDrawEnv(long nX, long nY, float fScaleX, float fScaleY);
            // (fn_80069D70) The frame shown, -1 when no movie is open.
            long GetFrame() const;
            // (fn_80069D8C) The movie is open and runs at 30 frames a
            // second (the time counts in 60ths).
            int Is30fps() const;
            // Added for game/msw/menu/CMovieScnObj.cpp (names guessed, DOL
            // fn_8006A190, fn_8006A328, fn_8006A434): msw::menu::CMovieScnObj
            // draws the current frame with these three, in this order, on
            // the screen rectangle (nX, nY, nW, nH) with texture coordinates
            // (fS0, fT0) to (fS1, fT1). Draw returns the frame drawn, -1
            // when there is none.
            void BeginDraw();
            long Draw(long nX, long nY, long nW, long nH, float fS0, float fT0, float fS1, float fT1);
            void EndDraw();
            // Added for game/msw/menu/CMovieScnObj.cpp (DOL fn_80069DCC,
            // name guessed): bit 0x8000 of the state word at 0x34. The menu
            // movie player stops the movie whenever it is set.
            int IsError() const;

            // Added for MenuF game/menu/CBgDisplay.cpp (inline: emitted out
            // of line there as GetTHPVideoInfo__Q23bcs6CMovieCFv).
            const THPVideoInfo *GetTHPVideoInfo() const { return &m_VideoInfo; }

        private:
            // (fn_8006A458) Takes over the file the thread opened.
            void Prepare();
            // (fn_8006A6E8) Frees the movie's buffers and clears the state.
            void Close();
            // (fn_8006A890) Decodes the next frame.
            void DecodeFrame();
            // (fn_8006AF5C) Hooked as the pad sampling callback.
            static void SamplingCallback();

            // The frame count of the open movie, 0 when none is open.
            long GetNumFrames() const
            {
                if (m_nFlags & 1) {
                    return m_Header.numFrames;
                }
                return 0;
            }

            static CMovie *s_pInstance;

            // 1 open, 2 loop, 8 a frame decoded, 0x10 started, 0x20 drawn
            // by Disp, 0x80 paused, 0x8000 the thread failed.
            unsigned long m_nFlags;            // 0x34
            float m_fTime;                     // 0x38, in 60ths of a second
            long m_nStartFrame;                // 0x3C
            unsigned char *m_pWork;            // 0x40
            THPHeader m_Header;                // 0x44
            THPFrameCompInfo m_CompInfo;       // 0x74

        public:
            // 0x88 to 0xB0 added for MenuF game/menu/CBgDisplay.cpp: the
            // video's description and the Y, U and V planes of the frame
            // being shown (names guessed; the file's own helpers read the
            // planes directly).
            THPVideoInfo m_VideoInfo;      // 0x88
            THPAudioInfo m_AudioInfo;      // 0x94
            void *m_pTextureY;             // 0xA4
            void *m_pTextureU;             // 0xA8
            void *m_pTextureV;             // 0xAC

        private:
            // Allocated and freed in pairs by the functions after Disp.
            struct Buffer {
                void *m_p0;
                void *m_p1;
            };
            Buffer m_aBuffer[3];               // 0xB0
            unsigned long m_UnkC8;             // 0xC8
            long m_nDecodedFrame;              // 0xCC
            unsigned long m_UnkD0;             // 0xD0
            unsigned long m_UnkD4;             // 0xD4
            unsigned long m_UnkD8;             // 0xD8
            long m_nCurFrame;                  // 0xDC
            unsigned long m_aUnkE0[12];        // 0xE0
            unsigned char *m_pAudioBuffer0;    // 0x110
            unsigned char *m_pAudioBuffer1;    // 0x114
            unsigned long m_Unk118;            // 0x118
            unsigned long m_Unk11C;            // 0x11C
            unsigned long m_Unk120;            // 0x120
            float m_fCurVolume;                // 0x124
            float m_fTargetVolume;             // 0x128
            float m_fDeltaVolume;              // 0x12C
            long m_nRampCount;                 // 0x130
            long m_nDrawX;                     // 0x134
            long m_nDrawY;                     // 0x138
            float m_fScaleX;                   // 0x13C
            float m_fScaleY;                   // 0x140
            PADSamplingCallback m_pOldSamplingCallback; // 0x144
            CMovieThread *m_pThread;           // 0x148
            unsigned char *m_pThreadStack;     // 0x14C
    };
}

#endif /* !BCS_CMOVIE_HPP_ */
