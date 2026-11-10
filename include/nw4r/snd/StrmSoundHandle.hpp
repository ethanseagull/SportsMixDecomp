#ifndef NW4R_SND_STRMSOUNDHANDLE_HPP_
#define NW4R_SND_STRMSOUNDHANDLE_HPP_

#include "nw4r/snd/SoundHandle.hpp"

namespace nw4r {
    namespace snd {
        // What a stream's header says (0x10 bytes; only the sample rate is
        // read so far).
        struct StrmDataInfo {
            bool m_bLoop;              // 0x00
            long m_nSampleRate;        // 0x04
            unsigned long m_nLoopStart; // 0x08
            unsigned long m_nLoopEnd;   // 0x0C
        };

        namespace detail {
            class StrmPlayer {
                public:
                    bool ReadStrmDataInfo(StrmDataInfo *pInfo) const;
                    long GetPlaySamplePosition() const;
            };

            // A streamed sound. Only its player, at 0x110, is mapped.
            class StrmSound {
                public:
                    bool ReadStrmDataInfo(StrmDataInfo *pInfo) const { return m_StrmPlayer.ReadStrmDataInfo(pInfo); }
                    long GetPlaySamplePosition() const { return m_StrmPlayer.GetPlaySamplePosition(); }

                    unsigned char m_Unk000[0x110];
                    StrmPlayer m_StrmPlayer; // 0x110
            };
        }

        // A handle on a streamed sound, taken from a SoundHandle when the
        // sound is a stream; detached again when it goes out of scope.
        class StrmSoundHandle {
            public:
                StrmSoundHandle(SoundHandle *pHandle);
                ~StrmSoundHandle() { DetachSound(); }

                void DetachSound();
                bool IsAttachedSound() const { return m_pSound != 0; }

                bool ReadStrmDataInfo(StrmDataInfo *pInfo) const
                {
                    if (!IsAttachedSound()) {
                        return false;
                    }
                    return m_pSound->ReadStrmDataInfo(pInfo);
                }

                long GetPlaySamplePosition() const
                {
                    if (!IsAttachedSound()) {
                        return -1;
                    }
                    return m_pSound->GetPlaySamplePosition();
                }

            private:
                detail::StrmSound *m_pSound; // 0x00
        };
    }
}

#endif /* !NW4R_SND_STRMSOUNDHANDLE_HPP_ */
