#ifndef MSW_BALLBOSSSPIKESHOTARG_HPP_
#define MSW_BALLBOSSSPIKESHOTARG_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {
    namespace BALL {
        namespace BOSS {

            // The parameters of a spike in the boss game, passed to
            // CBall::SetVsBossSpikeShotMode. Size 0x94 (the last field the
            // constructor writes is 0x90; 0x68 is left alone). The
            // constructor is in BossF's game/boss/CBallBoss.cpp. Field
            // meanings are not known; types are the access widths.
            struct SPIKE_SHOT_ARG {
                SPIKE_SHOT_ARG();

                bcs::FVEC3 m_vUnk00;     // 0x00
                long m_nUnk0C;           // 0x0C
                long m_nUnk10;           // 0x10
                long m_nUnk14;           // 0x14
                long m_nUnk18;           // 0x18
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
                long m_nUnk48;           // 0x48
                long m_nUnk4C;           // 0x4C
                long m_nUnk50;           // 0x50
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
            };
        }
    }
}

#endif /* !MSW_BALLBOSSSPIKESHOTARG_HPP_ */
