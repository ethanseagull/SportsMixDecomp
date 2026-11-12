#ifndef BCS_CANMSHP_HPP_
#define BCS_CANMSHP_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmShp.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    class CMdlObj;

    // Shape animation (DOL class, 0x3C bytes, unnamed in the DOL: class name
    // guessed after the other animation kinds). Vtable lbl_80463D88, laid
    // out as bcs::CAnmTexPat's (see bcs::CAnmObjMat). It uses the
    // CAnmMatTable part for the model's vertex position arrays instead of
    // its materials.
    class CAnmShp : public CAnmObjMat {
        public:
            CAnmShp();
            virtual ~CAnmShp();

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

            // 0x50 The result for vertex position array idx of the model:
            // the precomputed one when Create allocated them, else evaluated
            // into pResult.
            virtual const nw4r::g3d::ShpAnmResult *GetResult(nw4r::g3d::ShpAnmResult *pResult, unsigned long idx);
            // 0x54 Maps the model's vertex arrays to the animation. Returns
            // 1 if any position array is animated.
            virtual int Bind(nw4r::g3d::ResMdl mdl);
            // 0x58 Switches to another animation resource for the model.
            // Returns 0 if it does not fit the arrays Create allocated.
            virtual int ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmShp res);
            // 0x5C Unbinds every vertex array.
            virtual void Release();

            // Returns 1 on success (arguments as for bcs::CAnmTexPat).
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmShp res, int bAllocResult, long nHeap);

        private:
            // (name guessed) Detaches the animation and frees what Create
            // allocated.
            void Destroy()
            {
                DetachFromParent();
                if (m_pVtxSet != 0) {
                    delete[] m_pVtxSet;
                }
                m_pVtxSet = 0;
                m_nVtxSetNum = 0;
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
            // frame. Inline in SetFrame and UpdateFrame.
            void CalcResult()
            {
                if (m_pResult != 0) {
                    float frame = m_Frame.m_fFrame;
                    for (unsigned long i = 0; i < m_nNum; i++) {
                        unsigned short id = m_pTable[i];
                        if (!(id & 0x8000)) {
                            m_Res.GetAnmResult(&m_pResult[id & 0x3FFF], id & 0x3FFF, frame, m_pVtxSet);
                        }
                    }
                }
            }

            nw4r::g3d::ResAnmShp m_Res;            // 0x28
            nw4r::g3d::ShpAnmVtxSet *m_pVtxSet;    // 0x2C, one per vertex array name of the resource
            long m_nVtxSetNum;                     // 0x30
            nw4r::g3d::ShpAnmResult *m_pResult;    // 0x34, one per animated shape
            unsigned long m_nResultNum;            // 0x38
    }; // size 0x3C
}

#endif /* !BCS_CANMSHP_HPP_ */
