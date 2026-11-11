// msw::CCollManHockey: the collision manager of the hockey game (the rink
// and the goals), and the one of its party game. Started from
// src/game/boss/CCollManBoss.cpp by tools/decomp/port_unit.py; the rest is
// HockeyF's own.
#include "game/msw/CCollManHockey.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC4.hpp"
#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"

namespace {

    // The collision manager of the hockey party game (CGame game type 2):
    // inside the court's rectangle with rounded corners, outside the outer
    // field. The class has no name in the module's export table, so it was
    // local to this file; its name here is a guess.
    class CCollManHockeyParty : public msw::CCollMan {
        public:
            CCollManHockeyParty(bcs::CTask *pParent) : CCollMan(pParent) {}

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags);
    };

    // (name guessed) The box a sphere of radius fRadius sweeps from rFrom
    // to rTo.
    inline void GetSweepBox(bcs::FVEC3 &rMin, bcs::FVEC3 &rMax, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                            float fRadius)
    {
        if (rFrom.x <= rTo.x) {
            rMin.x = rFrom.x - fRadius;
            rMax.x = rTo.x + fRadius;
        } else {
            rMin.x = rTo.x - fRadius;
            rMax.x = rFrom.x + fRadius;
        }
        if (rFrom.y <= rTo.y) {
            rMin.y = rFrom.y - fRadius;
            rMax.y = rTo.y + fRadius;
        } else {
            rMin.y = rTo.y - fRadius;
            rMax.y = rFrom.y + fRadius;
        }
        if (rFrom.z <= rTo.z) {
            rMin.z = rFrom.z - fRadius;
            rMax.z = rTo.z + fRadius;
        } else {
            rMin.z = rTo.z - fRadius;
            rMax.z = rFrom.z + fRadius;
        }
    }

    // (name guessed) The bounding box of a goal, two units high: across the
    // goal fHalfWidth to each side of rPos, along rDir from fFront in front
    // of the mouth to fBack behind it.
    inline void GetGoalBox(bcs::FVEC3 &rMin, bcs::FVEC3 &rMax, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir,
                           float fHalfWidth, float fFront, float fBack)
    {
        float fLeft = -fHalfWidth;
        float fBehind = -fBack;
        bcs::FVEC3 corner[4];
        int i;

        // Across the goal is (dir.z, -dir.x).
        corner[0].Set(fLeft * rDir.z + fFront * rDir.x, 0.0f, fFront * rDir.z - fLeft * rDir.x);
        corner[1].Set(fHalfWidth * rDir.z + fFront * rDir.x, 0.0f, fFront * rDir.z - fHalfWidth * rDir.x);
        corner[2].Set(fLeft * rDir.z + fBehind * rDir.x, 0.0f, fBehind * rDir.z - fLeft * rDir.x);
        corner[3].Set(fHalfWidth * rDir.z + fBehind * rDir.x, 0.0f, fBehind * rDir.z - fHalfWidth * rDir.x);

        rMax = corner[0];
        rMin = corner[0];
        for (i = 1; i < 4; i++) {
            if (rMin.x > corner[i].x) {
                rMin.x = corner[i].x;
            }
            if (rMin.z > corner[i].z) {
                rMin.z = corner[i].z;
            }
            if (rMax.x < corner[i].x) {
                rMax.x = corner[i].x;
            }
            if (rMax.z < corner[i].z) {
                rMax.z = corner[i].z;
            }
        }
        rMin.y = 0.0f;
        rMax.y = 2.0f;
        bcs::FVEC3Add(&rMin, &rMin, &rPos);
        bcs::FVEC3Add(&rMax, &rMax, &rPos);
    }


    // (name guessed) Dot product in the ground plane.
    inline float DotXZ(const bcs::FVEC3 &r0, const bcs::FVEC3 &r1)
    {
        return r0.x * r1.x + r0.z * r1.z;
    }
}

msw::CCollMan *msw::CCollManHockey::CreateInstance(bcs::CTask *pParent)
{
    if (CCollMan::m_pInstance == 0) {
        if (CGame::g_GameWork.GetGameType() == 2) {
            CCollMan::m_pInstance = new (0) CCollManHockeyParty(pParent);
        } else {
            CCollMan::m_pInstance = new (0) CCollManHockey(pParent);
        }
    }
    return CCollMan::m_pInstance;
}

