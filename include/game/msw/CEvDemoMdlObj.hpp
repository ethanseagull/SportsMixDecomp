#ifndef CEVDEMOMDLOBJ_HPP_
#define CEVDEMOMDLOBJ_HPP_

#include "bcs/CMdlObj.hpp"
#include "game/msw/CEventModel.hpp"

namespace msw {

    // An object model of an event demo (the props and the NPCs). A DOL
    // class built on msw::CEventModel; its own members are not mapped.
    // Only the members used so far.
    class CEvDemoMdlObj : public CEventModel {
        public:
            // Size 0x14. What CreateModel builds a model from. The layout is
            // read from the two creaters in BossF (msw::CEvMdlCreaterGmMdlObj
            // and GmMdlNpc), which fill it the same way; the constructor is
            // inline there and its parameter and the member names are
            // guesses.
            struct _SETUP {
                _SETUP(int disp)
                {
                    pMdlRes = 0;
                    pMotRes = 0;
                    pInit = 0;
                    nType = 0;
                    nIndex = 0;
                    nDrawLayer = 2;
                    bDisp = disp;
                    nShadowSize = -1;
                    nLightSelector = -1;
                }

                void *pInit;                 // 0x00
                nw4r::g3d::ResFile *pMdlRes; // 0x04: file with the model
                nw4r::g3d::ResFile *pMotRes; // 0x08: file with its animations
                short nType;                 // 0x0C
                signed char nIndex;          // 0x0E
                signed char nDrawLayer;      // 0x0F
                signed char bDisp;           // 0x10
                signed char nLightSelector;  // 0x11
                short nShadowSize;           // 0x12
            };

            // Static: their bodies in the DOL take the setup in r3. The
            // first makes the model from the given files; the second from
            // model pName in archive nArc (a msw::CLoadData entry; pSub as
            // for EvDemo::InitArcResData). (meaning of the int not known;
            // parameter names guessed)
            static CEvDemoMdlObj *CreateModel(const _SETUP &rSetup, int);
            static CEvDemoMdlObj *CreateModel(_SETUP &rSetup, long nArc, const char *pSub, const char *pName);
            // (parameter names guessed) A model from file pFile of the
            // effect resources (msw::CTournamentMan, TourF).
            static CEvDemoMdlObj *CreateModelEffRes(_SETUP &rSetup, const char *pFile, const char *pName);

            // (The members below are added for the DOL's CreateModel; the
            // names of the functions unnamed in the DOL are the base's, all
            // parameter and member names guessed. Vtable lbl_80489260, size
            // 0xE0.) The constructor is inline in CreateModel.
            CEvDemoMdlObj() : CEventModel(0), m_pMdl(0), m_pShadow(0), m_pMotion(0), m_pName(0)
            {
                for (int i = 0; i < 4; i++) {
                    m_hEffect[i] = 0;
                    m_nEffectId[i] = -1;
                }
            }
            virtual ~CEvDemoMdlObj();                                   // fn_80220CA0
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
            // Inline: the original emits them weak after CreateModelEffRes.
            virtual bcs::CScnObj *GetScnObj() { return m_pMdl; }
            virtual const bcs::CScnObj *GetScnObj() const { return m_pMdl; }
            virtual const bcs::FMTX34 *GetNodeWorldMtxPtr(unsigned long nNode) const;
            virtual long GetNodeId(const char *pName) const;
            virtual void SetMatTevColor(const char *pMat, bcs::BCS_Color color, _GXTevRegID reg);
            virtual void SetBaseColor(bcs::BCS_Color color);
            virtual void SetColorScale(bcs::BCS_Color color);
            virtual void SetCameraView(long nView);
            virtual void SetAnimSpeed(float fSpeed);
            virtual int IsAnimResLoopType(long nMot) const;
            virtual float AnimResFrame(long nMot) const;
            virtual float AnimResTrgFrame(long nMot, unsigned long nTrg, float) const;
            virtual float AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq);
            virtual float AnimPlay(const char *pName, unsigned long nFlag, float fBlend, float fFrame, int bStopSeq);
            virtual float AnimPlay(nw4r::g3d::ResFile file, const char *pName, unsigned long nFlag, float fBlend,
                                   float fFrame, int bStopSeq);
            virtual float GetNowAlpha() const;
            // Four effect slots.
            virtual long GetEffMax() const;
            virtual unsigned long GetEffcetHandle(long n) const;
            virtual void SetEffectHandle(unsigned long hEffect, long n);
            virtual long GetEffcetId(long n) const;
            virtual void SetEffectId(long nId, long n);
            virtual void Destroy();                                     // fn_80220D14

        private:
            // (name guessed) The model m_pName in the model file, the first
            // one without a name. Inline in CreateModel.
            nw4r::g3d::ResMdl GetResMdl() const
            {
                if (m_pName != 0) {
                    return GetMdlFile().GetResMdl(m_pName);
                }
                return GetMdlFile().GetResMdl(0);
            }

            long m_nEffectId[4];            // 0xB0: -1 for none
            unsigned long m_hEffect[4];     // 0xC0
            bcs::CMdlObj *m_pMdl;           // 0xD0: a msw::CEventMdlObj (CEventModel::_CreateMdlObj)
            bcs::CMdlShadow *m_pShadow;     // 0xD4: only with a shadow size
            bcs::CAnmMotion *m_pMotion;     // 0xD8
            const char *m_pName;            // 0xDC: the setup's pInit, the model's name in its file
    }; // size 0xE0
}

#endif /* !CEVDEMOMDLOBJ_HPP_ */
