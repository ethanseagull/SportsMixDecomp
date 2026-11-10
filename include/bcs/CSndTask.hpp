#ifndef BCS_CSNDTASK_HPP_
#define BCS_CSNDTASK_HPP_

#include "bcs/CTask.hpp"

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
    };
}

#endif /* !BCS_CSNDTASK_HPP_ */