msw::CCollManHockey::~CCollManHockey()
{
}

void msw::CCollManHockey::Main()
{
    m_bEnableViewColl = 1;
    CCollMan::Main();
}

void msw::CCollManHockey::Disp()
{
    CCollMan::Disp();
}

// Keeps a sphere of radius fRadius that moved from rFrom to rTo inside the
// rink (a rectangle with rounded corners of the court's radius). The puck
// and the players (nKind 1, 4, 5) also bounce off the goals, and the puck
// (nKind 1) off the extra circle of collision `flags & 0xFF` and off the
// edge of what the camera shows. Bits 24-30 of the flags choose the area:
// 2 the ground of team `flags & 0xFF`, anything else the court. Bit 31 is
// passed on to the CCollMan routines. Returns their results ORed.
int msw::CCollManHockey::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                       const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags)
{
    rPos = rTo;
    rNormal.Set(0.0f, 0.0f, 0.0f);
    int result = 0;
    bcs::FVEC3 from = rFrom;
    bcs::FVEC3 to = rTo;
    bcs::FVEC4 rect;

    switch (nKind) {
        case 1:
        case 4:
        case 5:
            result = DetectGoalHit_(rPos, rNormal, rTime, from, to, fRadius);
            to = rPos;
            break;
    }

    float fCornerRadius = CStageMan::GetInstance()->GetCourtRadius();
    long area = (flags >> 24) & 0x7F;
    unsigned long reflect = flags & 0x80000000;
    long sub = (flags >> 8) & 0xFF;

    if (area == 2) {
        unsigned long team = flags & 0xFF;

        if (team <= 1 && nKind == 1 && sub == 0 && m_bExCollEnable[team] != 0) {
            rPos = to;
            result |= DetectMapHitOuterCircle_(rPos, rNormal, rTime, m_ExCollPos[team], m_fExCollRadius[team], from,
                                               to, fRadius, reflect);
            to = rPos;
        }
        CStageMan::GetInstance()->GetTeamGroundRect(team, &rect.x, &rect.y, &rect.z, &rect.w);
    } else {
        if (nKind == 1 && sub == 0 && m_bExCollEnable[0] != 0) {
            rPos = to;
            result |= DetectMapHitOuterCircle_(rPos, rNormal, rTime, m_ExCollPos[0], m_fExCollRadius[0], from, to,
                                               fRadius, reflect);
            to = rPos;
        }
        CStageMan::GetInstance()->GetCourtRect(&rect.x, &rect.y, &rect.z, &rect.w);

        // The puck stops at the edge of the view when the camera looks along
        // the rink.
        if (m_bEnableViewColl != 0 && m_nViewCollTargetDir >= 0 && nKind == 1 && sub == 0) {
            int bWall = 0;
            bcs::FVEC3 point(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 normal;

            if (m_nViewCollTargetDir != 0) {
                point.z = m_Unk44.z + m_Unk40;
                if (point.z > rect.z) {
                    bWall = 1;
                    normal.Set(0.0f, 0.0f, 1.0f);
                }
            } else {
                point.z = m_Unk44.z - m_Unk40;
                if (point.z < rect.w) {
                    bWall = 1;
                    normal.Set(0.0f, 0.0f, -1.0f);
                }
            }

            if (bWall != 0) {
                bcs::FVEC3 hit;
                float t;

                point.z = normal.z * fRadius + point.z;

                bcs::FVEC3 fromSide(point.x - from.x, 0.0f, point.z - from.z);
                bcs::FVEC3 toSide(point.x - to.x, 0.0f, point.z - to.z);
                float fFromDist = DotXZ(fromSide, normal);
                float fToDist = DotXZ(toSide, normal);

                t = -1.0f;
                hit = to;
                if (!(fFromDist < 0.0f && fToDist < 0.0f)) {
                    if (fFromDist >= 0.0f && fToDist >= 0.0f) {
                        t = 0.0f;
                        hit = from;
                    } else if (fFromDist < 0.0f && fToDist >= 0.0f) {
                        t = GetIntersectionTimeOfFrontPlaneAndLine(hit, normal, point, from, to);
                    }
                }

                if (t >= 0.0f && t <= 1.0f) {
                    if (reflect != 0) {
                        to.x = hit.x;
                        to.z = hit.z;
                    } else {
                        to.z = point.z;
                    }
                    rPos.x = to.x;
                    rPos.z = to.z;
                    rNormal = normal;
                    result = m_nViewCollTargetDir != 0 ? result | 0x108 : result | 0x210;
                }
            }
        }
    }

    return result | DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, fCornerRadius, from, to, fRadius, reflect);
}

