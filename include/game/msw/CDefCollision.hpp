#ifndef MSW_CDEFCOLLISION_HPP_
#define MSW_CDEFCOLLISION_HPP_

#include "game/msw/CCollUnit.hpp"

namespace msw {

    // A set of defence (reflect) collision units, 0xC bytes. A DOL class.
    //
    // INCOMPLETE: only what the DOL's msw::CStgGmk_WaterPillar_Act calls;
    // the three words are not mapped, parameter meanings are guesses.
    class CDefCollision {
        public:
            CDefCollision();
            ~CDefCollision();
            void Create(long num, long, long, unsigned long);
            // Unit index (the units are msw::CCollDefUnit, which derive from
            // CCollUnit).
            CCollUnit *GetColl(long index) const;
            void SetEnableAll();
            void SetDisableAll();

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CDEFCOLLISION_HPP_ */
