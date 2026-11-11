#ifndef MSW_CCOLLUNIT_HPP_
#define MSW_CCOLLUNIT_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // One collision unit, the base of the attack, body and defence units.
    // A DOL class.
    //
    // INCOMPLETE: only what the DOL's msw::CStgGmk_WaterPillar_Act sets on
    // the unit of its CDefCollision; member names and meanings are guesses.
    class CCollUnit {
        public:
            void SetCollKind(long kind);
            void SetCollType(long type);
            void SetPos(const bcs::FVEC3 &rPos);

            // (names guessed) Inline in msw::CStgGmk_WaterPillar_Act.
            void SetShape(unsigned long shape) { m_nShape = shape; }
            void SetSize(const bcs::FVEC3 &rSize) { m_Size = rSize; }
            void SetSize(float x, float y, float z)
            {
                m_Size.x = x;
                m_Size.y = y;
                m_Size.z = z;
            }
            void OnFlag10(unsigned short flag) { m_nFlag10 |= flag; }
            void OffFlag10(unsigned short flag) { m_nFlag10 &= ~flag; }
            void OnFlagA0(unsigned long flag) { m_nFlagA0 |= flag; }
            // (names guessed) Inline in BasketF's CChrCoreBasketYoshi:
            // both set to the bit of the owner's player number.
            void SetNoHitFlag(unsigned long flag) { m_nNoHitFlag = flag; }
            void SetMyNoHitFlag(unsigned long flag) { m_nMyNoHitFlag = flag; }

        private:
            unsigned char m_Unk00[0x10];
            unsigned short m_nFlag10;    // 0x10
            unsigned char m_Unk12[6];
            unsigned long m_nShape;      // 0x18, the pillar sets 4
            unsigned long m_nNoHitFlag;  // 0x1C (names guessed)
            unsigned long m_nMyNoHitFlag; // 0x20
            unsigned char m_Unk24[0x38];
            bcs::FVEC3 m_Size;           // 0x5C
            unsigned char m_Unk68[0x38];
            unsigned long m_nFlagA0;     // 0xA0
    };
}

#endif /* !MSW_CCOLLUNIT_HPP_ */
