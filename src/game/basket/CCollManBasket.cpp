// msw::CCollManBasket: the collision manager of the basket game.
#include "game/msw/CCollManBasket.hpp"

#include "bcs/New.hpp"
#include "game/msw/CStageMan.hpp"

msw::CCollMan *msw::CCollManBasket::CreateInstance(bcs::CTask *pParent)
{
    if (CCollMan::m_pInstance == 0) {
        CCollMan::m_pInstance = new (0x100) CCollManBasket(pParent);
    }
    return CCollMan::m_pInstance;
}

msw::CCollManBasket::~CCollManBasket()
{
}

void msw::CCollManBasket::Main()
{
    m_bEnableViewColl = 1;
    CCollMan::Main();
}

void msw::CCollManBasket::Disp()
{
    CCollMan::Disp();
}

// (names guessed) The smaller and the larger of two values.
template <typename T> static inline T Min(T a, T b)
{
    return a > b ? b : a;
}

template <typename T> static inline T Max(T a, T b)
{
    return a < b ? b : a;
}

// (name guessed) Orders a rectangle's edges: left <= right, front <= back.
static inline void SortRect(bcs::FVEC4 &rRect)
{
    float t;

    if (rRect.x > rRect.y) {
        t = rRect.x;
        rRect.x = rRect.y;
        rRect.y = t;
    }
    if (rRect.z > rRect.w) {
        t = rRect.z;
        rRect.z = rRect.w;
        rRect.w = t;
    }
}

inline void msw::CCollManBasket::ClipRectByViewColl_(bcs::FVEC4 &rRect, long mode) const
{
    if (m_bEnableViewColl != 0 && m_nViewCollTargetDir >= 0 && mode == 1) {
        if (m_nViewCollTargetDir != 0) {
            rRect.z = Max(rRect.z, m_Unk44.z + m_Unk40);
        } else {
            rRect.w = Min(rRect.w, m_Unk44.z - m_Unk40);
        }
    }
}

