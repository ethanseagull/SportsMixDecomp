#ifndef BCS_CANMMOTION_HPP_
#define BCS_CANMMOTION_HPP_

#include "bcs/CAnmChrMotion.hpp"
#include "bcs/CAnmObjMotBase.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {

    class CMdlObj;

    // Skeletal animation, the object in a model's first animation slot. A
    // DOL class, size 0x4C, vtable 0x80463F68 (code 0x80008824 to
    // 0x80009C78 of the animation file). It plays one motion at a time
    // through two bcs::CAnmChrMotion objects (the current one and the
    // previous one) and cross-fades between them over the blend frames
    // given to Play.
    class CAnmMotion : public CAnmObjMotBase {
        public:
            CAnmMotion();
            virtual ~CAnmMotion();

            // m_nFlag bits (names guessed).
            enum {
                FLAG_PLAY = 1,       // a motion plays
                FLAG_BLEND = 2,      // blending from the previous motion
                FLAG_HOLD = 4,       // the new motion holds its first frame during the blend
                FLAG_BLEND_KEEP = 8  // the blend started from the result of a blend
            };

            virtual void ObjProc(unsigned long nTask, unsigned long, void *pInfo); // 0x1C
            // 0x20 to 0x40 forward to the current motion.
            virtual void SetFrame(float frame);
            virtual float GetFrame() const;
            virtual float VFunc28() const;
            virtual void SetRate(float rate);
            virtual float GetRate() const;
            virtual void UpdateFrame(float fStep);
            virtual int IsLooped() const;
            virtual int VFunc3C() const;
            virtual int IsEnd() const;

            // 0x44 to 0xAC: the slots are declared in bcs::CAnmObjMotBase.
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
            // 0x7C, 0x80: inline (weak copies at the end of the file).
            virtual unsigned long GetPartNum() const { return 1; }
            virtual unsigned long GetLayerNum() const { return 1; }
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
            virtual const NODE_EX_INFO *GetNodeExInfo() const;

            // Binds the motions of `file` to pMdl. (meanings of the last two
            // arguments not known)
            void Create(const CMdlObj *pMdl, nw4r::g3d::ResFile file, const NODE_EX_INFO *pInfo, unsigned long,
                        long);

            // The three floats are passed on unchanged in the DOL; their
            // names are guessed from msw::CMenuMdlTableSet::PlayImpl.
            void Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame);
            // The same for motion number idx of the file given to Create.
            void Play(long idx, unsigned long flag, float rate, float blend, float frame);
            // Clears bit 0 of the word at 0x18.
            void Stop();
            // (DOL fn_8000931C, name guessed; called from the DOL's
            // 0x8015AB9C code to replay the current motion) The ResAnmChr
            // of the current motion.
            nw4r::g3d::ResAnmChr GetResAnmChr() const;
            // (DOL fn_800095A0, unnamed; name guessed) Clears bit 1 (looped) of
            // the word at 0x18 of the current motion. msw::CChrOptMdlObj
            // passes 3; the argument is not read.
            void ResetLoopedFlag(long);
            // (DOL) The frame of trigger nTrg of animation nAnim. (meaning
            // of the float not known)
            float SearchTrgFrame(long nAnim, unsigned long nTrg, float) const;
            // (DOL fn_80008D38, unnamed; name guessed) Takes the motions from
            // another file.
            void ChangeResFile(nw4r::g3d::ResFile file);
            // (DOL; parameter names guessed) Calls pFunc with pUser for each
            // trigger the motion passes.
            void SetTrgCallback(void (*pFunc)(const MOT_TRG *pTrg, void *pUser), void *pUser);

        private:
            // (DOL fn_80008C08, name guessed) Detaches the motion and frees
            // what Create allocated.
            void Release();
            // (DOL fn_80008F8C, name guessed) What both Play functions do.
            void PlayImpl(nw4r::g3d::ResAnmChr res, unsigned long flag, long nMotID, const MOT_TRG_LIST *pTrgList,
                          float rate, float blend, float frame);
            // (DOL fn_80009114, name guessed) Blends the results of the two
            // motions into m_pResult[m_nCur].
            void CalcBlend();

            // The motion being played, and the one before it (blended from).
            CAnmChrMotion *GetCurMotion() const { return m_pMotion[m_nCur]; }
            CAnmChrMotion *GetPrevMotion() const { return m_pMotion[1 - m_nCur]; }

            // (name guessed) The ResAnmChr of motion number nAnim (negative:
            // mirrored).
            nw4r::g3d::ResAnmChr GetResAnmChr(long nAnim) const
            {
                unsigned long idx;
                if (m_pExInfo != 0) {
                    idx = m_pExInfo->GetResAnmIndex(nAnim);
                } else {
                    idx = abs(nAnim);
                }
                // The original reads the count here without using it (an
                // assertion compiled out, probably).
                m_ResFile.GetResAnmChrNumEntries();
                return m_ResFile.GetResAnmChr(idx);
            }

            // (name guessed) Takes the motion extra data of m_ResFile.
            void LoadMotExInfo();

            unsigned long m_nFlag;                      // 0x18
            const CMdlObj *m_pMdl;                      // 0x1C
            const NODE_EX_INFO *m_pNodeExInfo;          // 0x20
            MOT_EX_INFO *m_pExInfo;                     // 0x24
            nw4r::g3d::ResFile m_ResFile;               // 0x28
            long m_nCur;                                // 0x2C the current motion (0 or 1)
            float m_fBlendFrame;                        // 0x30
            float m_fBlendStep;                         // 0x34
            float m_fBlendRate;                         // 0x38 weight of the previous motion, 1 down to 0
            CAnmChrMotion *m_pMotion[2];                // 0x3C
            nw4r::g3d::ChrAnmResult *m_pResult[2];      // 0x44 one per node
    };
}

#endif /* !BCS_CANMMOTION_HPP_ */
