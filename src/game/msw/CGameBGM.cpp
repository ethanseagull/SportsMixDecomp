// msw::CGameBGM: the music of a game, as a task with one state per music
// situation (stage music, star, bonus time, jingles, hurry-up).
//
// In the original this code is the second half of one file with
// msw::CGameSnd (0x80146548..0x8014A618): both share the .sdata2 constants
// 1.0f and 0.0f, and the .data of the file starts with two tables CGameSnd
// reads (0x804717E0, 0x80471844). That is also why the calls below go to
// bcs::CSndTask::PlayBGM directly: CGameSnd::PlayBGM, defined earlier in the
// same file, is only a branch to it and was inlined.
#include "game/msw/CGameBGM.hpp"

#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    namespace {
        // Fade lengths, in frames.
        const float FADE_NONE = 0.0f;
        const float FADE_SLOW = 30.0f;

        // Music of each stage (indexed by GameWork::m_nStage) and its fast
        // version after the hurry-up jingle.
        short s_aStageBgm[100] = {
            0x04, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x04, 0x12,
            0x14, 0x16, 0x18, 0x04, 0x04, 0x1A, 0x1C, 0x1E, 0x04, 0x04,
            0x20, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x28, 0x28, 0x28, 0x28, 0x28,
            0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
            0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25,
            0x25, 0x25, 0x25, 0x25, 0x25, 0x23, 0x23, 0x23, 0x23, 0x23,
            0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
        };
        short s_aStageBgmFast[100] = {
            0x05, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x05, 0x13,
            0x15, 0x17, 0x19, 0x05, 0x05, 0x1B, 0x1D, 0x1F, 0x05, 0x05,
            0x21, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
            0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
            0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
            0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x29, 0x29, 0x29, 0x29, 0x29,
            0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
            0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27,
            0x27, 0x27, 0x27, 0x27, 0x27, 0x24, 0x24, 0x24, 0x24, 0x24,
            0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
        };

        // Sequences (names guessed).
        enum {
            SEQ_JINGLE_2 = 0x2,
            SEQ_BONUS = 0x26,
            SEQ_JINGLE_37 = 0x37,
            SEQ_JINGLE_38 = 0x38,
            SEQ_MUSIC_3C = 0x3C,
            SEQ_STAR = 0x3E,
            SEQ_INTRO = 0xBBA,
            SEQ_HURRYUP = 0xBFD
        };
    }

    CGameBGM *CGameBGM::s_pInstance;

    inline bcs::CSndTask *CGameBGM::GetSnd(long nSnd)
    {
        return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_ahSnd[nSnd]));
    }

    inline void CGameBGM::StopSnd(long nSnd, float fFadeFrames)
    {
        bcs::CSndTask *pSnd = GetSnd(nSnd);
        if (pSnd != 0) {
            pSnd->Stop(fFadeFrames);
        }
        m_ahSnd[nSnd] = 0;
    }

    inline void CGameBGM::PlaySnd(long nSnd, long nSeq)
    {
        StopSnd(nSnd, FADE_NONE);
        m_ahSnd[nSnd] = bcs::CSndTask::PlayBGM(nSeq, 1, 1.0f, 0.0f);
    }

    inline void CGameBGM::PauseSnd(long nSnd, int bPause, float fFadeFrames)
    {
        bcs::CSndTask *pSnd = GetSnd(nSnd);
        if (pSnd != 0) {
            pSnd->Pause(fFadeFrames, bPause);
        }
    }

    inline void CGameBGM::StopAllSnd(float fFadeFrames)
    {
        for (long i = 0; i < SND_NUM; i++) {
            StopSnd(i, fFadeFrames);
        }
    }

    void CGameBGM::Create()
    {
        if (s_pInstance == 0) {
            s_pInstance = new CGameBGM;
        }
    }

    long CGameBGM::GetStageBgm(long nStage, int bFast)
    {
        if (s_aStageBgm[nStage] < 0) {
            return -1;
        }

        CGame::GameWork &work = CGame::g_GameWork;
        if (work.m_nGameType == 2) {
            // Party games: basketball, dodgeball and hockey have their own
            // music when party parameter 0 is set.
            long nMode = work.m_nGameMode;
            if (nMode == CGame::GAME_MODE_BASKET) {
                if (CGame::GetPartyGameParam(0) != 0) {
                    return bFast == 0 ? 0x2E : 0x2F;
                }
            } else if (nMode == CGame::GAME_MODE_DODGE) {
                if (CGame::GetPartyGameParam(0) != 0) {
                    return bFast == 0 ? 0x30 : 0x31;
                }
            } else if (nMode == CGame::GAME_MODE_HOCKEY) {
                if (CGame::GetPartyGameParam(0) != 0) {
                    return bFast == 0 ? 0x2C : 0x2D;
                }
            }
        }

        long nBgm;
        if (bFast == 0) {
            nBgm = s_aStageBgm[nStage];
        } else {
            nBgm = s_aStageBgmFast[nStage];
        }
        return nBgm;
    }

    void CGameBGM::SetBgmState(long state)
    {
        if (s_pInstance == 0) {
            return;
        }

        long cur = GetBgmState();
        // States 4 and 5 neither restart themselves nor replace states 6 to
        // 8.
        if (state == 4) {
            if (cur == 4) {
                return;
            }
            if (cur == 6) {
                return;
            }
            if (cur == 7) {
                return;
            }
            if (cur == 8) {
                return;
            }
        }
        if (state == 5) {
            if (cur == 5) {
                return;
            }
            if (cur == 6) {
                return;
            }
            if (cur == 7) {
                return;
            }
            if (cur == 8) {
                return;
            }
        }
        s_pInstance->RequestState(state);
    }

    long CGameBGM::GetBgmState()
    {
        if (s_pInstance != 0) {
            return s_pInstance->m_nState;
        }
        return 0;
    }

    inline int CGameBGM::StartHurryUp()
    {
        // Only while the stage, star or bonus music plays.
        if ((unsigned long)(m_nState - STATE_PLAY) > STATE_BONUS - STATE_PLAY) {
            return 0;
        }
        if (m_bHurryUpLock != 0 || m_bWaitFastBgm != 0) {
            return 0;
        }

        PlaySnd(SND_HURRYUP, SEQ_HURRYUP);
        StopSnd(SND_MAIN, FADE_SLOW);
        PauseSnd(SND_STAR, 1, FADE_SLOW);
        PauseSnd(SND_BONUS, 1, FADE_SLOW);
        m_bWaitFastBgm = 1;
        m_bHurryUpLock = 1;
        return 1;
    }

    int CGameBGM::HurryUp()
    {
        if (s_pInstance != 0) {
            return s_pInstance->StartHurryUp();
        }
        return 0;
    }

    CGameBGM::CGameBGM()
        : CTask(0, 0x3E)
    {
        SetUnk08(0xFC01);
        m_ahSnd[SND_MAIN] = 0;
        m_ahSnd[SND_STAR] = 0;
        m_ahSnd[SND_BONUS] = 0;
        m_ahSnd[SND_JINGLE] = 0;
        m_ahSnd[SND_HURRYUP] = 0;
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;

        InitFsm(this, STATE_NUM);
        SetState(STATE_STOP, &CGameBGM::CtorStateStop, &CGameBGM::LoopStateStop, &CGameBGM::DtorStateStop);
        SetState(STATE_INTRO, &CGameBGM::CtorStateIntro, &CGameBGM::LoopStateIntro, &CGameBGM::DtorStateIntro);
        SetState(STATE_START, &CGameBGM::CtorStateStart, &CGameBGM::LoopStateStart, &CGameBGM::DtorStateStart);
        SetState(STATE_PLAY, &CGameBGM::CtorStatePlay, &CGameBGM::LoopStatePlay, &CGameBGM::DtorStatePlay);
        SetState(STATE_STAR, &CGameBGM::CtorStateStar, &CGameBGM::LoopStateStar, &CGameBGM::DtorStateStar);
        SetState(STATE_BONUS, &CGameBGM::CtorStateBonus, &CGameBGM::LoopStateBonus, &CGameBGM::DtorStateBonus);
        SetState(STATE_JINGLE_37, &CGameBGM::CtorStateJingle37, &CGameBGM::LoopStateJingle37,
                 &CGameBGM::DtorStateJingle37);
        SetState(STATE_JINGLE_38, &CGameBGM::CtorStateJingle38, &CGameBGM::LoopStateJingle38,
                 &CGameBGM::DtorStateJingle38);
        SetState(STATE_JINGLE_38B, &CGameBGM::CtorStateJingle38B, &CGameBGM::LoopStateJingle38B,
                 &CGameBGM::DtorStateJingle38B);
        SetState(STATE_JINGLE_2, &CGameBGM::CtorStateJingle2, &CGameBGM::LoopStateJingle2,
                 &CGameBGM::DtorStateJingle2);
        SetState(STATE_MUSIC_3C, &CGameBGM::CtorStateMusic3C, &CGameBGM::LoopStateMusic3C,
                 &CGameBGM::DtorStateMusic3C);
        SetState(STATE_FADE_OUT, &CGameBGM::CtorStateFadeOut, &CGameBGM::LoopStateFadeOut,
                 &CGameBGM::DtorStateFadeOut);
        SetState(STATE_CUT, &CGameBGM::CtorStateCut, &CGameBGM::LoopStateCut, &CGameBGM::DtorStateCut);
        ChangeState(STATE_STOP);
    }

    CGameBGM::~CGameBGM()
    {
        StopAllSnd(FADE_NONE);
    }

    void CGameBGM::Main()
    {
        ExecState();
    }

    void CGameBGM::CtorStateStop()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopAllSnd(FADE_NONE);
    }

    void CGameBGM::LoopStateStop() {}

    void CGameBGM::DtorStateStop() {}

    void CGameBGM::CtorStateIntro()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopAllSnd(FADE_NONE);
        PlaySnd(SND_MAIN, SEQ_INTRO);
        ChangeState(STATE_PLAY);
    }

    void CGameBGM::LoopStateIntro() {}

    void CGameBGM::DtorStateIntro() {}

    void CGameBGM::CtorStateStart()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 0;
        StopAllSnd(FADE_SLOW);
        PlaySnd(SND_MAIN, GetStageBgm(CGame::g_GameWork.m_nStage, 0));
        ChangeState(STATE_PLAY);
    }

    void CGameBGM::LoopStateStart() {}

    void CGameBGM::DtorStateStart() {}

    void CGameBGM::CtorStatePlay() {}

    void CGameBGM::LoopStatePlay()
    {
        // After the hurry-up jingle, the fast version of the stage music.
        if (m_bWaitFastBgm != 0 && GetSnd(SND_HURRYUP) == 0) {
            long nSeq = GetStageBgm(CGame::g_GameWork.m_nStage, 1);
            PlaySnd(SND_MAIN, nSeq);
            m_bWaitFastBgm = 0;
        }
    }

    void CGameBGM::DtorStatePlay() {}

    void CGameBGM::CtorStateStar()
    {
        PauseSnd(SND_MAIN, 1, FADE_SLOW);
        PlaySnd(SND_STAR, SEQ_STAR);
        // The hurry-up jingle has priority.
        if (m_bWaitFastBgm != 0) {
            PauseSnd(SND_STAR, 1, FADE_NONE);
        }
    }

    void CGameBGM::LoopStateStar()
    {
        int bEnd = 1;
        if (CChrMan::IsExistStarChr()) {
            bEnd = 0;
        }
        if (CBallMan::IsExistStarBall()) {
            bEnd = 0;
        }
        if (bEnd) {
            ChangeState(STATE_PLAY);
        } else if (m_bWaitFastBgm != 0 && GetSnd(SND_HURRYUP) == 0) {
            // The jingle has ended: the star music goes on, the fast stage
            // music waits paused under it.
            PauseSnd(SND_STAR, 0, FADE_SLOW);
            long nSeq = GetStageBgm(CGame::g_GameWork.m_nStage, 1);
            PlaySnd(SND_MAIN, nSeq);
            PauseSnd(SND_MAIN, 1, FADE_NONE);
            m_bWaitFastBgm = 0;
        }
    }

    void CGameBGM::DtorStateStar()
    {
        PauseSnd(SND_MAIN, 0, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
    }

    void CGameBGM::CtorStateBonus()
    {
        PauseSnd(SND_MAIN, 1, FADE_SLOW);
        PlaySnd(SND_BONUS, SEQ_BONUS);
        if (m_bWaitFastBgm != 0) {
            PauseSnd(SND_BONUS, 1, FADE_NONE);
        }
    }

    void CGameBGM::LoopStateBonus()
    {
        int bEnd = 1;
        if (CGame::IsNowBonusTime()) {
            bEnd = 0;
        }
        if (bEnd) {
            ChangeState(STATE_PLAY);
        } else if (m_bWaitFastBgm != 0 && GetSnd(SND_HURRYUP) == 0) {
            PauseSnd(SND_BONUS, 0, FADE_SLOW);
            long nSeq = GetStageBgm(CGame::g_GameWork.m_nStage, 1);
            PlaySnd(SND_MAIN, nSeq);
            PauseSnd(SND_MAIN, 1, FADE_NONE);
            m_bWaitFastBgm = 0;
        }
    }

    void CGameBGM::DtorStateBonus()
    {
        PauseSnd(SND_MAIN, 0, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
    }

    void CGameBGM::CtorStateJingle37()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_37);
    }

    void CGameBGM::LoopStateJingle37()
    {
        if (GetSnd(SND_JINGLE) == 0) {
            ChangeState(STATE_PLAY);
        }
    }

    void CGameBGM::DtorStateJingle37() {}

    void CGameBGM::CtorStateJingle38()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_38);
    }

    void CGameBGM::LoopStateJingle38()
    {
        if (GetSnd(SND_JINGLE) == 0) {
            ChangeState(STATE_PLAY);
        }
    }

    void CGameBGM::DtorStateJingle38() {}

    void CGameBGM::CtorStateJingle38B()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_38);
    }

    void CGameBGM::LoopStateJingle38B()
    {
        if (GetSnd(SND_JINGLE) == 0) {
            ChangeState(STATE_PLAY);
        }
    }

    void CGameBGM::DtorStateJingle38B() {}

    void CGameBGM::CtorStateJingle2()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        StopSnd(SND_HURRYUP, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_2);
    }

    void CGameBGM::LoopStateJingle2() {}

    void CGameBGM::DtorStateJingle2() {}

    void CGameBGM::CtorStateMusic3C()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        PlaySnd(SND_MAIN, SEQ_MUSIC_3C);
    }

    void CGameBGM::LoopStateMusic3C() {}

    void CGameBGM::DtorStateMusic3C() {}

    void CGameBGM::CtorStateFadeOut()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 0;
        StopAllSnd(FADE_SLOW);
    }

    void CGameBGM::LoopStateFadeOut() {}

    void CGameBGM::DtorStateFadeOut() {}

    void CGameBGM::CtorStateCut()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 0;
        StopAllSnd(FADE_NONE);
    }

    void CGameBGM::LoopStateCut() {}

    void CGameBGM::DtorStateCut() {}
}
