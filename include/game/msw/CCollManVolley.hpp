#ifndef MSW_CCOLLMANVOLLEY_HPP_
#define MSW_CCOLLMANVOLLEY_HPP_

#include "game/msw/CCollMan.hpp"

namespace msw {

    // The collision manager of the volley game (VolleyF). It adds no members
    // to CCollMan (size 0x7C): DetectMapHit_ collides with a rectangle of the
    // stage chosen by its flags. CreateInstance makes a variant local to
    // CCollManVolley.cpp instead for game type 2 (guessed to be the party game).
    class CCollManVolley : public CCollMan {
        public:
            CCollManVolley(bcs::CTask *pParent) : CCollMan(pParent) {}
            virtual ~CCollManVolley();
            virtual void Main();
            virtual void Disp();

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long, unsigned long);

            // Returns the instance, creating it if there is none yet.
            static CCollMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CCOLLMANVOLLEY_HPP_ */