// The sweep of a sphere against the frames of the goals, seen from above: a
// goal is a bar across its mouth with a round post at each end, and from
// behind a bar across its back. Returns 0x20 for a hit from behind, 0x40 or
// (goal 1) 0x80 for one from the front, else 0. rPos and rNormal get the
// corrected position and the push direction.
int msw::CCollManHockey::DetectGoalHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                        const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius)
{
    static const float GOAL_F_RADIUS = 0.15f;
    bcs::FVEC3 sweepMax;
    bcs::FVEC3 sweepMin;
    CStageMan *pStage;
    long i;

    GetSweepBox(sweepMin, sweepMax, rFrom, rTo, fRadius);
    pStage = CStageMan::GetInstance();

    for (i = 0; i < CGame::g_GameWork.m_nGoalNum; i++) {
        float fRR = pStage->GetGoalRRRadius(i);
        float fHalfWidth = 0.5f * pStage->GetGoalX1Length(i) + fRR;
        bcs::FVEC3 goalPos = CGame::GetGoalPos(i);
        bcs::FVEC3 goalDir = CGame::GetGoalDir(i);
        bcs::FVEC3 boxMax;
        bcs::FVEC3 boxMin;

        GetGoalBox(boxMin, boxMax, goalPos, goalDir, fHalfWidth, GOAL_F_RADIUS, fRR);
        // Skip the goal unless its box and the sweep's overlap.
        if (sweepMin.z > boxMax.z || sweepMin.x > boxMax.x || sweepMin.y > boxMax.y || boxMin.z > sweepMax.z ||
            boxMin.x > sweepMax.x || boxMin.y > sweepMax.y) {
            continue;
        }

        float fLen = fHalfWidth - fRR;
        float fHitRadius = fRR + fRadius;
        int hit = 0x20;
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        bcs::FVEC3 toGoal;

        bcs::FVEC3Sub(&toGoal, &goalPos, &from);
        if (DotXZ(toGoal, goalDir) <= 0.0f) {
            // In front of the goal: the bar across the mouth.
            fLen = fHalfWidth - GOAL_F_RADIUS;
            fHitRadius = GOAL_F_RADIUS + fRadius;
            hit = 0x40;
            if (i == 1) {
                hit = 0x80;
            }
        }

        bcs::FVEC3 bar0(goalDir.z * fLen + goalPos.x, 0.0f, goalPos.z - goalDir.x * fLen);
        bcs::FVEC3 bar1(goalPos.x - goalDir.z * fLen, 0.0f, goalDir.x * fLen + goalPos.z);
        float fS;
        bcs::FVEC3 closest;
        bcs::FVEC3 closestBar;

        if (GetClosestP_SegmentSegment(from, to, bar0, bar1, rTime, fS, closest, closestBar) <=
            fHitRadius * fHitRadius) {
            closest.x -= closestBar.x;
            closest.z -= closestBar.z;
            bcs::BCS_Vec_Normalize(rNormal, bcs::FVEC3(closest.x, 0.0f, closest.z));
            closest.x = rNormal.x * fHitRadius;
            closest.z = rNormal.z * fHitRadius;
            closest.x += closestBar.x;
            closest.z += closestBar.z;
            rPos = closest;
            rPos.y = rTo.y;
            return hit;
        }
    }
    return 0;
}

