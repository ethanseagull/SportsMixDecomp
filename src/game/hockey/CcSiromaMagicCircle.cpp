// msw::CChrCoreHockeySiroma (the White Mage) in the hockey game, and her
// special shot's magic circle (CC15_HOCKEY::CcSiromaMagicCircle). The circle
// is the BasketF one (src/game/basket/CcSiromaMagicCircle.cpp) with other
// effects; the computer's aim follows Luigi's (CChrCoreHockeyLuigi.cpp).
//
// When the trick starts she draws the circle a little in front of her,
// towards the goal. The target marker (or the computer) moves it while it
// waits; when the
// shot leaves her hand the circle fires: a pillar of light stands on it for
// a moment as an attack collision that sinks into the ground, and the
// circle fades out and vanishes once its effects are over.
#include "game/msw/CChrCoreHockey.hpp"

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
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "nw4r/g3d/ResFile.hpp"

#include <math.h>

namespace {

    // The two circles of the trick: their models, and the start and loop
    // motions of each.
    const char *s_apMdlName[2] = {"c15_sp_mg_square00", "c15_sp_mg_square01"};
    const char *s_apAnmName[2][2] = {
        {"c15_sp_mg_square00_start", "c15_sp_mg_square00_loop"},
        {"c15_sp_mg_square01_start", "c15_sp_mg_square01_loop"},
    };

    // (guessed) The effects the circle fires with, and the depth sort type
    // of their models (negative: left as it is).
    struct EFFECT_INFO {
        short nEffect;
        short nZSortType;
    };
    const EFFECT_INFO s_FireEffect[2] = {{0x626, -1}, {0x627, -1}};

    // (guessed helpers) An angle brought into [0, 2 pi); the larger of two
    // values; fValue kept within [fMin, fMax].
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    // (guessed) The same, written the other way round.
    inline float AtLeast(float fValue, float fMin)
    {
        return fValue < fMin ? fMin : fValue;
    }

    // (guessed) From fFrom to fTo by fRate.
    inline float Lerp(float fFrom, float fTo, float fRate)
    {
        return fFrom + (fTo - fFrom) * fRate;
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
}

namespace msw {
    namespace CC15_HOCKEY {

        // One of the two circles: a model with its animations, playing the
        // start motion once and then the loop. 0x60 bytes. (member names
        // guessed)
        class CcSiromaMagicCircleObj : public CGameTask {
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

        // The trick's magic circle: an obstacle of type 0xC (the White
        // Mage's GetTargetMarkerPos finds it by that). 0x10C bytes. (member
        // names guessed)
        class CcSiromaMagicCircle : public CChrObstacle, public bcs::CFsm<CcSiromaMagicCircle> {
            public:
                enum {
                    STATE_SLEEP,
                    STATE_APPEAR,
                    STATE_LOOP, // waits; the target marker moves it
                    STATE_FIRE, // the pillar of light
                    STATE_DISAPPEAR,
                    STATE_NUM
                };

                CcSiromaMagicCircle(bcs::CTask *pParent, CChrCore *pChr, const bcs::FVEC3 &rPos);
                virtual ~CcSiromaMagicCircle();
                virtual void Main();

                long GetState() const { return m_nState; }
                const bcs::FVEC3 &GetPos() const { return m_Pos; }

                // Moves the circle to rPos (on the ground) while it waits.
                void MovePos(const bcs::FVEC3 &rPos)
                {
                    if (GetState() == STATE_LOOP) {
                        m_Pos.Set(rPos.x, 0.0f, rPos.z);
                    }
                }

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

