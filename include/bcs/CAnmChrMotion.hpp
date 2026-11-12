#ifndef BCS_CANMCHRMOTION_HPP_
#define BCS_CANMCHRMOTION_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CFrameCtrl.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // (class name guessed) One skeletal motion: a single nw4r ResAnmChr
    // played on a model. A DOL class with no named function; vtable
    // 0x80464018, size 0x50 (bcs::CAnmMotion::Create allocates two;
    // bcs::CAnmBlendMotion builds arrays of them with
    // __construct_new_array). bcs::CAnmMotion and bcs::CAnmBlendMotion
    // play their motions through it.
    class CAnmChrMotion : public CAnmObjMotBase {
        // The two owners set up and read the node flags of their motions.
        friend class CAnmMotion;
        friend class CAnmBlendMotion;

        public:
            // Callback for each trigger the frame passes.
            typedef void (*TrgCallback)(const MOT_TRG *pTrg, void *pUser);

            // m_nPlayFlag bits (names guessed).
            enum {
                PLAY_MIRROR = 1, // play mirrored (NODE_EX_INFO)
                PLAY_LOOP = 2
            };

            // Not inline (bcs::CAnmBlendMotion passes it to
            // __construct_new_array), but expanded in bcs::CAnmMotion::Create.
            CAnmChrMotion();
            virtual ~CAnmChrMotion();

            virtual void ObjProc(unsigned long nTask, unsigned long, void *pInfo); // 0x1C
            virtual void SetFrame(float frame);                                    // 0x20
            virtual float GetFrame() const;                                        // 0x24
            virtual float VFunc28() const;                                         // 0x28
            virtual void SetRate(float rate);                                      // 0x2C
            virtual float GetRate() const;                                         // 0x30
            virtual void UpdateFrame(float fStep);                                 // 0x34
            // 0x38 to 0x40: inline, emitted after their first caller
            // (bcs::CAnmMotion's forwarders).
            virtual int IsLooped() const { return (m_Frame.m_nFlag >> 1) & 1; }    // 0x38
            virtual int VFunc3C() const { return (m_Frame.m_nFlag >> 2) & 1; }     // 0x3C
            virtual int IsEnd() const { return (m_Frame.m_nFlag >> 3) & 1; }       // 0x40

            virtual const nw4r::g3d::ChrAnmResult *GetResult(nw4r::g3d::ChrAnmResult *pResult, unsigned long nNode);
            virtual int VFunc48();
            virtual void SetAnmFrame(unsigned long nMask, float fFrame);
            virtual float GetAnmFrame(unsigned long nMask) const;
            virtual float VFunc58(long nMask) const;
            virtual void SetAnmRate(unsigned long nMask, float fRate);
            virtual float GetAnmRate(unsigned long nMask) const;
            virtual long VFunc64(long nMask, unsigned long nLayer) const;
            virtual float GetBlendWeight(unsigned long nMask, unsigned long nLayer) const;
            virtual long GetNodeMotID(unsigned long nNode, unsigned long nLayer) const;
            virtual const char *GetAnmName(unsigned long nMask, unsigned long nLayer) const;
            virtual const void *GetMotExtInfo(unsigned long nMask, unsigned long nLayer) const;
            virtual const char *GetNodeAnmName(unsigned long nNode, unsigned long nLayer) const;
            virtual unsigned long GetPartNum() const;
            virtual unsigned long GetLayerNum() const;
            virtual unsigned long GetResAnmNum() const;
            virtual float VFunc88(long nAnim) const;
            virtual int VFunc8C(long nAnim) const;
            virtual const void *GetResMotExtInfo(long nAnim) const;
            virtual const char *GetResAnmName(long nAnim) const;
            virtual int VFunc98(long nMask, unsigned long nLayer) const;
            virtual int VFunc9C(long nMask, unsigned long nLayer) const;
            virtual int VFuncA0(long nMask) const;
            virtual float VFuncA4(long nMask) const;
            virtual void VFuncA8(int bLoop);
            virtual const NODE_EX_INFO *GetNodeExInfo() const { return m_pNodeExInfo; } // 0xAC (inline)

            // (DOL fn_800074FC, name guessed) Starts res. nMotID is what
            // VFunc64 reports; pTrgList the motion's triggers (may be 0).
            void SetAnm(nw4r::g3d::ResAnmChr res, unsigned long nPlayFlag, long nMotID, const MOT_TRG_LIST *pTrgList,
                        float fRate, float fFrame);

            // (name guessed) Allocates the node flags and binds to mdl.
            // Inline in bcs::CAnmMotion::Create.
            void Create(nw4r::g3d::ResMdl mdl, const NODE_EX_INFO *pInfo, unsigned long nPartMask, long nHeap)
            {
                CreateNodeFlag(mdl, nHeap);
                m_nPartMask = nPartMask;
                Init(mdl, pInfo);
            }

            // (name guessed) Unbinds and rewinds. Inline in the Create
            // functions of bcs::CAnmMotion and bcs::CAnmBlendMotion, after
            // they have set the node flags.
            void Init(nw4r::g3d::ResMdl mdl, const NODE_EX_INFO *pInfo)
            {
                VFunc4C();
                m_ResMdl = mdl;
                m_pNodeExInfo = pInfo;
                m_Frame.m_fEndFrame = 0.0f;
                m_Frame.m_nFlag &= ~(CFrameCtrl::FLAG_LOOP | CFrameCtrl::FLAG_LOOPED | CFrameCtrl::FLAG_WRAP |
                                     CFrameCtrl::FLAG_END);
                m_Frame.SetFrame(0.0f);
            }

            // (name guessed) Inline in bcs::CAnmMotion::SetTrgCallback.
            void SetTrgCallback(TrgCallback pFunc, void *pUser)
            {
                m_pTrgCallback = pFunc;
                m_pTrgUser = pUser;
            }

            // (DOL fn_800071DC, name guessed) Detaches the motion and frees
            // what was allocated.
            void Release();

        private:
            // (DOL fn_80007C18, name guessed) Mirrors a node's result the
            // way kind nKind of its NODE_EX_INFO says.
            void MirrorResult(nw4r::g3d::ChrAnmResult *pResult, unsigned char nKind, unsigned short nAxis);
            // (DOL fn_80008454, fn_800086E0, names guessed) Calls the trigger
            // callback for the triggers between the last frame and this one,
            // and finds the trigger index after a jump to another frame.
            void CallTrg();
            void SeekTrg();

            // Calls the callback for pTrg if it is for one of our parts.
            void CallTrg(const MOT_TRG *pTrg)
            {
                if (m_pTrgCallback != 0 && (pTrg->m_nPartMask & m_nPartMask)) {
                    m_pTrgCallback(pTrg, m_pTrgUser);
                }
            }

            // (name guessed) Recomputes the kept results at the current
            // frame. Inline in SetFrame and UpdateFrame.
            void CalcResult()
            {
                if (m_pResultBuf != 0) {
                    float frame = m_Frame.m_fFrame;
                    for (unsigned long i = 0; i < m_nNodeNum; i++) {
                        unsigned short id = m_pNodeFlag[i];
                        if (!(id & NODE_UNBOUND)) {
                            m_ResAnm.GetAnmResult(&m_pResultBuf[i], id & 0x3FFF, frame);
                            if ((m_nPlayFlag & PLAY_MIRROR) && m_pNodeExInfo != 0) {
                                MirrorResult(&m_pResultBuf[i], m_pNodeExInfo[i].nKind, m_pNodeExInfo[i].nUnk04);
                            }
                        }
                    }
                }
            }

        public:
            CFrameCtrl m_Frame;                          // 0x18
            nw4r::g3d::ResMdl m_ResMdl;                  // 0x28
            nw4r::g3d::ResAnmChr m_ResAnm;               // 0x2C
            const NODE_EX_INFO *m_pNodeExInfo;           // 0x30
            nw4r::g3d::ChrAnmResult *m_pResultBuf;       // 0x34 precomputed results (not allocated by the DOL's code)
            unsigned long m_nPlayFlag;                   // 0x38
            short m_nMotID;                              // 0x3C
            short m_nMotID2;                             // 0x3E
            TrgCallback m_pTrgCallback;                  // 0x40
            void *m_pTrgUser;                            // 0x44
            long m_nTrgIdx;                              // 0x48 the next trigger
            const MOT_TRG_LIST *m_pTrgList;              // 0x4C
    };
}

#endif /* !BCS_CANMCHRMOTION_HPP_ */