// Whether the puck, a sphere of radius fRadius moving from rFrom to rTo,
// goes into a goal. A guardian's box or the posts and the back of the goal
// stop it; then rPos and rNormal get where and the push direction and the
// return is 0x20 (0x40 for a hit on the mouth of a goal other than the
// one m_nArtsExColl names, which also sets rGoal). Returns 0 otherwise.
int msw::CCollManHockey::DetectBallGoalIn_(long &rGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal,
                                           const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius)
{
    // static-order swap: declared in the reverse of the original's order,
    // which gives its bytes (notes/decisions.md).
    static const float GOAL_Y_RANGE = 1.6f;
    static const float GOAL_F_RADIUS = 0.075f;
    CStageMan *pStage = CStageMan::GetInstance();
    bcs::FVEC3 sweepMax;
    bcs::FVEC3 sweepMin;
    long nGoalNum;
    long i;

    GetSweepBox(sweepMin, sweepMax, rFrom, rTo, fRadius);
    nGoalNum = CGame::g_GameWork.m_nGoalNum;

    for (i = 0; i < nGoalNum; i++) {
        float fRR = pStage->GetGoalRRRadius(i);
        float fHalfWidth = 0.5f * pStage->GetGoalX1Length(i) + fRR;
        bcs::FVEC3 goalPos = CGame::GetGoalPos(i);
        bcs::FVEC3 goalDir = CGame::GetGoalDir(i);
        bcs::FVEC3 boxMax;
        bcs::FVEC3 boxMin;

        GetGoalBox(boxMin, boxMax, goalPos, goalDir, fHalfWidth, GOAL_F_RADIUS, fRR);
        // Skip the goal unless its box and the sweep's overlap.
        if (sweepMin.z > boxMax.z || sweepMin.x > boxMax.x || sweepMin.y > boxMax.y || boxMin.z > sweepMax.z ||
            boxMin.x > sweepMax.x || boxMin.y > sweepMax.y) {
            continue;
        }

        bcs::FVEC3 fromSide(goalPos.x - rFrom.x, 0.0f, goalPos.z - rFrom.z);
        bcs::FVEC3 toSide(goalPos.x - rTo.x, 0.0f, goalPos.z - rTo.z);
        float fFromDist = DotXZ(fromSide, goalDir);
        float fToDist = DotXZ(toSide, goalDir);

        // Through the mouth, from the front.
        if (fFromDist <= 0.0f && fToDist >= 0.0f) {
            GOAL_GUARDIAN &rGuardian = m_aGuardian[i];

            if (rGuardian.m_bEnable != 0) {
                bcs::FVEC3 from = rFrom;
                bcs::FVEC3 to = rTo;
                bcs::FVEC3 pos = rGuardian.m_Pos;
                bcs::FVEC3 size = rGuardian.m_Size;
                float fRotY = rGuardian.m_fRotY;
                bcs::FVEC3 hit;

                if (COLL::DetectHitPos_CapsuleCuboid(from, to, fRadius, pos, size, fRotY, hit)) {
                    rGoal = i;
                    rPos = hit;
                    bcs::BCS_VecOneZ_RotY(rNormal, fRotY);
                    return 0x20;
                }
            }

            if (rFrom.y - goalPos.y < GOAL_Y_RANGE && rTo.y - goalPos.y < GOAL_Y_RANGE) {
                bcs::FVEC3 from = rFrom;
                bcs::FVEC3 to = rTo;
                bcs::FVEC3 hit;
                float t;

                from.y = 0.0f < from.y ? from.y : 0.0f;
                to.y = 0.0f < to.y ? to.y : 0.0f;
                t = GetIntersectionTimeOfFrontPlaneAndLine(hit, goalDir, goalPos, from, to);
                if (t >= 0.0f && t <= 1.0f) {
                    bcs::FVEC3 hitXZ(hit.x, 0.0f, hit.z);
                    bcs::FVEC3 goalXZ(goalPos.x, 0.0f, goalPos.z);

                    if (bcs::FVEC3DistSq(&hitXZ, &goalXZ) < (fHalfWidth - fRadius) * (fHalfWidth - fRadius) &&
                        hit.y - goalPos.y < GOAL_Y_RANGE) {
                        rGoal = i;
                        rPos = hit;
                        rNormal = goalDir;
                        if (i != m_nArtsExColl) {
                            return 0x40;
                        }
                        return 0x20;
                    }
                }
            }
        }

        // From the front: the two posts.
        if (fFromDist <= 0.0f) {
            float fLen = fHalfWidth - GOAL_F_RADIUS;
            float fHitRadius = GOAL_F_RADIUS + fRadius;
            bcs::FVEC3 from(rFrom.x, goalPos.y, rFrom.z);
            bcs::FVEC3 to(rTo.x, goalPos.y, rTo.z);
            bcs::FVEC3 post[2];
            int j;

            post[0].Set(goalDir.z * fLen + goalPos.x, goalPos.y, goalPos.z - goalDir.x * fLen);
            post[1].Set(goalPos.x - goalDir.z * fLen, goalPos.y, goalDir.x * fLen + goalPos.z);

            for (j = 0; j < 2; j++) {
                bcs::FVEC3 center = post[j];
                bcs::FVEC3 hit;
                float t = GetIntersectionTimeOfSegmentSphere(hit, from, to, center, fHitRadius);

                if (t >= 0.0f && t <= 1.0f) {
                    bcs::FVEC3 dir;

                    dir.x = hit.x - center.x;
                    dir.z = hit.z - center.z;
                    bcs::BCS_Vec_Normalize(rNormal, bcs::FVEC3(dir.x, 0.0f, dir.z));
                    dir.x = hit.x - center.x;
                    dir.z = hit.z - center.z;
                    if (dir.x * dir.x + dir.z * dir.z < fHitRadius * fHitRadius) {
                        hit.x = rNormal.x * fHitRadius;
                        hit.z = rNormal.z * fHitRadius;
                        hit.x += center.x;
                        hit.z += center.z;
                    }

                    dir.x = to.x - from.x;
                    dir.z = to.z - from.z;
                    if (DotXZ(rNormal, dir) <= 0.0f) {
                        rPos = hit;
                        rPos.y = rTo.y;
                        return 0x20;
                    }
                }
            }
        }

        // From behind: the back of the goal.
        if (fFromDist >= 0.0f) {
            float fLen = fHalfWidth - fRR;
            float fHitRadius = fRR + fRadius;
            bcs::FVEC3 from(rFrom.x, goalPos.y, rFrom.z);
            bcs::FVEC3 to(rTo.x, goalPos.y, rTo.z);
            bcs::FVEC3 bar0(goalDir.z * fLen + goalPos.x, goalPos.y, goalPos.z - goalDir.x * fLen);
            bcs::FVEC3 bar1(goalPos.x - goalDir.z * fLen, goalPos.y, goalDir.x * fLen + goalPos.z);
            float fT;
            float fS;
            bcs::FVEC3 closest;
            bcs::FVEC3 closestBar;

            if (GetClosestP_SegmentSegment(from, to, bar0, bar1, fT, fS, closest, closestBar) <
                fHitRadius * fHitRadius) {
                bcs::FVEC3 normal(0.0f, 0.0f, 0.0f);

                closest.x -= closestBar.x;
                closest.z -= closestBar.z;
                bcs::BCS_Vec_Normalize(normal, bcs::FVEC3(closest.x, 0.0f, closest.z));
                // A little further out than touching.
                float fPush = 0.02f + fHitRadius;
                closest.x = normal.x * fPush;
                closest.z = normal.z * fPush;
                closest.x += closestBar.x;
                closest.z += closestBar.z;

                closestBar.x = to.x - from.x;
                closestBar.z = to.z - from.z;
                bcs::FVEC3 move(closestBar.x, 0.0f, closestBar.z);
                if (bcs::FVEC3LenSq(&move) <= 0.0001f) {
                    closestBar.Set(0.0f, 0.0f, 0.0f);
                }
                if (DotXZ(normal, closestBar) <= 0.0f) {
                    rPos = closest;
                    rPos.y = rTo.y;
                    rNormal = normal;
                    return 0x20;
                }
            }
        }
    }
    return 0;
}

