#include "bcs/CSndTask.hpp"
#include "bcs/CSnd.hpp"

namespace bcs {

    // Whether a sound task with an id of 0x40 to 0x63 is alive; Pause then
    // ignores the fade time (name guessed). CSnd::PauseAll has the same test.
    static inline bool IsSpecialSoundPlaying()
    {
        if (CSnd::s_nState != 5) {
            return false;
        }
        CTask *pRoot = CSnd::s_pTaskRoot;
        if (pRoot == 0) {
            return false;
        }
        for (CSndTask *pTask = static_cast<CSndTask *>(pRoot->GetNextChild(0)); pTask != 0;
             pTask = static_cast<CSndTask *>(pRoot->GetNextChild(pTask))) {
            if (!pTask->IsDelete()) {
                unsigned long nId = pTask->m_nSoundId;
                if (nId >= 0x40 && nId <= 0x63) {
                    return true;
                }
            }
        }
        return false;
    }

    int CSndTask::IsAttachedSound() const {
        return m_Handle.IsAttachedSound();
    }

    void CSndTask::Stop(float fFadeFrames) {
        int nFadeFrames = fFadeFrames;
        m_Handle.Stop(nFadeFrames);
        m_nFlag |= 2;
    }

    void CSndTask::SetVolume(float fVolume, float fFadeFrames) {
        if (m_nFlag & 2) {
            return;
        }
        int nFadeFrames = fFadeFrames;
        m_Handle.SetVolume(fVolume, nFadeFrames);
    }

    void CSndTask::SetPitch(float fPitch) {
        if (m_nFlag & 2) {
            return;
        }
        m_Handle.SetPitch(fPitch);
    }

    void CSndTask::Pause(float fFadeFrames, int bPause) {
        if (m_nFlag & 2) {
            return;
        }
        if (IsSpecialSoundPlaying()) {
            fFadeFrames = 0.0f;
        }
        if (bPause) {
            if (++m_nPauseCount == 1) {
                int nFadeFrames = fFadeFrames;
                m_Handle.Pause(true, nFadeFrames);
            }
        } else {
            if (--m_nPauseCount == 0) {
                int nFadeFrames = fFadeFrames;
                m_Handle.Pause(false, nFadeFrames);
            }
        }
        if (m_nPauseCount < 0) {
            m_nPauseCount = 0;
        }
    }

    int CSndTask::IsPrepared() const {
        if (!(m_nFlag & 8)) {
            return 0;
        }
        return m_Handle.IsPrepared();
    }

    void CSndTask::StartPrepared() {
        if (!(m_nFlag & 8)) {
            return;
        }
        m_Handle.StartPrepared();
    }
}
