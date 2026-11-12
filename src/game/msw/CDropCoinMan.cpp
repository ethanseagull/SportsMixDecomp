#include "game/msw/CDropCoinMan.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CDropCoin.hpp"
#include "game/msw/CEffectCoinGetBase.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"
#include <math.h>

// One source file for the coins on the court: their manager
// (CDropCoinMan), the coins themselves (CDropCoin) and the base of the
// effect played when one is collected (CEffectCoinGetBase). The three share
// their float constants and the coin table.

namespace {

    // What each coin type is made of. (struct and member names guessed)
    struct COIN_DATA {
        const char *pFileName;       // 0x00, in the item archive
        const char *pMdlName;        // 0x04
        const char *pShadowMdlName;  // 0x08
        const char *pRefMdlName;     // 0x0C
        const char *pEffectFileName; // 0x10, CEffectCoinGetBase's model
        const char *pEffectMdlName;  // 0x14
        long nGetSe;                 // 0x18, played by CEffectCoinGetBase
    };

    const COIN_DATA s_aCoinData[] = {
        {"Coin.brres", "coin", "coin", "coin", "Coin.brres", "coin", 0xC43},
        {"Coin.brres", "coin", "coin", "coin", "Coin.brres", "coin", 0xC43},
        {"StarPiece.brres", "star_piece", "star_piece", "star_piece", "Coin.brres", "coin", 0xC43},
        {"StarPiece.brres", "star_piece", "star_piece", "star_piece", "Coin.brres", "coin", 0xC43},
        {"StarPiece.brres", "star_piece", "star_piece", "star_piece", "Coin.brres", "coin", 0xC43},
    };

    // The colours a star piece is given (red, green, blue; the fourth,
    // yellow, is never picked).
    const unsigned long s_aStarPieceColor[] = {
        0xFF0000FF,
        0x00FF00FF,
        0x0000FFFF,
        0xFFFF00FF,
    };

    // Brings an angle in radians into [0, 2 pi].
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float Clamp(float value, float min, float max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    // Queues a scene object for deletion and forgets it.
    template <class T>
    inline void DeleteScnObj(T *&pObj)
    {
        if (pObj != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(pObj);
        }
        pObj = 0;
    }

    // rVel = rVel reflected on the plane with normal rNormal.
    inline void Reflect(bcs::FVEC3 &rVel, const bcs::FVEC3 &rNormal)
    {
        bcs::FVEC3 work;

        bcs::FVEC3Scale(&work, &rNormal, 2.0f * bcs::FVEC3Dot(&rVel, &rNormal));
        bcs::FVEC3Sub(&rVel, &rVel, &work);
    }
}

namespace msw {

    nw4r::g3d::ResFile CDropCoinMan::GetRsrcFile(DROP_COIN::CoinType type)
    {
        nw4r::g3d::ResFile archive = CLoadData::GetRes(0x23);
        return nw4r::g3d::ResFile::FromAligned(archive.GetExternalData(s_aCoinData[type].pFileName));
    }

    CDropCoinMan::CDropCoinMan(bcs::CTask *pParent)
        : bcs::CUnivTaskEx(pParent, 0x17), m_fCoinReduceY(0.625f), m_fCoinReduceXZ(0.92f),
          m_fStarPieceReduceY(0.625f), m_fStarPieceReduceXZ(0.92f)
    {
        nw4r::ut::List_Init(&m_CoinList, 0x34); // offset of CDropCoin::m_Link
    }

    CDropCoinMan::~CDropCoinMan()
    {
        AllReset();
    }

    void CDropCoinMan::Main()
    {
    }

    void CDropCoinMan::AllReset()
    {
        DeleteAllChild();
        nw4r::ut::List_Init(&m_CoinList, 0x34);
    }

    void CDropCoinMan::SendMessage(long, long, long, long)
    {
    }

