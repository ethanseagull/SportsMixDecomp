// msw::CChrCoreDodgeNinja: the ninja in the dodgeball game.
//
// His special shot: he jumps to one of three places along his side of the
// court and two clones of himself (CC14_DODGE::CcNinjaCloneBody) appear at
// the other two, 3 units apart. Each clone gets a copy of the ball he holds
// and a target marker of its own (CC14_DODGE::CcNinjaCloneMarker) that
// follows his, turned by 120 degrees per place. When he throws, the clones
// throw their balls too, one after the other, then vanish.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CChrTargetMarkerDodge.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CLoadData.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/g3d/ResNode.hpp"

#include <math.h>

// Where the ninja (column 0) and his two clones (columns 1 and 2) stand, in
// steps of 3 units across the court, for the three arrangements
// SubStateArtsShotSetTrickMoveParam_ draws from. Mirrored for the other
// team. The clones' places are relative to his.
static const float s_ClonePosTbl[3][3] = {
    {-1.0f, 1.0f, 2.0f},
    {0.0f, -1.0f, 1.0f},
    {1.0f, -2.0f, -1.0f},
};

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // The state of the air trick.
    const long STATE_AIR_TRICK = 0x27;

    inline float WrapRad(float rad)
    {
        while (rad > PI2) {
            rad -= PI2;
        }
        while (rad < 0.0f) {
            rad += PI2;
        }
        return rad;
    }

    // The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    inline float Clamp(float x, float min, float max)
    {
        return x > max ? max : (x < min ? min : x);
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }
}

namespace CC14_DODGE {

    class CcNinjaCloneBody;

    // The target marker of one clone: shown while the ninja's own marker
    // is, at the point the clone throws at.
    class CcNinjaCloneMarker : public msw::CChrArtsTargetMarkerDodge {
        public:
            CcNinjaCloneMarker(CcNinjaCloneBody *pBody, long nChr);
            virtual ~CcNinjaCloneMarker() {}

        protected:
            // (declared in this order: the weak copies come out in reverse
            // order of declaration, CheckAppear_ first in the original)
            virtual void GetTargetPos_(bcs::FVEC3 &rPos);
            virtual int CheckAppear_();

            CcNinjaCloneBody *m_pBody; // 0xA8
    };

    // One clone: the ninja's model playing his special-shot motions, 0xC0
    // bytes. bcs::CFsm at 0x34 (vptr at 0x78). Member names are guessed.
    class CcNinjaCloneBody : public msw::CGameTask, public bcs::CFsm<CcNinjaCloneBody> {
            friend class CcNinjaCloneMarker;
            friend class msw::CChrCoreDodgeNinja;

        public:
            enum {
                STATE_APPEAR,    // runs its jump motion beside the ninja
                STATE_LOOP,      // waits in the air with him
                STATE_IMPACT,    // waits its turn, then throws
                STATE_DISAPPEAR, // shrinks and fades away, then deletes the task
                STATE_NUM
            };

            // nIndex: which clone (0 or 1); fOfsX: its place beside the
            // ninja in steps of 3 units; fFrame: the frame of his motion
            // the clone's starts at.
            CcNinjaCloneBody(bcs::CTask *pParent, msw::CChrCoreDodgeNinja *pChr, long nIndex, float fOfsX,
                             float fFrame);
            virtual ~CcNinjaCloneBody();
            virtual void Main();

            virtual void AnimPlay(long nMotId, int bLoop, float fBlend, float fFrame);
            virtual void AnimStop();
            virtual void AnimSetSpeed(float fSpeed);
            virtual void AnimSetFrame(float fFrame);

            // Throws after fWait frames, at frame fShotFrame of the motion.
            void StartShot(float fWait, float fShotFrame)
            {
                m_fWait = Max(fWait, 0.0f);
                m_fShotFrame = Max(fShotFrame, 0.0f);
                ChangeState(STATE_IMPACT);
            }

            // Puts pBall in the clone's hand (node nNode).
            void AttachBall(msw::CBall *pBall, unsigned long nNode)
            {
                if (pBall) {
                    pBall->Attach(m_pMdl, nNode, 0.0f, 3, 0x18, 0);
                }
            }

        private:
            void CreateMdl_();