                bcs::FVEC3 m_TargetPos;              // 0xDC
                long m_nStep;                        // 0xE8
                long m_nPlayerNo;                    // 0xEC, the White Mage's
                float m_fFrame;                      // 0xF0
                float m_fWait;                       // 0xF4
                bcs::CScnGroup *m_pGroup;            // 0xF8
                CcSiromaMagicCircleObj *m_apObj[2];  // 0xFC
                unsigned long m_ahEffect[2];         // 0x104
        };
    }
}

msw::CChrCoreHockeySiroma::CChrCoreHockeySiroma()
{
    m_hMagic = 0;
    m_fComAimWait = 0.0f;
    m_fComAimKeep = 0.0f;
    m_bComAimTimeUp = 0;
    m_fComAimDelay = 0.0f;
}

msw::CChrCoreHockeySiroma::~CChrCoreHockeySiroma()
{
}

inline void msw::CChrCoreHockeySiroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

// The computer's level: its own, or with pad players in the game the level
// set for the team they play against (or a fixed one, see GameWork 0x151).
inline float msw::CChrCoreHockeySiroma::GetComDatLevel_()
{
    float fLevel = GetComLevel();
    if (GetExistPTPadDrvNum_() > 0) {
        if (CGame::g_GameWork.m_Unk151) {
            fLevel = CComMan::GetDatF(0xF53, 0L);
        } else {
            fLevel = CComMan::GetDatF(0xF4B, CGame::GetTeamComLevel(1 - GetTeamNo()));
        }
    }
    return fLevel;
}

void msw::CChrCoreHockeySiroma::Reset_()
{
    ReleaseMagic_();
    CChrCoreHockey::Reset_();
}

// While the circle is out, the target marker sets the yaw it moves to.
int msw::CChrCoreHockeySiroma::MoveTargetMarkerPos(float fRotY)
{
    if (GetState() != 0x27) {
        return 0;
    }
    if (m_hMagic) {
        STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
        pWork->m_fTargetRotY = WrapRad(fRotY);
        return 1;
    }
    return 0;
}

// The computer's aim: after a delay it moves the circle towards the nearest
// opponent within its range or, among those in the oval in front of it, the
// one closest in direction to the goal. An opponent already on the circle
// stops the search; when nobody is in reach for long enough, it gives up.
int msw::CChrCoreHockeySiroma::AutoMoveTargetMarkerPos()
{
    if (GetState() != 0x27) {
        return 0;
    }
    if (m_hMagic) {
        m_fComAimWait -= m_fFrameStep * m_fTimeScale;
        if (m_fComAimDelay > 0.0f) {
            m_fComAimDelay -= m_fFrameStep * m_fTimeScale;
            if (m_fComAimDelay <= 0.0f) {
                m_fComAimDelay = 0.0f;
            } else {
                return 0;
            }
        }

        CC15_HOCKEY::CcSiromaMagicCircle *pMagic =
            static_cast<CC15_HOCKEY::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
        if (pMagic->IsDelete()) {
            return 0;
        }

        STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
        float fDist = GetDist_ToPosXZ(GetTargetGoalPos());
        float fMinDist = 999.0f;
        float fRange = 0.0f;
        float fMinDiff = fMinDist;
        int bGiveUp = 0;
        float fWidth = fRange;
        CChrCore *pTarget = 0;
        if (!IsPadCtrl()) {
            float fLevel = GetComDatLevel_();
            fWidth = CComMan::GetDatF(0x960, fLevel);
            fRange = CComMan::GetDatF(0x970, fLevel);
        }

        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable() || pChr->IsHockeyDamage() || pChr->IsKeeper() ||
                pChr->GetTeamNo() == GetTeamNo()) {
                continue;
            }
            bcs::FVEC3 pos;
            pChr->GetCompPos(pos);
            if (HitCheckOval_(GetPos(), pos, GetRotY(), fDist, fWidth, 0.0f)) {
                float fChrDist = pChr->GetDist_ToPosXZ(pMagic->GetPos());
                if (fChrDist < 2.5f) {
                    // Already on the circle.
                    if (fChrDist <= 0.5f) {
                        pWork->m_fTargetRotY = -1.0f;
                    }
                    if (m_fComAimWait <= 0.0f) {
                        m_bComAimTimeUp = 1;
                        break;
                    }
                } else {
                    float fGoalRot = GetRotY_FromPosXZ(GetTargetGoalPos());
                    float fDiff = fabsf(CalcRotDiff_(GetRotY_FromPosXZ(pChr->GetPos()), fGoalRot));
                    if (fMinDiff > fDiff) {
                        fMinDiff = fDiff;
                        pTarget = pChr;
                    }
                }
            } else {
                float fChrDist = pChr->GetDist_ToPosXZ(pMagic->GetPos());
                if (pChr->GetDist_ToPosXZ(pMagic->GetPos()) < 2.5f) {
                    if (m_fComAimWait <= 0.0f) {
                        m_bComAimTimeUp = 1;
                    }
                    break;
                }
                if (fChrDist < fRange) {
                    if (fMinDist > fChrDist) {
                        fMinDist = fChrDist;
                        pTarget = pChr;
                    }
                } else if (m_fComAimWait <= 0.0f) {
                    bGiveUp = 1;
                }
            }
        }

