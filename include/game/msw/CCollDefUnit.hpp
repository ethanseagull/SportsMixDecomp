#ifndef MSW_CCOLLDEFUNIT_HPP_
#define MSW_CCOLLDEFUNIT_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CBall;

    // (purpose guessed from the name) The defence (reflect) volume of a
    // collision. Lives in the DOL. Only the members used so far; the callback
    // and its argument are at 0xAC and 0xB0.
    class CCollDefUnit {
        public:
            typedef unsigned long (*ReflectHitCallback)(const CCollDefUnit *pDef, const CBall *pBall,
                                                        bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, void *pArg);

            void SetReflectHitCallback(ReflectHitCallback pCallback, void *pArg);

        private:
            unsigned char m_Unk00[0xAC];         // 0x00
            ReflectHitCallback m_pReflectHitCallback; // 0xAC
            void *m_pReflectHitArg;              // 0xB0
    };
}

#endif /* !MSW_CCOLLDEFUNIT_HPP_ */
