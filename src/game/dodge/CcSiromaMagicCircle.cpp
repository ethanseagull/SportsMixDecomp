// msw::CChrCoreDodgeSiroma (the White Mage) in the dodgeball game, and her
// special shot's magic circle (CC15_DODGE::CcSiromaMagicCircle).
//
// When the trick starts she draws the circle a little in front of her,
// towards the goal; it follows her target marker. When the shot leaves her
// hand the circle fires with an effect, fades out and vanishes once the
// effect is over. The same class as BasketF's msw::CC15_BASKET one
// (game/basket/CcSiromaMagicCircle.cpp), without its pillar of light.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace {

    // The two circles of the trick: their models, and the start and loop
    // motions of each.
    const char *s_apMdlName[2] = {"c15_sp_mg_square00", "c15_sp_mg_square01"};
    const char *s_apAnmName[2][2] = {
        {"c15_sp_mg_square00_start", "c15_sp_mg_square00_loop"},
        {"c15_sp_mg_square01_start", "c15_sp_mg_square01_loop"},
    };

    // (guessed) The effect the circle fires with, and the depth sort type
    // of its models (negative: left as it is).
    struct EFFECT_INFO {
        short nEffect;
        short nZSortType;
    };
    // One entry here (BasketF's circle has two); the code loops over it
    // as the basketball one does.
    const EFFECT_INFO s_FireEffect[1] = {{0x5CC, -1}};

    // (guessed helpers) The larger of two values (written two ways); fValue
    // kept within [fMin, fMax].
    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    inline float AtLeast(float fValue, float fMin)
    {
        return fValue < fMin ? fMin : fValue;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // Queues a scene object for deletion and forgets it.
    template <class T>
    inline void DeleteScnObj(T *&pObj)
    {
        if (pObj) {
            bcs::CScnObj::AppendDelayDeleteObject(pObj);
        }
        pObj = 0;
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // (name guessed) 60 degrees; nothing in the unit reads it.
    float s_fUnkRange = DegToRad(60.0f);
}

namespace CC15_DODGE {

    // One of the two circles: a model with its animations, playing the
    // start motion once and then the loop. 0x60 bytes. (member names
    // guessed)
    class CcSiromaMagicCircleObj : public msw::CGameTask {
        public:
            CcSiromaMagicCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                   nw4r::g3d::ResFile file);
            virtual ~CcSiromaMagicCircleObj();
            virtual void Main();

            bcs::CMdlObj *GetMdl() const { return m_pMdl; }

        private:
            void Create_(bcs::CScnGroup *pGroup, nw4r::g3d::ResFile file);

            nw4r::g3d::ResFile m_File;     // 0x34
            long m_nType;                  // 0x38, index in s_apAnmName
            bcs::CMdlObj *m_pMdl;          // 0x3C
            bcs::CAnmMotion *m_pAnmChr;    // 0x40
            bcs::CAnmMatClr *m_pAnmClr;    // 0x44
            bcs::CAnmTexSrt *m_pAnmTexSrt; // 0x48
            bcs::CAnmTexPat *m_pAnmTexPat; // 0x4C
            long m_nChrStep;               // 0x50: 0 start motion, 1 loop, 2 done
            long m_nClrStep;               // 0x54, the same for the colour animation
            long m_nTexSrtStep;            // 0x58 (not used)
            long m_nTexPatStep;            // 0x5C (not used)
    };

    // The trick's magic circle. 0xB4 bytes. (member names guessed)
    class CcSiromaMagicCircle : public msw::CGameTask, public bcs::CFsm<CcSiromaMagicCircle> {
        public:
            enum {
                STATE_SLEEP,
                STATE_APPEAR,
                STATE_LOOP, // follows the target marker
                STATE_FIRE,
                STATE_DISAPPEAR,
                STATE_NUM
            };

            CcSiromaMagicCircle(bcs::CTask *pParent, msw::CChrCore *pChr, const bcs::FVEC3 &rPos);
            virtual ~CcSiromaMagicCircle();
            virtual void Main();

            const bcs::FVEC3 &GetPos() const { return m_Pos; }

            // Fires where the circle is.
            void Fire()
            {
                m_TargetPos = m_Pos;
                ChangeState(STATE_FIRE);
            }

        private:
            void InitState();

            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateLoop();
            void LoopStateLoop();
            void DtorStateLoop();
            void CtorStateFire();
            void LoopStateFire();
            void DtorStateFire();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

            long m_nStep;                        // 0x7C
            bcs::FVEC3 m_Pos;                    // 0x80
            bcs::FVEC3 m_TargetPos;              // 0x8C
            long m_nPlayerNo;                    // 0x98, the White Mage's
            float m_fFrame;                      // 0x9C
            float m_fWait;                       // 0xA0
            bcs::CScnGroup *m_pGroup;            // 0xA4
            CcSiromaMagicCircleObj *m_apObj[2];  // 0xA8
            unsigned long m_ahEffect[1];         // 0xB0
    };
}

