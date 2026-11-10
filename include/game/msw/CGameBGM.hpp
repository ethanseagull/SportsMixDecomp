#ifndef MSW_CGAMEBGM_HPP_
#define MSW_CGAMEBGM_HPP_

#include "bcs/CTask.hpp"
#include "bcs/CFsm.hpp"

namespace bcs {
    class CSndTask;
}

namespace msw {

    // Background music of a game: a task whose state machine is the music
    // state. bcs::CTask at 0x00, bcs::CFsm<CGameBGM> at 0x34 (vptr at 0x78),
    // own members from 0x7C; 0x98 bytes (Create allocates that much).
    class CGameBGM : public bcs::CTask, public bcs::CFsm<CGameBGM> {
        public:
            // The music states (names guessed from what each state plays).
            enum {
                STATE_STOP,       // all sounds cut, hurry-up disabled
                STATE_INTRO,      // sequence 0xBBA, then STATE_PLAY
                STATE_START,      // the stage's music, then STATE_PLAY
                STATE_PLAY,       // the stage's music runs
                STATE_STAR,       // star music (0x3E) over the paused stage music
                STATE_BONUS,      // bonus time music (0x26) over the paused stage music
                STATE_JINGLE_37,  // one-shot 0x37, then STATE_PLAY
                STATE_JINGLE_38,  // one-shot 0x38, then STATE_PLAY
                STATE_JINGLE_38B, // the same as STATE_JINGLE_38
                STATE_JINGLE_2,   // sequence 2 after everything else stops
                STATE_MUSIC_3C,   // sequence 0x3C on the main track
                STATE_FADE_OUT,   // all sounds fade out
                STATE_CUT,        // all sounds cut
                STATE_NUM
            };

            // The sound tracks this task holds (names guessed).
            enum {
                SND_MAIN,    // the stage's music
                SND_STAR,    // star music
                SND_BONUS,   // bonus time music
                SND_JINGLE,  // one-shot jingles
                SND_HURRYUP, // the hurry-up jingle
                SND_NUM
            };

            CGameBGM();
            virtual ~CGameBGM();
            virtual void Main();

            // (name guessed) Creates the instance unless there is one.
            static void Create();
            // Changes the music state, except that states 4 and 5 do not
            // replace themselves or states 6 to 8.
            static void SetBgmState(long state);
            // The current music state, 0 when there is no instance.
            static long GetBgmState();
            // Plays the hurry-up jingle and switches the stage music to its
            // fast version when the jingle ends. Not zero when it did.
            static int HurryUp();

            // (name guessed) A virtual call made from an inline body: that is
            // what makes the unit emit CFsm<CGameBGM>::ChangeState right after
            // SetBgmState, as in the original.
            void RequestState(long state) { ChangeState(state); }

            static CGameBGM *s_pInstance; // lbl_805C0C6C (name guessed)

        private:
            // (name guessed; fn_8014892C, unnamed) The music of stage
            // nStage, its fast version when bFast is set; -1 for a stage
            // without music. Party games have their own.
            long GetStageBgm(long nStage, int bFast);

            // The body of HurryUp, on the instance.
            int StartHurryUp();

            bcs::CSndTask *GetSnd(long nSnd);
            void StopSnd(long nSnd, float fFadeFrames);
            void PlaySnd(long nSnd, long nSeq);
            void PauseSnd(long nSnd, int bPause, float fFadeFrames);
            void StopAllSnd(float fFadeFrames);

            // State hooks, in the game's CtorState/LoopState/DtorState
            // naming (msw::CGame's virtuals use it).
            void CtorStateStop();
            void LoopStateStop();
            void DtorStateStop();
            void CtorStateIntro();
            void LoopStateIntro();
            void DtorStateIntro();
            void CtorStateStart();
            void LoopStateStart();
            void DtorStateStart();
            void CtorStatePlay();
            void LoopStatePlay();
            void DtorStatePlay();
            void CtorStateStar();
            void LoopStateStar();
            void DtorStateStar();
            void CtorStateBonus();
            void LoopStateBonus();
            void DtorStateBonus();
            void CtorStateJingle37();
            void LoopStateJingle37();
            void DtorStateJingle37();
            void CtorStateJingle38();
            void LoopStateJingle38();
            void DtorStateJingle38();
            void CtorStateJingle38B();
            void LoopStateJingle38B();
            void DtorStateJingle38B();
            void CtorStateJingle2();
            void LoopStateJingle2();
            void DtorStateJingle2();
            void CtorStateMusic3C();
            void LoopStateMusic3C();
            void DtorStateMusic3C();
            void CtorStateFadeOut();
            void LoopStateFadeOut();
            void DtorStateFadeOut();
            void CtorStateCut();
            void LoopStateCut();
            void DtorStateCut();

            unsigned long m_ahSnd[SND_NUM]; // 0x7C, sound task ids
            // 0x90: set by HurryUp; the fast music starts when the jingle
            // has ended (names guessed)
            int m_bWaitFastBgm;
            // 0x94: HurryUp does nothing while it is set
            int m_bHurryUpLock;
    };
}

#endif /* !MSW_CGAMEBGM_HPP_ */
