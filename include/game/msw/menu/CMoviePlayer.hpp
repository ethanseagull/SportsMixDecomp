#ifndef MENU_CMOVIEPLAYER_HPP_
#define MENU_CMOVIEPLAYER_HPP_

#include <string.h>

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    namespace menu {

        // Size 0xF4 (vptr at 0xE0). Defined in the DOL's CMovieScnObj.cpp,
        // whose one global instance (g_MoviePlayer) the menus use to play a
        // THP movie through bcs::CMovie: a requested movie is opened after
        // a short delay, and a new request while one plays switches to it.
        // The class, member and method names are guessed from behaviour.
        //
        // CMoviePlayerWork is the data part, a base class without virtuals
        // (a guess): the vptr follows it at 0xE0 because the compiler puts
        // a class's vptr after its non-virtual bases.
        struct CMoviePlayerWork {
            int m_bActive;                      // 0x00
            unsigned long m_nState;             // 0x04
            char m_aName[3][0x40];              // 0x08
            float m_afArg[3];                   // 0xC8
            float m_fTimer;                     // 0xD4
            int m_nOpenArg;                     // 0xD8: CMovie::Open's second argument
            int m_bLoop;                        // 0xDC: 1 starts with flag 0x40
        };

        class CMoviePlayer : public CMoviePlayerWork {
            public:
                enum {
                    STATE_IDLE,         // 0
                    STATE_STOP,         // 1: stop, then idle
                    STATE_RESTART,      // 2: stop, then wait to open
                    STATE_WAIT_OPEN,    // 3
                    STATE_WAIT_READY,   // 4
                    STATE_WAIT_PLAY,    // 5
                    STATE_PLAY,         // 6
                    STATE_CHANGE_WAIT,  // 7: a new request, settling
                    STATE_CHANGE_STOP   // 8: stopping before the new movie
                };
                enum {
                    ENTRY_REQUEST,      // what the menu asked for
                    ENTRY_OPEN,         // what was opened
                    ENTRY_PENDING,      // what the change wait compares with
                    ENTRY_NUM
                };
                enum { NAME_LEN = 0x40 };

                CMoviePlayer() {}
                virtual ~CMoviePlayer();

                // Clears everything and stops a playing movie (once).
                void Init();
                // Stops a playing movie and goes idle (once).
                void Term();
                // Per frame while initialised.
                void Update();
                void Calc();
                // 1 while the movie plays (also while a change settles).
                int IsPlaying() const;
                // The movie to play, from frame fArg (null clears it).
                void Request(const char *pName, float fArg);
                // Shows the current frame in pPane; returns 0.
                int RemapPane(nw4r::lyt::Pane *pPane);

            private:
                // The entry's file name, null when empty.
                const char *GetName(int nEntry) const
                {
                    return m_aName[nEntry][0] ? m_aName[nEntry] : 0;
                }
                // Copies entry nSrc to nDst unless nSrc is empty.
                void Copy(int nDst, int nSrc)
                {
                    float fArg = m_afArg[nSrc];
                    const char *pName = GetName(nSrc);
                    if (pName) {
                        // The length is unused; the original calls strlen
                        // here too (probably a stripped check).
                        size_t nLen = strlen(pName);
                        strncpy(m_aName[nDst], pName, NAME_LEN);
                        m_afArg[nDst] = fArg;
                    }
                }

            public:
                // 0xE0: vptr
                unsigned char m_Unk[0x10];          // 0xE4: never touched here
        };

        extern CMoviePlayer g_MoviePlayer;
    }
}

#endif /* !MENU_CMOVIEPLAYER_HPP_ */