// Keeps a sphere of radius fRadius that moved from rFrom to rTo inside the
// area that bits 24-30 of `flags` choose:
//   1: both team courts together (the parts of their bounding rectangle
//      beside the narrower court and the gap between them are kept out of);
//   2: the court of team `flags & 0xFF`;
//   3: the outer field `(flags >> 8) & 0xFF` of team `flags & 0xFF`; for
//      field 0 the other team's court is kept out of as well;
//   anything else: the whole ground.
// Bit 31 is passed on to the CCollMan region tests. Returns their results
// or-ed together; rPos and rNormal get the corrected position and the push
// direction.
int msw::CCollManBasket::DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                       const bcs::FVEC3 &rTo, float fRadius, long mode, unsigned long flags)
{
    bcs::FVEC4 court[2];
    bcs::FVEC4 rect;
    unsigned long passFlags;
    long area;
    // The original keeps this 0 in a register for the zero normal and the
    // Inner/Outer calls outside the two-court case, and loads the constant
    // again for the calls inside it: a local here, the literal there.
    float zero = 0.0f;

    rPos = rTo;
    passFlags = flags & 0x80000000;
    rNormal.Set(zero, zero, zero);

    area = (flags >> 24) & 0x7F;
    switch (area) {
    case 2:
        CStageMan::GetInstance()->GetTeamCourtRect((unsigned char)flags, &rect.x, &rect.y, &rect.z, &rect.w);
        break;
    case 3: {
        long field = (unsigned char)(flags >> 8);
        long team = (unsigned char)flags;

        CStageMan::GetInstance()->GetTeamOuterFieldRect(team, &rect.x, &rect.y, &rect.z, &rect.w, field);
        if (field == 0) {
            bcs::FVEC4 other;
            int result = DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, passFlags);

            // The other team's court, stretched by half the field's size
            // where it reaches the field's edge.
            CStageMan::GetInstance()->GetTeamCourtRect(1 - team, &other.x, &other.y, &other.z, &other.w);
            if (other.x <= rect.x) {
                other.x -= 0.5f * __fabsf(rect.y - rect.x);
            }
            if (other.y >= rect.y) {
                other.y += 0.5f * __fabsf(rect.y - rect.x);
            }
            if (other.z <= rect.z) {
                other.z -= 0.5f * __fabsf(rect.w - rect.z);
            }
            if (other.w >= rect.w) {
                other.w += 0.5f * __fabsf(rect.w - rect.z);
            }
            return result | DetectMapHitOuterRgn_(rPos, rNormal, rTime, other, zero, rFrom, rPos, fRadius, passFlags);
        }
        break;
    }
    case 1:
        CStageMan::GetInstance()->GetTeamCourtRect(0, &court[0].x, &court[0].y, &court[0].z, &court[0].w);
        CStageMan::GetInstance()->GetTeamCourtRect(1, &court[1].x, &court[1].y, &court[1].z, &court[1].w);
        SortRect(court[0]);
        SortRect(court[1]);
        // court[0] is the one in front.
        if (court[0].z > court[1].z) {
            bcs::FVEC4 tmp;
            tmp = court[0];
            court[0] = court[1];
            court[1] = tmp;
        }
        // Both courts together.
        rect.x = Min(court[0].x, court[1].x);
        rect.y = Max(court[0].y, court[1].y);
        rect.z = Min(court[0].z, court[1].z);
        rect.w = Max(court[0].w, court[1].w);

        // Unless the two courts make one rectangle.
        if (mode != 2 &&
            (court[0].x != court[1].x || court[0].y != court[1].y ||
             (court[0].z != court[1].w && court[0].w != court[1].z))) {
            int result;

            ClipRectByViewColl_(rect, mode);
            result = DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, passFlags);
            if (rect.x < court[0].x) {
                bcs::FVEC4 out(rect.x - 100.0f, court[0].x, court[0].z - 100.0f,
                               Max(court[0].w, court[1].z));
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, out, 0.0f, rFrom, rPos, fRadius, passFlags);
            }
            if (rect.y > court[0].y) {
                bcs::FVEC4 out(court[0].y, rect.y + 100.0f, court[0].z - 100.0f,
                               Max(court[0].w, court[1].z));
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, out, 0.0f, rFrom, rPos, fRadius, passFlags);
            }
            if (rect.x < court[1].x) {
                bcs::FVEC4 out(rect.x - 100.0f, court[1].x, Min(court[0].w, court[1].z),
                               court[1].w + 100.0f);
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, out, 0.0f, rFrom, rPos, fRadius, passFlags);
            }
            if (rect.y > court[1].y) {
                bcs::FVEC4 out(court[1].y, rect.y + 100.0f, Min(court[0].w, court[1].z),
                               court[1].w + 100.0f);
                result |= DetectMapHitOuterRgn_(rPos, rNormal, rTime, out, 0.0f, rFrom, rPos, fRadius, passFlags);
            }

            // The gap between the courts. (what the 0.1 and the comparison
            // with the height of rPos are for is not known)
            float gap = court[1].z - court[0].w;
            float depth = gap - 0.1f;
            if (depth > rPos.y) {
                depth = gap;
            } else {
                depth -= rPos.y;
            }
            if (depth > 0.0f) {
                float center = 0.5f * (court[0].w + court[1].z);
                bcs::FVEC4 out(rect.x, rect.y, center - 0.5f * depth, center + 0.5f * depth);
                result |= DetectMapHitOuterRgnZ_(rPos, rNormal, rTime, out, 0.0f, rFrom, rPos, fRadius, passFlags);
            }
            return result;
        }
        break;
    default:
        CStageMan::GetInstance()->GetGroundRect(&rect.x, &rect.y, &rect.z, &rect.w);
        break;
    }

    ClipRectByViewColl_(rect, mode);
    return DetectMapHitInnerRgn_(rPos, rNormal, rTime, rect, zero, rFrom, rTo, fRadius, passFlags);
}
