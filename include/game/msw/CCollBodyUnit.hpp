#ifndef MSW_CCOLLBODYUNIT_HPP_
#define MSW_CCOLLBODYUNIT_HPP_

namespace bcs {
    struct FVEC3;
}

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
            // 0x14 (name guessed): the owner's index; the DOL's
            // msw::CStgGmk_WaterPillar_Act::OnBodyPush passes it to
            // CChrMan::GetChrCore for an owner of kind 1.
            signed char m_nOwnerIdx;

            // (DOL) Runs this unit's push callback for a push by pOther.
            void DoBodyPushCallback(CCollBodyUnit *pOther, bcs::FVEC3 *pPush);

            // (name guessed) Inline: DodgeF compares the byte as a signed int.
            long GetOwnerKind() const { return m_nOwnerKind; }
    };
}

#endif /* !MSW_CCOLLBODYUNIT_HPP_ */
