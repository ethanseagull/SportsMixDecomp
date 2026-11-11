// msw::CChrCoreHockeyWaluigi: Waluigi in the hockey game. His special
// shot splits the puck: three copies of it (CC06_HOCKEY::CBallHockeyWaluigi)
// and the puck itself hang from the four hands of his option model, a
// quarter turn about X; he throws the copies one after the other at the
// point m_ArtsTargetPos, with a pause and a new motion between the
// throws, then shoots the puck as in the common special shot.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallHockey.hpp"
#include "game/msw/CBallMan.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"

namespace msw {
    namespace CC06_HOCKEY {

        // A copy of the puck for Waluigi's special shot, 0x48C bytes. It
        // cannot be caught once its special shot is over. The class has no
        // exported constructor, so it was most likely local to this file.
        class CBallHockeyWaluigi : public CBallHockey {
            public:
                CBallHockeyWaluigi(bcs::CTask *pParent, long nBallNo, long nOrgBallNo)
                    : CBallHockey(pParent, nBallNo)
                {
                    m_nOrgBallNo = nOrgBallNo;
                    m_bArtsOver = 0;
                    m_nHockeyFlag |= 3;
                }
                virtual ~CBallHockeyWaluigi() {}

                virtual long AttachItemEffect(long) { return 0; }
                virtual int CanbeCatch() const
                {
                    if (m_bArtsOver) {
                        return 0;
                    }
                    return CBallHockey::CanbeCatch();
                }
                virtual float GetNoCatchCount() const
                {
                    if (m_bArtsOver) {
                        return 99999.0f;
                    }
                    return m_fNoCatchCount;
                }
                virtual int TrgHockeyArtsOver(long)
                {
                    m_bArtsOver = 1;
                    return 1;
                }

            private:
                long m_nOrgBallNo; // 0x484, the ball number of the puck it copies (name guessed)
                int m_bArtsOver;   // 0x488
        };
    }
}

// The option model's nodes the copies hang from, then the puck's.
static const char *s_SplitBallNode[] = {
    "L_hand_a",
    "R_hand_b",
    "L_hand_b",
    "R_hand_a",
};

// Where the balls sit on those nodes.
static const float s_afSplitBallOfs[3] = { 0.52f, 0.0f, 0.0f };

// (name guessed) The puck's turn while it hangs from a hand: a quarter
// turn about X (32 is an eighth of the sine table's full turn, the half
// angle).
static inline void GetHangRot(Quaternion &rRot)
{
    bcs::FVEC3 axis(1.0f, 0.0f, 0.0f);
    float fSin;
    float fCos;
    nw4r::math::SinCosFIdx(&fSin, &fCos, 32.0f);
    rRot.x = axis.x * fSin;
    rRot.y = axis.y * fSin;
    rRot.z = axis.z * fSin;
    rRot.w = fCos;
}

msw::CChrCoreHockeyWaluigi::CChrCoreHockeyWaluigi()
{
    m_anSplitBall[0] = -1;
    m_anSplitBall[1] = -1;
    m_anSplitBall[2] = -1;
}

msw::CChrCoreHockeyWaluigi::~CChrCoreHockeyWaluigi()
{
}

inline void msw::CChrCoreHockeyWaluigi::DeleteSplitBall_()
{
    if (CBallMan::IsActive()) {
        for (long i = 0; i < SPLIT_BALL_NUM; i++) {
            CBall *pBall = m_anSplitBall[i] < 0 ? 0 : CBallMan::GetBallPtr(m_anSplitBall[i]);
            if (pBall != 0 && pBall->IsDelete()) {
                pBall = 0;
            }
            if (pBall != 0) {
                CBallMan::DelBall(pBall->GetBallNo());
            }
            m_anSplitBall[i] = -1;
        }
    }
}

void msw::CChrCoreHockeyWaluigi::CtorStateAirTrick()
{
    DeleteSplitBall_();
    CChrCoreHockey::CtorStateAirTrick();
}

void msw::CChrCoreHockeyWaluigi::Reset_()
{
    CChrCoreHockey::Reset_();
    DeleteSplitBall_();
}

// The puck splits near the end of the trick motion (frame 84), then turns
// a quarter about X while the rest of the motion plays.
int msw::CChrCoreHockeyWaluigi::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 84.0f) {
                SubStateArtsShotCreateSplitBall_();
                pWork->m_nTrickStep++;
            }
            break;
        case 1: {
            CBall *pBall = GetAttachBall();
            if (pBall != 0) {
                Quaternion rot;
                GetHangRot(rot);
                pBall->SetRot(rot);
            }
            if (IsAnimEnd(3)) {
                bReady = 1;
                pWork->m_nTrickStep++;
            }
            break;
        }
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeyWaluigi::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x98, 3);
    ArtsOptCreate(1, 0, 1);
    ArtsOptSetAliveSE(0x424);
    ArtsOptSetFadeOutSE(0x425);
    ArtsOptCreateAttachEffectAuto(0, 0x99, 3);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    SE_PlayArtsJumpSE(0);
    pWork->m_nShotStep = 0;
    pWork->m_fShotTimer = 0.0f;
}

void msw::CChrCoreHockeyWaluigi::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x14, 0x9A, 3);
}

