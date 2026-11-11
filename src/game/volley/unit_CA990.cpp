// msw::CComDrvVolleyRR: going round obstacles. A file of its own, away from
// the rest of the class (game/volley/CComDrvVolleyRR.cpp), as the same three
// functions of CComDrvVolleyGame are (game/volley/unit_C5D08.cpp). The file
// name is not known.
#include "game/msw/CComDrvVolleyRR.hpp"

int msw::CComDrvVolleyRR::GetPosTurnAroundGimmick(bcs::FVEC3 &rPos)
{
    long nKind = -1;
    long nTarget;
    long nDir = -1;

    if (m_nTurnKind < 0) {
        // Not turning yet: look for a character, then an item, in the way.
        nTarget = RunCorrect_GetChrToTarget(rPos);

        if (nTarget >= 0) {
            nKind = 1;
            nDir = RunCorrect_GetPosTurnAroundChr(rPos, nTarget, -1);
        } else {
            nTarget = Item_GetItemToTarget(rPos);
            if (nTarget != 0) {
                nKind = 2;
                nDir = Item_GetPosTurnAroundItem(rPos, nTarget, -1, 0);
            }
        }

        if (nKind >= 0) {
            m_nTurnKind = nKind;
            m_nTurnTarget = nTarget;
            m_nTurnDir = nDir;
            m_fTurnFrame = 60.0f;
        }
    } else {
        // Keep going round the same one while it is still in the way.
        nDir = m_nTurnDir;

        switch (m_nTurnKind) {
            case 1: {
                long nChr = RunCorrect_GetChrToTarget(rPos);
                if (nChr == m_nTurnTarget) {
                    RunCorrect_GetPosTurnAroundChr(rPos, nChr, nDir);
                }
                break;
            }
            case 2: {
                long nItem = Item_GetItemToTarget(rPos);
                if (nItem == m_nTurnTarget) {
                    Item_GetPosTurnAroundItem(rPos, nItem, nDir, 0);
                }
                break;
            }
        }
    }
    return m_nTurnKind >= 0;
}

void msw::CComDrvVolleyRR::GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos)
{
    GetPosTurnAroundGimmick(rPos);
}

void msw::CComDrvVolleyRR::GetViewPointPosForRunEx(bcs::FVEC3 &rPos)
{
    Item_GetViewPointPosFromItem(rPos);
}
