#ifndef MSW_CCOLLATKUNIT_HPP_
#define MSW_CCOLLATKUNIT_HPP_

namespace msw {

    // One attack collision unit (0xAC bytes; derived from msw::CCollUnit,
    // whose SetPos and SetAtkType msw::CBombExplosion::Create_ calls on it).
    // A DOL class.
    //
    // INCOMPLETE: only the flag word msw::CBombExplosionBoss::Create_ sets,
    // and what DodgeF's CItemDodgeBombDD_Base::CallbackBodyDamage_ reads.
    class CCollAtkUnit {
        public:
            unsigned char m_Unk00[0x18];
            unsigned long m_nFlag; // 0x18, CBombExplosion::Create_ sets 0x2A
            unsigned char m_Unk1C[0x2C - 0x1C];
            float m_fPosX;         // 0x2C, (names guessed) where the attack is
            float m_fPosY;         // 0x30
            float m_fPosZ;         // 0x34
            unsigned char m_Unk38[0x84 - 0x38];
            // 0x84, (name guessed) bits 8 to 15 are the kind of attack
            // (DodgeF's time bomb reacts to kind 0x15).
            unsigned long m_nAtkType;

            unsigned long GetAtkKind() const { return (m_nAtkType >> 8) & 0xFF; }
    };
}

#endif /* !MSW_CCOLLATKUNIT_HPP_ */