        if (m_bComAimTimeUp) {
            m_fComAimKeep -= m_fFrameStep * m_fTimeScale;
            if (m_fComAimKeep <= 0.0f) {
                bGiveUp = 1;
            }
        }

        if (bGiveUp) {
            pWork->m_fUnk0C = 0.0f;
            return 1;
        }
        if (pTarget) {
            pWork->m_fTargetRotY = WrapRad(pTarget->GetRotY_FromPosXZ(pMagic->GetPos()));
            return 1;
        }
    }
    return 0;
}

void msw::CChrCoreHockeySiroma::CtorStateAirTrick()
{
    CChrCoreHockey::CtorStateAirTrick();

    m_fComAimWait = 20.0f;
    m_fComAimDelay = 0.0f;
    m_fComAimKeep = 0.0f;
    m_bComAimTimeUp = 0;
    if (!IsPadCtrl()) {
        float fLevel = GetComDatLevel_();
        m_fComAimWait = CComMan::GetDatF(0x958, fLevel);
        m_fComAimKeep = CComMan::GetDatF(0x968, fLevel);
        m_fComAimDelay = 40.0f;
    }
}

void msw::CChrCoreHockeySiroma::LoopStateAirTrick()
{
    SubStateAirTrickMoveTargetMarker_(FsmqGetWork<STATE_WORK_AIR_TRICK>());
    CChrCoreHockey::LoopStateAirTrick();
}

// A yaw set by the target marker moves the circle a step that way.
void msw::CChrCoreHockeySiroma::SubStateAirTrickMoveTargetMarker_(STATE_WORK_AIR_TRICK *pWork)
{
    if (m_hMagic) {
        CC15_HOCKEY::CcSiromaMagicCircle *pMagic =
            static_cast<CC15_HOCKEY::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
        if (pMagic && !pMagic->IsDelete()) {
            if (pWork->m_fTargetRotY >= 0.0f) {
                bcs::FVEC3 pos;
                bcs::BCS_VecZ_RotY(pos, 0.16666667f, pWork->m_fTargetRotY);
                pos.x += pMagic->GetPos().x;
                pos.z += pMagic->GetPos().z;
                pMagic->MovePos(pos);
                pWork->m_fTargetRotY = -1.0f;
            }
        } else {
            pWork->m_nFlag &= ~1;
        }
    }
}

// The circle is drawn 3 short of her, towards the goal.
void msw::CChrCoreHockeySiroma::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xC3, 3);
    Eff_CreateAttachAuto(0x14, 0xC4, 3);
    SE_PlayArtsJumpSE(0);

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
    CC15_HOCKEY::CcSiromaMagicCircle *pMagic =
        new (0x100) CC15_HOCKEY::CcSiromaMagicCircle(CGame::GetTempParent(), this, magicPos);
    m_hMagic = pMagic->GetId();
    pWork->m_nFlag |= 1;
}

void msw::CChrCoreHockeySiroma::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xC5, 3);
    Eff_CreateAttachAuto(0x15, 0xC6, 3);
    Eff_CreateAttach(0x16, 0x621, m_nEffNodeDunk, 0, 0, 3, 0);
    SE_PlayMove(8, 0x47D, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreHockeySiroma::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    SE_Stop(8, 0.0f);
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x17, 0xC7, 3);
    Eff_CreateAttachAuto(0x18, 0xC8, 3);

    CC15_HOCKEY::CcSiromaMagicCircle *pMagic =
        static_cast<CC15_HOCKEY::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
    if (pMagic && !pMagic->IsDelete()) {
        bcs::FVEC3 pos(pMagic->GetPos());
        PlaySePos(0x47E, pos, 0, -1, 0.0f, 1.0f, 0.0f);
    }
    pWork->m_nShotStep = 0;
}

