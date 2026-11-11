#ifndef CEVDEMOMDLCHR_HPP_
#define CEVDEMOMDLCHR_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "game/msw/CEventModel.hpp"

namespace msw {

    // A character model of an event demo. A DOL class, 0xE0 bytes (the size
    // CreateModel allocates), built on msw::CEventModel: a msw::CChrMdlObj
    // with a bcs::CAnmMotion.
    class CEvDemoMdlChr : public CEventModel {
        public:
            // Size 0x18. What CreateModel builds a model from. The layout
            // is read from CreateModel in the DOL and from the two callers
            // in the modules (msw::CEvMdlCreaterTmMdlChr in TourF,
            // msw::CEvMdlCreaterGmMdlChr in BossF), which both fill it the
            // same way; the constructor is inline there and its parameters
            // are a guess, as are the member names.
            struct _SETUP {
                _SETUP(nw4r::g3d::ResFile &rMdlRes, nw4r::g3d::ResFile &rMotRes, int disp)
                {
                    pMotTable = 0;
                    pInit = 0;
                    nType = 0;
                    nIndex = 0;
                    nDrawLayer = 2;
                    bDisp = disp;
                    nUnk16 = -1;
                    nLightSelector = 0;
                    pMdlRes = &rMdlRes;
                    pMotRes = &rMotRes;
                }

                void *pInit;                 // 0x00: kept by the model at 0xD0
                const short *pMotTable;      // 0x04: the model's motion table
                nw4r::g3d::ResFile *pMdlRes; // 0x08: file with the model
                nw4r::g3d::ResFile *pMotRes; // 0x0C: file with its animations
                short nType;                 // 0x10: CEventModel::GetType; the character
                signed char nIndex;          // 0x12: CEventModel::GetIndex
                signed char nDrawLayer;      // 0x13
                signed char bDisp;           // 0x14: argument of SetDisp
                signed char nLightSelector;  // 0x15: argument of SetLightSelector; -1 for none
                short nUnk16;                // 0x16
            };

            // Static: its body in the DOL takes the setup in r3. The second
            // argument, kept by the model at 0xDC, picks how the model is
            // built (2: CChrMdlObj::CreateEventMiiObj). (name guessed)
            static CEvDemoMdlChr *CreateModel(const _SETUP &rSetup, long nKind);

            // (DOL functions unnamed in the DOL from here on; their names
            // are the base's, the parameter names guessed. Vtable
            // lbl_8046F3CC.)
            CEvDemoMdlChr(const short *pMotTable);
            virtual ~CEvDemoMdlChr();
            virtual void _SetPos(const bcs::FVEC3 &rPos);
            virtual void _SetRot(const bcs::FVEC3 &rRot);
            virtual void _SetScale(const bcs::FVEC3 &rScale);
            virtual void SetDisp(int bDisp);
            virtual void SetShadowDisp(int bDisp);
            virtual void UpdateFrame(float fFrame);
            virtual void SetDrawLayer(long nLayer);
            virtual void SetDrawPriority(long nPriority);
            virtual void AppendChild(bcs::CScnGroup *pGroup);
            virtual void SetLightSelector(long nSelector);
            virtual void ResetLightSelector();
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual const bcs::FMTX34 *GetNodeWorldMtxPtr(unsigned long nNode) const;
            virtual long GetNodeId(const char *pName) const;
            virtual void ChangeAnimFile(nw4r::g3d::ResFile file);
            // A negative fFrame leaves the frame; bStop stops the motion.
            virtual void SetAnimFrame(float fFrame, int bStop);
            virtual void SetAnimSpeed(float fSpeed);
            virtual int IsAnimResLoopType(long nMot) const;
            virtual float AnimResFrame(long nMot) const;
            virtual float AnimResTrgFrame(long nMot, unsigned long nTrg, float) const;
            virtual float AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq);
            virtual float GetNowAlpha() const;
            // Four sound slots and four effect slots.
            virtual long GetSeMax() const { return 4; }
            virtual bcs::CSndTask *GetSndTaskSe(long n);
            virtual void SetSndTaskSe(long n, unsigned long hSe);
            virtual long GetEffMax() const { return 4; }
            virtual unsigned long GetEffcetHandle(long n) const;
            virtual void SetEffectHandle(unsigned long hEffect, long n);
            virtual void Destroy();

        private:
            // The motion's trigger callback (pUser is the model) and what it
            // does: voices, the face, foot sounds.
            static void CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pUser);
            void _OnMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg);
            // Plays sound nSe in slot nSlot at node pNode of the model.
            void _PlayNodeSe(const char *pNode, long nSe, long nSlot);
            // Plays the character's foot sound nVar in slot nSlot at node
            // pNode. (inline in the original)
            void _PlayFootSe(const char *pNode, long nVar, long nSlot);

            unsigned long m_hSe[4];      // 0xB0
            unsigned long m_hEffect[4];  // 0xC0
            void *m_pInit;               // 0xD0: the setup's pInit
            CChrMdlObj *m_pMdl;          // 0xD4
            bcs::CAnmMotion *m_pMotion;  // 0xD8
            long m_nKind;                // 0xDC: CreateModel's second argument
    }; // size 0xE0
}

#endif /* !CEVDEMOMDLCHR_HPP_ */
