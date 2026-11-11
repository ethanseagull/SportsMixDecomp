// msw::CStgGmk_Trolley_Act_Basket: the actor of the trolley of basket
// stage 17. It only supplies the BASKET_PRM_S17_* tuning to
// msw::CStgGmk_Trolley_Act (a DOL class). Its inline getters and those of
// msw::CStgGmk_Trolley_Act are emitted here with the vtable.
#include "game/msw/CStgGmk_Trolley_Basket.hpp"

void msw::CStgGmk_Trolley_Act_Basket::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
}

long msw::CStgGmk_Trolley_Act_Basket::GetParam_Effect(long type) const
{
    return -1;
}

long msw::CStgGmk_Trolley_Act_Basket::GetParam_TrolleyPatValidNum() const
{
    return 0;
}

// (meaning guessed) Index of a raw location: each of the ten rows starts
// 20 entries after the previous one, the first at 149. The original checks
// the row against 40, not 10, so rows 10 to 39 read past the table.
long msw::CStgGmk_Trolley_Act_Basket::GetLocRawTblIdx(long row, long idx) const
{
    const long tbl[] = { 149, 169, 189, 209, 229, 249, 269, 289, 309, 329 };
    long rowLen = tbl[1] - tbl[0];

    if (row < 0 || row >= 40 || idx < 0 || idx >= rowLen) {
        return -1;
    }
    return idx + tbl[row];
}

msw::CStgGmk_Trolley_Act_Basket::~CStgGmk_Trolley_Act_Basket()
{
}