// After the shot: at the shot frame the circle fires, then two more motions
// follow, the second with the ball shot trigger.
int msw::CChrCoreHockeySiroma::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    switch (pWork->m_nShotStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Eff_Stop(0x14);
                Eff_Stop(0x15);
                CC15_HOCKEY::CcSiromaMagicCircle *pMagic =
                    static_cast<CC15_HOCKEY::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
                if (pMagic && !pMagic->IsDelete()) {
                    pMagic->Fire();
                }
                pWork->m_nShotStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0xA2, 0, 0.0f, 0.0f);
                pWork->m_nShotStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                Eff_Stop(0x17);
                Eff_Stop(0x18);
                AnimPlay(3, 0xA3, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x17, 0xC9, 3);
                Eff_Stop(0x16);
                SubStateArtsShotDoGetBallShotTrg_(pWork, 16.0f);
                pWork->m_nShotStep++;
            }
            break;
        default:
            return CChrCoreHockey::SubStateArtsShotIsEndArtsShot_(pWork);
    }
    return 0;
}

namespace msw {
    namespace CC15_HOCKEY {

        CcSiromaMagicCircleObj::CcSiromaMagicCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                                       nw4r::g3d::ResFile file)
            : CGameTask(pParent, -1)
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

        // The circle at rPos, with a scene group of its own and an attack
        // collision (off until it fires) that hits the other team only.
        CcSiromaMagicCircle::CcSiromaMagicCircle(bcs::CTask *pParent, CChrCore *pChr, const bcs::FVEC3 &rPos)
            : CChrObstacle(pParent, 0xC, 0x17), m_nStep(0), m_nPlayerNo(-1), m_fWait(0.0f), m_pGroup(0)
        {
            m_nPlayerNo = pChr->GetPlayerNo();
            m_Pos = rPos;
            m_apObj[0] = 0;
            m_apObj[1] = 0;
            m_ahEffect[0] = 0;
            m_ahEffect[1] = 0;

            InitState();

            m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
            m_pGroup->m_vTrans = m_Pos;
            m_pGroup->CalcRootMtx();
            bcs::CRenderMan::PushBack(m_pGroup, 5);

            unsigned long nNoHit = 0x55;
            if (m_nPlayerNo & 1) {
                nNoHit = 0xAA;
            }
            m_AtkColl.Create(1, 4, 3, 0);
            m_AtkColl.SetAtkTypeAll(0xA01);
            m_AtkColl.SetTargetFlagAll(2);
            m_AtkColl.SetNoHitFlagAll(nNoHit);
            m_AtkColl.SetRadiusAll(7.25f);
            m_AtkColl.SetHitEffectIDAll(-1);
            m_AtkColl.SetHitEffectFrameAll(40.0f);
            m_AtkColl.SetHitCamVibIDAll(0x27);
            m_AtkColl.SetHitSeIDAll(0x480);
            m_AtkColl.SetDisableAll();
            m_AtkColl.SetPosAll(m_Pos);
        }

        CcSiromaMagicCircle::~CcSiromaMagicCircle()
        {
            for (long i = 0; i < 2; i++) {
                if (m_ahEffect[i]) {
                    CEffectCtrl::EraseAttachEffect(m_ahEffect[i]);
                    m_ahEffect[i] = 0;
                }
            }
            DeleteScnObj(m_pGroup);
        }

        // An ordinary function that the constructor inlines; the original
        // module has no copy of it (its link dropped it as unreferenced),
        // only its second copy of the state table in .data.
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