            float AnimGetFrame() const { return m_pAnm != 0 ? m_pAnm->GetFrame() : 0.0f; }
            float AnimGetEndFrame() const { return m_pAnm != 0 ? m_pAnm->VFunc28() : 0.0f; }
            int IsAnimEnd() const { return m_pAnm != 0 ? m_pAnm->IsEnd() : 1; }

            void StopEffect_()
            {
                if (m_hEffect) {
                    msw::CEffectCtrl::StopAttachEffect(m_hEffect);
                    m_hEffect = 0;
                }
            }

            // Turns the model like the ninja.
            void SetMdlRot_(const bcs::FVEC3 &rRot) { m_pMdl->SetRotXYZ(rRot.x, rRot.y, rRot.z); }

            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateLoop();
            void LoopStateLoop();
            void DtorStateLoop();
            void CtorStateImpact();
            void LoopStateImpact();
            void DtorStateImpact();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

            msw::CChrCoreDodgeNinja *m_pChr; // 0x7C
            msw::CChrMdlObj *m_pMdl;         // 0x80
            bcs::CAnmMotion *m_pAnm;         // 0x84
            unsigned long m_Unk88;           // 0x88 (cleared, never read here)
            CcNinjaCloneMarker *m_pMarker;   // 0x8C
            float m_fOfsX;                   // 0x90
            long m_nIndex;                   // 0x94
            long m_nStep;                    // 0x98, STATE_IMPACT: 0 waiting, 1 throwing
            float m_fRate;                   // 0x9C, STATE_DISAPPEAR: 0 to 1
            float m_fWait;                   // 0xA0, frames left before the throw
            float m_fShotFrame;              // 0xA4, the frame of the throw motion the ball leaves at
            float m_fStartFrame;             // 0xA8
            unsigned long m_hEffect;         // 0xAC, the effect of the current state
            unsigned long m_hEffect2;        // 0xB0, the effect it appears with
            bcs::FVEC3 m_TargetPos;          // 0xB4, where its ball goes
    };
}

namespace CC14_DODGE {

    inline CcNinjaCloneMarker::CcNinjaCloneMarker(CcNinjaCloneBody *pBody, long nChr)
        : CChrArtsTargetMarkerDodge(pBody, nChr, 0), m_pBody(pBody)
    {
    }

    // Shown while the ninja's marker is.
    inline int CcNinjaCloneMarker::CheckAppear_()
    {
        if (m_pBody && m_pBody->m_pChr->IsDodgeArtsTargetMarkerActive()) {
            return 2;
        }
        return 3;
    }

    // The clone's target (the marker's own place without a clone).
    inline void CcNinjaCloneMarker::GetTargetPos_(bcs::FVEC3 &rPos)
    {
        rPos.x = 0.0f;
        rPos.y = 0.0f;
        rPos.z = 0.0f;
        if (m_pMdl) {
            rPos = m_pMdl->m_vTrans;
        }
        if (m_pBody) {
            rPos = m_pBody->m_TargetPos;
        }
    }
}

// The task of clone nIndex (null once it is gone).
inline CC14_DODGE::CcNinjaCloneBody *msw::CChrCoreDodgeNinja::GetClone_(long nIndex) const
{
    return static_cast<CC14_DODGE::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[nIndex]));
}

inline void msw::CChrCoreDodgeNinja::DeleteClone_()
{
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_ahClone[i]);
            if (pTask && !pTask->IsDelete()) {
                pTask->Delete();
            }
            m_ahClone[i] = 0;
        }
    }
}

msw::CChrCoreDodgeNinja::CChrCoreDodgeNinja()
{
    m_nPosType = 0;
    for (long i = 0; i < CLONE_NUM; i++) {
        m_ahClone[i] = 0;
        m_ahDivideBall[i] = 0;
    }
}

msw::CChrCoreDodgeNinja::~CChrCoreDodgeNinja()
{
    DeleteClone_();
    DivideBallDeleteAll();
}

void msw::CChrCoreDodgeNinja::Reset_()
{
    DeleteClone_();
    DivideBallDeleteAll();
    CChrCoreDodge::Reset_();
}

