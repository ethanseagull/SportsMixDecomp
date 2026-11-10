#include "game/msw/CCameraBasketHB.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallManBasket.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "bcs/CWorldEnv.hpp"

msw::CCameraBasketHB::CCameraBasketHB(long view) : CCameraBasket(view)
{
    bcs::CWorldEnv::SetViewFovy(view, 33.0f);

    // The sum is computed at run time in the original (fadds), so the
    // distance is a local here; written with constants only it is folded.
    float dist = 100.0f;
    float y = -(4.5f + dist);

    m_Unk100 = dist;
    m_Unk104 = dist;
    m_Unk108 = 0.0f;
    m_Unk10C = 0.0f;
    m_Unk110 = y;
    m_Unk240 = 0.97f;
    SetCamParam();
}

void msw::CCameraBasketHB::MainCamera()
{
    if (CanbeMain()) {
        CGameCamera::MainCamera();
        bcs::CWorldEnv::SetViewFovy(m_nView, m_Unk250);
        m_Unk100 = m_Unk254;
        m_Unk104 = m_Unk258;
        SetCamMode(0, 0);
        MainCamera_Normal();
        CalcCamData(0.96435547f);
    }
}

void msw::CCameraBasketHB::MainCamera_Normal()
{
    // The vectors are declared in this order because the original's stack
    // layout has them so (the last declared at the lowest address); the
    // order changes nothing else.
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 at;
    bcs::FVEC3 arg3;
    bcs::FVEC3 arg2;
    bcs::FVEC3 look;

    float rate = m_fLookRate;

    look.Set(0.0f, 0.0f, 0.0f);
    arg2.Set(0.0f, 0.0f, 0.0f);
    arg3.Set(0.0f, 0.0f, 0.0f);
    long mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);

    at = look;

    at.x = at.x > 100.0f ? 100.0f : (at.x < -100.0f ? -100.0f : at.x);
    at.z = at.z > 100.0f ? 100.0f : (at.z < -100.0f ? -100.0f : at.z);
    CalcLookPos(at, arg2, arg3, rate, mask);

    pos = m_Unk118;
    float curveX[6] = { m_CurvePoint[0].x, m_CurvePoint[0].x, m_CurvePoint[1].x,
                        m_CurvePoint[1].x, m_CurvePoint[2].x, m_CurvePoint[2].x };
    float curveY[6] = { m_CurvePoint[0].y, m_CurvePoint[0].y, m_CurvePoint[1].y,
                        m_CurvePoint[1].y, m_CurvePoint[2].y, m_CurvePoint[2].y };
    float curveZ[6] = { m_CurvePoint[0].z, m_CurvePoint[0].z, m_CurvePoint[1].z,
                        m_CurvePoint[1].z, m_CurvePoint[2].z, m_CurvePoint[2].z };

    // Read once for the three blends.
    float near = m_Unk100;
    float far = m_Unk104;
    float x = CalcCurveBlend(pos.x, pos.z, near, far, curveX[0], curveX[1], curveX[2], curveX[3],
                             curveX[4], curveX[5]);
    float y = CalcCurveBlend(pos.x, pos.z, near, far, curveY[0], curveY[1], curveY[2], curveY[3],
                             curveY[4], curveY[5]);
    float z = CalcCurveBlend(pos.x, pos.z, near, far, curveZ[0], curveZ[1], curveZ[2], curveZ[3],
                             curveZ[4], curveZ[5]);

    target.Set(0.0f, 0.0f, m_Unk110);

    pos.x *= m_fScaleX;
    target.x = pos.x;
    MakeingFrontCamData(pos, target, x, y, z, m_Unk280, m_Unk240, m_Unk1C0);
}

long msw::CCameraBasketHB::GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    // Kept in a register for both loops.
    float persRate = 0.035f;
    float minX = m_fMinX;
    float maxX = m_fMaxX;
    float maxZ = m_fMaxZ;
    long mask = 0;
    bcs::FVEC3 pos;

    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);

        if (pBall != 0 && pBall->IsActive() && !CBallManBasket::GetInstance()->IsAttachString_(pBall)) {
            pos = GetBallPosXZ(pBall);
            mask |= 1 << i;
            NormalizePersPos(pos, persRate, -m_Unk104, m_Unk104);
            pos.z *= m_fScaleZ;
            if (pos.z > maxZ) {
                maxZ = pos.z;
            }
        }
    }

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pCore = CChrMan::GetChrCore(i);

        if (pCore != 0 && !pCore->IsDisable()) {
            pos = GetCorePosXZ(pCore);
            NormalizePersPos(pos, persRate, -m_Unk104, m_Unk104);
            pos.z *= m_fScaleZ;
            if (pos.x > maxX) {
                maxX = pos.x;
            }
            if (pos.x < minX) {
                minX = pos.x;
            }
            if (pos.z > maxZ) {
                maxZ = pos.z;
            }
        }
    }

    rLook.x = minX + (maxX - minX) * 0.5f;
    rLook.y = 0.0f;
    rLook.z = maxZ;
    rLook.x = rLook.x > 100.0f ? 100.0f : (rLook.x < -100.0f ? -100.0f : rLook.x);
    rLook.z = rLook.z > 100.0f ? 100.0f : (rLook.z < -100.0f ? -100.0f : rLook.z);
    return mask;
}

void msw::CCameraBasketHB::Reset()
{
    CGameCamera::Reset();
    SetCamParam();
}

int msw::CCameraBasketHB::MessageHandler(MSG_ID id, MSG_PARAM *pParam)
{
    return 0;
}

bool msw::CCameraBasketHB::CanbeMain() const
{
    return true;
}

void msw::CCameraBasketHB::SetCamParam()
{
    if (m_UnkFC & 0x8) {
        m_Unk250 = 34.0f;
        m_Unk254 = 7.0f;
        m_Unk258 = 12.0f;
        m_CurvePoint[0].x = 5.3f;
        m_CurvePoint[0].y = 4.399999f;
        m_CurvePoint[0].z = 15.0f;
        m_CurvePoint[1].x = 5.300001f;
        m_CurvePoint[1].y = 4.4f;
        m_CurvePoint[1].z = 13.500002f;
        m_CurvePoint[2].x = 5.300001f;
        m_CurvePoint[2].y = 4.4f;
        m_CurvePoint[2].z = 12.0f;
        m_Unk280 = 0.96f;
        m_fLookRate = 0.97f;
        m_fScaleX = 0.75f;
        m_fScaleZ = 0.75f;
        m_fMinX = 0.0f;
        m_fMaxX = 0.0f;
        m_fMaxZ = -100.0f;
    } else {
        m_Unk250 = 37.500008f;
        m_Unk254 = 7.0f;
        m_Unk258 = 12.0f;
        m_CurvePoint[0].x = 5.4f;
        m_CurvePoint[0].y = 4.349998f;
        m_CurvePoint[0].z = 15.0f;
        m_CurvePoint[1].x = 5.400001f;
        m_CurvePoint[1].y = 4.349999f;
        m_CurvePoint[1].z = 13.500002f;
        m_CurvePoint[2].x = 5.400001f;
        m_CurvePoint[2].y = 4.349999f;
        m_CurvePoint[2].z = 12.0f;
        m_Unk280 = 0.96f;
        m_fLookRate = 0.97f;
        m_fScaleX = 0.75f;
        m_fScaleZ = 0.75f;
        m_fMinX = 0.0f;
        m_fMaxX = 0.0f;
        m_fMaxZ = -100.0f;
    }
}

msw::CCameraBasketHB::~CCameraBasketHB() {}
