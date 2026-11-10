// msw::CCollManVolley: the collision manager of the volley game.
#include "game/msw/CCollManVolley.hpp"

#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"

// The flags argument of DetectMapHit_: the low byte is a team, bits 24-30
// say which rectangle of the stage to collide with (values below), and
// the top bit is handed on to CCollMan::DetectMapHitInnerRgn_ as it is.
// (meanings read from the code; names guessed)
#define MAPHIT_TEAM(flags) ((long)((flags) & 0xFF))
#define MAPHIT_AREA(flags) ((long)(((flags) >> 24) & 0x7F))
#define MAPHIT_TOP_BIT(flags) ((flags) & 0x80000000)

namespace {

    enum {
        MAPHIT_AREA_TEAM_OUTER_FIELD = 2, // the team's half of the outer field
        MAPHIT_AREA_TEAM_BEHIND = 3,      // from the team's court to the outer field, on the far side
        MAPHIT_AREA_TEAM_COURT = 4        // the team's court
        // anything else: the whole ground
    };
}

namespace msw {

    namespace {

        // (name guessed) The collision manager of game type 2, guessed to be
        // the party game. It is DetectMapHit_ of CCollManVolley with team 0
        // instead of the team in the flags. The class has no name in the
        // module (its functions and vtable are not exported), so it was most
        // likely local to this file.
        class CCollManVolleyParty : public CCollMan {
            public:
                CCollManVolleyParty(bcs::CTask *pParent) : CCollMan(pParent) {}

                virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                          const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius, long,
                                          unsigned long flags);
        };
    }
}

msw::CCollMan *msw::CCollManVolley::CreateInstance(bcs::CTask *pParent)
{
    if (CCollMan::m_pInstance == 0) {
        if (CGame::g_GameWork.GetGameType() == 2) {
            CCollMan::m_pInstance = new (0) CCollManVolleyParty(pParent);
        } else {
            CCollMan::m_pInstance = new (0) CCollManVolley(pParent);
        }
    }
    return CCollMan::m_pInstance;
}

msw::CCollManVolley::~CCollManVolley()
{
}

void msw::CCollManVolley::Main()
{
    CCollMan::Main();
}

void msw::CCollManVolley::Disp()
{
    CCollMan::Disp();
}

// Picks the rectangle of the stage that the flags ask for and collides with
// it. rPos starts as rTo and rNormal as zero.
int msw::CCollManVolley::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                       const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius, long,
                                       unsigned long flags)
{
    float fOuterFront;
    bcs::FVEC4 rgn;
    float fOuterBack;

    float fZero = 0.0f;
    const unsigned long topBit = MAPHIT_TOP_BIT(flags);

    rPos = rTo;
    rNormal.Set(fZero, fZero, fZero);

    switch (MAPHIT_AREA(flags)) {
        case MAPHIT_AREA_TEAM_OUTER_FIELD:
            CStageMan::GetInstance()->GetTeamOuterFieldRect(MAPHIT_TEAM(flags), &rgn.x, &rgn.y, &rgn.z, &rgn.w, 0);
            break;
        case MAPHIT_AREA_TEAM_BEHIND:
            CStageMan::GetInstance()->GetTeamCourtRect(MAPHIT_TEAM(flags), &rgn.x, &rgn.y, &rgn.z, &rgn.w);
            CStageMan::GetInstance()->GetTeamOuterFieldRect(MAPHIT_TEAM(flags), 0, 0, &fOuterFront, &fOuterBack, 0);
            // Keep the court's edge that is further from the centre line and
            // extend the area from it to the outer field on that side.
            if (__fabs(rgn.z) > __fabs(rgn.w)) {
                rgn.w = rgn.z;
                rgn.z = fOuterFront;
            } else {
                rgn.z = rgn.w;
                rgn.w = fOuterBack;
            }
            break;
        case MAPHIT_AREA_TEAM_COURT:
            CStageMan::GetInstance()->GetTeamCourtRect(MAPHIT_TEAM(flags), &rgn.x, &rgn.y, &rgn.z, &rgn.w);
            break;
        default:
            CStageMan::GetInstance()->GetGroundRect(&rgn.x, &rgn.y, &rgn.z, &rgn.w);
            break;
    }

    return DetectMapHitInnerRgn_(rPos, rNormal, rTime, rgn, fZero, rFrom, rTo, fRadius, topBit);
}

// The same with team 0 for every area.
int msw::CCollManVolleyParty::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                            const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius, long,
                                            unsigned long flags)
{
    bcs::FVEC4 rgn;
    float fOuterFront;
    float fOuterBack;

    float fZero = 0.0f;
    const unsigned long topBit = MAPHIT_TOP_BIT(flags);

    rPos = rTo;
    rNormal.Set(fZero, fZero, fZero);

    switch (MAPHIT_AREA(flags)) {
        case MAPHIT_AREA_TEAM_OUTER_FIELD:
            CStageMan::GetInstance()->GetTeamOuterFieldRect(0, &rgn.x, &rgn.y, &rgn.z, &rgn.w, 0);
            break;
        case MAPHIT_AREA_TEAM_BEHIND:
            CStageMan::GetInstance()->GetTeamCourtRect(0, &rgn.x, &rgn.y, &rgn.z, &rgn.w);
            CStageMan::GetInstance()->GetTeamOuterFieldRect(0, 0, 0, &fOuterFront, &fOuterBack, 0);
            if (__fabs(rgn.z) > __fabs(rgn.w)) {
                rgn.w = rgn.z;
                rgn.z = fOuterFront;
            } else {
                rgn.z = rgn.w;
                rgn.w = fOuterBack;
            }
            break;
        case MAPHIT_AREA_TEAM_COURT:
            CStageMan::GetInstance()->GetTeamCourtRect(0, &rgn.x, &rgn.y, &rgn.z, &rgn.w);
            break;
        default:
            CStageMan::GetInstance()->GetGroundRect(&rgn.x, &rgn.y, &rgn.z, &rgn.w);
            break;
    }

    return DetectMapHitInnerRgn_(rPos, rNormal, rTime, rgn, fZero, rFrom, rTo, fRadius, topBit);
}
