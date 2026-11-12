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
            // Out of line (fn_801067A4); the vtable (lbl_8046EA98) has only it.
            virtual ~CCollUnit();

            void SetCollKind(long kind);
            void SetCollType(long type);
            void SetPos(const bcs::FVEC3 &rPos);
            // (name guessed; VolleyF's CStgGmk_Pinball_Act_Volley)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }
            // (name guessed; BasketF's CcSiromaMagicCircle) The other end.
            const bcs::FVEC3 &GetPos2() const { return m_Pos2; }

            // (names guessed) Inline in msw::CStgGmk_WaterPillar_Act.
            void SetShape(unsigned long shape) { m_nShape = shape; }
            void SetSize(const bcs::FVEC3 &rSize) { m_Size = rSize; }
            // (added for game/msw/CStgGmk_Case.cpp)
            const bcs::FVEC3 &GetSize() const { return m_Size; }
            void SetSize(float x, float y, float z)
            {
                m_Size.x = x;
                m_Size.y = y;
                m_Size.z = z;
            }
            void OnFlag10(unsigned short flag) { m_nFlag10 |= flag; }
            void OffFlag10(unsigned short flag) { m_nFlag10 &= ~flag; }
            void OnFlagA0(unsigned long flag) { m_nFlagA0 |= flag; }
            void OffFlagA0(unsigned long flag) { m_nFlagA0 &= ~flag; }
            // (name guessed; VolleyF's CStgGmk_Trolley_Act_Volley) Radians, 0 to
            // 2 pi: the callers wrap the angle first.
            void SetRotY(float rot) { m_fRotY = rot; }
            // (name guessed; inline in game/msw/CStgGmk_Trolley.cpp, DOL)
            // The same, first bringing the angle into [0, 2 pi).
            void SetRotYWrapped(float rot)
            {
                while (rot > 6.2831855f) {
                    rot -= 6.2831855f;
                }
                while (rot < 0.0f) {
                    rot += 6.2831855f;
                }
                m_fRotY = rot;
            }
            // (names guessed) Inline in BasketF's CChrCoreBasketYoshi:
            // both set to the bit of the owner's player number.
            void SetNoHitFlag(unsigned long flag) { m_nNoHitFlag = flag; }
            void SetMyNoHitFlag(unsigned long flag) { m_nMyNoHitFlag = flag; }

        protected:
            // The set classes write the units' fields directly.
            friend class CDefCollision;

            // 0x00: vtable
            unsigned char m_Unk04[8];
            unsigned long m_nUnk0C;      // 0x0C, the last constructor argument
            unsigned short m_nFlag10;    // 0x10
            unsigned char m_nCollKind;   // 0x12 (SetCollKind)
            unsigned char m_nCollType;   // 0x13 (SetCollType)
            signed char m_nIndex;        // 0x14 (CDefCollision::SetIndexAll)
            unsigned char m_Unk15;
            signed char m_nTeamNo;       // 0x16 (CDefCollision::SetTeamNoAll)
            signed char m_nOwnerChrIdx;  // 0x17 (GetCollOwnerChrIdx)
            unsigned long m_nShape;      // 0x18, the pillar sets 4
            // (names guessed) CDefCollision::SetMyNoHitFlagAll writes 0x1C, so
            // these two may be the other way round.
            unsigned long m_nNoHitFlag;  // 0x1C (names guessed)
            unsigned long m_nMyNoHitFlag; // 0x20
            unsigned char m_Unk24[8];
            bcs::FVEC3 m_Pos;            // 0x2C (name guessed)
            bcs::FVEC3 m_Pos2;           // 0x38 (name guessed): the other end of a capsule (CAtkCollision::SetPos2)
            unsigned char m_Unk44[0xC];
            bcs::FVEC3 m_PrevPos;        // 0x50 (name guessed; CCollUnit::SetPos)
            bcs::FVEC3 m_Size;           // 0x5C
            unsigned char m_Unk68[0x18];
            float m_fRotY;               // 0x80 (SetRotY)
            unsigned char m_Unk84[0x1C];
            // The constructor (fn_801066C8) initialises nothing past 0x9E, so
            // CCollUnit is 0xA0 bytes and this word belongs to the derived
            // units (msw::CCollDefUnit's reflect flags); kept here for the
            // callers that reach it through CDefCollision::GetColl.
            unsigned long m_nFlagA0;     // 0xA0
    };
}

#endif /* !MSW_CCOLLUNIT_HPP_ */
