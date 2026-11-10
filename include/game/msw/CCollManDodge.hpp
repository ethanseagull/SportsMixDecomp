#ifndef MSW_CCOLLMANDODGE_HPP_
#define MSW_CCOLLMANDODGE_HPP_

#include "game/msw/CCollMan.hpp"

namespace msw {

    // The collision manager of the dodge game (DodgeF). It adds no members
    // to CCollMan (size 0x7C). The party game uses another, file-local class
    // (see CCollManDodge.cpp).
    class CCollManDodge : public CCollMan {
        public:
            CCollManDodge(bcs::CTask *pParent) : CCollMan(pParent) {}
            virtual ~CCollManDodge();
            virtual void Main();
            virtual void Disp();

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags);

        private:
            // The extra spheres around the goals (m_ExCollPos and
            // m_fExCollRadius); see the definition. (name guessed)
            inline void DetectExColl_(int &rResult, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                      const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius, long nKind,
                                      unsigned long flags);

        public:

            // Returns the instance, creating it if there is none yet.
            static CCollMan *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CCOLLMANDODGE_HPP_ */
