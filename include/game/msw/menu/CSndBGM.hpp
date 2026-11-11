#ifndef MENU_CSNDBGM_HPP_
#define MENU_CSNDBGM_HPP_

namespace msw {
    namespace menu {

        // Lives in the DOL (game/msw/menu/CVibEff.cpp). Size 0x18 (s_Instance
        // in .bss), vptr at 0x14. The menu music. Member and argument names
        // are guesses.
        class CSndBGM {
            public:
                CSndBGM() : m_nState(0) {}

                void Initialize();
                void Finalize();
                // Menu music 0xBB7..0xBB9 is loaded first (MainLoop loads it
                // once the previous music has faded, and plays it when bPlay
                // is set); other music is started at once when bPlay is set.
                void SeqLoad(long nSeq, int bPlay);
                void Play(long nSeq);
                // Fades out the music of sound task nTaskId over fFade.
                void Stop(unsigned long nTaskId, float fFade);
                // 1 while sound task nTaskId exists and is not being deleted.
                int IsPlayNow(unsigned long nTaskId);
                void MainLoop();

                static CSndBGM s_Instance;

                // (name guessed; inline, no symbol; added for
                // game/menu/CTitle.cpp) The sound task of the music playing.
                unsigned long GetTaskId() const { return m_nTaskId; }

            private:
                // Values of m_nState.
                enum {
                    STATE_NONE = 0,
                    STATE_READY = 1,
                    STATE_WAIT_PLAY = 2, // loaded, start m_nNextSeq after the fade
                    STATE_WAIT_LOAD = 3, // load m_nNextSeq after the fade
                    STATE_WAIT_LOAD_PLAY = 4 // load m_nNextSeq after the fade, then play it
                };

                long m_nSeq;            // 0x00, the music playing, -1 for none
                long m_nNextSeq;        // 0x04, the menu music being loaded
                unsigned long m_nTaskId; // 0x08, sound task of m_nSeq
                unsigned char m_nState; // 0x0C
                float m_fFade;          // 0x10, frames left of the fade out

            public:
                // Declared after the data: the vptr is at 0x14. Empty, but
                // defined in the .cpp: __sinit registers it for s_Instance.
                virtual ~CSndBGM();
        };
    }
}

#endif /* !MENU_CSNDBGM_HPP_ */