// The copy leaves towards m_ArtsTargetPos. (The yaw to the goal is asked
// for and not used.)
inline void msw::CChrCoreHockeyWaluigi::ShootSplitBall_(long idx)
{
    if (m_anSplitBall[idx] >= 0) {
        CBall *pBall = CBallMan::GetBallPtr(m_anSplitBall[idx]);
        if (pBall != 0) {
            float rotY = GetRotY_ToGoal();
            bcs::FVEC3 dir(pBall->GetPos());
            dir.x = m_ArtsTargetPos.x - dir.x;
            dir.z = m_ArtsTargetPos.z - dir.z;
            rotY = nw4r::math::Atan2Rad(dir.x, dir.z);
            while (rotY > 6.2831855f) {
                rotY -= 6.2831855f;
            }
            while (rotY < 0.0f) {
                rotY += 6.2831855f;
            }
            Util_ShootBall_(pBall, 9, m_fUnk264, rotY);
            PlayVoice(0x83, 0);
            SE_PlayArtsShootSE(pBall->GetPos());
        }
    }
}

// Three throws, each at the shot frame of a motion (even steps), and 90
// frames after it the next motion (odd steps); the last one fades the
// option model out. Then the common special shot shoots the puck.
int msw::CChrCoreHockeyWaluigi::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    switch (pWork->m_nShotStep) {
        case 0:
            pWork->m_fShotTimer += m_fFrameStep;
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                ShootSplitBall_(0);
                pWork->m_nShotStep++;
            }
            break;
        case 1:
            pWork->m_fShotTimer += m_fFrameStep;
            if (pWork->m_fShotTimer >= 90.0f) {
                AnimPlay(3, 0xA2, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x16, 0x9C, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork, 16.0f);
                pWork->m_nShotStep++;
                pWork->m_fShotTimer = 0.0f;
            }
            break;
        case 2:
            pWork->m_fShotTimer += m_fFrameStep;
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                ShootSplitBall_(1);
                pWork->m_nShotStep++;
            }
            break;
        case 3:
            pWork->m_fShotTimer += m_fFrameStep;
            if (pWork->m_fShotTimer >= 90.0f) {
                AnimPlay(3, 0xA3, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x17, 0x9D, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork, 16.0f);
                pWork->m_nShotStep++;
                pWork->m_fShotTimer = 0.0f;
            }
            break;
        case 4:
            pWork->m_fShotTimer += m_fFrameStep;
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                ShootSplitBall_(2);
                pWork->m_nShotStep++;
            }
            break;
        case 5:
            pWork->m_fShotTimer += m_fFrameStep;
            if (pWork->m_fShotTimer >= 90.0f) {
                AnimPlay(3, 0xA4, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x18, 0x9E, 3);
                SubStateArtsShotDoGetBallShotTrg_(pWork, 16.0f);
                ArtsOptFadeOut(5.0f, 92.0f, 1);
                pWork->m_nShotStep++;
            }
            break;
        default:
            return CChrCoreHockey::SubStateArtsShotIsEndArtsShot_(pWork);
    }
    return 0;
}

void msw::CChrCoreHockeyWaluigi::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    Eff_Stop(0x14);
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x9B, 3);
}

void msw::CChrCoreHockeyWaluigi::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// Builds the three copies where the puck is, each destroyed when the game
// resets, and hangs them and the puck from the option model's hands.
void msw::CChrCoreHockeyWaluigi::SubStateArtsShotCreateSplitBall_()
{
    CBall *pBall = GetAttachBall();
    if (pBall == 0) {
        return;
    }

    long nBallNum = CBallMan::GetBallNum();
    Quaternion rot;
    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = 0.0f;
    rot.w = 1.0f;
    GetHangRot(rot);

    for (long i = 0; i < SPLIT_BALL_NUM; i++) {
        CC06_HOCKEY::CBallHockeyWaluigi *pCopy = new (0x100)
            CC06_HOCKEY::CBallHockeyWaluigi(CBallMan::GetInstance(), nBallNum + i, pBall->GetBallNo());
        pCopy->Create(pBall->GetResFile());
        pCopy->WarpPos(pBall->GetPos());
        pCopy->SetRot(rot);
        pCopy->SetDestroyOnReset(1);

        CBall *pAdded = CBallMan::AddBall(pCopy);
        if (pAdded != 0) {
            m_anSplitBall[i] = pAdded->GetBallNo();
            long nNode = ArtsOptGetNodeIdx(s_SplitBallNode[i]);
            if (nNode >= 0) {
                pAdded->Attach(GetArtsOptMdl(), nNode, 0.0f, 3, 0x18, 0);
                bcs::FVEC3 ofs(s_afSplitBallOfs[0], s_afSplitBallOfs[1], s_afSplitBallOfs[2]);
                pAdded->SetAttachPos(ofs, 1);
                pAdded->SetHockeyArtsStart();
                pAdded->SetRot(rot);
            }
        }
    }

    long nNode = ArtsOptGetNodeIdx(s_SplitBallNode[SPLIT_BALL_NUM]);
    if (nNode >= 0) {
        pBall->Attach(GetArtsOptMdl(), nNode, 0.0f, 3, 0x58, 0);
        bcs::FVEC3 ofs(s_afSplitBallOfs[0], s_afSplitBallOfs[1], s_afSplitBallOfs[2]);
        pBall->SetAttachPos(ofs, 1);
        GetHangRot(rot);
        pBall->SetRot(rot);
        pBall->SetDisableMarker(1);
    }
}
