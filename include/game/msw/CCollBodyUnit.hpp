#ifndef MSW_CCOLLBODYUNIT_HPP_
#define MSW_CCOLLBODYUNIT_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // One body collision unit. A DOL class.
    //
    // INCOMPLETE: only the byte DodgeF's CItemDodgeBombDD_Base::CallbackBodyPush_
    // reads.
    class CCollBodyUnit {
        public:
            unsigned char m_Unk00[0x12];
            // 0x12, (name guessed) what owns the unit; DodgeF's bombs stop
            // when they are pushed by an owner of kind 1.
            unsigned char m_nOwnerKind;
            unsigned char m_Unk13;
            // 0x14 (name guessed): the owner's index (the character's, when
            // the kind is 1); read signed. The DOL's
            // msw::CStgGmk_WaterPillar_Act::OnBodyPush passes it to
            // CChrMan::GetChrCore for an owner of kind 1.
            signed char m_nOwnerIdx;
            unsigned char m_Unk15[2];
            // 0x17 (meaning unknown; read signed): the DOL's shells pass it
            // to CItem::SendHitChrMessage when an owner of kind 1 pushes them.
            signed char m_nUnk17;
            unsigned char m_Unk18[0x2C - 0x18];
            // 0x2C (name guessed): the unit's position; VolleyF's
            // CStgGmk_Pinball_Act_Volley::OnBallCatch and DodgeF's
            // msw::CStgGmk_Pinball_Act_Dodge::OnDamage read it.
            bcs::FVEC3 m_Pos;
            unsigned char m_Unk38[0x5C - 0x38];
            // 0x5C (name guessed, after CCollUnit::m_Size): the unit's size;
            // BasketF's CcNinjaClone gives its own unit the owner's.
            bcs::FVEC3 m_Size;
            unsigned char m_Unk68[0xA0 - 0x68];
            // 0xA0 (meaning unknown): the DOL's msw::CStgGmk_Pylon_Act::OnBodyPush
            // does not move the pylon, nor slows the push, for a unit where
            // it is 5.
            unsigned char m_nUnkA0;

            const bcs::FVEC3 &GetPos() const { return m_Pos; }

            // (name guessed) Inline: DodgeF compares the byte as a signed int.
            long GetOwnerKind() const { return m_nOwnerKind; }
            // (name guessed) Compared as a signed int by the DOL's pylon.
            long GetUnkA0() const { return m_nUnkA0; }

            // (DOL) Runs this unit's push callback for being pushed by
            // pOther by pPush (null: no push).
            void DoBodyPushCallback(CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
    };
}

#endif /* !MSW_CCOLLBODYUNIT_HPP_ */
