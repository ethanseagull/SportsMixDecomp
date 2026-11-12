// msw::CChrCoreVolleyWaluigi: Waluigi in the volleyball game. His air
// trick splits the ball in four: three copies of it
// (CC06_VOLLEY::CBallVolleyWaluigi) and the ball itself hang from the four
// hands of his option model, and he spikes them at the other court one
// after the other, the target marker jumping between the spikes.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallVolley.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {
    namespace CC06_VOLLEY {

        // A copy of the ball for Waluigi's air trick (0x48C bytes, as
        // CBallVolley). It is a fake: once the special shot is over it
        // cannot be caught and fades out. The class has no exported
        // constructor, so it was most likely local to this file.
        class CBallVolleyWaluigi : public CBallVolley {
            public:
                CBallVolleyWaluigi(bcs::CTask *pParent, long nBallNo)
                    : CBallVolley(pParent, nBallNo)
                {
                    m_nVolleyFlag444 |= 3;
                    m_nVolleyFlag318 |= 2;
                }

                virtual long AttachItemEffect(long) { return 0; }
                virtual int IsVolleyFakeBall() const { return 1; }
                virtual void PlayVolleySpReceiveSuccessSe(unsigned long)
                {
                    PlaySePos(0x423, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                }
                virtual float GetNoCatchCount() const
                {
                    if (m_bArtsOver) {
                        return 99999.0f;
                    }
                    return 0.0f;
                }
                virtual int TrgVolleyArtsOver(long a, long b, unsigned long c)
                {
                    m_bArtsOver = 1;
                    // (name guessed) The ball fades out over fFrame frames.
                    float fFrame = 5.0f;
                    if (fFrame > 0.0f) {
                        m_fArtsOverRate = 1.0f / fFrame;
                    } else {
                        m_fArtsOverRate = 1.0f;
                    }
                    return TrgVolleyArtsOver_(a, b, c);
                }
                virtual ~CBallVolleyWaluigi() {}
        };
    }
}

// (name guessed) The arts shot motions: the first spike, then one per
// copy. Only read with constant indices, so the motion numbers below are
// written directly.
static const long s_anArtsShotMot[4] = { 0x7B, 0x7C, 0x7D, 0x7E };

// The option model's hands the copies hang from, then the ball's.
static const char *s_SplitBallNode[] = {
    "R_hand_a",
    "L_hand_b",
    "R_hand_b",
    "L_hand_a",
};

// Where the balls sit on those hands.
static const float s_afSplitBallOfs[3] = { 0.52f, 0.0f, 0.0f };

msw::CChrCoreVolleyWaluigi::CChrCoreVolleyWaluigi()
{
    m_ahSplitBall[0] = 0;
    m_ahSplitBall[1] = 0;
    m_ahSplitBall[2] = 0;
}

msw::CChrCoreVolleyWaluigi::~CChrCoreVolleyWaluigi()
{
}

inline msw::CBall *msw::CChrCoreVolleyWaluigi::GetSplitBall_(long nIdx)
{
    if (m_ahSplitBall[nIdx] == 0) {
        return 0;
    }
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_ahSplitBall[nIdx]));
    if (pBall == 0 || pBall->IsDelete()) {
        return 0;
    }
    return pBall;
}

inline void msw::CChrCoreVolleyWaluigi::DeleteSplitBall_()
{
    if (CBallMan::IsActive()) {
        for (long i = 0; i < SPLIT_BALL_NUM; i++) {
            CBall *pBall = GetSplitBall_(i);
            if (pBall) {
                CBallMan::DelBall(pBall->GetBallNo());
            }
            m_ahSplitBall[i] = 0;
        }
    }
}

void msw::CChrCoreVolleyWaluigi::Reset_()
{
    CChrCoreVolley::Reset_();
    DeleteSplitBall_();
}

void msw::CChrCoreVolleyWaluigi::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x673, m_nEffNode, 0, 1, 3, 0);
}

void msw::CChrCoreVolleyWaluigi::CtorStateAirTrick()
{
    DeleteSplitBall_();
    CChrCoreVolley::CtorStateAirTrick();
}

