#ifndef MSW_CCOLLATKUNIT_HPP_
#define MSW_CCOLLATKUNIT_HPP_

namespace msw {

    class CCollUnit;

    // One attack collision unit (0xAC bytes; derived from msw::CCollUnit,
    // whose SetPos and SetAtkType msw::CBombExplosion::Create_ calls on it).
    // A DOL class.
    //
    // INCOMPLETE: only the flag word msw::CBombExplosionBoss::Create_ sets,
    // and what DodgeF's CItemDodgeBombDD_Base::CallbackBodyDamage_ reads.
    class CCollAtkUnit {
        public:
            // CCollAtkUnit derives from CCollUnit in the DOL (BasketF's
            // CChrCoreBasketKinopio::UpdateCollAtk_ calls CCollUnit::SetPos
            // and SetCollType on it). The two headers map the same bytes
            // under different names and are not merged yet, so the base is
            // reached through this. (name guessed)
            CCollUnit *GetCollUnit() { return reinterpret_cast<CCollUnit *>(this); }

            unsigned char m_Unk00[0x12];
            // 0x12, 0x14 (names guessed; as in CCollBodyUnit): the kind of
            // owner (1: a character) and its index.
            unsigned char m_nOwnerKind;
            unsigned char m_Unk13;
            signed char m_nOwnerNo;
            unsigned char m_Unk15[0x18 - 0x15];
            unsigned long m_nFlag; // 0x18, CBombExplosion::Create_ sets 0x2A
            unsigned char m_Unk1C[0x2C - 0x1C];
            float m_fPosX;         // 0x2C, (names guessed) where the attack is
            float m_fPosY;         // 0x30
            float m_fPosZ;         // 0x34
            unsigned char m_Unk38[0x84 - 0x38];
            // 0x84, (name guessed) bits 8 to 15 are the kind of attack
            // (DodgeF's time bomb reacts to kind 0x15).
            unsigned long m_nAtkType;
            // 0x88 to 0x94 (names guessed; set by CChrCoreBasketKinopio from
            // CChrCoreBasket::GetAtkVanishPower and GetAtkDownFrame).
            float m_fVanishPower;   // 0x88
            unsigned char m_Unk8C[0x90 - 0x8C];
            short m_nUnk90;         // 0x90
            short m_nHitSe;         // 0x92, a sound id (meaning guessed)
            float m_fDownFrame;     // 0x94

            long GetOwnerKind() const { return m_nOwnerKind; }
            unsigned long GetAtkKind() const { return (m_nAtkType >> 8) & 0xFF; }
    };
}

#endif /* !MSW_CCOLLATKUNIT_HPP_ */
