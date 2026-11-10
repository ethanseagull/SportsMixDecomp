#ifndef BCS_CSNDTASK_HPP_
#define BCS_CSNDTASK_HPP_

#include "bcs/CTask.hpp"
#include "nw4r/snd/SoundHandle.hpp"

namespace bcs {
    struct FVEC3;
}

namespace bcs {

    // A task that plays one sound (0x60 bytes: fn_8007A03C allocates it).
    // Callers keep its id and look it up with CTaskMan::GetTask. Only the
    // members used so far are declared; the layout is not mapped.
    class CSndTask : public CTask {
        public:
            // Stops the sound, fading out over fFadeFrames.
            void Stop(float fFadeFrames);
            void SetPitch(float fPitch);
            int IsAttachedSound() const;
            // (name and parameter meanings guessed; fn_8007A62C, unnamed in
            // the DOL) Pauses (bPause != 0) or resumes the sound over
            // fFadeFrames frames.
            void Pause(float fFadeFrames, int bPause);

            // (name guessed; fn_8007A03C, unnamed in the DOL) Starts music
            // nSeq and returns the id of its task, 0 when nothing was
            // started. msw::CGameSnd::PlayBGM is a branch to it.
            static unsigned long PlayBGM(long nSeq, unsigned long nArg1, float fVolume, float fArg3);

            // Added for game/msw/CGameSnd.cpp (names guessed).
            // Plays sound nSe, at *pPos when pPos is not null, and returns
            // the id of its task (msw::CGameSnd::Play is a branch to these).
            // fn_80079E84, fn_80079F60
            static unsigned long Play(long nSe, const FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                                      float fVolume, float fArg6);
            static unsigned long Play(long nSe, const FVEC3 &rPos, unsigned long nArg2, long nArg3, float fArg4,
                                      float fVolume, float fArg6);
            // As PlayBGM, but the music waits for StartPrepared.
            // fn_8007A104
            static unsigned long PrepareBGM(long nSeq, unsigned long nArg1, float fVolume, float fArg3);
            // Volume fVolume, reached over fFadeFrames frames
            // (BasicSound::SetVolume). fn_8007A5C8
            void SetVolume(float fVolume, float fFadeFrames);
            int IsPrepared() const;
            // Starts a sound PrepareBGM made. fn_8007A7F8
            void StartPrepared();

            // 0x34, right after CTask (0x34 bytes): the sound this task plays
            nw4r::snd::SoundHandle m_Handle;
    };
}

#endif /* !BCS_CSNDTASK_HPP_ */
