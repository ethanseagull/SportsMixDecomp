#ifndef MSW_CITEMVOLLEYRR_HPP_
#define MSW_CITEMVOLLEYRR_HPP_

#include "game/msw/CItemVolley.hpp"

namespace msw {

    // The two items the beach-volley ("RR") game throws
    // (CItemManVolleyRR::CreateThrowItem). Written in
    // game/volley/CItemManVolley.cpp; constructors and destructors are
    // inline, as in CItemVolley.hpp.

    // The toadstool of the RR game: it spins up while flying (the spin is
    // the DOL toadstool's member at 0x1AC) and does not hit target 0x10.
    class CItemVolleyToadstoolRR : public CItemVolleyToadstool {
        public:
            CItemVolleyToadstoolRR(CItemMan *pMan) : CItemVolleyToadstool(pMan) {}
            virtual ~CItemVolleyToadstoolRR() {}

            virtual void Create(const bcs::FVEC3 &pos);
            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoTouchDown();
            virtual void LoopStateThrow();
    };

    // The banana of the RR game: it spins while flying until it lands.
    // Allocated at 0x1B0: it adds the spin speed.
    class CItemVolleyBananaRR : public CItemVolleyBanana {
        public:
            CItemVolleyBananaRR(CItemMan *pMan) : CItemVolleyBanana(pMan), m_fSpin(0.0f) {}
            virtual ~CItemVolleyBananaRR() {}

            virtual void Create(const bcs::FVEC3 &pos);
            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoTouchDown();
            virtual void LoopStateThrow();

        protected:
            float m_fSpin; // 0x1AC, radians per frame (name guessed)
    };
}

#endif /* !MSW_CITEMVOLLEYRR_HPP_ */