// His own marker, and one per clone during the air trick.
long msw::CChrCoreDodgeNinja::GetDodgeArtsTargetMarkerNum() const
{
    if (!IsDodgeArtsTargetMarkerActive()) {
        return 0;
    }
    long nNum = 1;
    if (GetState() == STATE_AIR_TRICK) {
        if (m_ahClone[0]) {
            nNum++;
        }
        if (m_ahClone[1]) {
            nNum++;
        }
    }
    return nNum;
}

// Marker 0 is his own (the trick's target), 1 and 2 the clones'.
long msw::CChrCoreDodgeNinja::GetDodgeArtsTargetMarkerPos(bcs::FVEC3 &rPos, long nIndex)
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    if (!IsDodgeArtsTargetMarkerActive()) {
        return 0;
    }
    if (nIndex >= 1 && nIndex <= CLONE_NUM && GetState() == STATE_AIR_TRICK) {
        unsigned long hClone = m_ahClone[nIndex - 1];
        if (hClone) {
            CC14_DODGE::CcNinjaCloneBody *pClone =
                static_cast<CC14_DODGE::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(hClone));
            if (pClone && !pClone->IsDelete()) {
                rPos = pClone->m_TargetPos;
                return 1;
            }
        }
    }

    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    if (pWork->m_nFlag04 & 2) {
        rPos = pWork->m_TargetPos;
        return 1;
    }
    return 0;
}

// When he stands at one end of the row, the middle one is a clone: the
// special shot's effects centre on it.
void msw::CChrCoreDodgeNinja::GetSpRootPos(bcs::FVEC3 &rPos) const
{
    rPos = GetPos();
    if (GetState() == STATE_AIR_TRICK) {
        long nIndex = -1;
        switch (m_nPosType) {
            case 0:
                nIndex = 0;
                break;
            case 2:
                nIndex = 1;
                break;
        }
        if (nIndex >= 0 && m_ahClone[nIndex]) {
            CC14_DODGE::CcNinjaCloneBody *pClone = GetClone_(nIndex);
            if (pClone && !pClone->IsDelete() && pClone->m_pMdl) {
                rPos = pClone->m_pMdl->m_vTrans;
            }
        }
    }
}

void msw::CChrCoreDodgeNinja::DtorStateAirTrick()
{
    CChrCoreDodge::DtorStateAirTrick();
    DeleteClone_();
    DivideBallDeleteAll();
}

void msw::CChrCoreDodgeNinja::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x9F, 3);
    Eff_CreateAttachAuto(0x14, 0xA0, 3);
}

// A puff of smoke on each clone's chest.
void msw::CChrCoreDodgeNinja::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotReadyArtsShot_(pWork);

    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_DODGE::CcNinjaCloneBody *pClone =
                static_cast<CC14_DODGE::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone == 0) {
                m_ahClone[i] = 0;
            } else if (pClone->IsDelete()) {
                m_ahClone[i] = 0;
            } else if (pClone->m_pMdl) {
                nw4r::g3d::ResMdl mdl = pClone->m_pMdl->GetResMdl();
                if (mdl.IsValid()) {
                    nw4r::g3d::ResNode node = mdl.GetResNode("mune_jnt");
                    if (node.IsValid()) {
                        bcs::FMTX34 mtx;
                        pClone->m_pMdl->GetScnMtxPos(&mtx, node.GetID(), (bcs::CScnObj::ScnObjMtxType)1);
                        bcs::FVEC3 pos;
                        pos.x = mtx.m[0][3];
                        pos.y = mtx.m[1][3];
                        pos.z = mtx.m[2][3];
                        CEffectCtrl::SimpleEffect(0x4E7, pos, 3, 0, 0);
                    }
                }
            }
        }
    }
}

void msw::CChrCoreDodgeNinja::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xA1, 3);
    SE_PlayMove(8, 0x476, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreDodgeNinja::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0xA2, 3);
}

// He faces the other court.
float msw::CChrCoreDodgeNinja::SubStateArtsShotGetTrickStartRot_(STATE_WORK_AIR_TRICK_CMN *)
{
    if (GetTargetGoalIdx() != 0) {
        return 0.0f;
    }
    return PI;
}

void msw::CChrCoreDodgeNinja::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotShootBall_(pWork);
}

