#ifndef MSW_CCOLLATKUNIT_HPP_
#define MSW_CCOLLATKUNIT_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCollUnit.hpp"

namespace msw {

    class CCollBodyUnit;

    // One attack collision unit (0xAC bytes). A DOL class; vtable
    // __vt__Q23msw12CCollAtkUnit.
    class CCollAtkUnit : public CCollUnit {
        public:
            // Out of line (msw::CAtkCollision builds arrays of them). Both
            // join CCollMan's attack list and turn on bit 2 of m_nFlag10.
            CCollAtkUnit();
            // (arguments as CCollUnit's) msw::CBombExplosion::Create_ passes
            // (4, 0, the model handle).
            CCollAtkUnit(long kind, long type, unsigned long handle);
            virtual ~CCollAtkUnit();

            // Called when the unit hits a body; pArg is what was registered.
            typedef void (*AttackHitCallback)(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                              const bcs::FVEC3 &rPos, unsigned long flag, void *pArg);

            // Sets m_pAttackHitCallback and m_pAttackHitArg.
            void SetAttackHitCallback(AttackHitCallback pFunc, void *pArg);
            // Runs the callback, if any, for hitting pBody at rPos.
            void DoAttackHitCallback(const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos, unsigned long flag);

            // The base. (name guessed; from before the two headers were merged)
            CCollUnit *GetCollUnit() { return this; }

            // 0x00-0x9F: CCollUnit
            // 0xA0 (name guessed): radians in [0, 2 pi); the shells store
            // their direction of travel here.
            float m_fAtkRotY;
            // 0xA4, 0xA8: called on a hit (CAtkCollision::SetAttackHitCallback).
            AttackHitCallback m_pAttackHitCallback;
            void *m_pAttackHitArg;

            unsigned long GetAtkKind() const { return (m_nAtkType >> 8) & 0xFF; }
            // The low byte of 0x84 (meaning not known; HockeyF's pylon
            // reacts to 6). (name guessed)
            unsigned long GetAtkType() const { return (unsigned char)m_nAtkType; }
            // The three floats at 0x2C as a vector (HockeyF's pylon reads it
            // as one). (name guessed)
            const bcs::FVEC3 &GetPos() const { return *reinterpret_cast<const bcs::FVEC3 *>(&m_fPosX); }
            // (name guessed; inline in msw::CBombExplosion::Create_)
            void SetUnk9C(long n9C, long n9E)
            {
                m_nUnk9C = n9C;
                m_nUnk9E = n9E;
            }
    };
}

#endif /* !MSW_CCOLLATKUNIT_HPP_ */