void msw::CChrCoreVolleyWaluigi::DtorStateAirTrick()
{
    CChrCoreVolley::DtorStateAirTrick();
}

float msw::CChrCoreVolleyWaluigi::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return AnimGetResFrameNum(0x75) + AnimGetResFrameNum(0x76);
}

float msw::CChrCoreVolleyWaluigi::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return pWork->m_fTrickFrame;
}

// Step 0 waits for the trick motion, then plays the split motion 0x76; the
// ball splits at its frame 50 (step 1). Ready once the motion is over.
int msw::CChrCoreVolleyWaluigi::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SubStateAirTrickDoBallAdsorb_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            SubStateAirTrickDoJumpMove_(pWork);
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x76, 0, 0.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
                Eff_CreateAttachAuto(0x14, 0x78, 3);
                ArtsOptCreateAttachEffectAuto(1, 0x79, 3);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            SubStateAirTrickDoJumpMove_(pWork);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 50.0f) {
                SubStateAirTrickCreateSplitBall_();
                pWork->m_nAdsorbStep = 1;
                pWork->m_nTrickStep++;
            }
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
        default:
            SubStateAirTrickDoJumpMove_(pWork);
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->StopTailEffect();
    }
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickStartArtsShot_(pWork);
    pWork->m_nTrickStep = 0;
}

// Copy nIdx (if still there) is spiked at the work's target, and the target
// marker moves on for the next one.
inline void msw::CChrCoreVolleyWaluigi::ShootSplitBall_(STATE_WORK_AIR_TRICK *pWork, long nIdx)
{
    CBall *pBall = GetSplitBall_(nIdx);
    if (pBall) {
        Util_ArtsShootBall_(pBall, pWork->m_ShootPos);
    }

    float fRot = bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f));
    bcs::FVEC3 ofs;
    bcs::BCS_VecZ_RotY(ofs, 0.0f, fRot);
    m_TargetMarkerPos.x += ofs.x;
    m_TargetMarkerPos.z += ofs.z;
    SetTargetMarkerPosClip__(m_nTargetMarkerCourt, m_TargetMarkerPos);
}