// He jumps to his place in the drawn arrangement, 2 units from the middle
// line on his side.
void msw::CChrCoreDodgeNinja::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long)
{
    float fZ = fabsf(2.0f);
    pWork->m_MovePos = GetPos();
    if (GetTargetGoalIdx() != 0) {
        fZ = -fZ;
    }
    pWork->m_MovePos.z = fZ;

    long nPosType = bcs::BCS_RandMax(2L);
    float fX = s_ClonePosTbl[nPosType][0];
    if (GetTargetGoalIdx() != 0) {
        fX = -fX;
    }
    pWork->m_MovePos.x = 3.0f * fX;

    float fFrame = 0.0f;
    if (pWork->m_fAnimSpeed > fFrame) {
        fFrame = Max(fFrame / pWork->m_fAnimSpeed, fFrame);
    }

    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_MovePos, GetPos());
    pWork->m_MoveParam.m_fFrame = fFrame;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    m_nPosType = nPosType;
}

// Step 0: when his trick motion starts, the clones appear at their places
// and the ball is copied for them. Step 1: the loop motion follows the trick
// motion; then 60 frames of waiting.
int msw::CChrCoreDodgeNinja::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);

    switch (pWork->m_nTrickStep) {
        case 0:
            SubStateArtsShotDoJumpMove_(pWork);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 0.0f) {
                long nPosType = m_nPosType;
                float fFrame = AnimGetFrame(3);
                for (long i = 0; i < CLONE_NUM; i++) {
                    if (m_ahClone[i] == 0) {
                        float fOfsX = s_ClonePosTbl[nPosType][1 + i];
                        if (GetTargetGoalIdx() != 0) {
                            fOfsX = -fOfsX;
                        }
                        CC14_DODGE::CcNinjaCloneBody *pClone = new (0x100)
                            CC14_DODGE::CcNinjaCloneBody(CGame::GetTempParent(), this, i, fOfsX, fFrame);
                        if (pClone) {
                            m_ahClone[i] = pClone->GetId();
                        }
                    }
                }
                DivideBallCreate();
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            SubStateArtsShotDoJumpMove_(pWork);
            if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
                SubStateArtsShotPlayLoopMotion_(pWork);
                pWork->m_nArtsLevel = 1;
                pWork->m_nTrickStep++;
                pWork->m_fTrickFrame = 60.0f;
                pWork->m_fTrickCount = 0.0f;
            }
            break;
        default:
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                bReady = 1;
            }
            break;
    }
    return bReady;
}

// The clones throw at the trigger of his throw motion, 50 frames apart.
void msw::CChrCoreDodgeNinja::SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_nTrickStep = 0;
    pWork->m_fShotWait = 100.0f;
    float fShotFrame = AnimSearchTrgFrame(0x7F, 2, -1.0f);
    if (fShotFrame < 0.0f) {
        fShotFrame = 16.0f;
    }
    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_DODGE::CcNinjaCloneBody *pClone =
                static_cast<CC14_DODGE::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone && !pClone->IsDelete()) {
                pClone->StartShot(50.0f * i, fShotFrame);
            }
        }
    }
}

// Step 0 waits m_fShotWait frames, then plays his throw motion; the ball
// leaves at its m_fShotFrame.
int msw::CChrCoreDodgeNinja::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (pWork->m_nTrickStep == 0) {
        pWork->m_fShotWait -= m_fFrameStep * m_fTimeScale;
        if (pWork->m_fShotWait <= 0.0f) {
            SE_Stop(8, 8.0f);
            SubStateArtsShotPlayArtsShotMotion_(pWork);
            SubStateArtsShotDoGetBallShotTrg_(pWork);
            pWork->m_nTrickStep++;
        }
    } else if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
        if (!(pWork->m_nFlag04 & 4)) {
            Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
        }
        SubStateArtsShotShootBall_(pWork);
        pWork->m_nArtsLevel = 0;
        return 1;
    }
    return 0;
}

// Clone nIndex throws its ball at rPos.
void msw::CChrCoreDodgeNinja::TrgCloneShoot(const bcs::FVEC3 &rPos, long nIndex)
{
    bcs::FVEC3 target(rPos);
    if (m_ahDivideBall[nIndex]) {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_ahDivideBall[nIndex]));
        if (pBall) {
            ReleaseBallDelete(nIndex);
            pBall->Detach();
            Util_ShootBall(pBall, target, 9, 1);
            PlayVoice(0x63, 0);
            SE_PlayArtsShootSE(pBall->GetPos());
        }
    }
}

