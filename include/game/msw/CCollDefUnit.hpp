#ifndef MSW_CCOLLDEFUNIT_HPP_
#define MSW_CCOLLDEFUNIT_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCollUnit.hpp"

namespace msw {

    class CBall;

    // (purpose guessed from the name) The defence (reflect) volume of a
    // collision. A DOL class, 0xBC bytes (msw::CDefCollision allocates
    // arrays of them); vtable __vt__Q23msw12CCollDefUnit.
    class CCollDefUnit : public CCollUnit {
        public:
            // Shape 2; joins CCollMan's defence list.
            CCollDefUnit();
            virtual ~CCollDefUnit();

            typedef unsigned long (*ReflectHitCallback)(const CCollDefUnit *pDef, const CBall *pBall,
                                                        bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, void *pArg);

            void SetReflectHitCallback(ReflectHitCallback pCallback, void *pArg);
            // Runs the callback for pBall; 1 when there is none.
            unsigned long DoReflectHitCallback(const CBall *pBall, bcs::FVEC3 &rPos, bcs::FVEC3 &rVel) const;

            // (name guessed) The kind of defence the unit is, read signed
            // (CChrCoreVolleyKinopio's block reacts to kinds 0, 2 and 3).
            // It is CCollUnit's byte at 0x14 (CDefCollision::SetIndexAll).
            long GetKind() const { return m_nOwnerIdx; }

            // (names guessed) Bit 0 of the reflect flags: CStgGmk_Case and
            // CChrCoreBoss turn it on, CDefCollision::SetReflectDisableAll off.
            void OnFlagA0(unsigned long flag) { m_nFlagA0 |= flag; }
            void OffFlagA0(unsigned long flag) { m_nFlagA0 &= ~flag; }

        private:
            friend class CDefCollision;

            // 0x00-0x9F: CCollUnit
            unsigned long m_nFlagA0;             // 0xA0
            float m_fUnkA4;                      // 0xA4
            float m_fUnkA8;                      // 0xA8
            ReflectHitCallback m_pReflectHitCallback; // 0xAC
            void *m_pReflectHitArg;              // 0xB0
            unsigned long m_UnkB4[2];            // 0xB4
    };
}

#endif /* !MSW_CCOLLDEFUNIT_HPP_ */
