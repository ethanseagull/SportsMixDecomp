#ifndef MSW_CSTGSRT_HPP_
#define MSW_CSTGSRT_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // Position, rotation and scale of a stage object; msw::CStgAct embeds
    // one at 0x24 (0x144 bytes, up to its member at 0x168). A DOL class.
    // Only the position is mapped: msw::CStgGmk_WaterPillar::GetPos reads
    // it at 0x00.
    class CStgSrt {
        public:
            // (name guessed)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }

        private:
            bcs::FVEC3 m_Pos;             // 0x00
            unsigned char m_Unk0C[0x138]; // 0x0C
    };
}

#endif /* !MSW_CSTGSRT_HPP_ */
