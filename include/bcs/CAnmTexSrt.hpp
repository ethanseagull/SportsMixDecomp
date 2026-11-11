#ifndef BCS_CANMTEXSRT_HPP_
#define BCS_CANMTEXSRT_HPP_

#include "bcs/CAnmObjMat.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    class CMdlObj;

    // Texture matrix animation. Size 0x34 (the modules allocate it). A DOL
    // class. Vtable 0x80463E48; the virtuals past 0x40 are its own (see
    // bcs::CAnmObjMat).
    class CAnmTexSrt : public CAnmObjMat {
        public:
            CAnmTexSrt();
            virtual ~CAnmTexSrt();

            virtual void ObjProc(unsigned long nTask, unsigned long, void *pInfo); // 0x1C
            virtual void SetFrame(float frame);                                    // 0x20
            // 0x24 to 0x40: inline, emitted (and kept) by another file of
            // the DOL.
            virtual float GetFrame() const { return m_Frame.m_fFrame; }            // 0x24
            virtual float VFunc28() const { return m_Frame.m_fEndFrame; }          // 0x28
            virtual void SetRate(float rate) { m_Frame.m_fRate = rate; }           // 0x2C
            virtual float GetRate() const { return m_Frame.m_fRate; }              // 0x30
            virtual void UpdateFrame(float fStep);                                 // 0x34
            virtual int IsLooped() const { return (m_Frame.m_nFlag >> 1) & 1; }    // 0x38
            virtual int VFunc3C() const { return (m_Frame.m_nFlag >> 2) & 1; }     // 0x3C
            virtual int IsEnd() const { return (m_Frame.m_nFlag >> 3) & 1; }       // 0x40

            // 0x50 (names of 0x50 to 0x5C guessed) The result for material
            // idx of the model: the precomputed one when Create allocated
            // them, else evaluated into pResult.
            virtual const nw4r::g3d::TexSrtAnmResult *GetResult(nw4r::g3d::TexSrtAnmResult *pResult, unsigned long idx);
            // 0x54 Maps the model's materials to the animation. Returns 1 if
            // any material is animated.
            virtual int Bind(nw4r::g3d::ResMdl mdl);
            // 0x58 Switches to another animation resource for the model.
            // Returns 0 if it does not fit the arrays Create allocated.
            virtual int ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt res);
            // 0x5C Unbinds every material.
            virtual void Release();

            // Returns 1 on success. With bAllocResult the results of all
            // materials are kept and recomputed by SetFrame. nHeap is
            // passed on to operator new (with 0x100 added).
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexSrt res, int bAllocResult, long nHeap);

        private:
            // (name guessed) Detaches the animation and frees what Create
            // allocated.
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
            // frame. Inline in SetFrame and UpdateFrame.
            void CalcResult()
            {
                if (m_pResult != 0) {
                    float frame = m_Frame.m_fFrame;
                    for (unsigned long i = 0; i < m_nNum; i++) {
                        unsigned short id = m_pTable[i];
                        if (!(id & 0x8000)) {
                            m_Res.GetAnmResult(&m_pResult[id & 0x3FFF], id & 0x3FFF, frame);
                        }
                    }
                }
            }

            nw4r::g3d::ResAnmTexSrt m_Res;               // 0x28
            nw4r::g3d::TexSrtAnmResult *m_pResult;      // 0x2C, one per animated material
            long m_nResultNum;                       // 0x30
    };
}

#endif /* !BCS_CANMTEXSRT_HPP_ */
