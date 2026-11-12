#ifndef BCS_CANMMATCLR_HPP_
#define BCS_CANMMATCLR_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    class CMdlObj;

    // Material colour animation. Size 0x34 (the modules allocate it). A DOL
    // class. Vtable 0x80463D28, laid out as bcs::CAnmTexPat's (see
    // bcs::CAnmObjMat).
    class CAnmMatClr : public CAnmObjMat {
        public:
            CAnmMatClr();
            virtual ~CAnmMatClr();

            virtual void ObjProc(unsigned long nTask, unsigned long, void *pInfo); // 0x1C
            virtual void SetFrame(float frame);                                    // 0x20
            virtual float GetFrame() const { return m_Frame.m_fFrame; }            // 0x24
            virtual float VFunc28() const { return m_Frame.m_fEndFrame; }          // 0x28
            virtual void SetRate(float rate) { m_Frame.m_fRate = rate; }           // 0x2C
            virtual float GetRate() const { return m_Frame.m_fRate; }              // 0x30
            virtual void UpdateFrame(float fStep);                                 // 0x34
            virtual int IsLooped() const { return (m_Frame.m_nFlag >> 1) & 1; }    // 0x38
            virtual int VFunc3C() const { return (m_Frame.m_nFlag >> 2) & 1; }     // 0x3C
            virtual int IsEnd() const { return (m_Frame.m_nFlag >> 3) & 1; }       // 0x40

            // Returns 1 on success, 0 if the model has no resource. With
            // bAllocResult the results of all materials are kept and
            // recomputed by SetFrame. nHeap is passed on to operator new
            // (with 0x100 added). msw::CTableSetMdlObj passes 0 for both.
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmClr res, int bAllocResult, long nHeap);

            // 0x50 The result for material idx of the model: the
            // precomputed one when Create allocated them, else evaluated
            // into pResult; empty when the material is not animated or
            // nothing is bound.
            virtual const nw4r::g3d::ClrAnmResult *GetResult(nw4r::g3d::ClrAnmResult *pResult, unsigned long idx);
            // 0x54 Maps the model's materials to the animation. Returns 1 if
            // any material is animated.
            virtual int Bind(nw4r::g3d::ResMdl mdl);
            // 0x58 Switches to another animation resource for the model
            // (VolleyF's CC15_VOLLEY::CcSiromaMagicCircleObj::Main). Returns
            // 0 if it does not fit the arrays Create allocated.
            virtual int ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr res);
            // 0x5C Unbinds every material.
            virtual void Release();

        private:
            // (name guessed) Detaches the animation and frees what Create
            // allocated; the same as bcs::CAnmTexSrt's.
            void Destroy()
            {
                DetachFromParent();
                if (m_pResult != 0) {
                    delete[] m_pResult;
                }
                m_pResult = 0;
                m_nResultNum = 0;
                ResetFrame();
                ReleaseMatTable();
            }

            // (name guessed) Takes the length and the loop policy of the
            // resource and rewinds. Inline in Create and ChangeRes.
            void InitFrame()
            {
                m_Frame.m_fEndFrame = m_Res.GetNumFrame();
                m_Frame.SetLoop(m_Res.GetAnmPolicy() == nw4r::g3d::ANM_POLICY_LOOP);
                m_Frame.SetFrame(0.0f);
            }

            // (name guessed) Recomputes the kept results at the current
            // frame. Out of line (after its callers) unlike CAnmTexPat's.
            void CalcResult();

            // Laid out like CAnmTexSrt and CAnmTexPat, whose constructors
            // and destructors are copies of this class's.
            nw4r::g3d::ResAnmClr m_Res;  // 0x28
            nw4r::g3d::ClrAnmResult *m_pResult; // 0x2C, one per material of the model
            long m_nResultNum;           // 0x30
    };
}

#endif /* !BCS_CANMMATCLR_HPP_ */