msw::CChrCoreDodgeSiroma::CChrCoreDodgeSiroma()
{
}

msw::CChrCoreDodgeSiroma::~CChrCoreDodgeSiroma()
{
}

inline void msw::CChrCoreDodgeSiroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

void msw::CChrCoreDodgeSiroma::Reset_()
{
    ReleaseMagic_();
    CChrCoreDodge::Reset_();
}

// The circle is drawn 3 short of her, towards the goal.
void msw::CChrCoreDodgeSiroma::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xA4, 3);
    Eff_CreateAttachAuto(0x18, 0xA5, 3);

    bcs::FVEC3 pos(GetTargetGoalPos());
    const bcs::FVEC3 &rChrPos = GetPos();
    bcs::FVEC3 dir;
    dir.x = pos.x - rChrPos.x;
    dir.z = pos.z - rChrPos.z;
    bcs::FVEC3 dirXZ(dir.x, 0.0f, dir.z);
    bcs::BCS_Vec_Normalize(dir, dirXZ);
    bcs::FVEC3Scale(&dir, &dir, -3.0f);
    pos.x += dir.x;
    pos.y = 0.0f;
    pos.z += dir.z;

    ReleaseMagic_();
    bcs::FVEC3 magicPos(pos.x, 0.0f, pos.z);
    CC15_DODGE::CcSiromaMagicCircle *pMagic =
        new (0x100) CC15_DODGE::CcSiromaMagicCircle(CGame::GetTempParent(), this, magicPos);
    m_hMagic = pMagic->GetId();
}

void msw::CChrCoreDodgeSiroma::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xA6, 3);
    Eff_CreateAttachAuto(0x18, 0xA7, 3);
    SE_PlayMove(8, 0x47D, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreDodgeSiroma::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xA8, 3);
    Eff_CreateAttachAuto(0x18, 0xA9, 3);

    CC15_DODGE::CcSiromaMagicCircle *pMagic =
        static_cast<CC15_DODGE::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
    if (pMagic && !pMagic->IsDelete()) {
        bcs::FVEC3 pos(pMagic->GetPos());
        PlaySePos(0x47E, pos, 0, -1, 0.0f, 1.0f, 0.0f);
    }
    pWork->m_nAfterStep = 0;
}

// After the shot: at frame 30 of the shot motion the circle fires.
int msw::CChrCoreDodgeSiroma::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    switch (pWork->m_nAfterStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 30.0f) {
                CC15_DODGE::CcSiromaMagicCircle *pMagic =
                    static_cast<CC15_DODGE::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
                if (pMagic && !pMagic->IsDelete()) {
                    pMagic->Fire();
                }
                pWork->m_nAfterStep++;
            }
            break;
        default:
            return CChrCoreDodge::SubStateArtsShotIsEndArtsShot_(pWork);
    }
    return 0;
}

void msw::CChrCoreDodgeSiroma::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
}

namespace CC15_DODGE {

