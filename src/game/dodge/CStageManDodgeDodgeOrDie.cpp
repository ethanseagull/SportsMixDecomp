// msw::CStageManDodgeDodgeOrDie: the stage of the "Dodge or Die" party game
// of the dodge game, base of the DOD stages. One court of 13 x 13 centred on
// the team offsets, a narrow ground around it and an outer field that differs
// by side; it has no goals.
#include "game/msw/CStageManDodgeDodgeOrDie.hpp"

namespace {

    // (name guessed) Whether rPos lies inside the rectangle, edges included.
    inline bool IsInRect(const bcs::FVEC3 &rPos, float left, float right, float front, float back)
    {
        if (left <= rPos.x && right >= rPos.x && front <= rPos.z && back >= rPos.z) {
            return true;
        }
        return false;
    }
}

msw::CStageManDodgeDodgeOrDie::CStageManDodgeDodgeOrDie(bcs::CTask *pParent) : CStageManDodge(pParent)
{
}

msw::CStageManDodgeDodgeOrDie::~CStageManDodgeDodgeOrDie()
{
}

float msw::CStageManDodgeDodgeOrDie::GetBaseCourtWidth_() const
{
    return 13.0f;
}

float msw::CStageManDodgeDodgeOrDie::GetBaseCourtDepth_() const
{
    return 13.0f;
}

float msw::CStageManDodgeDodgeOrDie::GetBaseGroundWidth_() const
{
    return 1.4f;
}

float msw::CStageManDodgeDodgeOrDie::GetBaseGroundDepth_() const
{
    return 1.4f;
}

float msw::CStageManDodgeDodgeOrDie::GetBaseOuterWidth_() const
{
    return 5.0f;
}

float msw::CStageManDodgeDodgeOrDie::GetBaseOuterDepth_() const
{
    return 5.0f;
}

