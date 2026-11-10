#ifndef NW4R_SND_SOUNDHANDLE_HPP_
#define NW4R_SND_SOUNDHANDLE_HPP_

namespace nw4r {
    namespace snd {
        namespace detail {
            class BasicSound;
        }

        // A handle on a playing sound: only the sound pointer is known.
        // Laid out after the NW4R library as other Wii decompilations
        // describe it; written here from the game's code.
        class SoundHandle {
            public:
                detail::BasicSound *m_pSound; // 0x00
        };
    }
}

#endif /* !NW4R_SND_SOUNDHANDLE_HPP_ */
