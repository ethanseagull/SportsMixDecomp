// msw::CCollManDodge: the collision manager of the dodge game, and the one
// of its party game.
#include "game/msw/CCollManDodge.hpp"

#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"

namespace CCollManDodge_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float constant of this
// file with its own lis, which this compiler does when the first .rodata object of the file is
// weak (a function-local const). The module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

namespace {

    // The collision manager of the dodge party game (CGame game type 2):
    // everything is kept inside the court's rectangle. The class has no name
    // in the module's export table, so it was local to this file; its name
    // here is a guess.
    class CCollManDodgeParty : public msw::CCollMan {
        public:
            CCollManDodgeParty(bcs::CTask *pParent) : CCollMan(pParent) {}

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags);
    };
}

msw::CCollMan *msw::CCollManDodge::CreateInstance(bcs::CTask *pParent)
{
    if (CCollMan::m_pInstance == 0) {
        if (CGame::g_GameWork.GetGameType() == 2) {
            CCollMan::m_pInstance = new (0) CCollManDodgeParty(pParent);
        } else {
            CCollMan::m_pInstance = new (0) CCollManDodge(pParent);
        }
    }
    return CCollMan::m_pInstance;
}

msw::CCollManDodge::~CCollManDodge()
{
}

void msw::CCollManDodge::Main()
{
    CCollMan::Main();
}

void msw::CCollManDodge::Disp()
{
    CCollMan::Disp();
}

// The extra collision sphere of goal `idx` (flags bits 0-7), tested only
// for a ball (nKind 1) that a team holds (flags bits 16-23, team + 1) and
// only when the sphere is enabled: the goal that team attacks keeps the
// ball out, the other one keeps it in. The result goes to rResult, which
// is left alone when the sphere is not tested.
inline void msw::CCollManDodge::DetectExColl_(int &rResult, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                              const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius,
                                              long nKind, unsigned long flags)
{
    long idx = flags & 0xFF;

    if (idx >= 0 && idx < 2 && nKind == 1 && (long)((flags >> 16) & 0xFF) > 0 && m_bExCollEnable[idx] != 0) {
        if (idx == CGame::GetTargetGoalIdx(((flags >> 16) & 0xFF) - 1)) {
            rResult = DetectMapHitInnerCircle_(rPos, rNormal, rTime, m_ExCollPos[idx], m_fExCollRadius[idx], rFrom,
                                               rTo, fRadius, flags & 0x80000000);
        } else {
            rResult = DetectMapHitOuterCircle_(rPos, rNormal, rTime, m_ExCollPos[idx], m_fExCollRadius[idx], rFrom,
                                               rTo, fRadius, flags & 0x80000000);
        }
    }
}