        // The circle stays on the floor of the map.
        void CcSiromaMagicCircle::Main()
        {
            bcs::FVEC3 pos(m_Pos);
            bcs::FVEC3 hitPos;
            bcs::FVEC3 normal;
            float fTime;
            if (CCollMan::DetectMapHit(hitPos, normal, fTime, pos, pos, 2.0f, 5, 0x1000000)) {
                m_Pos.Set(hitPos.x, 0.0f, hitPos.z);
            }
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

        // Both circles are made, 2.9 times their size.
        void CcSiromaMagicCircle::CtorStateAppear()
        {
            CGameSnd::Play(0x4E5, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

            bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
            if (pRes) {
                for (long i = 0; i < 2; i++) {
                    if (s_apMdlName[i]) {
                        nw4r::g3d::ResFile file = pRes->GetG3DResFileByMdlName(s_apMdlName[i]);
                        if (file.IsValid()) {
                            m_apObj[i] = new CcSiromaMagicCircleObj(this, m_pGroup, i, file);
                            bcs::CMdlObj *pMdl = m_apObj[i]->GetMdl();
                            if (pMdl) {
                                pMdl->m_vScale.x = 2.9f;
                                pMdl->m_vScale.y = 2.9f;
                                pMdl->m_vScale.z = 2.9f;
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

        // On the ground; gone with the character.
        void CcSiromaMagicCircle::LoopStateLoop()
        {
            CChrCore *pChr = CChrMan::GetChrCore(m_nPlayerNo);
            if (!pChr || pChr->IsDisable()) {
                ChangeState(STATE_DISAPPEAR);
                return;
            }

            m_Pos.y = 0.0f;
            m_pGroup->m_vTrans = m_Pos;
            m_pGroup->CalcRootMtx();
        }

        void CcSiromaMagicCircle::DtorStateLoop()
        {
        }

        void CcSiromaMagicCircle::CtorStateFire()
        {
            float fFrame = 13.0f;
            m_fFrame = Max(fFrame, 1.0f);
            m_nStep = 0;
        }

        // Step 0: over 13 frames the first circle slides onto the target and
        // shrinks from 2.9 to 2.7 times its size, the second grows to 7 and
        // fades; then the effects start and the pillar (radius 3.375, from 19
        // to 21 high) is switched on for 40 frames. Step 1: after 5 frames the pillar
        // sinks 4 a frame into the ground. Step 2: the first circle fades out
        // over 10 frames. Then it ends once the effects are over.
        void CcSiromaMagicCircle::LoopStateFire()
        {
            switch (m_nStep) {
                case 0: {
                    m_fFrame -= GetFrameRateF();
                    m_fFrame = AtLeast(m_fFrame, 0.0f);
                    float fFrameMax = 13.0f;
                    fFrameMax = Max(fFrameMax, 1.0f);
                    float fRate = Clamp(1.0f - m_fFrame / fFrameMax, 0.0f, 1.0f);

                    if (m_pGroup) {
                        m_pGroup->CalcRootMtx();
                        if (m_apObj[0]) {
                            float fPow = pow(1.0f - fRate, 3.3333333f);
                            bcs::FVEC3 ofs;
                            bcs::FVEC3Sub(&ofs, &m_TargetPos, &m_Pos);
                            float fMove = 1.0f - fPow;
                            ofs.x *= fMove;
                            ofs.z *= fMove;
                            bcs::CMdlObj *pMdl = m_apObj[0]->GetMdl();
                            if (pMdl) {
                                pMdl->m_vTrans = ofs;
                            }
                            float fScale = Lerp(2.9f, 2.7f, fMove);
                            pMdl = m_apObj[0]->GetMdl();
                            if (pMdl) {
                                pMdl->m_vScale.x = fScale;
                                pMdl->m_vScale.y = fScale;
                                pMdl->m_vScale.z = fScale;
                            }
                        }
                        if (m_apObj[1]) {
                            float fAlpha = (float)pow(1.0f - fRate, 2.0);
                            float fScale = Lerp(2.9f, 7.0f, 1.0f - fAlpha);
                            bcs::CMdlObj *pMdl = m_apObj[1]->GetMdl();
                            if (pMdl) {
                                pMdl->m_vScale.x = fScale;
                                pMdl->m_vScale.y = fScale;
                                pMdl->m_vScale.z = fScale;
                            }
                            pMdl = m_apObj[1]->GetMdl();
                            if (pMdl) {
                                pMdl->SetAlphaRateF(fAlpha);
                            }
                        }
                    }

                    if (m_fFrame <= 0.0f) {
                        CGameSnd::Play(0x47F, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

                        for (long i = 0; i < 2; i++) {
                            if (m_ahEffect[i]) {
                                CEffectCtrl::StopAttachEffect(m_ahEffect[i]);
                                m_ahEffect[i] = 0;
                            }
                            const EFFECT_INFO *pInfo = &s_FireEffect[i];
                            long nZSortType = pInfo->nZSortType;
                            if (pInfo->nEffect >= 0) {
                                m_ahEffect[i] =
                                    CEffectCtrl::CreateAttachEffect(pInfo->nEffect, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                                if (m_ahEffect[i]) {
                                    CEffectCluster *pCluster = CEffectCtrl::GetObject(m_ahEffect[i]);
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

                        m_fFrame = 40.0f;
                        m_AtkColl.SetRadiusAll(3.375f);
                        m_AtkColl.SetEnableAll();
                        bcs::FVEC3 top(m_TargetPos);
                        bcs::FVEC3 bottom(top);
                        top.y = 21.0f;
                        bottom.y = 19.0f;
                        m_AtkColl.SetPosAll(top, bottom);
                        m_fWait = 5.0f;
                        m_nStep++;
                    }
                    break;
                }
                case 1: {
                    if (m_fWait > 0.0f) {
                        m_fWait -= GetFrameRateF();
                        if (m_fWait <= 0.0f) {
                            // (unused) The pillar's top on the ground.
                            bcs::FVEC3 pos(m_AtkColl.GetColl(0)->GetPos2());
                            pos.y = 0.0f;
                        }
                    }
                    bcs::FVEC3 pos(m_AtkColl.GetColl(0)->GetPos2());
                    pos.y -= 4.0f * GetFrameRateF();
                    pos.y = Max(pos.y, 0.0f);
                    m_AtkColl.SetPos2(0, pos);

                    m_fFrame -= GetFrameRateF();
                    if (m_fFrame <= 0.0f) {
                        m_AtkColl.SetDisableAll();
                        m_fFrame = 10.0f;
                        m_nStep++;
                    }
                    break;
                }
                case 2:
                    m_fFrame -= GetFrameRateF();
                    m_fFrame = AtLeast(m_fFrame, 0.0f);
                    if (m_apObj[0]) {
                        float fFrameMax = 10.0f;
                        if (fFrameMax > 0.0f) {
                            float fAlpha = Clamp(m_fFrame / fFrameMax, 0.0f, 1.0f);
                            bcs::CMdlObj *pMdl = m_apObj[0]->GetMdl();
                            if (pMdl) {
                                pMdl->SetAlphaRateF(fAlpha);
                            }
                        }
                    }
                    if (m_fFrame <= 0.0f) {
                        m_nStep++;
                    }
                    break;
                default: {
                    int bEnd = 1;
                    for (long i = 0; i < 2; i++) {
                        if (m_ahEffect[i] && CEffectCtrl::GetObject(m_ahEffect[i])) {
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
            for (long i = 0; i < 2; i++) {
                if (m_ahEffect[i]) {
                    CEffectCtrl::StopAttachEffect(m_ahEffect[i]);
                    m_ahEffect[i] = 0;
                }
            }
            m_nStep = 0;
        }

        // Both circles fade out over 30 frames, then it ends once the
        // effects are over.
        void CcSiromaMagicCircle::LoopStateDisappear()
        {
            static const float HOCKEY_PRM_SIROMA_MGC_CIRCLE_FOUT_FRAME = 30.0f;

            switch (m_nStep) {
                case 0:
                    m_fFrame = HOCKEY_PRM_SIROMA_MGC_CIRCLE_FOUT_FRAME;
                    m_nStep++;
                    break;
                case 1: {
                    m_fFrame -= GetFrameRateF();
                    m_fFrame = AtLeast(m_fFrame, 0.0f);
                    float fFrameMax = HOCKEY_PRM_SIROMA_MGC_CIRCLE_FOUT_FRAME;
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
                    for (long i = 0; i < 2; i++) {
                        if (m_ahEffect[i] && CEffectCtrl::GetObject(m_ahEffect[i])) {
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
            // (guessed) Dead comparisons: the original .rodata has 90, 100
            // and 6 after the last state's constants and nothing reads them
            // (likely stripped code); this keeps them in place. See
            // notes/decisions.md.
            if (m_fFrame == 90.0f) {
            }
            if (m_fFrame == 100.0f) {
            }
            if (m_fFrame == 6.0f) {
            }
        }
    }
}
