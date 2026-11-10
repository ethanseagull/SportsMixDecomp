#ifndef MSW_CCOLLMANBASKET_HPP_
#define MSW_CCOLLMANBASKET_HPP_

#include "game/msw/CCollMan.hpp"

namespace msw {

    // The collision manager of the basket game (BasketF). It adds no members
    // to CCollMan (size 0x7C).
    class CCollManBasket : public CCollMan {
        public:
            CCollManBasket(bcs::CTask *pParent) : CCollMan(pParent) {}
            virtual ~CCollManBasket();
            virtual void Main();
            virtual void Disp();

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long mode, unsigned long flags);

            // Returns the instance, creating it if there is none yet.
            static CCollMan *CreateInstance(bcs::CTask *pParent);

        private:
            // (name guessed; inline twice in DetectMapHit_, defined in
            // CCollManBasket.cpp) With the view
            // collision on and mode 1, moves the front or back edge of rRect
            // (the side m_nViewCollTargetDir names) out to the view collision
            // plane, the value CCollMan::GetViewCollPosZ_ returns.
            inline void ClipRectByViewColl_(bcs::FVEC4 &rRect, long mode) const;
    };
}

#endif /* !MSW_CCOLLMANBASKET_HPP_ */
