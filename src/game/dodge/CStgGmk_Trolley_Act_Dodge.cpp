// msw::CStgGmk_Trolley_Act_Dodge: the actor of the trolley of dodge
// stage 17. It only supplies the DODGE_PRM_S17_* tuning to
// msw::CStgGmk_Trolley_Act (a DOL class). Its inline getters and those of
// msw::CStgGmk_Trolley_Act are emitted here with the vtable.
#include "game/msw/CStgGmk_Trolley_Dodge.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CStgSrt.hpp"

namespace {
    // (stand-ins for operators) Reached through this one more inline level,
    // the float registers come out numbered as in the original (see
    // notes/toolchain.md on asm register numbering by inline depth).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecNormalize(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, rIn);
    }
}

// A ball that hits the trolley is knocked out of play: it flies away from
// the trolley in the XZ plane at 0.3 times its speed.
void msw::CStgGmk_Trolley_Act_Dodge::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
    bcs::FVEC3 dir;
    bcs::FVEC3 vel = pBall->GetVelocity();

    VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
    dir.y = 0.0f;
    VecNormalize(dir, dir);
    bcs::FVEC3Scale(&dir, &dir, 0.3f * bcs::FVEC3Len(&vel));
    pBall->SetOutBallMode(dir, 0x80, -1);
}

long msw::CStgGmk_Trolley_Act_Dodge::GetParam_Effect(long type) const
{
    return -1;
}

long msw::CStgGmk_Trolley_Act_Dodge::GetParam_TrolleyPatValidNum() const
{
    return 0;
}

// (meaning guessed) Index of a raw location: each of the ten rows starts
// 20 entries after the previous one, the first at 161. The original checks
// the row against 40, not 10, so rows 10 to 39 read past the table.
long msw::CStgGmk_Trolley_Act_Dodge::GetLocRawTblIdx(long row, long idx) const
{
    const long tbl[] = { 161, 181, 201, 221, 241, 261, 281, 301, 321, 341 };
    long rowLen = tbl[1] - tbl[0];

    if (row < 0 || row >= 40 || idx < 0 || idx >= rowLen) {
        return -1;
    }
    return idx + tbl[row];
}

msw::CStgGmk_Trolley_Act_Dodge::~CStgGmk_Trolley_Act_Dodge()
{
}
