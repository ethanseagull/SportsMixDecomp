#include "game/msw/CStgSeSlot.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CGameSnd.hpp"

namespace msw {

    // The task of the slot's sound, null when it has none.
    inline bcs::CSndTask *CStgSeSlot::CSESlot::GetTask() const
    {
        if (m_nHandle == 0) {
            return 0;
        }
        return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_nHandle));
    }

    // The sound's task still plays (or waits for its sound to load).
    inline bool CStgSeSlot::CSESlot::IsPlaying() const
    {
        bcs::CSndTask *pTask = GetTask();
        if (pTask == 0 || pTask->IsDelete() || (pTask->IsPrepared() && !pTask->IsAttachedSound())) {
            return false;
        }
        return true;
    }

    inline bool CStgSeSlot::CSESlot::IsAlive() const
    {
        if (m_nState & FLAG_ALIVE) {
            return true;
        }
        if (m_fDelay >= 0.0f) {
            return true;
        }
        return IsPlaying();
    }

    inline void CStgSeSlot::CSESlot::StopSound(float fade)
    {
        bcs::CSndTask *pTask = GetTask();
        if (pTask != 0) {
            pTask->Stop(fade);
        }
    }

    inline void CStgSeSlot::CSESlot::Stop(float fade)
    {
        StopSound(fade);
        m_fDelay = -1.0f;
        m_fStopDelay = -1.0f;
        m_nState = 0;
        m_fInterval = 0.0f;
        m_fIntervalRand = 0.0f;
    }

    // Picks the time until a repeating sound plays again.
    inline void CStgSeSlot::CSESlot::SetNextDelay()
    {
        if (!(m_nFlag & FLAG_REPEAT)) {
            m_fDelay = -1.0f;
        } else if (m_fIntervalRand > 0.0f) {
            m_fDelay = bcs::BCS_OrgRandRange(m_fInterval, m_fInterval + m_fIntervalRand);
        } else {
            m_fDelay = m_fInterval;
        }
    }

    inline void CStgSeSlot::CSESlot::UpdateDelay(float rate)
    {
        if (m_fDelay < 0.0f) {
            return;
        }
        m_fDelay -= rate;
        if (m_fDelay <= 0.0f) {
            if (!(m_nFlag & FLAG_NOTRACK)) {
                StopSound(0.0f);
            }
            Start(false);
            SetNextDelay();
        }
    }

    inline void CStgSeSlot::CSESlot::Set(long se, long n, long mode, unsigned long a, long b, float f4, float vol,
                                         float f6)
    {
        m_nSe = se;
        m_nFlag = n;
        m_nMode = mode;
        m_nArg2 = a;
        m_nArg3 = b;
        m_fArg4 = f4;
        m_fVolume = vol;
        m_fArg6 = f6;
    }

    inline void CStgSeSlot::CSESlot::SetPos(CPosFunc *pFunc)
    {
        m_pFunc = pFunc;
        if (pFunc != 0) {
            pFunc->GetPos(&m_Pos);
        } else {
            m_Pos.Set(0.0f, 0.0f, 0.0f);
        }
    }

    inline unsigned long CStgSeSlot::CSESlot::Play(long se, long n, unsigned long a, long b, float f4, float vol,
                                                   float f6)
    {
        Stop(0.0f);
        Set(se, n, MODE_NONE, a, b, f4, vol, f6);
        m_pFunc = 0;
        Start(false);
        return m_nHandle;
    }

    inline unsigned long CStgSeSlot::CSESlot::Play(long se, const bcs::FVEC3 &rPos, long n, unsigned long a, long b,
                                                   float f4, float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n, MODE_POS, a, b, f4, vol, f6);
        m_pFunc = 0;
        m_Pos = rPos;
        Start(false);
        return m_nHandle;
    }

    inline void CStgSeSlot::CSESlot::DelayPlay(float delay, long se, long n, unsigned long a, long b, float f4,
                                               float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n, MODE_NONE, a, b, f4, vol, f6);
        m_pFunc = 0;
        m_fDelay = delay;
    }

    inline void CStgSeSlot::CSESlot::DelayPlay(float delay, long se, const bcs::FVEC3 &rPos, long n, unsigned long a,
                                               long b, float f4, float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n, MODE_POS, a, b, f4, vol, f6);
        m_Pos = rPos;
        m_pFunc = 0;
        m_fDelay = delay;
    }

    // A repeating sound starts after delay, or after its interval when delay
    // is negative.
    inline void CStgSeSlot::CSESlot::StartRepeat(float delay, float interval, float rand)
    {
        m_nState = m_nFlag & ~(FLAG_ALIVE | FLAG_LOOP);
        m_fInterval = interval;
        m_fIntervalRand = rand;
        if (delay < 0.0f) {
            SetNextDelay();
        } else {
            m_fDelay = delay;
        }
    }

    CStgSeSlot::CStgSeSlot() : m_pSlot(0), m_nSlotNum(0) {}

    CStgSeSlot::~CStgSeSlot()
    {
        SE_StopAll();
        if (m_pSlot != 0) {
            delete[] m_pSlot;
        }
        m_pSlot = 0;
    }

    void CStgSeSlot::SE_Init()
    {
        m_nSlotNum = SE_GetSlotNum();
        if (m_pSlot == 0 && m_nSlotNum > 0) {
            m_pSlot = new (1) CSESlot[m_nSlotNum];
        }
    }

    unsigned long CStgSeSlot::SE_GetHandle(long slot) { return m_pSlot[slot].m_nHandle; }

    bcs::CTask *CStgSeSlot::SE_GetObject(long slot) { return m_pSlot[slot].GetTask(); }

    bool CStgSeSlot::SE_IsAlive(long slot) { return m_pSlot[slot].IsAlive(); }

    unsigned long CStgSeSlot::SE_PlayEx(long slot, long se, long n, unsigned long a, long b, float f4, float vol,
                                        float f6)
    {
        if (se < 0) {
            return 0;
        }
        return m_pSlot[slot].Play(se, n, a, b, f4, vol, f6);
    }

    unsigned long CStgSeSlot::SE_PlayPosEx(long slot, long se, const bcs::FVEC3 &rPos, long n, unsigned long a,
                                           long b, float f4, float vol, float f6)
    {
        if (se < 0) {
            return 0;
        }
        return m_pSlot[slot].Play(se, rPos, n, a, b, f4, vol, f6);
    }

    unsigned long CStgSeSlot::SE_PlayPosEx(long slot, long se, long n, CSESlot::CPosFunc *pFunc, unsigned long a,
                                           long b, float f4, float vol, float f6)
    {
        if (se < 0) {
            return 0;
        }
        pFunc = pFunc != 0 ? pFunc : SE_GetDefaultFunc();
        return m_pSlot[slot].Play(se, pFunc, n, a, b, f4, vol, f6);
    }

    void CStgSeSlot::SE_Stop(long slot, float fade) { m_pSlot[slot].Stop(fade); }

    void CStgSeSlot::SE_StopAll()
    {
        for (int i = 0; i < m_nSlotNum; i++) {
            SE_Stop(i, 0.0f);
        }
    }

    void CStgSeSlot::SE_SetVolume(long slot, float vol, float fade)
    {
        CSESlot &rSlot = m_pSlot[slot];
        rSlot.m_fVolume = vol;
        bcs::CSndTask *pTask = rSlot.GetTask();
        if (pTask != 0) {
            pTask->SetVolume(vol, fade);
        }
    }

    void CStgSeSlot::SE_DelayPlayEx(long slot, float delay, long se, long n, unsigned long a, long b, float f4,
                                    float vol, float f6)
    {
        if (se >= 0) {
            m_pSlot[slot].DelayPlay(delay, se, n, a, b, f4, vol, f6);
        }
    }

    void CStgSeSlot::SE_DelayPlayPosEx(long slot, float delay, long se, const bcs::FVEC3 &rPos, long n,
                                       unsigned long a, long b, float f4, float vol, float f6)
    {
        if (se >= 0) {
            m_pSlot[slot].DelayPlay(delay, se, rPos, n, a, b, f4, vol, f6);
        }
    }

    void CStgSeSlot::SE_DelayPlayPosEx(long slot, float delay, long se, long n, CSESlot::CPosFunc *pFunc,
                                       unsigned long a, long b, float f4, float vol, float f6)
    {
        if (se >= 0) {
            pFunc = pFunc != 0 ? pFunc : SE_GetDefaultFunc();
            m_pSlot[slot].DelayPlay(delay, se, pFunc, n, a, b, f4, vol, f6);
        }
    }

    void CStgSeSlot::SE_DelayStop(long slot, float delay, float fade)
    {
        CSESlot &rSlot = m_pSlot[slot];
        rSlot.m_fStopDelay = delay;
        rSlot.m_fStopFade = fade;
    }

    void CStgSeSlot::SE_RepeatPlayEx(long slot, long se, float interval, float rand, float delay, long n,
                                     unsigned long a, long b, float f4, float vol, float f6)
    {
        if (se >= 0) {
            m_pSlot[slot].RepeatPlay(delay, se, interval, rand, n, a, b, f4, vol, f6);
        }
    }

    void CStgSeSlot::SE_RepeatPlayPosEx(long slot, long se, const bcs::FVEC3 &rPos, float interval, float rand,
                                        float delay, long n, unsigned long a, long b, float f4, float vol, float f6)
    {
        if (se >= 0) {
            m_pSlot[slot].RepeatPlay(delay, se, rPos, interval, rand, n, a, b, f4, vol, f6);
        }
    }

    void CStgSeSlot::SE_RepeatPlayPosEx(long slot, long se, float interval, float rand, float delay, long n,
                                        CSESlot::CPosFunc *pFunc, unsigned long a, long b, float f4, float vol,
                                        float f6)
    {
        if (se >= 0) {
            pFunc = pFunc != 0 ? pFunc : SE_GetDefaultFunc();
            m_pSlot[slot].RepeatPlay(delay, se, pFunc, interval, rand, n, a, b, f4, vol, f6);
        }
    }

    void CStgSeSlot::SE_Update(float rate)
    {
        if (m_pSlot != 0) {
            for (int i = 0; i < m_nSlotNum; i++) {
                m_pSlot[i].Update(rate);
            }
        }
    }

    CStgSeSlot::CSESlot::CSESlot()
        : m_nHandle(0), m_nState(0), m_fDelay(-1.0f), m_fStopDelay(-1.0f), m_fInterval(0.0f), m_fIntervalRand(0.0f),
          m_pFunc(0)
    {
    }

    CStgSeSlot::CSESlot::~CSESlot() { Stop(0.0f); }

    // Starts the sound with the settings kept in the slot; a looping sound
    // started again (bLoop) passes 0 for the last argument. Returns the id of
    // the new task.
    unsigned long CStgSeSlot::CSESlot::Start(int bLoop)
    {
        unsigned long hSound = 0;
        float f6 = !bLoop ? m_fArg6 : 0.0f;
        switch (m_nMode) {
            case MODE_NONE:
                hSound = CGameSnd::Play(m_nSe, (const bcs::FVEC3 *)0, m_nArg2, m_nArg3, m_fArg4, m_fVolume, f6);
                break;
            case MODE_POS:
                hSound = CGameSnd::Play(m_nSe, m_Pos, m_nArg2, m_nArg3, m_fArg4, m_fVolume, f6);
                break;
            case MODE_FUNC:
                hSound = CGameSnd::Play(m_nSe, &m_Pos, m_nArg2, m_nArg3, m_fArg4, m_fVolume, f6);
                break;
        }
        if (hSound != 0) {
            m_nState = m_nFlag;
        }
        if (!(m_nState & FLAG_NOTRACK)) {
            m_nHandle = hSound;
        }
        return hSound;
    }

    unsigned long CStgSeSlot::CSESlot::Play(long se, CPosFunc *pFunc, long n, unsigned long a, long b, float f4,
                                            float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n, MODE_FUNC, a, b, f4, vol, f6);
        SetPos(pFunc);
        Start(false);
        return m_nHandle;
    }

    void CStgSeSlot::CSESlot::DelayPlay(float delay, long se, CPosFunc *pFunc, long n, unsigned long a, long b,
                                        float f4, float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n, MODE_FUNC, a, b, f4, vol, f6);
        SetPos(pFunc);
        m_fDelay = delay;
    }

    void CStgSeSlot::CSESlot::RepeatPlay(float delay, long se, float interval, float rand, long n, unsigned long a,
                                         long b, float f4, float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n | FLAG_REPEAT, MODE_NONE, a, b, f4, vol, f6);
        m_pFunc = 0;
        StartRepeat(delay, interval, rand);
    }

    void CStgSeSlot::CSESlot::RepeatPlay(float delay, long se, const bcs::FVEC3 &rPos, float interval, float rand,
                                         long n, unsigned long a, long b, float f4, float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n | FLAG_REPEAT, MODE_POS, a, b, f4, vol, f6);
        m_Pos = rPos;
        m_pFunc = 0;
        StartRepeat(delay, interval, rand);
    }

    void CStgSeSlot::CSESlot::RepeatPlay(float delay, long se, CPosFunc *pFunc, float interval, float rand, long n,
                                         unsigned long a, long b, float f4, float vol, float f6)
    {
        Stop(0.0f);
        Set(se, n | FLAG_REPEAT, MODE_FUNC, a, b, f4, vol, f6);
        SetPos(pFunc);
        StartRepeat(delay, interval, rand);
    }

    void CStgSeSlot::CSESlot::Update(float rate)
    {
        if (!IsAlive()) {
            if (m_fDelay > 0.0f) {
                UpdateDelay(rate);
            }
        } else if (m_fDelay > 0.0f) {
            UpdateDelay(rate);
        } else {
            if (m_pFunc != 0) {
                m_pFunc->GetPos(&m_Pos);
            }
            if (m_fStopDelay > 0.0f) {
                m_fStopDelay -= rate;
                if (m_fStopDelay <= 0.0f) {
                    m_fStopDelay = -1.0f;
                    Stop(m_fStopFade);
                }
            }
            if ((m_nState & FLAG_LOOP) && !IsPlaying()) {
                Start(true);
            }
            if (m_nState & FLAG_REPEAT) {
                UpdateDelay(rate);
            }
        }
    }
}
