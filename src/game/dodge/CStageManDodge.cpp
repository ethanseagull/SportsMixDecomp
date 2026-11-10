// msw::CStageManDodge: the stage manager of the dodge game, base of every
// dodge stage.
#include "game/msw/CStageManDodge.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageFactoryDodge.hpp"

msw::CStageManDodge::CStageManDodge(bcs::CTask *pParent) : CStageMan(pParent)
{
    for (int i = 0; i < 2; i++) {
        // The two goals face each other across the court.
        m_aGoalPos[i].z = (i == 0) ? -GetBaseCourtDepth_() : GetBaseCourtDepth_();
        m_aGoalRot[i] = bcs::FVEC3(0.0f, (i == 0) ? 3.1415927f : 0.0f, 0.0f);
        bcs::BCS_VecZ_RotY(m_aGoalVec[i], 1.0f, m_aGoalRot[i].y);
    }
}

msw::CStageManDodge::~CStageManDodge()
{
}

msw::CStageManDodge *msw::CStageManDodge::CreateInstance(bcs::CTask *pParent, long stageNo)
{
    CStageManDodge *pStageMan = CStageFactoryDodge::Create(stageNo, pParent);

    pStageMan->SetStageNo(stageNo);
    return pStageMan;
}

void msw::CStageManDodge::SendMessage(long msg, long a, long type, long b)
{
    CStageMan::SendMessage(msg, a, type, b);

    switch (msg) {
        case 3:
        case 0x13:
            PlayAnim_AudienceFlash(1, -1.0f);
            InitAnim_AudienceFlag();
            break;
        case 5:
            InitAnim_AudienceFlag();
            break;
        case 0x3F:
            PlayAnim_AudienceFlash(2, 120.0f);
            break;
        case 0x42: {
            long teamA = GetTeamOf(a);
            long teamB = GetTeamOf(b);

            if (IsTeamHitType(type) && (teamB < 0 || teamB != teamA)) {
                PlayAnim_AudienceFlash(2, 120.0f);
            } else if (type == 5 && teamA != teamB) {
                PlayAnim_AudienceFlash(2, 120.0f);
            }
            break;
        }
    }

    switch (msg) {
        case 3:
        case 0x13:
            OnSoundEvent(0);
            break;
        case 0x3F:
            if (type == 9) {
                OnSoundEvent(5);
            } else {
                OnSoundEvent(4);
            }
            break;
        case 0x40:
            OnSoundEvent(6);
            break;
        case 0x42: {
            long teamA = GetTeamOf(a);
            long teamB = GetTeamOf(b);

            if (IsTeamHitType(type) && (teamB < 0 || teamB != teamA)) {
                if (type == 0) {
                    OnSoundEvent(2);
                } else {
                    OnSoundEvent(3);
                }
            } else if (type == 5 && teamA != teamB) {
                OnSoundEvent(1);
            }
            break;
        }
    }
}

float msw::CStageManDodge::GetBaseCourtWidth_() const
{
    return 9.5f;
}

float msw::CStageManDodge::GetBaseCourtDepth_() const
{
    return 9.5f;
}

float msw::CStageManDodge::GetBaseGroundWidth_() const
{
    return 1.3f;
}

float msw::CStageManDodge::GetBaseGroundDepth_() const
{
    return 1.3f;
}

float msw::CStageManDodge::GetBaseOuterWidth_() const
{
    return 1.6f;
}

float msw::CStageManDodge::GetBaseOuterDepth_() const
{
    return 1.6f;
}

float msw::CStageManDodge::GetBaseOuterFront_() const
{
    return 2.3f;
}

// The outer field of a team is the one behind the other team's court.
float msw::CStageManDodge::GetTeamOuterFieldLeft(long team, long n) const
{
    return CStageMan::GetTeamOuterFieldLeft(team ^ 1, n);
}

float msw::CStageManDodge::GetTeamOuterFieldRight(long team, long n) const
{
    return CStageMan::GetTeamOuterFieldRight(team ^ 1, n);
}

float msw::CStageManDodge::GetTeamOuterFieldFront(long team, long n) const
{
    return CStageMan::GetTeamOuterFieldFront(team ^ 1, n);
}

float msw::CStageManDodge::GetTeamOuterFieldBack(long team, long n) const
{
    return CStageMan::GetTeamOuterFieldBack(team ^ 1, n);
}

// A position outside both courts that lies on a team's ground belongs to the
// other team.
long msw::CStageManDodge::GetTeamIdxByPos(const bcs::FVEC3 &rPos) const
{
    static const long teamNum = 2;

    long team = CStageMan::GetTeamIdxByPos(rPos);
    if (team >= 0) {
        return team;
    }

    for (long i = 0; i < teamNum; i++) {
        if (GetTeamGroundLeft(i) <= rPos.x && rPos.x <= GetTeamGroundRight(i) && GetTeamGroundFront(i) <= rPos.z &&
            rPos.z <= GetTeamGroundBack(i)) {
            return i ^ 1;
        }
    }
    return -1;
}

long msw::CStageManDodge::GetQPanelEnabled_() const
{
    return (CGame::g_GameWork.m_nFlag18 & 0x8000) ? 0 : 0xFFFF;
}

void msw::CStageManDodge::OnPostInitialize()
{
}

void msw::CStageManDodge::Disp3D(long)
{
}
