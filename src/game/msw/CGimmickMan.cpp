#include "game/msw/CGimmickMan.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItemMan.hpp"
#include "game/msw/CStgGmk_Bubble.hpp"
#include "game/msw/CStgGmk_Karon.hpp"

namespace msw {

    CGimmickMan *CGimmickMan::m_pInstance;

    CGimmickMan *CGimmickMan::GetInstance()
    {
        return m_pInstance;
    }

    CGimmickMan *CGimmickMan::GetInstance_()
    {
        return m_pInstance;
    }

    void CGimmickMan::SetInstance_(CGimmickMan *pInstance)
    {
        m_pInstance = pInstance;
    }

    int CGimmickMan::IsActive()
    {
        return m_pInstance != 0;
    }

    CGimmickMan::CGimmickMan(bcs::CTask *pParent)
        : bcs::CUnivTaskEx(pParent, 0x13),
          m_pDropCoinMan(0),
          m_pQPanelMan(0),
          m_pItemMan(0)
    {
    }

    CGimmickMan::~CGimmickMan()
    {
        m_pInstance = 0;
    }

    void CGimmickMan::Main()
    {
    }

    void CGimmickMan::SetUp()
    {
        if (m_pInstance != 0) {
            m_pInstance->SetUp_();
        }
    }

    void CGimmickMan::SetUp_()
    {
        AllReset_();
        if (m_pQPanelMan != 0) {
            m_pQPanelMan->SetUp();
        }
    }

    void CGimmickMan::AllReset()
    {
        if (m_pInstance != 0) {
            m_pInstance->AllReset_();
        }
    }