// Whether the puck, a sphere of radius fRadius moving from rFrom to rTo,
// leaves goal rGoal: through its back or past a post it bounces (0x40, with
// rPos and rNormal), through the mouth it is out (0x20). Returns 0
// otherwise.
int msw::CCollManHockey::DetectBallGoalOut_(long &rGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal,
                                            const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius)
{
    // static-order swap: declared in the reverse of the original's order,
    // which gives its bytes (notes/decisions.md).
    static const float GOAL_Y_RANGE = 1.6f;
    static const float GOAL_F_RADIUS = 0.075f;
    int result = 0;
    bcs::FVEC3 from = rFrom;
    bcs::FVEC3 to = rTo;
    bcs::FVEC3 goalPos = CGame::GetGoalPos(rGoal);
    bcs::FVEC3 goalDir = CGame::GetGoalDir(rGoal);
    CStageMan *pStage = CStageMan::GetInstance();

    to.y = goalPos.y;
    from.y = goalPos.y;

    float fRR = pStage->GetGoalRRRadius(rGoal);
    float fHalfWidth = 0.5f * pStage->GetGoalX1Length(rGoal) + fRR;
    bcs::FVEC3 fromSide;
    bcs::FVEC3 toSide;

    fromSide.x = goalPos.x - rFrom.x;
    fromSide.z = goalPos.z - rFrom.z;
    toSide.x = goalPos.x - rTo.x;
    toSide.z = goalPos.z - rTo.z;
    float fFromDist = DotXZ(fromSide, goalDir);
    float fToDist = DotXZ(toSide, goalDir);

    // From inside: the back of the goal.
    if (fFromDist >= 0.0f) {
        float fLen = fHalfWidth - fRR;
        float fLimit = fRR - GOAL_F_RADIUS - fRadius;
        bcs::FVEC3 bar0(goalPos.x - fLen, goalPos.y, goalPos.z);
        bcs::FVEC3 bar1(goalPos.x + fLen, goalPos.y, goalPos.z);
        float fT;
        float fS;
        bcs::FVEC3 closest;
        bcs::FVEC3 closestBar;

        if (GetClosestP_SegmentSegment(to, from, bar0, bar1, fT, fS, closest, closestBar) >= fLimit * fLimit) {
            bcs::FVEC3 dir;

            dir.x = closest.x - closestBar.x;
            dir.z = closest.z - closestBar.z;
            bcs::BCS_Vec_Normalize(rNormal, bcs::FVEC3(dir.x, 0.0f, dir.z));
            dir.z = closest.z - closestBar.z;
            dir.x = closest.x - closestBar.x;
            if (dir.x * dir.x + dir.z * dir.z > fLimit * fLimit) {
                closest.x = rNormal.x * fLimit;
                closest.z = rNormal.z * fLimit;
                closest.x += closestBar.x;
                closest.z += closestBar.z;
            }
            rNormal.x *= -1.0f;
            rNormal.z *= -1.0f;

            dir.z = to.z - from.z;
            dir.x = to.x - from.x;
            if (DotXZ(rNormal, dir) <= 0.0f) {
                result = 0x40;
                rPos = closest;
                rPos.y = rTo.y;
                to.Set(closest.x, rTo.y, closest.z);
            }
        }
    }

    // The two posts.
    {
        float fHitRadius = GOAL_F_RADIUS + fRadius;
        float fHitRadiusSq = fHitRadius * fHitRadius;
        bcs::FVEC3 post[2];
        int j;

        post[0].Set(goalPos.x - fHalfWidth, goalPos.y, goalPos.z);
        post[1].Set(goalPos.x + fHalfWidth, goalPos.y, goalPos.z);

        for (j = 0; j < 2; j++) {
            bcs::FVEC3 center = post[j];
            float fT;
            bcs::FVEC3 closest;

            if (GetClosestP_PointSegment(center, from, to, fT, closest) <= fHitRadiusSq) {
                bcs::FVEC3 dir;

                dir.x = closest.x - center.x;
                dir.z = closest.z - center.z;
                bcs::BCS_Vec_Normalize(rNormal, bcs::FVEC3(dir.x, 0.0f, dir.z));
                dir.z = closest.z - center.z;
                dir.x = closest.x - center.x;
                if (dir.x * dir.x + dir.z * dir.z < fHitRadius * fHitRadius) {
                    closest.x = rNormal.x * fHitRadius;
                    closest.z = rNormal.z * fHitRadius;
                    closest.x += center.x;
                    closest.z += center.z;
                }

                dir.x = to.x - from.x;
                dir.z = to.z - from.z;
                if (DotXZ(rNormal, dir) <= 0.0f) {
                    result = 0x40;
                    rPos = closest;
                    rPos.y = rTo.y;
                    to.Set(closest.x, rTo.y, closest.z);
                    break;
                }
            }
        }
    }

    // Out through the mouth.
    if (fFromDist >= 0.0f && fToDist <= 0.0f) {
        bcs::FVEC3 hit;
        float t = GetIntersectionTimeOfFrontPlaneAndLine(hit, goalDir, goalPos, from, to);

        if (t >= 0.0f && t <= 1.0f && __fabsf(hit.x - goalPos.x) < fHalfWidth - fRadius &&
            hit.y - goalPos.y < GOAL_Y_RANGE) {
            result = 0x20;
        }
    }
    return result;
}