    CcSiromaMagicCircleObj::CcSiromaMagicCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                                   nw4r::g3d::ResFile file)
        : msw::CGameTask(pParent, -1)
    {
        // (guessed) Written as assignments: with an initialiser list the
        // constructor is small enough to be inlined into its only caller,
        // which the original does not do.
        m_File = file;
        m_nType = nType;
        m_pMdl = 0;
        m_pAnmChr = 0;
        m_pAnmClr = 0;
        m_pAnmTexSrt = 0;
        m_pAnmTexPat = 0;
        m_nChrStep = 0;
        m_nClrStep = 0;
        m_nTexSrtStep = 0;
        m_nTexPatStep = 0;
        Create_(pGroup, file);
    }

    CcSiromaMagicCircleObj::~CcSiromaMagicCircleObj()
    {
        DeleteScnObj(m_pMdl);
    }

    // The model, drawn on top without depth test, with every animation
    // the file has, put in pGroup.
    void CcSiromaMagicCircleObj::Create_(bcs::CScnGroup *pGroup, nw4r::g3d::ResFile file)
    {
        if (!file.IsValid()) {
            return;
        }
        nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
        if (!mdl.IsValid()) {
            return;
        }

        m_pMdl = new (0x100) bcs::CMdlObj;
        unsigned long anmFlag = 0;
        file.HasResAnmChr(); // (guessed) tested, but adds no flag
        if (file.HasResAnmClr()) {
            anmFlag |= 0x108;
        }
        if (file.HasResAnmTexSrt()) {
            anmFlag |= 0x204;
        }
        if (file.HasResAnmTexPat()) {
            anmFlag |= 0x3;
        }
        if (file.HasResAnmShp()) {
            anmFlag |= 0x7000;
        }
        m_pMdl->Create(mdl, anmFlag, 0);
        m_pMdl->SetExBlendMode(0);
        m_pMdl->OnFlag(0x100);
        m_pMdl->OnFlag(0x4000);
        m_pMdl->MatSetZModeAll(0, GX_ALWAYS, 0);
        m_pMdl->SetDrawPriority(0xC9, 0xC9);
        m_pMdl->OnFlag(0x2000);

        if (file.HasResAnmChr()) {
            m_pAnmChr = new (0x100) bcs::CAnmMotion;
            if (m_pAnmChr) {
                m_pAnmChr->Create(m_pMdl, file, 0, 3, 0);
                m_pMdl->AttachAnmObj(m_pAnmChr, bcs::CMdlObj::ANMOBJTYPE_CHR);
            }
        }
        if (file.HasResAnmClr()) {
            nw4r::g3d::ResAnmClr res = file.GetResAnmClr(0);
            m_pAnmClr = new (0x100) bcs::CAnmMatClr;
            if (m_pAnmClr) {
                m_pAnmClr->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(m_pAnmClr, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
            }
        }
        if (file.HasResAnmTexSrt()) {
            nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(0);
            m_pAnmTexSrt = new (0x100) bcs::CAnmTexSrt;
            if (m_pAnmTexSrt) {
                m_pAnmTexSrt->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(m_pAnmTexSrt, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
            }
        }
        if (file.HasResAnmTexPat()) {
            nw4r::g3d::ResAnmTexPat res = file.GetResAnmTexPat(0);
            m_pAnmTexPat = new (0x100) bcs::CAnmTexPat;
            if (m_pAnmTexPat) {
                m_pAnmTexPat->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(m_pAnmTexPat, bcs::CMdlObj::ANMOBJTYPE_3);
            }
        }

        m_pMdl->CalcRootMtx();
        m_pMdl->CalcWorldMatrixAll(0);
        pGroup->AppendChild(m_pMdl);
    }

    // The start motion, then the loop motion once it is over (for the
    // skeletal and the colour animation alike).
    void CcSiromaMagicCircleObj::Main()
    {
        if (m_pMdl == 0) {
            return;
        }

        if (m_pAnmChr) {
            switch (m_nChrStep) {
                case 0: {
                    nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apAnmName[m_nType][0]);
                    if (res.IsValid()) {
                        m_pAnmChr->Play(res, 0, 1.0f, 0.0f, 0.0f);
                    }
                    m_nChrStep++;
                    break;
                }
                case 1:
                    if (m_pAnmChr->IsEnd()) {
                        nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apAnmName[m_nType][1]);
                        if (res.IsValid()) {
                            m_pAnmChr->Play(res, 2, 1.0f, 0.0f, 0.0f);
                        }
                        m_nChrStep++;
                    }
                    break;
            }
        }

        if (m_pAnmClr) {
            switch (m_nClrStep) {
                case 0: {
                    nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apAnmName[m_nType][0]);
                    if (res.IsValid()) {
                        m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                    }
                    m_nClrStep++;
                    break;
                }
                case 1:
                    if (m_pAnmClr->IsEnd()) {
                        nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apAnmName[m_nType][1]);
                        if (res.IsValid()) {
                            m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                        }
                        m_nClrStep++;
                    }
                    break;
            }
        }

        m_pMdl->UpdateFrame(GetFrameRateF());
    }

    // The circle at rPos, with a scene group of its own.
    CcSiromaMagicCircle::CcSiromaMagicCircle(bcs::CTask *pParent, msw::CChrCore *pChr, const bcs::FVEC3 &rPos)
        : msw::CGameTask(pParent, -1), m_nStep(0), m_nPlayerNo(-1), m_fWait(0.0f), m_pGroup(0)
    {
        m_nPlayerNo = pChr->GetPlayerNo();
        m_Pos = rPos;
        m_apObj[0] = 0;
        m_apObj[1] = 0;
        m_ahEffect[0] = 0;

        InitState();

        m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
        m_pGroup->m_vTrans = m_Pos;
        m_pGroup->CalcRootMtx();
        bcs::CRenderMan::PushBack(m_pGroup, 5);
    }

    CcSiromaMagicCircle::~CcSiromaMagicCircle()
    {
        for (long i = 0; i < 1; i++) {
            if (m_ahEffect[i]) {
                msw::CEffectCtrl::EraseAttachEffect(m_ahEffect[i]);
                m_ahEffect[i] = 0;
            }
        }
        DeleteScnObj(m_pGroup);
    }

    // An ordinary function that the constructor inlines; the original
    // module has no copy of it (its link dropped it as unreferenced), only
    // its second copy of the state table in .data.
    void CcSiromaMagicCircle::InitState()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_SLEEP, &CcSiromaMagicCircle::CtorStateSleep, &CcSiromaMagicCircle::LoopStateSleep,
                 &CcSiromaMagicCircle::DtorStateSleep);
        SetState(STATE_APPEAR, &CcSiromaMagicCircle::CtorStateAppear, &CcSiromaMagicCircle::LoopStateAppear,
                 &CcSiromaMagicCircle::DtorStateAppear);
        SetState(STATE_LOOP, &CcSiromaMagicCircle::CtorStateLoop, &CcSiromaMagicCircle::LoopStateLoop,
                 &CcSiromaMagicCircle::DtorStateLoop);
        SetState(STATE_FIRE, &CcSiromaMagicCircle::CtorStateFire, &CcSiromaMagicCircle::LoopStateFire,
                 &CcSiromaMagicCircle::DtorStateFire);
        SetState(STATE_DISAPPEAR, &CcSiromaMagicCircle::CtorStateDisappear,
                 &CcSiromaMagicCircle::LoopStateDisappear, &CcSiromaMagicCircle::DtorStateDisappear);
        ChangeState(STATE_SLEEP);
    }

    void CcSiromaMagicCircle::Main()
    {
        ExecState();
    }

    void CcSiromaMagicCircle::CtorStateSleep()
    {
    }

    void CcSiromaMagicCircle::LoopStateSleep()
    {
        ChangeState(STATE_APPEAR);
    }

    void CcSiromaMagicCircle::DtorStateSleep()
    {
    }

    // Both circles are made, 2.5 times their size.
    void CcSiromaMagicCircle::CtorStateAppear()
    {
        msw::CGameSnd::Play(0x4E5, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

        bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
        if (pRes) {
            for (long i = 0; i < 2; i++) {
                if (s_apMdlName[i]) {
                    nw4r::g3d::ResFile file = pRes->GetG3DResFileByMdlName(s_apMdlName[i]);
                    if (file.IsValid()) {
                        m_apObj[i] = new CcSiromaMagicCircleObj(this, m_pGroup, i, file);
                        bcs::CMdlObj *pMdl = m_apObj[i]->GetMdl();
                        if (pMdl) {
                            pMdl->m_vScale.x = 2.5f;
                            pMdl->m_vScale.y = 2.5f;
                            pMdl->m_vScale.z = 2.5f;
                        }
                    }
                }
            }
        }
    }

    void CcSiromaMagicCircle::LoopStateAppear()
    {
        ChangeState(STATE_LOOP);
    }

    void CcSiromaMagicCircle::DtorStateAppear()
    {
    }

    void CcSiromaMagicCircle::CtorStateLoop()
    {
        m_nStep = 0;
    }

    // On the target marker (on the ground); gone with the character.
    void CcSiromaMagicCircle::LoopStateLoop()
    {
        msw::CChrCore *pChr = msw::CChrMan::GetChrCore(m_nPlayerNo);
        if (!pChr || pChr->IsDisable()) {
            ChangeState(STATE_DISAPPEAR);
            return;
        }

        pChr->GetDodgeArtsTargetMarkerPos(m_Pos, 0);
        m_Pos.y = 0.0f;
        m_TargetPos = m_Pos;
        m_pGroup->m_vTrans = m_Pos;
        m_pGroup->CalcRootMtx();
    }

    void CcSiromaMagicCircle::DtorStateLoop()
    {
    }

    // The circle settles on the target with the effect.
    void CcSiromaMagicCircle::CtorStateFire()
    {
        m_Pos = m_TargetPos;
        msw::CGameSnd::Play(0x47F, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

        for (long i = 0; i < 1; i++) {
            if (m_ahEffect[i]) {
                msw::CEffectCtrl::StopAttachEffect(m_ahEffect[i]);
                m_ahEffect[i] = 0;
            }
            const EFFECT_INFO *pInfo = &s_FireEffect[i];
            long nZSortType = pInfo->nZSortType;
            if (pInfo->nEffect >= 0) {
                m_ahEffect[i] = msw::CEffectCtrl::CreateAttachEffect(pInfo->nEffect, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                if (m_ahEffect[i]) {
                    msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetObject(m_ahEffect[i]);
                    if (pCluster) {
                        pCluster->SetTranslate(&m_Pos);
                        if (nZSortType >= 0) {
                            long nNum = pCluster->GetMdlObjNum();
                            for (long j = 0; j < nNum; j++) {
                                bcs::CMdlObj *pMdl = pCluster->GetMdlObj(j);
                                if (pMdl) {
                                    pMdl->OnFlag(0x50000);
                                    pMdl->SetBBoxZSortCalcType(nZSortType);
                                }
                            }
                        }
                    }
                }
            }
        }

        m_nStep = 0;
    }

    // Step 0 sets 10 frames of fading after a wait of 5; step 1 fades both
    // circles out; then it ends once the effect is over.
    void CcSiromaMagicCircle::LoopStateFire()
    {
        switch (m_nStep) {
            case 0:
                m_fFrame = 10.0f;
                m_fWait = 5.0f;
                m_nStep++;
                break;
            case 1: {
                if (m_fWait > 0.0f) {
                    m_fWait -= GetFrameRateF();
                    if (m_fWait <= 0.0f) {
                        // (unused) The target on the ground.
                        bcs::FVEC3 pos(m_TargetPos);
                        pos.y = 0.0f;
                    }
                }
                m_fFrame -= GetFrameRateF();
                m_fFrame = AtLeast(m_fFrame, 0.0f);
                float fFrameMax = 10.0f;
                for (long i = 0; i < 2; i++) {
                    if (m_apObj[i] && fFrameMax > 0.0f) {
                        float fAlpha = Clamp(m_fFrame / fFrameMax, 0.0f, 1.0f);
                        bcs::CMdlObj *pMdl = m_apObj[i]->GetMdl();
                        if (pMdl) {
                            pMdl->SetAlphaRateF(fAlpha);
                        }
                    }
                }
                if (m_fFrame <= 0.0f) {
                    m_nStep++;
                }
                break;
            }
            default: {
                int bEnd = 1;
                for (long i = 0; i < 1; i++) {
                    if (m_ahEffect[i] && msw::CEffectCtrl::GetObject(m_ahEffect[i])) {
                        bEnd = 0;
                    }
                }
                if (bEnd) {
                    Delete();
                }
                break;
            }
        }
    }

    void CcSiromaMagicCircle::DtorStateFire()
    {
    }

    void CcSiromaMagicCircle::CtorStateDisappear()
    {
        for (long i = 0; i < 1; i++) {
            if (m_ahEffect[i]) {
                msw::CEffectCtrl::StopAttachEffect(m_ahEffect[i]);
                m_ahEffect[i] = 0;
            }
        }
        m_nStep = 0;
    }

    // Both circles fade out over 10 frames, then it ends once the effect is
    // over.
    void CcSiromaMagicCircle::LoopStateDisappear()
    {
        switch (m_nStep) {
            case 0:
                m_fFrame = 10.0f;
                m_nStep++;
                break;
            case 1: {
                m_fFrame -= GetFrameRateF();
                m_fFrame = AtLeast(m_fFrame, 0.0f);
                float fFrameMax = 10.0f;
                for (long i = 0; i < 2; i++) {
                    if (m_apObj[i] && fFrameMax > 0.0f) {
                        float fAlpha = Clamp(m_fFrame / fFrameMax, 0.0f, 1.0f);
                        bcs::CMdlObj *pMdl = m_apObj[i]->GetMdl();
                        if (pMdl) {
                            pMdl->SetAlphaRateF(fAlpha);
                        }
                    }
                }
                if (m_fFrame <= 0.0f) {
                    m_nStep++;
                }
                break;
            }
            default: {
                int bEnd = 1;
                for (long i = 0; i < 1; i++) {
                    if (m_ahEffect[i] && msw::CEffectCtrl::GetObject(m_ahEffect[i])) {
                        bEnd = 0;
                    }
                }
                if (bEnd) {
                    Delete();
                }
                break;
            }
        }
    }

    void CcSiromaMagicCircle::DtorStateDisappear()
    {
        // (guessed) Dead comparison: the original .rodata has -1.0f after
        // the last state's constants and nothing reads it (likely stripped
        // code); this keeps it in place. See notes/decisions.md.
        if (m_fFrame == -1.0f) {
        }
    }
}