// The three copies are spiked at the shot frame of a motion (even steps);
// after each the next motion plays (odd steps), the last one fading the
// option model out. Then the ball itself is spiked.
int msw::CChrCoreVolleyWaluigi::SubStateAirTrickIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    bcs::FVEC3 target;
    if (GetVolleyTargetMarkerPos(target, 0)) {
        pWork->m_fRotY = GetRotY_ToPosXZ(target);
    }

    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Util_ArtsShootGetTargetPos_(pWork->m_ShootPos, 0);
                SetBallCatchDisable_();
                ShootSplitBall_(pWork, 0);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, s_anArtsShotMot[1], 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                AnimSetSpeed(3, fFrameNum / (fFrameNum + 12.0f));
                Eff_CreateAttachAuto(0x17, 0x7C, 3);
                SubStateAirTrickDoGetBallShotTrg_(pWork);
                Eff_SetSpeed(0x17, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(3, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Util_ArtsShootGetTargetPos_(pWork->m_ShootPos, 0);
                ShootSplitBall_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                AnimPlay(3, s_anArtsShotMot[2], 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                AnimSetSpeed(3, fFrameNum / (fFrameNum + 12.0f));
                Eff_CreateAttachAuto(0x18, 0x7D, 3);
                SubStateAirTrickDoGetBallShotTrg_(pWork);
                Eff_SetSpeed(0x18, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(4, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        case 4:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Util_ArtsShootGetTargetPos_(pWork->m_ShootPos, 0);
                ShootSplitBall_(pWork, 2);
                pWork->m_nTrickStep++;
            }
            break;
        case 5:
            if (IsAnimEnd(3)) {
                AnimPlay(3, s_anArtsShotMot[3], 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                AnimSetSpeed(3, fFrameNum / (fFrameNum + 12.0f));
                Eff_CreateAttachAuto(0x19, 0x7E, 3);
                SubStateAirTrickDoGetBallShotTrg_(pWork);
                ArtsOptFadeOut(5.0f, 70.0f, 1);
                Eff_SetSpeed(0x19, AnimGetSpeed(3));
                ArtsOptSetEffectSpeed(5, AnimGetSpeed(3));
                pWork->m_nTrickStep++;
            }
            break;
        default:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Util_ArtsShootGetTargetPos_(pWork->m_ShootPos, 1);
                Eff_Stop(0x12);
                CBall *pBall = GetAttachBall();
                DetachBall();
                if (pBall) {
                    pWork->m_nFlag |= 1;
                    Util_ArtsShootBall_(pBall, pWork->m_ShootPos);
                }
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_fAdsorbFrameMax = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_AdsorbOfs.x = 0.0f;
    pWork->m_AdsorbOfs.y = 0.0f;
    pWork->m_AdsorbOfs.z = 0.0f;
    AttachBall(pBall->GetBallNo(), 4, pWork->m_fAdsorbFrameMax, 0);
    m_nStatusFlag &= ~0x2C;
}

float msw::CChrCoreVolleyWaluigi::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *)
{
    return 32.0f + AnimGetResFrameNum(0x75);
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x76, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptSetAliveSE(0x424);
    ArtsOptSetFadeOutSE(0x425);
    ArtsOptCreateAttachEffectAuto(0, 0x77, 3);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x7A, 3);
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_Stop(0x15);
    Eff_CreateAttachAuto(0x16, 0x7B, 3);
    Eff_SetSpeed(0x16, AnimGetSpeed(3));
    // The original reads the parameters here, once more below level 1, and
    // uses neither (most likely a value of them computed and left unused).
    GetCcParam_();
    if (pWork->m_nArtsLevel < 1) {
        GetCcParam_();
    }
    ArtsOptSetEffectSpeed(2, AnimGetSpeed(3));
}

void msw::CChrCoreVolleyWaluigi::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// Builds the three copies where the ball is, each destroyed when the game
// resets, and hangs them and the ball from the option model's hands.
void msw::CChrCoreVolleyWaluigi::SubStateAirTrickCreateSplitBall_()
{
    CBall *pBall = GetAttachBall();
    if (pBall == 0) {
        return;
    }

    for (long i = 0; i < SPLIT_BALL_NUM; i++) {
        CC06_VOLLEY::CBallVolleyWaluigi *pCopy =
            new (0) CC06_VOLLEY::CBallVolleyWaluigi(CBallMan::GetInstance(), pBall->GetBallNo());
        pCopy->Create(pBall->GetResFile());
        pCopy->WarpPos(pBall->GetPos());
        pCopy->SetRot(pBall->GetRot());
        pCopy->SetDestroyOnReset(1);

        CBall *pAdded = CBallMan::AddBall(pCopy);
        if (pAdded) {
            m_ahSplitBall[i] = pAdded->GetId();
            long nNode = ArtsOptGetNodeIdx(s_SplitBallNode[i]);
            if (nNode >= 0) {
                pAdded->Attach(GetArtsOptMdl(), nNode, 0.0f, 3, 0x18, 0);
                bcs::FVEC3 ofs(s_afSplitBallOfs[0], s_afSplitBallOfs[1], s_afSplitBallOfs[2]);
                pAdded->SetAttachPos(ofs, 1);
                pAdded->TrgVolleyArtsAttach(m_nTeamNo, m_nPlayerNo, m_nChrId, 0);
            }
        }
    }

    long nNode = ArtsOptGetNodeIdx(s_SplitBallNode[SPLIT_BALL_NUM]);
    if (nNode >= 0) {
        pBall->Attach(GetArtsOptMdl(), nNode, 0.0f, 3, 0x18, 0);
        bcs::FVEC3 ofs(s_afSplitBallOfs[0], s_afSplitBallOfs[1], s_afSplitBallOfs[2]);
        pBall->SetAttachPos(ofs, 1);
        pBall->TrgVolleyArtsAttach(m_nTeamNo, m_nPlayerNo, m_nChrId, 0);
    }
}
