#ifndef MSW_CCOLLDEFUNIT_HPP_
#define MSW_CCOLLDEFUNIT_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCollUnit.hpp"

namespace msw {

    class CBall;

    // (purpose guessed from the name) The defence (reflect) volume of a
    // collision. Lives in the DOL. Only the members used so far; the callback
    // and its argument are at 0xAC and 0xB0. 0xBC bytes (msw::CDefCollision
    // allocates arrays of them); vtable lbl_8046EA74.
    class CCollDefUnit : public CCollUnit {
        public:
            CCollDefUnit();                      // fn_801071AC
            virtual ~CCollDefUnit();             // fn_80107228

            typedef unsigned long (*ReflectHitCallback)(const CCollDefUnit *pDef, const CBall *pBall,
                                                        bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, void *pArg);

            void SetReflectHitCallback(ReflectHitCallback pCallback, void *pArg);

            // (name guessed) The kind of defence the unit is, read signed
            // (CChrCoreVolleyKinopio's block reacts to kinds 0, 2 and 3).
            // It is CCollUnit's byte at 0x14 (CDefCollision::SetIndexAll).
            long GetKind() const { return m_nIndex; }

        private:
            friend class CDefCollision;

            // 0x00-0xA3: CCollUnit (with m_nFlagA0, see there)
            float m_fUnkA4;                      // 0xA4
            float m_fUnkA8;                      // 0xA8
            ReflectHitCallback m_pReflectHitCallback; // 0xAC
            void *m_pReflectHitArg;              // 0xB0
            unsigned long m_UnkB4[2];            // 0xB4
    };
}

#endif /* !MSW_CCOLLDEFUNIT_HPP_ */