    void CGimmickMan::AllReset_()
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->AllReset();
        }
        if (m_pItemMan != 0) {
            m_pItemMan->AllReset();
        }
    }

    void CGimmickMan::QPanelAllReset()
    {
        if (m_pInstance != 0) {
            m_pInstance->QPanelAllReset_();
        }
    }

    void CGimmickMan::QPanelAllReset_()
    {
        if (m_pQPanelMan != 0) {
            m_pQPanelMan->AllReset();
        }
    }

    void CGimmickMan::SendMessage(long nMsg, long a, long b, long c)
    {
        if (m_pInstance != 0) {
            m_pInstance->SendMessage_(nMsg, a, b, c);
        }
    }

    void CGimmickMan::SendMessage_(long nMsg, long a, long b, long c)
    {
        if (m_pQPanelMan != 0) {
            m_pQPanelMan->SendMessage(nMsg, a, b, c);
        }
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->SendMessage(nMsg, a, b, c);
        }
        if (m_pItemMan != 0) {
            m_pItemMan->SendMessage(nMsg, a, b, c);
        }
    }

    CQPanel *CGimmickMan::QPanelGetFirst()
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelGetFirst_();
        }
        return 0;
    }

    CQPanel *CGimmickMan::QPanelGetFirst_()
    {
        if (m_pQPanelMan != 0) {
            return m_pQPanelMan->GetFirst();
        }
        return 0;
    }

    CQPanel *CGimmickMan::QPanelGetNext(CQPanel *pPanel)
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelGetNext_(pPanel);
        }
        return 0;
    }

    CQPanel *CGimmickMan::QPanelGetNext_(CQPanel *pPanel)
    {
        if (m_pQPanelMan != 0) {
            return m_pQPanelMan->GetNext(pPanel);
        }
        return 0;
    }

    CQPanel *CGimmickMan::QPanelGetNear(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelGetNear_(rPos);
        }
        return 0;
    }

    CQPanel *CGimmickMan::QPanelGetNear_(const bcs::FVEC3 &rPos)
    {
        if (m_pQPanelMan != 0) {
            return m_pQPanelMan->GetNear(rPos);
        }
        return 0;
    }

    long CGimmickMan::QPanelChoicePopupItem(long nPlayer, int n)
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelChoicePopupItem_(nPlayer, n);
        }
        return -1;
    }

    long CGimmickMan::QPanelChoicePopupItem_(long nPlayer, int n)
    {
        if (m_pQPanelMan != 0) {
            return m_pQPanelMan->ChoicePopupItem(nPlayer, n);
        }
        return -1;
    }

    void CGimmickMan::QPanelSetViewBaseRot(float fRot)
    {
        if (m_pInstance != 0) {
            m_pInstance->QPanelSetViewBaseRot_(fRot);
        }
    }

    void CGimmickMan::QPanelSetViewBaseRot_(float fRot)
    {
        // The offset of each of the four views.
        static const float s_afViewRot[4] = {0.0f, 3.1415927f, 0.0f, 3.1415927f};

        if (m_pQPanelMan != 0) {
            for (long i = 0; i < 4; i++) {
                float fViewRot = fRot + s_afViewRot[i];
                while (fViewRot > 6.2831855f) {
                    fViewRot -= 6.2831855f;
                }
                while (fViewRot < 0.0f) {
                    fViewRot += 6.2831855f;
                }
                m_pQPanelMan->SetViewBaseRot(i, fViewRot);
            }
        }
    }

    long CGimmickMan::QPanelCheck(const bcs::FVEC3 &rPos, long nTeam, int a, int b, int c, long *pItem,
                                  CItem **ppItem)
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelCheck_(rPos, nTeam, a, b, c, pItem, ppItem);
        }
        return 0;
    }

    long CGimmickMan::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, int a, int b, int c, long *pItem,
                                   CItem **ppItem)
    {
        if (m_pQPanelMan != 0) {
            unsigned long nFlag = 0;
            if (a) {
                nFlag |= 1;
            }
            if (b) {
                nFlag |= 2;
            }
            if (c) {
                nFlag |= 4;
            }
            if (nTeam >= 0 && nTeam < 4 && CGame::GetTeamItem(nTeam, 0) >= 0) {
                nFlag |= 8;
            }
            return m_pQPanelMan->QPanelCheck(rPos, nTeam, nFlag, pItem, ppItem);
        }
        return 0;
    }

    long CGimmickMan::QPanelCheck(const bcs::FVEC3 &rPos, const bcs::FVEC3 &, long nTeam, int a, int b, int,
                                  int d, long *pItem, CItem **ppItem)
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelCheck_(rPos, nTeam, a, b, d, pItem, ppItem);
        }
        return 0;
    }

    long CGimmickMan::FloorHitCheck(const bcs::FVEC3 &rPos, long n)
    {
        if (m_pInstance != 0) {
            return m_pInstance->FloorHitCheck_(rPos, n);
        }
        return 0;
    }

    long CGimmickMan::FloorHitCheck_(const bcs::FVEC3 &, long)
    {
        return 0;
    }

    void CGimmickMan::QPanelSetItemPopRate(const CQPanelMan::ITEM_POP_SET_ARG *pArg, long n)
    {
        if (m_pInstance != 0) {
            m_pInstance->QPanelSetItemPopRate_(pArg, n);
        }
    }

    void CGimmickMan::QPanelSetItemPopRate_(const CQPanelMan::ITEM_POP_SET_ARG *pArg, long n)
    {
        if (m_pQPanelMan != 0) {
            m_pQPanelMan->SetItemPopRate(pArg, n);
        }
    }

    long CGimmickMan::QPanelGetPopupItemTypeNum_(long n)
    {
        if (m_pQPanelMan != 0) {
            return m_pQPanelMan->GetPopupItemTypeNum(n);
        }
        return 0;
    }

    long CGimmickMan::QPanelGetSubstituteItem(long n)
    {
        if (m_pInstance != 0) {
            return m_pInstance->QPanelGetSubstituteItem_(n);
        }
        return 0;
    }

    long CGimmickMan::QPanelGetSubstituteItem_(long n)
    {
        if (m_pQPanelMan != 0) {
            return m_pQPanelMan->GetSubstituteItem(n);
        }
        return 0;
    }

    CDropCoin *CGimmickMan::CreateCoinSimple()
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateCoinSimple_();
        }
        return 0;
    }

    CDropCoin *CGimmickMan::CreateCoinSimple_()
    {
        if (m_pDropCoinMan != 0) {
            return m_pDropCoinMan->CreateCoinSimple();
        }
        return 0;
    }

    long CGimmickMan::CreateDropCoin(const bcs::FVEC3 &rPos, long nNum)
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateDropCoin_(rPos, nNum);
        }
        return 0;
    }

    long CGimmickMan::CreateDropCoin_(const bcs::FVEC3 &rPos, long nNum)
    {
        if (m_pDropCoinMan != 0) {
            return m_pDropCoinMan->CreateCoin(rPos, nNum, 0.0f, 0.0f);
        }
        return 0;
    }

    long CGimmickMan::CreateBuryCoin(const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, long nNum)
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateBuryCoin_(rMin, rMax, nNum);
        }
        return 0;
    }

    long CGimmickMan::CreateBuryCoin_(const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, long nNum)
    {
        if (m_pDropCoinMan != 0) {
            return m_pDropCoinMan->CreateBuryCoin(rMin, rMax, nNum);
        }
        return 0;
    }

    void CGimmickMan::DropCoinPickup(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                                     void (*pFunc)(const CDropCoin *, void *), void *pArg)
    {
        if (m_pInstance != 0) {
            m_pInstance->DropCoinPickup_(rPos, fRadius, fHeight, pFunc, pArg);
        }
    }

    void CGimmickMan::DropCoinPickup_(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                                      void (*pFunc)(const CDropCoin *, void *), void *pArg)
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->DropCoinPickup(rPos, fRadius, fHeight, pFunc, pArg);
        }
    }

    CDropCoin *CGimmickMan::DropCoinGetFirst()
    {
        if (m_pInstance != 0) {
            return m_pInstance->DropCoinGetFirst_();
        }
        return 0;
    }

    CDropCoin *CGimmickMan::DropCoinGetFirst_()
    {
        if (m_pDropCoinMan != 0) {
            return m_pDropCoinMan->DropCoinGetFirst();
        }
        return 0;
    }

    CDropCoin *CGimmickMan::DropCoinGetNext(CDropCoin *pCoin)
    {
        if (m_pInstance != 0) {
            return m_pInstance->DropCoinGetNext_(pCoin);
        }
        return 0;
    }

    CDropCoin *CGimmickMan::DropCoinGetNext_(CDropCoin *pCoin)
    {
        if (m_pDropCoinMan != 0) {
            return m_pDropCoinMan->DropCoinGetNext(pCoin);
        }
        return 0;
    }

    void CGimmickMan::EffectCoinGetCreate(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
    {
        if (m_pInstance != 0) {
            m_pInstance->EffectCoinGetCreate_(rPos, type);
        }
    }

    void CGimmickMan::EffectCoinGetCreate_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->EffectCoinGetCreate(rPos, type);
        }
    }

    void CGimmickMan::EffectCoinScoreCreate(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget, long n)
    {
        if (m_pInstance != 0) {
            m_pInstance->EffectCoinScoreCreate_(rPos, rTarget, n);
        }
    }

    void CGimmickMan::EffectCoinScoreCreate_(const bcs::FVEC3 &, const bcs::FVEC3 &, long)
    {
    }

    void CGimmickMan::CreateStarPiece(DROP_COIN::CoinType type, long nValue, const bcs::FVEC3 &rPos,
                                      const bcs::FVEC3 &rVel, long nBound)
    {
        if (m_pInstance != 0) {
            m_pInstance->CreateStarPiece_(type, nValue, rPos, rVel, nBound);
        }
    }

    void CGimmickMan::CreateStarPiece_(DROP_COIN::CoinType type, long nValue, const bcs::FVEC3 &rPos,
                                       const bcs::FVEC3 &rVel, long nBound)
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->CreateStarPiece(type, nValue, rPos, rVel, nBound);
        }
    }

    long CGimmickMan::GetStarPieceNum()
    {
        if (m_pInstance != 0) {
            return m_pInstance->GetStarPieceNum_();
        }
        return 0;
    }

    long CGimmickMan::GetStarPieceNum_() const
    {
        if (m_pDropCoinMan != 0) {
            return m_pDropCoinMan->GetStarPieceNum();
        }
        return 0;
    }

    void CGimmickMan::DeleteAllStarPiece()
    {
        if (m_pInstance != 0) {
            m_pInstance->DeleteAllStarPiece_();
        }
    }

    void CGimmickMan::DeleteAllStarPiece_()
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->DeleteAllStarPiece();
        }
    }

    void CGimmickMan::SetStarPieceRefelctVelReduce(float fY, float fXZ)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetStarPieceRefelctVelReduce_(fY, fXZ);
        }
    }

    void CGimmickMan::SetStarPieceRefelctVelReduce_(float fY, float fXZ)
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->SetStarPieceRefelctVelReduce(fY, fXZ);
        }
    }

    void CGimmickMan::SetCoinRefelctVelReduce(float fY, float fXZ)
    {
        if (m_pInstance != 0) {
            m_pInstance->SetCoinRefelctVelReduce_(fY, fXZ);
        }
    }

    void CGimmickMan::SetCoinRefelctVelReduce_(float fY, float fXZ)
    {
        if (m_pDropCoinMan != 0) {
            m_pDropCoinMan->SetCoinRefelctVelReduce(fY, fXZ);
        }
    }

    CItem *CGimmickMan::CreateItem(long nType, const bcs::FVEC3 &rPos)
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateItem_(nType, rPos);
        }
        return 0;
    }

    CItem *CGimmickMan::CreateItem_(long nType, const bcs::FVEC3 &rPos)
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->CreateItem(nType, rPos);
        }
        return 0;
    }

    CItem *CGimmickMan::CreateStockItem(long nType, CChrCore *pChr)
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateStockItem_(nType, pChr);
        }
        return 0;
    }

    CItem *CGimmickMan::CreateStockItem_(long nType, CChrCore *pChr)
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->CreateStockItem(nType, pChr);
        }
        return 0;
    }

    CItem *CGimmickMan::CreateThrowItem(long nType, const bcs::FVEC3 &rPos)
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateThrowItem_(nType, rPos);
        }
        return 0;
    }

    CItem *CGimmickMan::CreateThrowItem_(long nType, const bcs::FVEC3 &rPos)
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->CreateThrowItem(nType, rPos);
        }
        return 0;
    }

    CItem *CGimmickMan::CreateDriftItem(long nType, const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, float a,
                                        float b)
    {
        if (m_pInstance != 0) {
            return m_pInstance->CreateDriftItem_(nType, rMin, rMax, a, b);
        }
        return 0;
    }

    // A drifting item at a random place on the floor between rMin and rMax.
    CItem *CGimmickMan::CreateDriftItem_(long nType, const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, float a,
                                         float b)
    {
        if (m_pItemMan == 0) {
            return 0;
        }

        float fMinX = rMin.x;
        float fMaxX = rMax.x;
        float fMinZ = rMin.z;
        float fMaxZ = rMax.z;
        bcs::FVEC3 pos;
        pos.x = fMinX + bcs::BCS_RandMax(1.0f) * (fMaxX - fMinX);
        pos.y = 0.0f;
        pos.z = fMinZ + bcs::BCS_RandMax(1.0f) * (fMaxZ - fMinZ);
        return m_pItemMan->CreateDriftItem(nType, pos, a, b);
    }

    void CGimmickMan::ItemPickup_(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                                  void (*pFunc)(const CItem *, void *), void *pArg)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ItemPickup(rPos, fRadius, fHeight, pFunc, pArg);
        }
    }

    void CGimmickMan::ItemPickup(CCollBodyUnit *pBody, void (*pFunc)(const CItem *, void *), void *pArg)
    {
        if (m_pInstance != 0 && pBody != 0) {
            m_pInstance->ItemPickup_(pBody, pFunc, pArg);
        }
    }

    void CGimmickMan::ItemPickup_(CCollBodyUnit *pBody, void (*pFunc)(const CItem *, void *), void *pArg)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ItemPickup(pBody, pFunc, pArg);
        }
    }

    void CGimmickMan::EffectItemGetCreate_(long nType, const bcs::FVEC3 &rPos, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->EffectItemGetCreate(nType, rPos, f);
        }
    }

    void *CGimmickMan::ItemGetIconTexObj(long nType, int n)
    {
        if (m_pInstance != 0) {
            return m_pInstance->ItemGetIconTexObj_(nType, n);
        }
        return 0;
    }

    void *CGimmickMan::ItemGetIconTexObj_(long nType, int n)
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->GetIconTexObj(nType, n);
        }
        return 0;
    }

    long CGimmickMan::ItemGetNum_()
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->ItemGetNum();
        }
        return 0;
    }

    CItem *CGimmickMan::ItemGetFirst()
    {
        if (m_pInstance != 0) {
            return m_pInstance->ItemGetFirst_();
        }
        return 0;
    }

    CItem *CGimmickMan::ItemGetFirst_()
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->ItemGetFirst();
        }
        return 0;
    }

    CItem *CGimmickMan::ItemGetNext(CItem *pItem)
    {
        if (m_pInstance != 0) {
            return m_pInstance->ItemGetNext_(pItem);
        }
        return 0;
    }

    CItem *CGimmickMan::ItemGetNext_(CItem *pItem)
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->ItemGetNext(pItem);
        }
        return 0;
    }

    long CGimmickMan::ThrowItemGetNum()
    {
        if (m_pInstance != 0) {
            return m_pInstance->ThrowItemGetNum_();
        }
        return 0;
    }

    long CGimmickMan::ThrowItemGetNum_()
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->ThrowItemGetNum();
        }
        return 0;
    }

    CItem *CGimmickMan::ThrowItemGetFirst()
    {
        if (m_pInstance != 0) {
            return m_pInstance->ThrowItemGetFirst_();
        }
        return 0;
    }

    CItem *CGimmickMan::ThrowItemGetFirst_()
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->ThrowItemGetFirst();
        }
        return 0;
    }

    CItem *CGimmickMan::ThrowItemGetNext(CItem *pItem)
    {
        if (m_pInstance != 0) {
            return m_pInstance->ThrowItemGetNext_(pItem);
        }
        return 0;
    }

    CItem *CGimmickMan::ThrowItemGetNext_(CItem *pItem)
    {
        if (m_pItemMan != 0) {
            return m_pItemMan->ThrowItemGetNext(pItem);
        }
        return 0;
    }

    void CGimmickMan::ItemExParamInitialize()
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamInitialize_();
        }
    }

    void CGimmickMan::ItemExParamInitialize_()
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamInitialize();
        }
    }

    void CGimmickMan::ItemExParamSet_Life(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_Life_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_Life_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_Life(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_DispScale(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_DispScale_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_DispScale_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_DispScale(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_ThrowSpeed(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_ThrowSpeed_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_ThrowSpeed_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_ThrowSpeed(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_TurnSpeed(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_TurnSpeed_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_TurnSpeed_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_TurnSpeed(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_EffectFrame(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_EffectFrame_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_EffectFrame_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_EffectFrame(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_DropCoinRate(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_DropCoinRate_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_DropCoinRate_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_DropCoinRate(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_DropCoinMin(long nType, long n)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_DropCoinMin_(nType, n);
        }
    }

    void CGimmickMan::ItemExParamSet_DropCoinMin_(long nType, long n)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_DropCoinMin(nType, n);
        }
    }

    void CGimmickMan::ItemExParamSet_DropCoinMax(long nType, long n)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_DropCoinMax_(nType, n);
        }
    }

    void CGimmickMan::ItemExParamSet_DropCoinMax_(long nType, long n)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_DropCoinMax(nType, n);
        }
    }

    void CGimmickMan::ItemExParamSet_ReboundNum(long nType, long n)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_ReboundNum_(nType, n);
        }
    }

    void CGimmickMan::ItemExParamSet_ReboundNum_(long nType, long n)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_ReboundNum(nType, n);
        }
    }

    void CGimmickMan::ItemExParamSet_PThrowDist(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_PThrowDist_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_PThrowDist_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_PThrowDist(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_PThrowHeight(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_PThrowHeight_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_PThrowHeight_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_PThrowHeight(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_PThrowFrame(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_PThrowFrame_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_PThrowFrame_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_PThrowFrame(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_ExplodeRange(long nType, float f)
    {
        if (m_pInstance != 0) {
            m_pInstance->ItemExParamSet_ExplodeRange_(nType, f);
        }
    }

    void CGimmickMan::ItemExParamSet_ExplodeRange_(long nType, float f)
    {
        if (m_pItemMan != 0) {
            m_pItemMan->ExParamSet_ExplodeRange(nType, f);
        }
    }

    CStgGmk_Bubble *CGimmickMan::StgGmkCreate_Bubble(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance != 0) {
            return m_pInstance->StgGmkCreate_Bubble_(rPos);
        }
        return 0;
    }

    CStgGmk_Bubble *CGimmickMan::StgGmkCreate_Bubble_(const bcs::FVEC3 &rPos)
    {
        if (CGame::g_GameWork.GetStage() != 11) {
            return 0;
        }
        return new (0x100) CStgGmk_Bubble(CGame::GetTempParent(), rPos);
    }

    CStgGmk_Karon *CGimmickMan::StgGmkCreate_Karon(const bcs::FVEC3 &rPos)
    {
        if (m_pInstance != 0) {
            return m_pInstance->StgGmkCreate_Karon_(rPos);
        }
        return 0;
    }

    CStgGmk_Karon *CGimmickMan::StgGmkCreate_Karon_(const bcs::FVEC3 &rPos)
    {
        if (CGame::g_GameWork.GetStage() != 15) {
            return 0;
        }
        return new (0x100) CStgGmk_Karon(CGame::GetTempParent(), rPos, -1.0f);
    }

    // nType 0 is a blast in the air, which becomes a ground blast (1) when
    // it is low enough; a blast in the air adds a ground effect when it is
    // near the ground. nQuake, when not negative, shakes the camera.
    void CGimmickMan::EffectBombExplosionCreate(const bcs::FVEC3 &rPos, long nType, float fScale, long nQuake)
    {
        bcs::FVEC3 pos = rPos;
        if (nType == 0 && pos.y <= 1.0f) {
            nType = 1;
            pos.y = 0.0f;
        }

        unsigned long hBlast;
        unsigned long hGround = 0;
        if (nType == 1) {
            hBlast = CEffectCtrl::CreateEffect(6, 3, 0, 0, 0);
        } else {
            hBlast = CEffectCtrl::CreateEffect(7, 3, 0, 0, 0);
            if (pos.y <= 3.0f) {
                hGround = CEffectCtrl::CreateEffect(8, 3, 0, 0, 0);
            }
        }

        CEffectCluster *pEffect = CEffectCtrl::GetInstance()->FindList(hBlast);
        if (pEffect != 0) {
            pEffect->SetTranslate(&pos);
            bcs::FVEC3 scale(fScale, fScale, fScale);
            pEffect->SetScale(&scale);
        }

        pEffect = CEffectCtrl::GetInstance()->FindList(hGround);
        if (pEffect != 0) {
            pos.y = 0.0f;
            pEffect->SetTranslate(&pos);
            bcs::FVEC3 scale(fScale, fScale, fScale);
            pEffect->SetScale(&scale);
        }

        if (nQuake >= 0) {
            CGame::StartCameraQuake(nQuake, -1, -1);
        }
    }

    void CGimmickMan::Initialize_()
    {
    }
}
