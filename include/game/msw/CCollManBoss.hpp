#ifndef MSW_CCOLLMANBOSS_HPP_
#define MSW_CCOLLMANBOSS_HPP_

#include "game/msw/CCollMan.hpp"

namespace msw {

    // The collision manager of the boss game (BossF). It adds no members to
    // CCollMan (size 0x7C): the court is one circle around the origin.
    class CCollManBoss : public CCollMan {
        public:
            CCollManBoss(bcs::CTask *pParent) : CCollMan(pParent) {}
            virtual ~CCollManBoss();
            virtual void Main();
            virtual void Disp();

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long, unsigned long);

            // Returns the instance, creating it if there is none yet.
            static CCollMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CCOLLMANBOSS_HPP_ */
