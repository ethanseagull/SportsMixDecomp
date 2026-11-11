#ifndef MSW_CDEFCOLLISION_HPP_
#define MSW_CDEFCOLLISION_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CCollDefUnit.hpp"

namespace msw {

    // A set of defence (reflect) collision units, 0xC bytes. A DOL class
    // (vtable __vt__Q23msw13CDefCollision). Every Set*All applies one value to all units.
    // Parameter meanings are guesses.
    class CDefCollision {
        public:
            CDefCollision();
            CDefCollision(long num, long kind, long type, unsigned long arg);
            virtual ~CDefCollision();
            void Create(long num, long kind, long type, unsigned long arg);
            // Unit index (the units are msw::CCollDefUnit, which derive from
            // CCollUnit).
            CCollUnit *GetColl(long index) const;
            void SetEnableAll();
            void SetDisableAll();
            void SetPosAll(const bcs::FVEC3 &rPos);
            void SetSizeAll(const bcs::FVEC3 &rSize);
            // Radians, wrapped into [0, 2 pi).
            void SetYRotAll(float rot);
            // (added for the DOL's pinball ball)
            void SetIndexAll(long nIndex);
            void SetTeamNoAll(long nTeamNo);
            // (added for VolleyF's CStgGmk_Trolley_Act_Volley)
            void SetReflectDisableAll();
            void SetMyNoHitFlagAll(unsigned long flag);
            void SetReflectHitCallbackAll(CCollDefUnit::ReflectHitCallback pCallback, void *pArg);

        private:
            // CCollUnit::SetPos, which the original inlines here (both were
            // one file there).
            static inline void SetUnitPos_(CCollUnit &rUnit, const bcs::FVEC3 &rPos);
            // CCollDefUnit::SetReflectHitCallback, likewise.
            static inline void SetUnitReflectHitCallback_(CCollDefUnit &rUnit,
                                                          CCollDefUnit::ReflectHitCallback pCallback, void *pArg);

            // 0x00: vtable
            CCollDefUnit *m_pColl;   // 0x04, array of m_nNum
            unsigned long m_nNum;    // 0x08
    };
}

#endif /* !MSW_CDEFCOLLISION_HPP_ */
