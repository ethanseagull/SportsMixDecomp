// msw::CStage04Base: the cart part of a sport's stage 04. The stage hands it
// the cart model; for each cart node of the model ("s04_cart_01" to
// "s04_cart_04") it keeps a CCartNode, which plays the cart's sound
// (0xD1C) from the cart's position each time the cart comes past the front
// of the camera, at most once every 180 frames.
#include "game/msw/CStage04Base.hpp"

#include <math.h>

#include "bcs/CMdlObj.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgSeSlot.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "revolution/mtx.h"

namespace msw {

    // (names guessed) One cart node of the cart model. A list node of
    // 0x34 bytes, vptr at 0x0C (vtable lbl_8048E4D8): the stage's sound
    // slots, the slot it plays in, the model's handle and the node's index,
    // the view whose camera it watches, the frames left before it may play
    // again, and the position function the sound follows. Taking it out of
    // the list deletes it.
    class CStage04Base::CCartNode : public bcs::CListNodeBase {
        public:
            // The node's world position, from the model's matrix of the
            // node (0 when the model is gone). Vtable lbl_8048E500.
            class CPosFunc : public CStgSeSlot::CSESlot::CPosFunc {
                public:
                    CPosFunc(unsigned long nHandle, unsigned long nNode) : m_nHandle(nHandle), m_nNode(nNode) {}
                    virtual ~CPosFunc();
                    virtual void GetPos(bcs::FVEC3 *pPos);

                private:
                    unsigned long m_nHandle; // 0x04
                    unsigned long m_nNode;   // 0x08
            };

            CCartNode(CStgSeSlot *pSeSlot, long nSlot, unsigned long nHandle, unsigned long nNode)
                : m_pSeSlot(pSeSlot),
                  m_nSlot(nSlot),
                  m_nHandle(nHandle),
                  m_nNode(nNode),
                  m_nView(0),
                  m_fWait(10.0f),
                  m_PosFunc(nHandle, nNode)
            {
            }

        private:
            CStgSeSlot *m_pSeSlot;  // 0x10
            long m_nSlot;           // 0x14
            unsigned long m_nHandle; // 0x18
            unsigned long m_nNode;   // 0x1C
            long m_nView;           // 0x20
            float m_fWait;          // 0x24
            CPosFunc m_PosFunc;     // 0x28

        public:
            virtual ~CCartNode();
            virtual void PostUnlink();
            virtual void Main(float frame);
            virtual void StopSE();
    };

    CStage04Base::CStage04Base(CStageMan *pStage)
        : m_pStage(pStage)
    {
    }

    CStage04Base::~CStage04Base()
    {
    }

    void CStage04Base::Stage04_RegistCart(bcs::CMdlObj *pMdl, long nSlot)
    {
        const char *node_tbl[] = {
            "s04_cart_01",
            "s04_cart_02",
            "s04_cart_03",
            "s04_cart_04",
            0
        };

        if (pMdl != 0) {
            nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
            if (mdl.IsValid()) {
                unsigned long nHandle = pMdl->GetHandle();
                for (long i = 0; node_tbl[i] != 0; i++) {
                    nw4r::g3d::ResNode node = mdl.GetResNode(node_tbl[i]);
                    if (node.IsValid()) {
                        unsigned long nNode = node.GetID();
                        long nCartSlot = nSlot + i;
                        CStgSeSlot *pSeSlot = m_pStage;
                        CCartNode *pCart = new CCartNode(pSeSlot, nCartSlot, nHandle, nNode);
                        if (pCart != 0) {
                            pCart->Link(m_CartList.GetEnd());
                        }
                    }
                }
            }
        }
    }

    void CStage04Base::Stage04_ReleaseCart()
    {
        m_CartList.Clear();
    }

    void CStage04Base::Stage04_Main(float frame)
    {
        for (bcs::CListNodeBase *pNode = m_CartList.CListBase::GetFirst(); pNode != m_CartList.GetEnd();
             bcs::CListNodeBase::Step(pNode)) {
            static_cast<CCartNode *>(pNode)->Main(frame);
        }
    }

    void CStage04Base::CCartNode::CPosFunc::GetPos(bcs::FVEC3 *pPos)
    {
        pPos->Set(0.0f, 0.0f, 0.0f);
        bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(m_nHandle));
        if (pMdl != 0) {
            bcs::FMTX34 mtx;
            pMdl->GetScnMtxPos(&mtx, m_nNode, bcs::CScnObj::SCNOBJMTXTYPE_1);
            pPos->Set(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        }
    }

    CStage04Base::CCartNode::CPosFunc::~CPosFunc()
    {
    }

    CStage04Base::CCartNode::~CCartNode()
    {
        StopSE();
    }

    void CStage04Base::CCartNode::Main(float frame)
    {
        if (frame <= 0.0f) {
            return;
        }
        if (m_fWait > 0.0f) {
            m_fWait -= frame;
            if (m_fWait < 0.0f) {
                m_fWait = 0.0f;
            }
        }

        // The cart's position seen from the camera.
        nw4r::g3d::Camera camera(reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(m_nView)));
        bcs::FVEC3 pos;
        Vec view;
        Mtx camMtx;
        Mtx invMtx;
        m_PosFunc.GetPos(&pos);
        camera.GetCameraMtx(reinterpret_cast<nw4r::math::MTX34 *>(camMtx));
        PSMTXInverse(camMtx, invMtx);
        PSMTXMultVec(invMtx, reinterpret_cast<Vec *>(&pos), &view);

        if (m_fWait <= 0.0f && fabsf(view.x) < 10.0f) {
            m_pSeSlot->SE_PlayPos(m_nSlot, 0xD1C, 0.0f, 0, &m_PosFunc);
            m_fWait = 180.0f;
        }
    }

    void CStage04Base::CCartNode::StopSE()
    {
        if (m_pSeSlot != 0 && m_nSlot >= 0) {
            m_pSeSlot->SE_Stop(m_nSlot, 0.0f);
        }
    }

    void CStage04Base::CCartNode::PostUnlink()
    {
        delete this;
    }
}