// His marker starts at its default place, across from him.
void msw::CChrCoreDodgeNinja::SubStateArtsShotStartTargetMarker_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (!(pWork->m_nFlag04 & 2)) {
        GetDodgeArtsTargetMarkerDefaultPos(pWork->m_TargetPos, 0);
        pWork->m_TargetPos.x = pWork->m_MovePos.x;
        pWork->m_nFlag04 |= 2;
    }
}

// A copy of the ball he holds for each clone, in its hand.
void msw::CChrCoreDodgeNinja::DivideBallCreate()
{
    DivideBallDeleteAll();

    CBall *pBall = GetAttachBall();
    bcs::CMdlObj *pBallMdl = pBall ? pBall->GetMdlObj() : 0;
    if (!pBall || !pBallMdl) {
        return;
    }
    if (!pBallMdl->GetResMdl().IsValid()) {
        return;
    }

    for (long i = 0; i < CLONE_NUM; i++) {
        if (m_ahClone[i]) {
            CC14_DODGE::CcNinjaCloneBody *pClone =
                static_cast<CC14_DODGE::CcNinjaCloneBody *>(bcs::CTaskMan::GetTask(m_ahClone[i]));
            if (pClone && !pClone->IsDelete()) {
                CBall *pDivide = CBallMan::AddBall(pBall);
                if (pDivide) {
                    pClone->AttachBall(pDivide, m_nEffNodeDunk);
                    m_ahDivideBall[i] = pDivide->GetId();
                }
            }
        }
    }
}

msw::CBall *msw::CChrCoreDodgeNinja::DivideBallGet(long nIndex)
{
    return static_cast<CBall *>(bcs::CTaskMan::GetTask(m_ahDivideBall[nIndex]));
}

const msw::CBall *msw::CChrCoreDodgeNinja::DivideBallGet(long nIndex) const
{
    return static_cast<CBall *>(bcs::CTaskMan::GetTask(m_ahDivideBall[nIndex]));
}

void msw::CChrCoreDodgeNinja::DivideBallDelete(long nIndex)
{
    m_ahDivideBall[nIndex] = 0;
}

// Forgets a ball that has been thrown.
void msw::CChrCoreDodgeNinja::ReleaseBallDelete(long nIndex)
{
    m_ahDivideBall[nIndex] = 0;
}

// Removes the copies still held and forgets them.
void msw::CChrCoreDodgeNinja::DivideBallDeleteAll()
{
    for (long i = 0; i < CLONE_NUM; i++) {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_ahDivideBall[i]));
        if (pBall) {
            CBallMan::DelBall(pBall->GetBallNo());
        }
        m_ahDivideBall[i] = 0;
    }
}

namespace CC14_DODGE {

