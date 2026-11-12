#include "game/msw/CStgEffectSlot.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    // One effect slot: the effect it holds, and an effect waiting to be
    // created or stopped after a delay. Names are guesses.
    class CStgEffectSlot::CEffSlot {
        public:
            // The data members come before the first virtual function: the
            // vptr is at 0x48.
            unsigned long m_hEffect;       // 0x00
            float m_fCreateDelay;          // 0x04, -1 when nothing waits
            float m_fEndDelay;             // 0x08, -1 when nothing waits
            long m_nCreateMode;            // 0x0C
            long m_nEndMode;               // 0x10
            float m_fFadeIn;               // 0x14
            float m_fFadeOut;              // 0x18
            long m_nEff;                   // 0x1C
            const bcs::CScnObj *m_pObj;    // 0x20
            unsigned long m_nNode;         // 0x24
            const bcs::FVEC3 *m_pOfs;      // 0x28, &m_Ofs or null
            int m_nArg0;                   // 0x2C
            int m_nArg1;                   // 0x30
            int m_nArg2;                   // 0x34
            unsigned long m_nFlag;         // 0x38
            bcs::FVEC3 m_Ofs;              // 0x3C

            // How a delayed effect is created (m_nCreateMode).
            enum {
                CREATE_ATTACH = 0,
                CREATE_ATTACH_FADEIN = 1,
            };

            // How a delayed effect ends (m_nEndMode); anything else stops it.
            enum {
                END_STOP = 2,
                END_ERASE = 3,
                END_FADEOUT = 4,
            };

            CEffSlot();
            virtual ~CEffSlot();

            // The slot's cluster, null when it has none.
            CEffectCluster *GetCluster() const
            {
                if (m_hEffect == 0) {
                    return 0;
                }
                return CEffectCtrl::GetInstance()->FindList(m_hEffect);
            }

            void Stop()
            {
                CEffectCtrl::StopAttachEffect(m_hEffect);
                m_fCreateDelay = -1.0f;
                m_fEndDelay = -1.0f;
            }

            void Erase()
            {
                CEffectCtrl::EraseAttachEffect(m_hEffect);
                m_fCreateDelay = -1.0f;
                m_fEndDelay = -1.0f;
            }

            void FadeIn(float fade)
            {
                CEffectCluster *pCluster = GetCluster();
                if (pCluster != 0) {
                    pCluster->SetFadeinTime(fade);
                }
            }

            void FadeOut(float fade)
            {
                CEffectCluster *pCluster = GetCluster();
                if (pCluster != 0) {
                    pCluster->SetFadeoutTime(fade);
                }
                m_fCreateDelay = -1.0f;
                m_fEndDelay = -1.0f;
            }

            void CreateAttach(long eff, const bcs::CScnObj *pObj, unsigned long node, const bcs::FVEC3 *pOfs, int a,
                              int b, int c, unsigned long flag)
            {
                Stop();
                m_hEffect = CEffectCtrl::CreateAttachEffect(eff, pObj, pOfs, node, a, b, c, flag, 0, 0, 0);
                m_fCreateDelay = -1.0f;
            }

            bool IsCreateWait() const
            {
                if (m_fCreateDelay > 0.0f) {
                    return true;
                }
                return false;
            }
            bool IsEndWait() const
            {
                if (m_fEndDelay > 0.0f) {
                    return true;
                }
                return false;
            }

            void SetEnd(long mode, float delay)
            {
                m_fEndDelay = delay;
                m_nEndMode = mode;
            }

            void DelayCreateAttach(float delay, long eff, const bcs::CScnObj *pObj, unsigned long node,
                                   const bcs::FVEC3 *pOfs, int a, int b, int c, unsigned long flag);
            void DelayCreateAttachFadeIn(float delay, float fade, long eff, const bcs::CScnObj *pObj,
                                         unsigned long node, const bcs::FVEC3 *pOfs, int a, int b, int c,
                                         unsigned long flag);
            void Update(float rate);
    };

    CStgEffectSlot::CStgEffectSlot() : m_pSlot(0), m_nSlotNum(0) {}

    CStgEffectSlot::~CStgEffectSlot()
    {
        Eff_StopAll();
        if (m_pSlot != 0) {
            delete[] m_pSlot;
        }
        m_pSlot = 0;
    }

    void CStgEffectSlot::Eff_Init()
    {
        m_nSlotNum = Eff_GetSlotNum();
        if (m_pSlot == 0 && m_nSlotNum > 0) {
            m_pSlot = new (1) CEffSlot[m_nSlotNum];
            for (int i = 0; i < m_nSlotNum; i++) {
                m_pSlot[i].m_hEffect = 0;
            }
        }
    }

    unsigned long CStgEffectSlot::Eff_CreateEffect(long slot, long eff, const bcs::FVEC3 *pPos, unsigned long flag)
    {
        if (eff < 0) {
            return 0;
        }
        CEffSlot &rSlot = m_pSlot[slot];
        rSlot.Stop();
        unsigned long hEffect = CEffectCtrl::CreateEffect(eff, flag, 0, 0, 0);
        rSlot.m_hEffect = hEffect;
        rSlot.m_fCreateDelay = -1.0f;
        CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
        if (pCluster != 0 && pPos != 0) {
            pCluster->SetTranslate(pPos);
        }
        return rSlot.m_hEffect;
    }

    unsigned long CStgEffectSlot::Eff_CreateAttach(long slot, long eff, long mdl, unsigned long node,
                                                   const bcs::FVEC3 *pOfs, int a, int b, int c, unsigned long flag)
    {
        if (eff < 0) {
            return 0;
        }
        CStgMdl *pMdl = GetMdl(mdl);
        bcs::CMdlObj *pMdlObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;
        CEffSlot &rSlot = m_pSlot[slot];
        rSlot.CreateAttach(eff, pMdlObj, node, pOfs, a, b, c, flag);
        return rSlot.m_hEffect;
    }

    unsigned long CStgEffectSlot::Eff_CreateAttach(long slot, long eff, long mdl, const char *pNode,
                                                   const bcs::FVEC3 *pOfs, int a, int b, int c, unsigned long flag)
    {
        if (eff < 0) {
            return 0;
        }
        CStgMdl *pMdl = GetMdl(mdl);
        bcs::CMdlObj *pMdlObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;
        long node = -1;
        if (pMdlObj != 0) {
            nw4r::g3d::ResMdl resMdl = pMdlObj->GetResMdl();
            if (resMdl.IsValid()) {
                node = resMdl.GetResNode(pNode).GetID();
            }
        }
        if (pMdlObj != 0 && node < 0) {
            return 0;
        }
        CEffSlot &rSlot = m_pSlot[slot];
        rSlot.CreateAttach(eff, pMdlObj, node, pOfs, a, b, c, flag);
        return rSlot.m_hEffect;
    }

    unsigned long CStgEffectSlot::Eff_CreateAttachFadeIn(long slot, float fade, long eff, long mdl,
                                                         unsigned long node, const bcs::FVEC3 *pOfs, int a, int b,
                                                         int c, unsigned long flag)
    {
        if (eff < 0) {
            return 0;
        }
        bcs::CMdlObj *pMdlObj = GetMdl(mdl)->GetMdlObj();
        CEffSlot &rSlot = m_pSlot[slot];
        rSlot.CreateAttach(eff, pMdlObj, node, pOfs, a, b, c, flag);
        rSlot.FadeIn(fade);
        return rSlot.m_hEffect;
    }

    unsigned long CStgEffectSlot::Eff_GetHandle(long slot) { return m_pSlot[slot].m_hEffect; }

    CEffectCluster *CStgEffectSlot::Eff_GetObject(long slot) { return m_pSlot[slot].GetCluster(); }

    void CStgEffectSlot::Eff_Stop(long slot) { m_pSlot[slot].Stop(); }

    void CStgEffectSlot::Eff_Erase(long slot) { m_pSlot[slot].Erase(); }

    void CStgEffectSlot::Eff_FadeIn(long slot, float fade) { m_pSlot[slot].FadeIn(fade); }

    void CStgEffectSlot::Eff_FadeOut(long slot, float fade) { m_pSlot[slot].FadeOut(fade); }

    void CStgEffectSlot::Eff_StopAll()
    {
        for (int i = 0; i < m_nSlotNum; i++) {
            Eff_Stop(i);
        }
    }

    void CStgEffectSlot::Eff_EraseAll()
    {
        for (int i = 0; i < m_nSlotNum; i++) {
            Eff_Erase(i);
        }
    }

    bool CStgEffectSlot::Eff_IsAlive(long slot) { return m_pSlot[slot].GetCluster() != 0; }

    void CStgEffectSlot::Eff_DelayCreateAttach(long slot, float delay, long eff, long mdl, unsigned long node,
                                               const bcs::FVEC3 *pOfs, int a, int b, int c, unsigned long flag)
    {
        CStgMdl *pMdl = GetMdl(mdl);
        m_pSlot[slot].DelayCreateAttach(delay, eff, pMdl != 0 ? pMdl->GetMdlObj() : 0, node, pOfs, a, b, c, flag);
    }

    void CStgEffectSlot::Eff_DelayCreateAttachFadeIn(long slot, float delay, float fade, long eff, long mdl,
                                                     unsigned long node, const bcs::FVEC3 *pOfs, int a, int b, int c,
                                                     unsigned long flag)
    {
        CStgMdl *pMdl = GetMdl(mdl);
        m_pSlot[slot].DelayCreateAttachFadeIn(delay, fade, eff, pMdl != 0 ? pMdl->GetMdlObj() : 0, node, pOfs, a, b,
                                              c, flag);
    }

    void CStgEffectSlot::Eff_DelayStop(long slot, float delay) { m_pSlot[slot].SetEnd(CEffSlot::END_STOP, delay); }

    void CStgEffectSlot::Eff_DelayErase(long slot, float delay) { m_pSlot[slot].SetEnd(CEffSlot::END_ERASE, delay); }

    void CStgEffectSlot::Eff_DelayFadeOut(long slot, float delay, float fade)
    {
        CEffSlot &rSlot = m_pSlot[slot];
        rSlot.SetEnd(CEffSlot::END_FADEOUT, delay);
        rSlot.m_fFadeOut = fade;
    }

    void CStgEffectSlot::Eff_Update(float rate)
    {
        if (m_pSlot != 0) {
            for (int i = 0; i < m_nSlotNum; i++) {
                m_pSlot[i].Update(rate);
            }
        }
    }

    CStgEffectSlot::CEffSlot::CEffSlot()
        : m_hEffect(0), m_fCreateDelay(-1.0f), m_fEndDelay(-1.0f), m_nCreateMode(CREATE_ATTACH), m_nEndMode(END_ERASE)
    {
    }

    CStgEffectSlot::CEffSlot::~CEffSlot() { Stop(); }

    void CStgEffectSlot::CEffSlot::DelayCreateAttach(float delay, long eff, const bcs::CScnObj *pObj,
                                                     unsigned long node, const bcs::FVEC3 *pOfs, int a, int b, int c,
                                                     unsigned long flag)
    {
        m_fCreateDelay = delay;
        m_nCreateMode = CREATE_ATTACH;
        m_nEff = eff;
        m_pObj = pObj;
        m_nNode = node;
        if (pOfs != 0) {
            m_Ofs = *pOfs;
            m_pOfs = &m_Ofs;
        } else {
            m_pOfs = 0;
        }
        m_nArg0 = a;
        m_nArg1 = b;
        m_nArg2 = c;
        m_nFlag = flag;
    }

    void CStgEffectSlot::CEffSlot::DelayCreateAttachFadeIn(float delay, float fade, long eff,
                                                           const bcs::CScnObj *pObj, unsigned long node,
                                                           const bcs::FVEC3 *pOfs, int a, int b, int c,
                                                           unsigned long flag)
    {
        m_fCreateDelay = delay;
        m_nCreateMode = CREATE_ATTACH_FADEIN;
        m_fFadeIn = fade;
        m_nEff = eff;
        m_pObj = pObj;
        m_nNode = node;
        if (pOfs != 0) {
            m_Ofs = *pOfs;
            m_pOfs = &m_Ofs;
        } else {
            m_pOfs = 0;
        }
        m_nArg0 = a;
        m_nArg1 = b;
        m_nArg2 = c;
        m_nFlag = flag;
    }

    void CStgEffectSlot::CEffSlot::Update(float rate)
    {
        if (GetCluster() == 0) {
            if (IsCreateWait()) {
                m_fCreateDelay -= rate;
                if (m_fCreateDelay <= 0.0f) {
                    m_fCreateDelay = -1.0f;
                    CreateAttach(m_nEff, m_pObj, m_nNode, m_pOfs, m_nArg0, m_nArg1, m_nArg2, m_nFlag);
                    if (m_nCreateMode == CREATE_ATTACH_FADEIN) {
                        CEffectCluster *pCluster = GetCluster();
                        if (pCluster != 0) {
                            pCluster->SetFadeinTime(m_fFadeIn);
                        }
                    }
                }
            }
        } else if (IsEndWait()) {
            m_fEndDelay -= rate;
            if (m_fEndDelay <= 0.0f) {
                m_fEndDelay = -1.0f;
                switch (m_nEndMode) {
                    case END_ERASE:
                        Erase();
                        break;
                    case END_FADEOUT:
                        FadeOut(m_fFadeOut);
                        break;
                    default:
                        Stop();
                        break;
                }
            }
        }
    }
}
