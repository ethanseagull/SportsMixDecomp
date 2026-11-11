#ifndef MSW_CSTAGENETVOLLEY_HPP_
#define MSW_CSTAGENETVOLLEY_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "game/msw/CStageNet.hpp"
#include "game/msw/CStgGmkAct.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    // A model of the volley net (size 0x12C, CStgMdl's). Its constructor is
    // inline in CStageNetVolley::NewMdl. FindNodeExInfoTbl is its own
    // virtual function (slot 0x124).
    class CStageNetMdlVolley : public CStgMdl {
        public:
            CStageNetMdlVolley(CStgObj *pParent) : CStgMdl(pParent) {}
            virtual ~CStageNetMdlVolley();
            virtual void Create();
            // The per-node motion settings of the net for a stage, or 0.
            virtual const bcs::CAnmObjMotBase::NODE_EX_INFO *FindNodeExInfoTbl(long stage) const;
    };

    // The actor of the volley net (VolleyF game/volley/CStageNetVolley.cpp):
    // a CStgGmkAct (size 0x174) that only moves the net's models with it.
    // Its constructor is inline in CStageNetVolley::NewAct.
    class CStageNetActVolley : public CStgGmkAct {
        public:
            CStageNetActVolley(CStgObj *pParent) : CStgGmkAct(pParent) {}
            virtual ~CStageNetActVolley();
            virtual void OnUpdate(float rate);
            virtual void UpdateMdlMtx();
    };

    // The net of the volley court (VolleyF). Size 0x6C. The virtual
    // functions follow __vt__Q23msw15CStageNetVolley; Hit, Reset, IsIdle and
    // GetIdleMotID are its own (return types guessed). NewMdl, NewAct and
    // IsIdle are inline (emitted weak after the file's other functions).
    //
    // Models: 0 the frame (the others hang on it), 1 the net (animated), 2
    // and 3 the goal parts, 4 to 6 their low-detail versions.
    class CStageNetVolley : public CStageNet {
        public:
            enum {
                MDL_FRAME,
                MDL_NET,
                MDL_GOAL_01,
                MDL_GOAL_02,
                MDL_NET_LO,
                MDL_GOAL_LO_01,
                MDL_GOAL_LO_02,
                MDL_NUM
            };

            CStageNetVolley(bcs::CTask *pParent);
            virtual ~CStageNetVolley();
            virtual void Main();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx) { return new (heap) CStageNetMdlVolley(this); }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStageNetActVolley(this); }
            virtual void OnPostCreate();
            // The ball hit the net at rPos moving along rVel (names guessed):
            // plays the net's hit motion for that spot of the net.
            virtual void Hit(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel);
            virtual void Reset();
            virtual int IsIdle() const { return m_bIdle; }
            virtual long GetIdleMotID() const;

        private:
            // 0x68, 1 while the net plays its idle motion (after
            // construction, Reset and the end of a hit motion); 0 after Hit.
            int m_bIdle;
    };
}

#endif /* !MSW_CSTAGENETVOLLEY_HPP_ */
