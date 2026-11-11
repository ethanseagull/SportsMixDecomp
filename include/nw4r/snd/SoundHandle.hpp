#ifndef NW4R_SND_SOUNDHANDLE_HPP_
#define NW4R_SND_SOUNDHANDLE_HPP_

#include "nw4r/snd/BasicSound.hpp"

namespace nw4r {
    namespace snd {

        // A handle on a playing sound: only the sound pointer is known.
        // Laid out after the NW4R library as other Wii decompilations
        // describe it; written here from the game's code.
        class SoundHandle {
            public:
                // The forwarding helpers below are inline: bcs::CSndTask's
                // functions test the pointer and call the sound directly.
                bool IsAttachedSound() const { return m_pSound != 0; }

                void Stop(int fadeFrames)
                {
                    if (IsAttachedSound()) {
                        m_pSound->Stop(fadeFrames);
                    }
                }
                void Pause(bool bPause, int fadeFrames)
                {
                    if (IsAttachedSound()) {
                        m_pSound->Pause(bPause, fadeFrames);
                    }
                }
                void SetVolume(float volume, int frames)
                {
                    if (IsAttachedSound()) {
                        m_pSound->SetVolume(volume, frames);
                    }
                }
                void SetPitch(float pitch)
                {
                    if (IsAttachedSound()) {
                        m_pSound->SetPitch(pitch);
                    }
                }
                void StartPrepared()
                {
                    if (IsAttachedSound()) {
                        m_pSound->StartPrepared();
                    }
                }
                bool IsPrepared() const
                {
                    bool bPrepared = false;
                    if (IsAttachedSound() && m_pSound->IsPrepared()) {
                        bPrepared = true;
                    }
                    return bPrepared;
                }

                detail::BasicSound *m_pSound; // 0x00
        };
    }
}

#endif /* !NW4R_SND_SOUNDHANDLE_HPP_ */
