#ifndef MSW_CCOLLBODYUNIT_HPP_
#define MSW_CCOLLBODYUNIT_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CCollUnit;
    class CCollAtkUnit;
    class CBall;

    // One body collision unit. A DOL class, 0xC4 bytes (msw::CBodyCollision
    // allocates arrays of them); vtable lbl_8046EA8C. Like msw::CCollAtkUnit
    // it derives from msw::CCollUnit in the DOL; the two headers map the same
    // bytes under different names and are not merged yet.
    class CCollBodyUnit {
        public:
            // Out of line (fn_80106ED0, fn_80106F60): the constructor sets
            // the push rank to 1, the target flags to 0x7F and joins
            // CCollMan's body list; the destructor leaves it.
            CCollBodyUnit();
            virtual ~CCollBodyUnit();

            // Called back with what was registered in pArg (see
            // msw::CBodyCollision).
            typedef void (*BodyPushCallback)(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);
            typedef unsigned long (*BodyDamageCallback)(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                        const bcs::FVEC3 &rPos, void *pArg);
            typedef void (*BallCatchCallback)(CCollBodyUnit *pBody, CBall *pBall, void *pArg);

            // The base, as CCollAtkUnit::GetCollUnit. (name guessed)
            CCollUnit *GetCollUnit() { return reinterpret_cast<CCollUnit *>(this); }

            // 0x00: vtable
            unsigned char m_Unk04[8];
            // 0x0C (name guessed): the owner's handle
            // (CBodyCollision::SetOwnerHandleAll).
            unsigned long m_nOwnerHandle;
            // 0x10: bit 0 on (CBodyCollision::SetEnable), bit 1 the first
            // position not taken yet, bit 2 a unit that keeps its previous
            // position (as CCollUnit's word at 0x10).
            unsigned short m_nFlag10;
            // 0x12, (name guessed) what owns the unit; DodgeF's bombs stop
            // when they are pushed by an owner of kind 1.
            unsigned char m_nOwnerKind;
            // 0x13: the shape (CBodyCollision::SetCollType; 3 moves in XZ).
            unsigned char m_nCollType;
            // 0x14 (name guessed): the owner's index (the character's, when
            // the kind is 1); read signed. The DOL's
            // msw::CStgGmk_WaterPillar_Act::OnBodyPush passes it to
            // CChrMan::GetChrCore for an owner of kind 1.
            signed char m_nOwnerIdx;
            unsigned char m_Unk15;
            // 0x16 (name guessed): the owner's team (CBodyCollision::SetTeamNoAll).
            signed char m_nTeamNo;
            // 0x17 (meaning unknown; read signed): the DOL's shells pass it
            // to CItem::SendHitChrMessage when an owner of kind 1 pushes them.
            signed char m_nUnk17;
            // 0x18 to 0x24 (names as in CAtkCollision): what the unit is hit
            // by, and the no-hit flags.
            unsigned long m_nTargetFlag;
            unsigned long m_nMyNoHitFlag;  // 0x1C
            unsigned long m_nNoHitFlag;    // 0x20
            unsigned long m_nTmpNoHitFlag; // 0x24
            unsigned char m_Unk28[4];
            // 0x2C (name guessed): the unit's position; VolleyF's
            // CStgGmk_Pinball_Act_Volley::OnBallCatch and DodgeF's
            // msw::CStgGmk_Pinball_Act_Dodge::OnDamage read it.
            bcs::FVEC3 m_Pos;
            bcs::FVEC3 m_Pos2;     // 0x38 (name guessed): the other end of a capsule
            unsigned char m_Unk44[0xC];
            bcs::FVEC3 m_PrevPos;  // 0x50 (name guessed), see CBodyCollision::SetPos
            // 0x5C (name guessed, after CCollUnit::m_Size): the unit's size;
            // BasketF's CcNinjaClone gives its own unit the owner's.
            bcs::FVEC3 m_Size;
            unsigned char m_Unk68[0x18];
            float m_fRotY;         // 0x80, radians in [0, 2 pi)
            // 0x84, 0x92 (names as in CCollAtkUnit): a unit that also
            // attacks (CBodyCollision::SetAttackEnableAll).
            unsigned long m_nAtkType;
            unsigned char m_Unk88[0xA];
            short m_nHitSe;        // 0x92
            unsigned char m_Unk94[0xC];
            // 0xA0 (meaning unknown): the DOL's msw::CStgGmk_Pylon_Act::OnBodyPush
            // does not move the pylon, nor slows the push, for a unit where
            // it is 5.
            // (It is the push rank: CBodyCollision::SetPushRank, 1 by default.)
            unsigned char m_nUnkA0;
            unsigned char m_nPushPower;  // 0xA1 (CBodyCollision::SetPushPower)
            // 0xA2: bit 0 body on, 1 push on, 2 ball catch on, 3 attack on.
            unsigned short m_nBodyFlag;
            unsigned long m_nUnkA4;
            BodyDamageCallback m_pBodyDamageCallback; // 0xA8
            void *m_pBodyDamageArg;                   // 0xAC
            BodyPushCallback m_pBodyPushCallback;     // 0xB0
            void *m_pBodyPushArg;                     // 0xB4
            unsigned long m_nUnkB8;
            BallCatchCallback m_pBallCatchCallback;   // 0xBC
            void *m_pBallCatchArg;                    // 0xC0

            const bcs::FVEC3 &GetPos() const { return m_Pos; }

            // (name guessed) Inline: DodgeF compares the byte as a signed int.
            long GetOwnerKind() const { return m_nOwnerKind; }
            // (name guessed) The shape, compared as a signed int by
            // CBodyCollision::AddPosXZAll.
            long GetCollType() const { return m_nCollType; }
            // (name guessed) Compared as a signed int by the DOL's pylon.
            long GetUnkA0() const { return m_nUnkA0; }

            // (DOL) Runs this unit's push callback for being pushed by
            // pOther by pPush (null: no push).
            void DoBodyPushCallback(CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
    };
}

#endif /* !MSW_CCOLLBODYUNIT_HPP_ */