    long CDropCoinMan::CreateCoin(const bcs::FVEC3 &rPos, long nNum, float fLife, float fLifeRand)
    {
        if (nNum <= 0) {
            return 0;
        }

        float life = 630.0f;
        if (fLife > 0.0f) {
            life = ceil(60.0f * fLife);
        }

        long count = 0;
        for (; nNum > 0; nNum--, count++) {
            float rot = WrapRad(bcs::BCS_RandMax(6.2831855f));
            float div = 12.0f + bcs::BCS_RandMax(4L);
            float speed = 0.8f + bcs::BCS_RandMax(0.8f);

            bcs::FVEC3 vel;
            vel.x = 0.04f * nw4r::math::SinRad(rot);
            vel.y = (2.0f * speed) / div;
            vel.z = 0.04f * nw4r::math::CosRad(rot);

            bcs::FVEC3 pos(rPos);
            pos.x += 0.6f * nw4r::math::SinRad(rot);
            pos.z += 0.6f * nw4r::math::CosRad(rot);

            CDropCoin *pCoin = new (0x100) CDropCoin(this, pos, DROP_COIN::COIN_TYPE_COIN);
            if (pCoin != 0) {
                float coinLife = life;
                if (fLifeRand > 0.0f) {
                    coinLife = life + (float)ceil(bcs::BCS_RandMax(60.0f * fLifeRand));
                }
                pCoin->m_fLife = coinLife;
                pCoin->m_vVel.x = vel.x;
                pCoin->m_vVel.y = vel.y;
                pCoin->m_vVel.z = vel.z;
                pCoin->m_nValue = 1;
            }
        }

        if (count != 0) {
            CGameSnd::Play(0xC44, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        return count;
    }

    CDropCoin *CDropCoinMan::CreateCoinSimple()
    {
        return new (0x100) CDropCoin(this);
    }

    long CDropCoinMan::CreateBuryCoin(const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, long nNum)
    {
        float minX;
        float width;
        if (nNum <= 0) {
            return 0;
        }

        float minZ = rMin.z;
        minX = rMin.x;
        width = rMax.x - minX;
        float depth = rMax.z - minZ;
        long count = 0;
        for (long i = 0; i < nNum; i++) {
            bcs::FVEC3 pos;
            pos.x = bcs::BCS_RandMax(1.0f) * width + minX;
            pos.y = 0.0f;
            pos.z = bcs::BCS_RandMax(1.0f) * depth + minZ;

            CDropCoin *pCoin = new (0x100) CDropCoin(this, pos, DROP_COIN::COIN_TYPE_COIN);
            if (pCoin != 0) {
                pCoin->m_fLife = 720.0f;
                pCoin->m_nValue = 1;
                pCoin->SetBury_();
                count++;
            }
        }
        return count;
    }

    CDropCoin *CDropCoinMan::CreateStarPiece(DROP_COIN::CoinType type, long nValue, const bcs::FVEC3 &rPos,
                                             const bcs::FVEC3 &rVel, long nBound)
    {
        CDropCoin *pCoin = new (0x100) CDropCoin(this, rPos, type);
        if (pCoin != 0) {
            pCoin->m_fLife = 30.0f;
            pCoin->m_vVel.x = rVel.x;
            pCoin->m_vVel.y = rVel.y;
            pCoin->m_vVel.z = rVel.z;
            pCoin->m_nFlag |= CDropCoin::FLAG_WAIT_BOUND | CDropCoin::FLAG_NO_GRAVITY;
            pCoin->m_nCount = nBound;
            pCoin->m_nValue = nValue;
        }
        return pCoin;
    }

    long CDropCoinMan::GetStarPieceNum() const
    {
        long num = 0;
        for (CDropCoin *pCoin = static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, 0)); pCoin != 0;
             pCoin = static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, pCoin))) {
            if (pCoin->m_nFlag & CDropCoin::FLAG_STAR_PIECE) {
                num++;
            }
        }
        return num;
    }

    void CDropCoinMan::DeleteAllStarPiece()
    {
        CDropCoin *pCoin = static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, 0));
        while (pCoin != 0) {
            CDropCoin *pNext = static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, pCoin));
            if (pCoin != 0 && !pCoin->IsDelete() && (pCoin->m_nFlag & CDropCoin::FLAG_STAR_PIECE)) {
                pCoin->Delete();
            }
            pCoin = pNext;
        }
    }

    void CDropCoinMan::SetStarPieceRefelctVelReduce(float fY, float fXZ)
    {
        float y = __fabsf(fY);
        float xz = __fabsf(fXZ);
        m_fStarPieceReduceY = y;
        m_fStarPieceReduceXZ = xz;
    }

    void CDropCoinMan::SetCoinRefelctVelReduce(float fY, float fXZ)
    {
        float y = __fabsf(fY);
        float xz = __fabsf(fXZ);
        m_fCoinReduceY = y;
        m_fCoinReduceXZ = xz;
    }

    CDropCoin *CDropCoinMan::DropCoinGetFirst()
    {
        return static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, 0));
    }

    CDropCoin *CDropCoinMan::DropCoinGetNext(CDropCoin *pCoin)
    {
        if (pCoin != 0) {
            return static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, pCoin));
        }
        return 0;
    }

    void CDropCoinMan::DropCoinPickup(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                                      void (*pFunc)(const CDropCoin *, void *), void *pArg)
    {
        float height = __fabsf(fHeight);
        float radius = __fabsf(fRadius);

        if ((CGame::g_GameWork.m_nFlag18 & 0x4306) != 0) {
            return;
        }
        if (!CGame::IsMainLoopState(1)) {
            return;
        }

        float halfHeight = height * 0.5f;
        bcs::FVEC3 center = rPos;
        center.y += halfHeight;
        for (CDropCoin *pCoin = static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, 0)); pCoin != 0;
             pCoin = static_cast<CDropCoin *>(nw4r::ut::List_GetNext(&m_CoinList, pCoin))) {
            if (!pCoin->CanbePickup()) {
                continue;
            }

            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &pCoin->m_vPos, &center);
            float rangeXZ = 0.5f * pCoin->m_fScale + radius;
            float rangeY = 0.25678f * pCoin->m_fScale + halfHeight;
            if (__fabsf(diff.x) > rangeXZ || __fabsf(diff.y) > rangeY || __fabsf(diff.z) > rangeXZ) {
                continue;
            }

            bcs::FVEC3 flat = diff;
            flat.y = 0.0f;
            if (bcs::FVEC3SquareMag(&flat) > rangeXZ * rangeXZ) {
                continue;
            }

            if (pFunc != 0) {
                pFunc(pCoin, pArg);
            }
            if (pCoin->CanbePickup()) {
                CGimmickMan::EffectCoinGetCreate(pCoin->m_pMdl->m_vTrans,
                                                 static_cast<DROP_COIN::CoinType>(pCoin->m_nType));
                pCoin->Delete();
            }
        }
    }

    void CDropCoinMan::EffectCoinGetCreate(const bcs::FVEC3 &, DROP_COIN::CoinType)
    {
    }

    CDropCoin::CDropCoin(CDropCoinMan *pMan)
        : bcs::CUnivTaskEx(pMan, 0x17), m_pMan(pMan), m_nType(0), m_nFlag(0), m_pMdl(0), m_pShadow(0), m_pRef(0),
          m_fLife(0.0f), m_fGravity(-0.0125f), m_fScale(1.0f), m_nValue(1), m_nCount(0)
    {
        m_Link.pPrev = 0;
        m_Link.pNext = 0;
        nw4r::ut::List_Append(&pMan->m_CoinList, this);
        m_vPos.Set(0.0f, 0.0f, 0.0f);
        m_vVel.Set(0.0f, 0.0f, 0.0f);
        CreateMdl_();
        m_nFlag &= ~FLAG_ENABLE;
    }

    CDropCoin::CDropCoin(CDropCoinMan *pMan, const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
        : bcs::CUnivTaskEx(pMan, 0x17), m_pMan(pMan), m_nType(type), m_nFlag(0), m_pMdl(0), m_pShadow(0),
          m_pRef(0), m_fLife(0.0f), m_fGravity(-0.0125f), m_fScale(1.0f), m_nValue(1), m_nCount(0)
    {
        m_Link.pPrev = 0;
        m_Link.pNext = 0;
        nw4r::ut::List_Append(&pMan->m_CoinList, this);
        m_vPos = rPos;
        m_vVel.Set(0.0f, 0.0f, 0.0f);
        CreateMdl_();
        m_nFlag |= FLAG_ENABLE;
        m_pMdl->m_vTrans = m_vPos;
        m_pMdl->SetRotY(bcs::BCS_RandMax(6.2831855f));
        m_pMdl->CalcRootMtx();
    }

    CDropCoin::~CDropCoin()
    {
        nw4r::ut::List_Remove(&m_pMan->m_CoinList, this);
        DeleteScnObj(m_pRef);
        DeleteScnObj(m_pShadow);
        DeleteScnObj(m_pMdl);
    }

    void CDropCoin::CreateMdl_()
    {
        nw4r::g3d::ResFile file = CDropCoinMan::GetRsrcFile(static_cast<DROP_COIN::CoinType>(m_nType));
        const COIN_DATA *pData = &s_aCoinData[m_nType];
        const char *pMdlName = pData->pMdlName;
        const char *pShadowMdlName = pData->pShadowMdlName;
        const char *pRefMdlName = pData->pRefMdlName;

        if (pMdlName != 0) {
            nw4r::g3d::ResMdl mdl = file.GetResMdl(pMdlName);
            m_pMdl = new (0x100) bcs::CMdlObj(mdl, 0, 0);
            m_pMdl->OnFlag(0x4000);
            bcs::CRenderMan::PushBack(m_pMdl, 2);

            bool bStarPiece = false;
            switch (m_nType) {
                case 2:
                case 3:
                case 4:
                    m_pMdl->SetMatTevColorRGB(GX_TEVREG0,
                                              bcs::BCS_Color(s_aStarPieceColor[bcs::BCS_RandMax(3L)]));
                    bStarPiece = true;
                    m_fScale = 1.5f;
                    break;
            }

            if (bStarPiece) {
                m_nFlag |= FLAG_STAR_PIECE;
                if (file.HasResAnmTexSrt()) {
                    nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(0);
                    bcs::CAnmTexSrt *pAnm = new (0x100) bcs::CAnmTexSrt;
                    if (pAnm != 0) {
                        pAnm->Create(m_pMdl, res, 0, 0);
                        m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
                        m_pMdl->OnFlag(0x2000);
                    }
                }
            }
        }

        if (pShadowMdlName != 0 && m_pMdl != 0) {
            nw4r::g3d::ResMdl mdl = file.GetResMdl(pShadowMdlName);
            m_pShadow = new (0x100) bcs::CMdlShadow(m_pMdl, mdl, 0x20, 0, 0);
            bcs::CRenderMan::PushBack(m_pShadow, 2);
        }

        if (pRefMdlName != 0 && m_pMdl != 0) {
            nw4r::g3d::ResMdl mdl = file.GetResMdl(pRefMdlName);
            m_pRef = new (0x100) bcs::CMdlRef(m_pMdl, mdl, 0, 0, 0);
            bcs::CRenderMan::PushBack(m_pRef, 4);
        }
    }

    void CDropCoin::SetBury_()
    {
        m_nFlag &= ~FLAG_ENABLE;
        m_nFlag |= FLAG_BURY | FLAG_WAIT_BOUND;
        m_vVel.Set(0.0f, 0.0f, 0.0f);
        m_fGravity = 0.0f;
        m_nCount = 2;
        m_fScale = 1.0f;

        if (m_pMdl != 0) {
            m_pMdl->m_vScale.Set(1.0f, 1.0f, 1.0f);
            m_vPos.y = 0.0f;
            m_pMdl->m_vTrans.y = 0.0f;
            m_pMdl->SetRot(0.0f, 0.0f, 0.0f);
            m_pMdl->SetRotX(1.5707964f);
            m_pMdl->SetRotY(WrapRad(bcs::BCS_RandMax(6.2831855f)));
            m_pMdl->SetAlphaRateF(0.0f);
        }
        if (m_pShadow != 0) {
            m_pShadow->SetEnable(false);
        }
    }

    // (name guessed) Moves rPos with the floor under it when the floor
    // moves things.
    static inline void AddGrdFlow(bcs::FVEC3 &rPos)
    {
        unsigned long attr;
        bcs::FVEC3 flow;
        CCollMan::GetGrdAttr(rPos, attr, flow);
        if (CCollMan::IsGrdAttrFlow(attr)) {
            rPos.x += flow.x;
            rPos.z += flow.z;
        }
    }

    void CDropCoin::Main()
    {
        if (m_pMdl == 0) {
            Delete();
            return;
        }

        if (m_nFlag & FLAG_BURY) {
            MainBury_();
        } else {
            MainMove_();
            float groundY = 0.25678f * m_fScale;
            if (!(m_nFlag & FLAG_FALL) && m_vPos.y <= groundY && m_vVel.y <= 0.0f) {
                AddGrdFlow(m_vPos);
            }
            m_pMdl->m_vTrans = m_vPos;
            float turn = 0.08726646f * GetFrameRateF();
            m_pMdl->SetRotY(m_pMdl->m_fRotY + turn);
        }

        bool bLifeRuns = true;
        if ((m_nFlag & FLAG_WAIT_BOUND) && m_nCount > 0) {
            bLifeRuns = false;
        }
        if (bLifeRuns && m_fLife > 0.0f) {
            m_fLife -= GetFrameRateF();
        }

        m_pMdl->m_vScale.Set(m_fScale, m_fScale, m_fScale);
        if (m_fLife > 0.0f && m_fLife < 16.0f) {
            m_pMdl->SetAlphaRateF(0.0625f * m_fLife);
        }

        bool bShadow = true;
        if (m_nFlag & FLAG_FALL) {
            bShadow = false;
        } else {
            unsigned long attr;
            bcs::FVEC3 flow;
            CCollMan::GetGrdAttr(m_vPos, attr, flow);
            if (attr == 8) {
                bShadow = false;
            }
        }
        if (m_pShadow != 0) {
            if (!bShadow) {
                m_pShadow->OnFlag(0xC0);
            } else {
                m_pShadow->OffFlag(0xC0);
            }
        }

        if (m_fLife <= 0.0f) {
            Delete();
        }
    }

    // One frame of flight: the velocity moves the coin and gravity the
    // velocity. (guessed to be an inline helper: the original has the code
    // twice, once with each gravity)
    static inline void MoveParabola(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fRate, float fGravity)
    {
        float addVel = fGravity * fRate;
        bcs::FVEC3 move;
        bcs::FVEC3Scale(&move, &rVel, fRate);
        rVel.y += addVel;
        move.y += 0.5f * (addVel * fRate);
        bcs::FVEC3Add(&rPos, &rPos, &move);
    }

    // (name guessed) The splash of a coin landing in water, on the ground
    // under rPos.
    static inline void SplashEffect(const bcs::FVEC3 &rPos)
    {
        bcs::FVEC3 pos(rPos);
        pos.y = 0.0f;
        CEffectCtrl::SimpleEffect(0x44, pos, 3, 0, 0);
    }

    void CDropCoin::MainMove_()
    {
        float rate = GetFrameRateF();
        if (m_nFlag & FLAG_NO_GRAVITY) {
            MoveParabola(m_vPos, m_vVel, rate, 0.0f);
        } else {
            MoveParabola(m_vPos, m_vVel, rate, m_fGravity);
        }

        if (!(m_nFlag & FLAG_FALL)) {
            bcs::FVEC3 hitPos;
            bcs::FVEC3 normal;
            float time;
            if (CCollMan::DetectMapHit(hitPos, normal, time, m_vPos, m_vPos, 0.2f, 4, 0)) {
                m_vPos = hitPos;
                Reflect(m_vVel, normal);
            }
        }

        if (m_nFlag & FLAG_FALL) {
            long se = -1;
            if (CGame::g_GameWork.GetStage() == 5) {
                se = 0xD2A;
            }
            if (m_vPos.y <= -3.0f) {
                unsigned long attr;
                bcs::FVEC3 flow;
                CCollMan::GetGrdAttr(m_vPos, attr, flow);
                if (se >= 0) {
                    CGameSnd::Play(se, m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
                }
                m_fLife = 0.0f;
            }
        } else {
            if (CheckBound_()) {
                m_nFlag |= FLAG_GROUND | FLAG_LANDED;
            } else {
                m_nFlag &= ~FLAG_GROUND;
            }
            if (m_vPos.y > 100.0f || m_vPos.y < -20.0f) {
                m_fLife = 0.0f;
            }
            if ((m_nFlag & FLAG_GROUND) && !(m_nFlag & FLAG_LAND_EFFECT)) {
                if (CStageMan::GetInstance()->GetHitMaterial(m_vPos) == 3) {
                    SplashEffect(m_vPos);
                }
                m_nFlag |= FLAG_LAND_EFFECT;
            }
        }
    }

    int CDropCoin::CheckBound_()
    {
        float groundY = 0.25678f * m_fScale;
        if (m_vPos.y <= groundY && m_vVel.y < 0.0f) {
            unsigned long attr = CStageMan::GetInstance()->GetFloorAttribute(m_vPos, CStageMan::FLOW_TYPE_0);
            if (attr == 8) {
                m_nFlag = (m_nFlag | FLAG_FALL) & ~FLAG_ENABLE;
                return 0;
            }

            float reduceY = m_pMan->m_fCoinReduceY;
            float reduceXZ = m_pMan->m_fCoinReduceXZ;
            if (m_nFlag & FLAG_STAR_PIECE) {
                reduceY = m_pMan->m_fStarPieceReduceY;
                reduceXZ = m_pMan->m_fStarPieceReduceXZ;
            }
            float velY = m_vVel.y * reduceY;
            m_vPos.y = groundY;
            m_vVel.y = velY;
            m_vVel.x *= reduceXZ;
            m_vVel.z *= reduceXZ;
            if (__fabsf(velY) < 0.02f) {
                m_vVel.y = 0.0f;
            }

            bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
            Reflect(m_vVel, up);

            if ((m_nFlag & FLAG_STAR_PIECE) && !(m_nFlag & FLAG_GROUND)) {
                CGameSnd::Play(0xD8D, m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            m_nFlag = (m_nFlag | FLAG_GROUND) & ~FLAG_NO_GRAVITY;
            if (m_nCount > 0) {
                m_nCount--;
            }
            return 1;
        }
        return 0;
    }

    void CDropCoin::MainBury_()
    {
        float rate = GetFrameRateF();
        switch (m_nCount) {
            case 2: {
                unsigned long attr;
                bcs::FVEC3 flow;
                bcs::FVEC3 pos(m_pMdl->m_vTrans);
                pos.x -= 2.8f;
                CCollMan::GetGrdAttr(pos, attr, flow);
                if (!CCollMan::IsGrdAttrFlow(attr)) {
                    m_nCount--;
                }
                break;
            }
            case 1:
                m_vVel.y += (1.0f / 30.0f) * rate;
                m_vVel.y = Clamp(m_vVel.y, 0.0f, 1.0f);
                m_pMdl->SetAlphaRateF(m_vVel.y);
                if (m_vVel.y >= 1.0f) {
                    m_vVel.Set(0.0f, 0.0f, 0.0f);
                    m_nCount = 0;
                    m_nFlag |= FLAG_ENABLE;
                }
                break;
        }

        if (m_pMdl != 0) {
            m_pMdl->SetRotY(m_pMdl->m_fRotY + rate * 0.08726646f);
        }
    }

    bool CDropCoin::CanbePickup() const
    {
        if ((m_nFlag & FLAG_ENABLE) == 0) {
            return false;
        }
        if (m_pMdl == 0 || IsDelete()) {
            return false;
        }
        return true;
    }

    void CDropCoin::DoParabolaThrow(const bcs::FVEC3 &rTarget, float fTopY, float fFrame)
    {
        if (m_pMdl == 0) {
            return;
        }

        float frame = 1.0f;
        m_vVel.Set(0.0f, 0.0f, 0.0f);
        if (frame < fFrame) {
            frame = fFrame;
        }

        bcs::FVEC3 dir;
        dir.x = rTarget.x - m_vPos.x;
        dir.z = rTarget.z - m_vPos.z;
        float distSq = dir.x * dir.x + dir.z * dir.z;
        if (distSq > 0.0f) {
            float dist = nw4r::math::FSqrt(distSq);
            bcs::BCS_Vec_Normalize(dir, bcs::FVEC3(dir.x, 0.0f, dir.z));
            float speed = dist / frame;
            m_vVel.x = dir.x * speed;
            m_vVel.z = dir.z * speed;
        }
        bcs::BCS_Phys_CalcYVG(frame, m_vPos.y, 0.25678f, fTopY, m_vVel.y, m_fGravity);
    }

    CEffectCoinGetBase::CEffectCoinGetBase(bcs::CTask *pParent)
        : bcs::CUnivTaskEx(pParent, 0x17),
          m_pMdl(0),
          m_fGravity(0.0f),
          m_fWait(0.0f),
          m_fTopY(0.0f),
          m_fScale(0.0f),
          m_fRotSpeed(0.0f),
          m_fLife(0.0f),
          m_nState(0),
          m_hEffect(0)
    {
        m_Vel.Set(0.0f, 0.0f, 0.0f);
    }

    CEffectCoinGetBase::~CEffectCoinGetBase()
    {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
        DeleteScnObj(m_pMdl);
    }

    void CEffectCoinGetBase::CreateMdl_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type, int bSound)
    {
        nw4r::g3d::ResFile archive = CLoadData::GetRes(0x23);
        const COIN_DATA *pData = &s_aCoinData[type];
        nw4r::g3d::ResFile file = nw4r::g3d::ResFile::FromAligned(archive.GetExternalData(pData->pEffectFileName));

        if (pData->pEffectMdlName != 0) {
            nw4r::g3d::ResMdl mdl = file.GetResMdl(pData->pEffectMdlName);
            m_pMdl = new (0x100) bcs::CMdlObj;
            if (m_pMdl != 0) {
                m_pMdl->Create(mdl, 0, 0);
                m_pMdl->m_vTrans = rPos;
                m_pMdl->m_vScale.Set(m_fScale, m_fScale, m_fScale);
                m_pMdl->CalcRootMtx();
                m_pMdl->CalcWorldMatrixAll(0);
                m_pMdl->OnFlag(0x4000);
                bcs::CRenderMan::PushBack(m_pMdl, 2);
            }
        }

        if (bSound && pData->nGetSe >= 0) {
            CGameSnd::Play(pData->nGetSe, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }
}
