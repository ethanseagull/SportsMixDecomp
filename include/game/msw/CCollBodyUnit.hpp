#ifndef MSW_CCOLLBODYUNIT_HPP_
#define MSW_CCOLLBODYUNIT_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCollUnit.hpp"

namespace msw {

    class CCollAtkUnit;
    class CBall;

    // One body collision unit. A DOL class, 0xC4 bytes (msw::CBodyCollision
    // allocates arrays of them); vtable __vt__Q23msw13CCollBodyUnit.
    class CCollBodyUnit : public CCollUnit {
        public:
            // Out of line: the constructor (shape 2) sets the push rank to 1,
            // the target flags to 0x7F and joins CCollMan's body list; the
            // destructor leaves it.
            CCollBodyUnit();
            virtual ~CCollBodyUnit();

            // Called back with what was registered in pArg (see
            // msw::CBodyCollision).
            typedef void (*BodyPushCallback)(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);
            typedef unsigned long (*BodyDamageCallback)(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                        const bcs::FVEC3 &rPos, void *pArg);
            typedef void (*BallCatchCallback)(CCollBodyUnit *pBody, CBall *pBall, void *pArg);

            // The base. (name guessed; from before the two headers were merged)
            CCollUnit *GetCollUnit() { return this; }

            // 0x00-0x9F: CCollUnit
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
            // 0xB8 (name guessed): the unit pushing this one while its push
            // callback runs (DoBodyPushCallback).
            union {
                unsigned long m_nUnkB8;
                CCollBodyUnit *m_pPushingUnit;
            };
            BallCatchCallback m_pBallCatchCallback;   // 0xBC
            void *m_pBallCatchArg;                    // 0xC0

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
