#ifndef MSW_CCOLLATKUNIT_HPP_
#define MSW_CCOLLATKUNIT_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CCollUnit;
    class CCollBodyUnit;

    // One attack collision unit (0xAC bytes; derived from msw::CCollUnit,
    // whose SetPos and SetAtkType msw::CBombExplosion::Create_ calls on it).
    // A DOL class.
    //
    // INCOMPLETE: only the flag word msw::CBombExplosionBoss::Create_ sets,
    // and what DodgeF's CItemDodgeBombDD_Base::CallbackBodyDamage_ reads.
    class CCollAtkUnit {
        public:
            // Out of line (fn_80107034, fn_80107118; msw::CAtkCollision
            // builds arrays of them).
            CCollAtkUnit();
            // (fn_801070AC; arguments passed on to CCollUnit's constructor,
            // meanings not known) msw::CBombExplosion::Create_ passes
            // (4, 0, the model handle).
            CCollAtkUnit(long, long, unsigned long);
            virtual ~CCollAtkUnit();

            // Called when the unit hits a body; pArg is what was registered.
            typedef void (*AttackHitCallback)(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                              const bcs::FVEC3 &rPos, unsigned long flag, void *pArg);

            // (fn_80107184) Sets m_pAttackHitCallback and m_pAttackHitArg.
            void SetAttackHitCallback(AttackHitCallback pFunc, void *pArg);

            // CCollAtkUnit derives from CCollUnit in the DOL (BasketF's
            // CChrCoreBasketKinopio::UpdateCollAtk_ calls CCollUnit::SetPos
            // and SetCollType on it). The two headers map the same bytes
            // under different names and are not merged yet, so the base is
            // reached through this. (name guessed)
            CCollUnit *GetCollUnit() { return reinterpret_cast<CCollUnit *>(this); }

            // 0x00: vtable
            unsigned char m_Unk04[8];
            unsigned long m_nUnk0C;   // 0x0C, the last argument of CAtkCollision::Create
            // 0x10: bit 0 on (CAtkCollision::SetEnable), bit 1 the first
            // position not taken yet, bit 2 a unit that keeps its previous
            // position (as CCollUnit's word at 0x10).
            unsigned short m_nFlag10;
            // 0x12, 0x14 (names guessed; as in CCollBodyUnit): the kind of
            // owner (1: a character) and its index. 0x12 is what
            // CAtkCollision::SetCollKind sets, 0x14 its SetIndexAll.
            unsigned char m_nOwnerKind;
            unsigned char m_nCollType; // 0x13 (CAtkCollision::SetCollType)
            signed char m_nOwnerNo;
            unsigned char m_Unk15;     // (CAtkCollision::SetUnk15All)
            // 0x16, 0x17 (names guessed): DodgeF's bomb blast sets the team
            // and the index of the character that threw the bomb.
            signed char m_nTeamNo;
            signed char m_nChrNo;
            // 0x18, CBombExplosion::Create_ sets 0x2A: the flags of what
            // the unit hits (CAtkCollision::SetTargetFlag).
            unsigned long m_nFlag;
            unsigned long m_nMyNoHitFlag;  // 0x1C (CAtkCollision::SetMyNoHitFlagAll)
            unsigned long m_nNoHitFlag;    // 0x20 (CAtkCollision::SetNoHitFlag)
            unsigned long m_nTmpNoHitFlag; // 0x24 (CAtkCollision::SetTmpNoHitFlag)
            unsigned char m_Unk28[4];
            float m_fPosX;         // 0x2C, (names guessed) where the attack is
            float m_fPosY;         // 0x30
            float m_fPosZ;         // 0x34
            bcs::FVEC3 m_Pos2;     // 0x38, the other end of a capsule
            unsigned char m_Unk44[0xC];
            bcs::FVEC3 m_PrevPos;  // 0x50 (name guessed), see CAtkCollision::SetPos
            bcs::FVEC3 m_Size;     // 0x5C, x is the radius of a sphere
            unsigned char m_Unk68[0x84 - 0x68];
            // 0x84, (name guessed) bits 8 to 15 are the kind of attack
            // (DodgeF's time bomb reacts to kind 0x15).
            unsigned long m_nAtkType;
            // 0x88 to 0x94 (names guessed; set by CChrCoreBasketKinopio from
            // CChrCoreBasket::GetAtkVanishPower and GetAtkDownFrame).
            float m_fVanishPower;   // 0x88
            // 0x8C: bit 0 set with the attack direction, read by
            // CAtkCollision::IsAtkHit (read signed).
            short m_nAtkHitFlag;
            short m_nHitCamVibID;   // 0x8E (CAtkCollision::SetHitCamVibIDAll)
            short m_nUnk90;         // 0x90, the effect of a hit (CAtkCollision::SetHitEffectID)
            short m_nHitSe;         // 0x92, a sound id (meaning guessed)
            float m_fDownFrame;     // 0x94 (CAtkCollision::SetHitEffectFrame)
            float m_fUnk98;         // 0x98 (CAtkCollision::SetUnk98All)
            short m_nUnk9C;         // 0x9C (CAtkCollision::SetUnk9CAll)
            short m_nUnk9E;         // 0x9E
            // 0xA0 (name guessed): radians in [0, 2 pi); the shells store
            // their direction of travel here.
            float m_fAtkRotY;
            // 0xA4, 0xA8: called on a hit (CAtkCollision::SetAttackHitCallback).
            AttackHitCallback m_pAttackHitCallback;
            void *m_pAttackHitArg;

            long GetOwnerKind() const { return m_nOwnerKind; }
            unsigned long GetAtkKind() const { return (m_nAtkType >> 8) & 0xFF; }
            // (name guessed; inline in msw::CBombExplosion::Create_)
            void SetUnk9C(long n9C, long n9E)
            {
                m_nUnk9C = n9C;
                m_nUnk9E = n9E;
            }
    };
}

#endif /* !MSW_CCOLLATKUNIT_HPP_ */