// Keeps a sphere that moves from rFrom to rTo inside the area it may be in.
// Bits 24-30 of the flags say which area:
//   2: the court of team `flags & 0xFF` (its inner area when bits 8-15 are
//      set), widened a little at the centre line;
//   3: the outer field of that team (bits 8-15 select which part); for part
//      0 the sphere is also kept out of the field next to it;
//   1: both team courts together; unless they form one rectangle (or nKind
//      is 2), the sphere is also kept out of the space around and between
//      them;
//   anything else: the whole ground.
// Bit 31 is passed on to the CCollMan routines. Returns their results ORed.
int msw::CCollManDodge::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                     const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags)
{
    bcs::FVEC4 rect;
    unsigned long reflect = flags & 0x80000000;
    int result = 0;
    // A variable in the original too (a guess from the code): this zero
    // stays in a register across the calls, while the literal 0.0f passed
    // further down is loaded again each time.
    float zero = 0.0f;
    long area = (flags >> 24) & 0x7F;

    rPos = rTo;
    rNormal.Set(zero, zero, zero);

    switch (area) {
        case 2: {
            long team = flags & 0xFF;
            long inner = (flags >> 8) & 0xFF;

            DetectExColl_(result, rPos, rNormal, rTime, rFrom, rTo, fRadius, nKind, flags);
            if (inner == 0) {
                CStageMan::GetInstance()->GetTeamCourtRect(team, &rect.x, &rect.y, &rect.z, &rect.w);
            } else {
                CStageMan::GetInstance()->GetTeamCourtInnerAreaRect(team, &rect.x, &rect.y, &rect.z, &rect.w);
            }
            if (team == 0) {
                rect.z += 0.15f;
            } else {
                rect.w -= 0.15f;
            }
            break;
        }
        case 3: {
            long part = (flags >> 8) & 0xFF;
            long team = flags & 0xFF;
            int inside;
            bcs::FVEC4 outside;

            CStageMan::GetInstance()->GetTeamOuterFieldRect(team, &rect.x, &rect.y, &rect.z, &rect.w, part);
            if (part != 0) {
                break;
            }
            inside = DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, reflect);

            // The part of the outer field next to part 0; which edge each
            // call returns depends on the names in CStageMan.hpp, which are
            // guesses.
            outside.x = CStageMan::GetInstance()->GetTeamOuterFieldRight(team, 3);
            outside.y = CStageMan::GetInstance()->GetTeamOuterFieldLeft(team, 2);
            if (1 - team == 0) { // the other team is team 0
                outside.w = CStageMan::GetInstance()->GetTeamOuterFieldFront(team, 1);
                outside.z = -100.0f;
            } else {
                outside.z = CStageMan::GetInstance()->GetTeamOuterFieldBack(team, 1);
                outside.w = 100.0f;
            }
            if (outside.x <= rect.x) {
                outside.x -= 0.5f * __fabsf(rect.y - rect.x);
            }
            if (outside.y >= rect.y) {
                outside.y += 0.5f * __fabsf(rect.y - rect.x);
            }
            if (outside.z <= rect.z) {
                outside.z -= 0.5f * __fabsf(rect.w - rect.z);
            }
            if (outside.w >= rect.w) {
                outside.w += 0.5f * __fabsf(rect.w - rect.z);
            }
            return inside | DetectMapHitOuterRgn_(rPos, rNormal, rTime, outside, zero, rFrom, rPos, fRadius, reflect);
        }
        case 1: {
            bcs::FVEC4 court[2];
            float t;
            float gap;
            float depth;

            DetectExColl_(result, rPos, rNormal, rTime, rFrom, rTo, fRadius, nKind, flags);
            if (((flags >> 8) & 0xFF) == 0) {
                CStageMan::GetInstance()->GetTeamCourtRect(0, &court[0].x, &court[0].y, &court[0].z, &court[0].w);
                CStageMan::GetInstance()->GetTeamCourtRect(1, &court[1].x, &court[1].y, &court[1].z, &court[1].w);
            } else {
                CStageMan::GetInstance()->GetTeamCourtInnerAreaRect(0, &court[0].x, &court[0].y, &court[0].z,
                                                                   &court[0].w);
                CStageMan::GetInstance()->GetTeamCourtInnerAreaRect(1, &court[1].x, &court[1].y, &court[1].z,
                                                                   &court[1].w);
            }

            // Edges in increasing order, the court with the smaller front first.
            if (court[0].x > court[0].y) {
                t = court[0].x;
                court[0].x = court[0].y;
                court[0].y = t;
            }
            if (court[0].z > court[0].w) {
                t = court[0].z;
                court[0].z = court[0].w;
                court[0].w = t;
            }
            if (court[1].x > court[1].y) {
                t = court[1].x;
                court[1].x = court[1].y;
                court[1].y = t;
            }
            if (court[1].z > court[1].w) {
                t = court[1].z;
                court[1].z = court[1].w;
                court[1].w = t;
            }
            if (court[0].z > court[1].z) {
                bcs::FVEC4 tmp = court[0];
                court[0] = court[1];
                court[1] = tmp;
            }

            // The rectangle around both.
            rect.x = court[0].x > court[1].x ? court[1].x : court[0].x;
            rect.y = court[0].y < court[1].y ? court[1].y : court[0].y;
            rect.z = court[0].z > court[1].z ? court[1].z : court[0].z;
            rect.w = court[0].w < court[1].w ? court[1].w : court[0].w;

            if (nKind == 2) {
                break;
            }
            if (court[0].x == court[1].x && court[0].y == court[1].y &&
                (court[0].z == court[1].w || court[0].w == court[1].z)) {
                break;
            }

            // Note: this replaces what DetectExColl_ returned.
            result = DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, reflect);

            // Keep out of the space beside the narrower court.
            if (rect.x < court[0].x) {
                bcs::FVEC4 area(rect.x - 100.0f, court[0].x, court[0].z - 100.0f,
                                court[0].w < court[1].z ? court[1].z : court[0].w);
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, area, 0.0f, rFrom, rPos, fRadius, reflect);
            }
            if (rect.y > court[0].y) {
                bcs::FVEC4 area(court[0].y, 100.0f + rect.y, court[0].z - 100.0f,
                                court[0].w < court[1].z ? court[1].z : court[0].w);
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, area, 0.0f, rFrom, rPos, fRadius, reflect);
            }
            if (rect.x < court[1].x) {
                bcs::FVEC4 area(rect.x - 100.0f, court[1].x, court[0].w > court[1].z ? court[1].z : court[0].w,
                                100.0f + court[1].w);
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, area, 0.0f, rFrom, rPos, fRadius, reflect);
            }
            if (rect.y > court[1].y) {
                bcs::FVEC4 area(court[1].y, 100.0f + rect.y, court[0].w > court[1].z ? court[1].z : court[0].w,
                                100.0f + court[1].w);
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, area, 0.0f, rFrom, rPos, fRadius, reflect);
            }

            // And out of the gap between the two courts, if there is one.
            gap = court[1].z - court[0].w;
            depth = gap - 0.1f;
            if (depth > rPos.y) {
                depth = gap;
            } else {
                depth -= rPos.y;
            }
            if (depth > 0.0f) {
                bcs::FVEC4 area;
                float center = 0.5f * (court[0].w + court[1].z);
                area.x = rect.x;
                area.y = rect.y;
                area.z = center - 0.5f * depth;
                area.w = center + 0.5f * depth;
                result |= DetectMapHitOuterRgnZ_(rPos, rNormal, rTime, area, 0.0f, rFrom, rPos, fRadius, reflect);
            }
            return result;
        }
        default:
            CStageMan::GetInstance()->GetGroundRect(&rect.x, &rect.y, &rect.z, &rect.w);
            break;
    }

    rect.x += 0.01f;
    rect.y -= 0.01f;
    rect.z += 0.01f;
    rect.w -= 0.01f;
    return result | DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, reflect);
}

int CCollManDodgeParty::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long, unsigned long flags)
{
    bcs::FVEC4 rect;
    int result = 0;
    float zero = 0.0f;

    rPos = rTo;
    rNormal.Set(zero, zero, zero);
    msw::CStageMan::GetInstance()->GetCourtRect(&rect.x, &rect.y, &rect.z, &rect.w);
    return result | DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, flags & 0x80000000);
}