float msw::CStageManDodgeDodgeOrDie::GetCourtLeft() const
{
    return 0.5f * -GetBaseCourtWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetCourtRight() const
{
    return 0.5f * GetBaseCourtWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetCourtFront() const
{
    return 0.5f * -GetBaseCourtDepth_();
}

float msw::CStageManDodgeDodgeOrDie::GetCourtBack() const
{
    return 0.5f * GetBaseCourtDepth_();
}

float msw::CStageManDodgeDodgeOrDie::GetCourtRadius() const
{
    return GetBaseCourtRadius_();
}

// Both teams play on one court, centred on the team's offset.
float msw::CStageManDodgeDodgeOrDie::GetTeamCourtLeft(long team) const
{
    return GetTeamCourtOffsetX(team) - 0.5f * GetBaseCourtWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetTeamCourtRight(long team) const
{
    return GetTeamCourtOffsetX(team) + 0.5f * GetBaseCourtWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetTeamCourtFront(long team) const
{
    return GetTeamCourtOffsetZ(team) - 0.5f * GetBaseCourtDepth_();
}

float msw::CStageManDodgeDodgeOrDie::GetTeamCourtBack(long team) const
{
    return GetTeamCourtOffsetZ(team) + 0.5f * GetBaseCourtDepth_();
}

float msw::CStageManDodgeDodgeOrDie::GetTeamCourtFrontZoneFront(long team) const
{
    if (team == 0) {
        return GetTeamCourtFront(team);
    }
    float depth = GetBaseCourtFrontZoneDepth_();
    return GetTeamCourtBack(team) - depth;
}

float msw::CStageManDodgeDodgeOrDie::GetTeamCourtFrontZoneBack(long team) const
{
    if (team == 0) {
        return GetTeamCourtFront(team) + GetBaseCourtFrontZoneDepth_();
    }
    return GetTeamCourtBack(team);
}

// The ground surrounds both teams' courts.
float msw::CStageManDodgeDodgeOrDie::GetGroundLeft() const
{
    float left1 = GetTeamCourtLeft(1);
    float left0 = GetTeamCourtLeft(0);
    float left = left0;
    if (left1 < left0) {
        left = left1;
    }
    return left - GetBaseGroundWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetGroundRight() const
{
    float right1 = GetTeamCourtRight(1);
    float right0 = GetTeamCourtRight(0);
    float right = right0;
    if (right1 > right0) {
        right = right1;
    }
    return right + GetBaseGroundWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetGroundFront() const
{
    float front1 = GetTeamCourtFront(1);
    float front0 = GetTeamCourtFront(0);
    float front = front0;
    if (front1 < front0) {
        front = front1;
    }
    return front - GetBaseGroundDepth_();
}

float msw::CStageManDodgeDodgeOrDie::GetGroundBack() const
{
    float back1 = GetTeamCourtBack(1);
    float back0 = GetTeamCourtBack(0);
    float back = back0;
    if (back1 > back0) {
        back = back1;
    }
    return back + GetBaseGroundDepth_();
}

float msw::CStageManDodgeDodgeOrDie::GetTeamGroundLeft(long team) const
{
    float offset = GetTeamCourtOffsetX(team);
    float ground = GetBaseGroundWidth_();
    return offset - (ground + 0.5f * GetBaseCourtWidth_());
}

float msw::CStageManDodgeDodgeOrDie::GetTeamGroundRight(long team) const
{
    float offset = GetTeamCourtOffsetX(team);
    float ground = GetBaseGroundWidth_();
    return offset + (ground + 0.5f * GetBaseCourtWidth_());
}

float msw::CStageManDodgeDodgeOrDie::GetTeamGroundFront(long team) const
{
    float offset = GetTeamCourtOffsetZ(team);
    float ground = GetBaseGroundDepth_();
    return offset - (ground + 0.5f * GetBaseCourtDepth_());
}

float msw::CStageManDodgeDodgeOrDie::GetTeamGroundBack(long team) const
{
    float offset = GetTeamCourtOffsetZ(team);
    float ground = GetBaseGroundDepth_();
    return offset + (ground + 0.5f * GetBaseCourtDepth_());
}

float msw::CStageManDodgeDodgeOrDie::GetOuterFieldLeft() const
{
    float width = GetBaseOuterWidth_();
    return GetCourtLeft() - width;
}

float msw::CStageManDodgeDodgeOrDie::GetOuterFieldRight() const
{
    return GetCourtRight() + GetBaseOuterWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetOuterFieldFront() const
{
    float depth = GetBaseOuterDepth_();
    return GetCourtFront() - depth;
}

float msw::CStageManDodgeDodgeOrDie::GetOuterFieldBack() const
{
    return GetCourtBack() + GetBaseOuterDepth_();
}

// Part `n` of a team's outer field (n is a guess at what the parts are: the
// values 0 to 4 are special-cased).
float msw::CStageManDodgeDodgeOrDie::GetTeamOuterFieldLeft(long team, long n) const
{
    if (n < 1 || n > 4) {
        if (n == 0) {
            return 5.0f;
        }
        float width = GetBaseOuterWidth_();
        return GetTeamCourtLeft(team) - width;
    }
    return 3.0f;
}

float msw::CStageManDodgeDodgeOrDie::GetTeamOuterFieldRight(long team, long n) const
{
    if (n == 3) {
        return GetTeamCourtLeft(team);
    }
    return GetTeamCourtRight(team) + GetBaseOuterWidth_();
}

float msw::CStageManDodgeDodgeOrDie::GetTeamOuterFieldFront(long team, long n) const
{
    if (team == 0) {
        if (n == 1) {
            return GetTeamCourtBack(team);
        }
        return GetTeamCourtFront(team) + GetBaseOuterFront_();
    }
    if (n == 2 || n == 3) {
        return GetTeamCourtFront(team);
    }
    float depth = GetBaseOuterDepth_();
    return GetTeamCourtFront(team) - depth;
}

float msw::CStageManDodgeDodgeOrDie::GetTeamOuterFieldBack(long team, long n) const
{
    if (team == 0) {
        if (n == 2 || n == 3) {
            return GetTeamCourtBack(team);
        }
        return GetTeamCourtBack(team) + GetBaseOuterDepth_();
    }
    if (n == 1) {
        return GetTeamCourtFront(team);
    }
    float front = GetBaseOuterFront_();
    return GetTeamCourtBack(team) - front;
}

// The bits of `mask` choose the areas to test: 0x000F the court (all four
// bits are set on a hit), 0x0F00 the four corners of the court widened by the
// outer field (the first hit only), 0xF000 each team's ground.
unsigned long msw::CStageManDodgeDodgeOrDie::GetAreaFlagByPos(const bcs::FVEC3 &rPos, long mask) const
{
    unsigned long flag = 0;
    float left;
    float right;
    float front;
    float back;

    if (mask & 0xF) {
        GetCourtRect(&left, &right, &front, &back);
        if (IsInRect(rPos, left, right, front, back)) {
            flag |= 0xF;
        }
    }

    if (mask & 0xF00) {
        float margin = GetTeamOuterFieldLeft(0, 0);

        for (long i = 0; i < 4; i++) {
            if (!((mask >> 8) & (1 << i))) {
                continue;
            }
            GetCourtRect(&left, &right, &front, &back);
            switch (i) {
                case 0:
                    right = left + margin;
                    back = front + margin;
                    break;
                case 1:
                    left = right - margin;
                    back = front + margin;
                    break;
                case 2:
                    left = right - margin;
                    front = back - margin;
                    break;
                case 3:
                    // Moves the left edge, not the right one (as the original
                    // does).
                    left = left + margin;
                    front = back - margin;
                    break;
            }
            if (IsInRect(rPos, left, right, front, back)) {
                flag |= 1 << (i + 8);
                break;
            }
        }
    }

    if (mask & 0xF000) {
        for (long i = 0; i < 2; i++) {
            if (!((mask >> 12) & (1 << i))) {
                continue;
            }
            GetTeamGroundRect(i, &left, &right, &front, &back);
            if (IsInRect(rPos, left, right, front, back)) {
                flag |= 1 << (i + 12);
            }
        }
    }

    return flag;
}

void msw::CStageManDodgeDodgeOrDie::Disp3D(long)
{
}

// No goals in this game.
const bcs::FVEC3 &msw::CStageManDodgeDodgeOrDie::GetGoalVec(long) const
{
    return m_vZero;
}

const bcs::FVEC3 &msw::CStageManDodgeDodgeOrDie::GetBoardPos(long) const
{
    return m_vZero;
}

const bcs::FVEC3 &msw::CStageManDodgeDodgeOrDie::GetGoalRot(long) const
{
    return m_vZero;
}

const bcs::FVEC3 &msw::CStageManDodgeDodgeOrDie::GetGoalPos(long) const
{
    return m_vZero;
}

// The front zone spans the whole width of the team's court.
float msw::CStageManDodgeDodgeOrDie::GetTeamCourtFrontZoneRight(long team) const
{
    return GetTeamCourtRight(team);
}

float msw::CStageManDodgeDodgeOrDie::GetTeamCourtFrontZoneLeft(long team) const
{
    return GetTeamCourtLeft(team);
}
