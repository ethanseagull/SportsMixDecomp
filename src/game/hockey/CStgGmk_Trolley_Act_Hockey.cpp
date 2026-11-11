// msw::CStgGmk_Trolley_Act_Hockey: the actor of the trolley of hockey
// stage 17. Besides the HOCKEY_PRM_S17_* tuning for msw::CStgGmk_Trolley_Act
// (a DOL class) it reflects the puck: the trolley's defence collision is
// on, and a hit sends the puck straight off the side it struck. Its inline
// getters and those of msw::CStgGmk_Trolley_Act are emitted here with the
// vtable.
#include "game/msw/CStgGmk_Trolley_Hockey.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CStgSrt.hpp"

void msw::CStgGmk_Trolley_Act_Hockey::OnInit()
{
    CStgGmk_Trolley_Act::OnInit();
}

// Counts the reflect wait down to zero.
void msw::CStgGmk_Trolley_Act_Hockey::OnUpdate(float rate)
{
    CStgGmk_Trolley_Act::OnUpdate(rate);
    m_fReflectWait -= rate;
    float wait = 0.0f;
    if (m_fReflectWait > wait) {
        wait = m_fReflectWait;
    }
    m_fReflectWait = wait;
}

// Body unit 2 does not take target 4 (meaning not known).
void msw::CStgGmk_Trolley_Act_Hockey::UpdateColl()
{
    CStgGmk_Trolley_Act::UpdateColl();
    unsigned long flag = m_BodyColl.GetTargetFlag(2);
    m_BodyColl.SetTargetFlag(2, flag & ~4);
}

void msw::CStgGmk_Trolley_Act_Hockey::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
}

// The puck leaves along the X or the Z axis, away from the trolley: along
// Z when it struck within 45 degrees of the front or back, along X
// otherwise. The new velocity has length 1 (the original measures the
// incoming speed and does not use it). Then the trolley ignores the puck
// for ten frames.
unsigned long msw::CStgGmk_Trolley_Act_Hockey::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                           bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
{
    if (m_fReflectWait > 0.0f) {
        return 0;
    }

    float speed = bcs::FVEC3Len(&rVel);
    bcs::FVEC3 dir;
    bcs::FVEC3Sub(&dir, &pBall->GetPos(), &GetSrt()->GetPos());
    dir.y = 0.0f;
    bcs::BCS_Vec_Normalize(dir, dir);

    bcs::FVEC3 front(0.0f, 0.0f, 1.0f);
    float cos = bcs::FVEC3Dot(&dir, &front);
    cos = cos > 1.0f ? 1.0f : (cos < -1.0f ? -1.0f : cos);
    float angle = bcs::BCS_ACosT(cos);
    if (angle < 0.7853982f || angle > 2.3561945f) {
        dir.x = 0.0f;
    } else {
        dir.z = 0.0f;
    }
    bcs::BCS_Vec_Normalize(rVel, dir);

    m_fReflectWait = 10.0f;
    return 1 | 0x400;
}

long msw::CStgGmk_Trolley_Act_Hockey::GetParam_Effect(long type) const
{
    return -1;
}

long msw::CStgGmk_Trolley_Act_Hockey::GetParam_TrolleyPatValidNum() const
{
    return 0;
}

// (meaning guessed) Index of a raw location: each of the ten rows starts
// 20 entries after the previous one, the first at 161. The original checks
// the row against patnum (40), not 10, so rows 10 to 39 read past the
// table. (The "@0" on patnum's symbol says another patnum was declared
// before it in the original file; colnum is not read.)
long msw::CStgGmk_Trolley_Act_Hockey::GetLocRawTblIdx(long row, long idx) const
{
    const long tbl[] = { 161, 181, 201, 221, 241, 261, 281, 301, 321, 341 };
    static const long patnum = 40;
    static const long colnum = 1;
    long rowLen = tbl[1] - tbl[0];

    if (row < 0 || row >= patnum || idx < 0 || idx >= rowLen * colnum) {
        return -1;
    }
    return idx + tbl[row];
}

msw::CStgGmk_Trolley_Act_Hockey::~CStgGmk_Trolley_Act_Hockey()
{
}