    CcNinjaCloneBody::CcNinjaCloneBody(bcs::CTask *pParent, msw::CChrCoreDodgeNinja *pChr, long nIndex, float fOfsX,
                                       float fFrame)
        : CGameTask(pParent, 0x15)
        , m_pChr(pChr)
        , m_pMdl(0)
        , m_Unk88(0)
        , m_pMarker(0)
        , m_fOfsX(fOfsX)
        , m_nIndex(nIndex)
        , m_nStep(0)
        , m_fRate(0.0f)
        , m_fStartFrame(fFrame)
        , m_hEffect(0)
        , m_hEffect2(0)
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_APPEAR, &CcNinjaCloneBody::CtorStateAppear, &CcNinjaCloneBody::LoopStateAppear,
                 &CcNinjaCloneBody::DtorStateAppear);
        SetState(STATE_LOOP, &CcNinjaCloneBody::CtorStateLoop, &CcNinjaCloneBody::LoopStateLoop,
                 &CcNinjaCloneBody::DtorStateLoop);
        SetState(STATE_IMPACT, &CcNinjaCloneBody::CtorStateImpact, &CcNinjaCloneBody::LoopStateImpact,
                 &CcNinjaCloneBody::DtorStateImpact);
        SetState(STATE_DISAPPEAR, &CcNinjaCloneBody::CtorStateDisappear, &CcNinjaCloneBody::LoopStateDisappear,
                 &CcNinjaCloneBody::DtorStateDisappear);

        CreateMdl_();
        m_TargetPos.x = 0.0f;
        m_TargetPos.y = 0.0f;
        m_TargetPos.z = 0.0f;
        m_hEffect = msw::CEffectCtrl::CreateAttachEffectAuto(0xA0, m_pMdl, 3);
        m_hEffect2 = msw::CEffectCtrl::CreateAttachEffectAuto(0x22, m_pMdl, 3);
        ChangeState(STATE_APPEAR);
    }

    CcNinjaCloneBody::~CcNinjaCloneBody()
    {
        if (m_hEffect2) {
            msw::CEffectCtrl::StopAttachEffect(m_hEffect2);
            m_hEffect2 = 0;
        }
        msw::CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
        if (m_pMdl) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdl);
        }
        m_pMdl = 0;
    }

    // The ninja's model and motions, drawn without the near-plane clip,
    // and the clone's target marker.
    void CcNinjaCloneBody::CreateMdl_()
    {
        nw4r::g3d::ResFile mdlFile = msw::CLoadData::GetRes(m_pChr->GetPlayerNo() + 3);
        nw4r::g3d::ResFile motFile = msw::CLoadData::GetRes(m_pChr->GetPlayerNo() + 0x13);

        m_pMdl = new (0x100) msw::CChrMdlObj;
        m_pMdl->CreateGameObj(mdlFile, m_pChr->GetChrIdS(), m_pChr->GetPlayerNo(), 0);

        m_pAnm = new (0x100) bcs::CAnmMotion;
        const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo =
            msw::CC_DATA::GetMdl_NodeExInfo(m_pChr->GetChrIdS(), msw::CGame::g_GameWork.GetGameMode());
        m_pAnm->Create(m_pMdl, motFile, pInfo, 3, 0);
        m_pAnm->OnFlag(8);
        m_pMdl->AttachAnmObj(m_pAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
        m_pMdl->OnFlag(0x2000);
        m_pMdl->OnFlag(0x4000);
        m_pMdl->OffFlag(0x100);
        m_pMdl->OffFlag(0x2);
        m_pMdl->EnableCameraNearClip(1);
        bcs::CRenderMan::PushBack(m_pMdl, 2);

        m_pMarker = new (0x100) CcNinjaCloneMarker(this, m_pChr->GetPlayerNo());
    }

    // Its target follows the ninja's marker, turned by 120 degrees per
    // place.
    void CcNinjaCloneBody::Main()
    {
        ExecState();

        if (m_pChr && m_pChr->IsDodgeArtsTargetMarkerActive()) {
            bcs::FVEC3 move;
            m_pChr->GetDodgeArtsTargetMarkerMoved(move);
            float fRot = WrapRad(m_fOfsX * (PI2 / 3.0f));
            bcs::BCS_Vec_RotY(move, move, fRot);
            m_TargetPos.x += move.x;
            m_TargetPos.z += move.z;
            m_pChr->SetDodgeArtsTargetMarkerPosClip(m_TargetPos);
        }
    }

    // His jump motion from where his is; the target starts 3 units per
    // place from his.
    void CcNinjaCloneBody::CtorStateAppear()
    {
        AnimPlay(0x7A, 0, 0.0f, m_fStartFrame);
        m_pChr->GetDodgeArtsTargetMarkerPos(m_TargetPos, 0);
        m_TargetPos.x += 3.0f * m_fOfsX;
    }

    // Moves out to its place over the motion.
    void CcNinjaCloneBody::LoopStateAppear()
    {
        if (m_pMdl) {
            SetMdlRot_(m_pChr->GetRot());
            bcs::FVEC3 pos(m_pChr->GetPos());
            float fFrame = AnimGetFrame() - m_fStartFrame;
            float fFrameNum = AnimGetEndFrame() - m_fStartFrame;
            float fRate = fFrameNum <= 0.0f ? 1.0f : fFrame / fFrameNum;
            pos.x += fRate * (3.0f * m_fOfsX);
            m_pMdl->m_vTrans = pos;
        }
        if (IsAnimEnd()) {
            ChangeState(STATE_LOOP);
        }
    }

    void CcNinjaCloneBody::DtorStateAppear()
    {
    }

    void CcNinjaCloneBody::CtorStateLoop()
    {
        StopEffect_();
        AnimPlay(0x85, 1, 0.0f, 0.0f);
        m_hEffect = msw::CEffectCtrl::CreateAttachEffectAuto(0xA1, m_pMdl, 3);
    }

    // Keeps its place beside the ninja.
    void CcNinjaCloneBody::LoopStateLoop()
    {
        if (m_pMdl) {
            SetMdlRot_(m_pChr->GetRot());
            bcs::FVEC3 pos(m_pChr->GetPos());
            pos.x += 3.0f * m_fOfsX;
            m_pMdl->m_vTrans = pos;
        }
    }

    void CcNinjaCloneBody::DtorStateLoop()
    {
    }

    void CcNinjaCloneBody::CtorStateImpact()
    {
        m_nStep = 0;
    }

    // Waits m_fWait frames, plays the throw motion and throws at its
    // m_fShotFrame.
    void CcNinjaCloneBody::LoopStateImpact()
    {
        if (m_nStep == 0) {
            m_fWait -= GetFrameRateF();
            if (m_fWait <= 0.0f) {
                StopEffect_();
                AnimPlay(0x7F, 0, 0.0f, 0.0f);
                m_hEffect = msw::CEffectCtrl::CreateAttachEffectAuto(0xA2, m_pMdl, 3);
                m_nStep++;
            }
        } else if (IsAnimEnd() || AnimGetFrame() >= m_fShotFrame) {
            m_pChr->TrgCloneShoot(m_TargetPos, m_nIndex);
            ChangeState(STATE_DISAPPEAR);
        }
    }

    void CcNinjaCloneBody::DtorStateImpact()
    {
    }

    void CcNinjaCloneBody::CtorStateDisappear()
    {
        m_fRate = 0.0f;
        if (m_hEffect2) {
            msw::CEffectCtrl::StopAttachEffect(m_hEffect2);
            m_hEffect2 = 0;
        }
        StopEffect_();
        m_hEffect = msw::CEffectCtrl::CreateAttachEffectAuto(0xA3, m_pMdl, 3);
        msw::CGameSnd::Play(0x4DE, m_pMdl->m_vTrans, 1, -1, 0.0f, 1.0f, 0.0f);
    }

    // Stretches up and fades out over 16 frames, then deletes the task.
    void CcNinjaCloneBody::LoopStateDisappear()
    {
        m_fRate += 0.0625f * GetFrameRateF();
        m_fRate = Clamp(m_fRate, 0.0f, 1.0f);
        if (m_pMdl) {
            m_pMdl->SetAlphaRateF(1.0f - m_fRate);
            m_pMdl->m_vScale.y = 2.0f * m_fRate + 1.0f;
            m_pMdl->m_vScale.x = 1.0f - 0.8f * m_fRate;
            m_pMdl->m_vScale.z = 1.0f - 0.8f * m_fRate;
        }
        if (m_fRate >= 1.0f) {
            Delete();
        }
    }

    void CcNinjaCloneBody::DtorStateDisappear()
    {
    }

    // fBlend below 0 blends over -fBlend frames another way (bit 3).
    void CcNinjaCloneBody::AnimPlay(long nMotId, int bLoop, float fBlend, float fFrame)
    {
        if (m_pAnm) {
            unsigned long nFlag = 0;
            if (fBlend < 0.0f) {
                fBlend = fabsf(fBlend);
                nFlag |= 8;
            }
            if (bLoop) {
                nFlag |= 2;
            }
            m_pAnm->Play(nMotId, nFlag, 1.0f, fBlend, fFrame);
        }
    }

    void CcNinjaCloneBody::AnimStop()
    {
        if (m_pAnm) {
            m_pAnm->Stop();
        }
    }

    void CcNinjaCloneBody::AnimSetSpeed(float fSpeed)
    {
        if (m_pAnm) {
            m_pAnm->SetRate(fSpeed);
        }
    }

    void CcNinjaCloneBody::AnimSetFrame(float fFrame)
    {
        if (m_pAnm) {
            m_pAnm->SetFrame(fFrame);
        }
    }
}
