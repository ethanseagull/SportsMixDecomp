#ifndef MSW_CSTGSRT_HPP_
#define MSW_CSTGSRT_HPP_

#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // Position, rotation and scale of a stage object; msw::CStgAct embeds
    // one at 0x24 (0x144 bytes, up to its member at 0x168). A DOL class.
    // msw::CStgGmk_WaterPillar::GetPos reads the position at 0x00; the
    // inline msw::CStgObj::LookAt writes the direction at 0x0C and sets bit
    // 1 of the flags at 0xC0. The vptr is at 0xC4 (the data members come
    // before the first virtual function): the DOL constructor (no exported
    // name, called by CStgAct's) stores it there, and the vtable's third
    // slot sets the position (flag bit 0) and calls it from
    // msw::CStgObj::SetPos. Names of the slots are guesses.
    class CStgSrt {
        public:
            // (name guessed)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }

            void SetFront(const bcs::FVEC3 &rFront);

            // (name guessed) Point the direction from the position towards
            // rTarget; inline in msw::CStgObj::LookAt.
            void SetLookAtPos(const bcs::FVEC3 &rTarget)
            {
                bcs::FVEC3Sub(&m_Dir, &rTarget, &m_Pos);
                m_nFlag |= 2;
            }

        private:
            bcs::FVEC3 m_Pos;             // 0x00
            bcs::FVEC3 m_Dir;             // 0x0C
            unsigned char m_Unk18[0xA8];  // 0x18
            unsigned long m_nFlag;        // 0xC0, 7 after construction

        public:
            virtual ~CStgSrt();                           // 0x08
            virtual void Reset();                         // 0x0C
            virtual void SetPos(const bcs::FVEC3 &rPos);  // 0x10

        private:
            unsigned char m_UnkC8[0x7C]; // 0xC8
    };
}

#endif /* !MSW_CSTGSRT_HPP_ */