void msw::CCollManHockey::EnableGoalGuardian_(long n, int bEnable)
{
    m_aGuardian[n].m_bEnable = bEnable;
}

void msw::CCollManHockey::SetGoalGuardianPos_(long n, const bcs::FVEC3 &rPos)
{
    m_aGuardian[n].m_Pos = rPos;
}

void msw::CCollManHockey::SetGoalGuardianRotY_(long n, float fRotY)
{
    while (fRotY > 6.2831855f) {
        fRotY -= 6.2831855f;
    }
    while (fRotY < 0.0f) {
        fRotY += 6.2831855f;
    }
    m_aGuardian[n].m_fRotY = fRotY;
}

void msw::CCollManHockey::SetGoalGuardianSize_(long n, const bcs::FVEC3 &rSize)
{
    m_aGuardian[n].m_Size = rSize;
}

// Keeps a sphere inside the court (bits 24-30 of the flags 2: the ground of
// team `flags & 0xFF`), whose corners are rounded with the court's radius,
// and out of the outer field when the stage has one. Nothing for nKind 4.
// Bit 16 is set in the result when either test hit.
int CCollManHockeyParty::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                       const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags)
{
    rPos = rTo;
    rNormal.Set(0.0f, 0.0f, 0.0f);
    int result = 0;
    bcs::FVEC3 from = rFrom;
    bcs::FVEC3 to = rTo;
    bcs::FVEC4 rect;

    if (nKind == 4) {
        return 0;
    }

    msw::CStageMan *pStage = msw::CStageMan::GetInstance();
    float fCornerRadius = pStage->GetCourtRadius();
    long area = (flags >> 24) & 0x7F;
    unsigned long reflect = flags & 0x80000000;
    if (area == 2) {
        pStage->GetTeamGroundRect(flags & 0xFF, &rect.x, &rect.y, &rect.z, &rect.w);
    } else {
        pStage->GetCourtRect(&rect.x, &rect.y, &rect.z, &rect.w);
    }

    // The rounded corners grow with the sphere.
    fCornerRadius += fRadius;
    rect.x -= fRadius;
    rect.y += fRadius;
    rect.z -= fRadius;
    rect.w += fRadius;
    int hit = DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, fCornerRadius, from, to, fRadius, reflect);
    if (hit != 0) {
        result = hit | 0x10000;
    }

    float fOuterRadius = pStage->GetOuterFieldRadius();
    if (fOuterRadius >= 0.0f) {
        pStage->GetOuterFieldRect(&rect.x, &rect.y, &rect.z, &rect.w);
        fOuterRadius -= fRadius;
        rect.x += fRadius;
        rect.y -= fRadius;
        rect.z += fRadius;
        rect.w -= fRadius;
        hit = DetectMapHitOuterRgn_(rPos, rNormal, rTime, rect, fOuterRadius, from, to, fRadius, reflect);
        if (hit != 0) {
            result = result | hit | 0x10000;
        }
    }
    return result;
}
