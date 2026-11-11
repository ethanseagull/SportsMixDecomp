#ifndef MSW_CSTAGENETVOLLEY_HPP_
#define MSW_CSTAGENETVOLLEY_HPP_

#include "game/msw/CStageNet.hpp"

namespace msw {

    // The net of the volley court (VolleyF). Size 0x6C. The virtual
    // functions follow __vt__Q23msw15CStageNetVolley; Hit, Reset, IsIdle and
    // GetIdleMotID are its own (return types guessed).
    class CStageNetVolley : public CStageNet {
        public:
            CStageNetVolley(bcs::CTask *pParent);
            virtual ~CStageNetVolley();
            virtual void Main();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap);
            virtual void OnPostCreate();
            // The ball hit the net at rPos moving along rVel (names guessed).
            virtual void Hit(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel);
            virtual void Reset();
            virtual int IsIdle() const;
            virtual long GetIdleMotID() const;

        private:
            long m_Unk68; // 0x68, 1 after construction
    };
}

#endif /* !MSW_CSTAGENETVOLLEY_HPP_ */
