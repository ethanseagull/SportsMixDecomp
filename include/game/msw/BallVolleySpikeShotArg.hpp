#ifndef MSW_BALLVOLLEYSPIKESHOTARG_HPP_
#define MSW_BALLVOLLEYSPIKESHOTARG_HPP_

namespace msw {
    namespace BALL {
        namespace VOLLEY {

            // The parameters of a volleyball spike, passed to
            // CBall::SetVolleySpikeShotMode. Size 0xA8 (the last field the
            // DOL's constructor writes is 0xA4; no allocation site).
            // The item balls (CItemBall::GetVolleySpikeParam) overwrite
            // part of it. Field meanings are not known; types are the
            // access widths seen in the game.
            struct SPIKE_SHOT_ARG {
                float m_fUnk00;          // 0x00
                float m_fUnk04;          // 0x04
                float m_fUnk08;          // 0x08
                long m_nUnk0C;           // 0x0C
                long m_nUnk10;           // 0x10
                long m_nUnk14;           // 0x14
                // 0x18. The red shell and the banana keep 1 or 2 and pick
                // one of the two at random otherwise.
                long m_nUnk18;
                long m_nUnk1C;           // 0x1C
                long m_nUnk20;           // 0x20
                long m_nUnk24;           // 0x24
                float m_fUnk28;          // 0x28
                long m_nUnk2C;           // 0x2C
                long m_nUnk30;           // 0x30
                long m_nUnk34;           // 0x34
                float m_fUnk38;          // 0x38
                long m_nUnk3C;           // 0x3C
                float m_fUnk40;          // 0x40
                long m_nUnk44;           // 0x44
                long m_nSe;              // 0x48, a sound id (name guessed)
                long m_nUnk4C;           // 0x4C
                // 0x50. The effect id the items also return from
                // GetVolleySpikeTailEffect. (name guessed)
                long m_nTailEffect;
                long m_nUnk54;           // 0x54
                float m_fUnk58;          // 0x58
                float m_fUnk5C;          // 0x5C
                float m_fUnk60;          // 0x60
                float m_fUnk64;          // 0x64
                float m_fUnk68;          // 0x68
                float m_fUnk6C;          // 0x6C
                float m_fUnk70;          // 0x70
                float m_fUnk74;          // 0x74
                float m_fUnk78;          // 0x78
                float m_fUnk7C;          // 0x7C
                float m_fUnk80;          // 0x80
                float m_fUnk84;          // 0x84
                float m_fUnk88;          // 0x88
                float m_fUnk8C;          // 0x8C
                float m_fUnk90;          // 0x90
                float m_fUnk94;          // 0x94
                float m_fUnk98;          // 0x98
                float m_fUnk9C;          // 0x9C
                long m_nUnkA0;           // 0xA0
                long m_nUnkA4;           // 0xA4
            };
        }
    }
}

#endif /* !MSW_BALLVOLLEYSPIKESHOTARG_HPP_ */
