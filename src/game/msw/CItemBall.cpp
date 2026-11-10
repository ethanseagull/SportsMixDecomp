#include "game/msw/CItemBall.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CItemBallChangeEffect.hpp"
#include "game/msw/CItemBallShellWreck.hpp"
#include "game/msw/CLoadData.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"

// One source file for the item behaviour of a ball (CItemBall), the wreck
// of a broken shell ball (CItemBallShellWreck) and the effect played while
// a ball changes into an item ball (CItemBallChangeEffect): the three share
// their float constants.

namespace {

    // The item ball models in the item archive (CLoadData resource 0x23), by
    // item kind.
    const char *s_aItemBallFileName[] = {
        "IB03.brres",
        "IB04.brres",
        "IB00.brres",
        "IB02.brres",
        "IB07.brres",
        "IB01.brres",
        "IB03.brres",
    };

    // The model file of an item ball kind. (guessed to be an inline helper
    // in the original too: the archive handle takes the lowest stack slot,
    // after CreateMdl_'s own locals)
    inline nw4r::g3d::ResFile GetItemBallFile(long type)
    {
        nw4r::g3d::ResFile archive = msw::CLoadData::GetRes(0x23);
        return nw4r::g3d::ResFile::FromAligned(archive.GetExternalData(s_aItemBallFileName[type]));
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
}

namespace msw {

    CItemBall::CItemBall(CBall *pBall, long type)
        : bcs::CUnivTaskEx(pBall, 0x16), m_nBallId(0), m_nTrgFlag(0), m_pMdl(0), m_pShadow(0), m_pRef(0), m_nStep(0),
          m_nType(type), m_bChgDemoDisp(0), m_pEffectMdl(0)
    {
        m_nBallId = pBall->GetId();
        m_vPos = pBall->GetPos();
        if (type >= 0) {
            CreateMdl_(type);
        }
    }

    CItemBall::~CItemBall()
    {
        DeleteScnObj(m_pEffectMdl);
        DeleteScnObj(m_pRef);
        DeleteScnObj(m_pShadow);
        DeleteScnObj(m_pMdl);
        m_nBallId = 0;
    }

    void CItemBall::CreateMdl_(long type)
    {
        nw4r::g3d::ResFile file = GetItemBallFile(type);

        nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
        m_pMdl = new (0x100) bcs::CMdlObj;

        unsigned long anmFlag = 0;
        file.HasResAnmChr(); // (guessed) tested, but adds no flag
        if (file.HasResAnmClr()) {
            anmFlag |= 0x108;
        }
        if (file.HasResAnmTexSrt()) {
            anmFlag |= 0x204;
        }
        if (file.HasResAnmTexPat()) {
            anmFlag |= 0x3;
        }
        if (file.HasResAnmShp()) {
            anmFlag |= 0x7000;
        }
        m_pMdl->Create(mdl, anmFlag, 0);
        m_pMdl->OnFlag(0x100);
        m_pMdl->OnFlag(0x2);
        m_pMdl->OnFlag(0x2000);

        if (file.HasResAnmClr()) {
            nw4r::g3d::ResAnmClr res = file.GetResAnmClr(0);
            bcs::CAnmMatClr *pAnm = new (0x100) bcs::CAnmMatClr;
            if (pAnm != 0) {
                pAnm->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
            }
        }
        if (file.HasResAnmTexSrt()) {
            nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(0);
            bcs::CAnmTexSrt *pAnm = new (0x100) bcs::CAnmTexSrt;
            if (pAnm != 0) {
                pAnm->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
            }
        }
        if (file.HasResAnmTexPat()) {
            nw4r::g3d::ResAnmTexPat res = file.GetResAnmTexPat(0);
            bcs::CAnmTexPat *pAnm = new (0x100) bcs::CAnmTexPat;
            if (pAnm != 0) {
                pAnm->Create(m_pMdl, res, 0, 0);
                m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_3);
            }
        }

        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            m_pMdl->AttachToObject(pBall->GetMdlObj(), 0, bcs::CScnObj::ATTACHMODE_0);
            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
        }
        bcs::CRenderMan::PushBack(m_pMdl, 2);

        nw4r::g3d::ResMdl shadowMdl = file.GetResMdl(1);
        m_pShadow = new (0x100) bcs::CMdlShadow(m_pMdl, shadowMdl, 0x20, 0, 0);
        bcs::CRenderMan::PushBack(m_pShadow, 2);

        nw4r::g3d::ResMdl refMdl = file.GetResMdl(2);
        m_pRef = new (0x100) bcs::CMdlRef(m_pMdl, refMdl, 0, 0, 0);
        bcs::CRenderMan::PushBack(m_pRef, 4);
    }

    void CItemBall::Main()
    {
        switch (m_nStep) {
            case 0:
                if (m_nBallId == 0 || bcs::CTaskMan::GetTask(m_nBallId) == 0) {
                    TrgDestroy_();
                    break;
                }
                MainFunc_();
                if (m_pMdl != 0) {
                    m_pMdl->UpdateFrame(GetFrameRateF());
                    m_pMdl->CalcWorldMatrixAll(0);
                }
                {
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
                    if (pBall != 0) {
                        m_vPos = pBall->GetPos();
                    }
                }
                if (m_pEffectMdl != 0) {
                    bcs::FVEC3 pos = m_vPos;
                    pos.y += 0.5f;
                    m_pEffectMdl->m_vTrans = pos;
                }
                break;
            case 1:
                m_nStep++;
                break;
            case 2:
                m_nStep++;
                Delete();
                break;
        }
    }

    void CItemBall::MainFunc_()
    {
    }

    int CItemBall::IsChgDemoDisp() const
    {
        return m_bChgDemoDisp;
    }

    // While the change effect shows the item behaviour (bDisp), the item's
    // own models are hidden.
    void CItemBall::ChgDemoDisp(int bDisp)
    {
        m_bChgDemoDisp = bDisp;
        CBall *pBall = GetBall();
        if (pBall != 0) {
            pBall->UpdateDisp();
        }

        bool bShow = m_bChgDemoDisp == 0;
        if (m_pMdl != 0) {
            if (!bShow) {
                m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
        if (m_pShadow != 0) {
            if (!bShow) {
                m_pShadow->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pShadow->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
        if (m_pRef != 0) {
            if (!bShow) {
                m_pRef->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pRef->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
        if (m_pEffectMdl != 0) {
            if (!bShow) {
                m_pEffectMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                m_pEffectMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
    }

    bool CItemBall::DoTrg(long trg)
    {
        switch (trg) {
            case TRG_SHOOT_START:
                m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
                return TrgShootStart_();
            case TRG_REBOUND:
                m_nTrgFlag |= 1 << (TRG_REBOUND + 1);
                return TrgRebound_();
            case TRG_REBOUND_TO_FREE:
                m_nTrgFlag |= 1 << (TRG_REBOUND_TO_FREE + 1);
                return TrgReboundToFree_();
            case TRG_GOAL_IN:
                m_nTrgFlag |= 1 << (TRG_GOAL_IN + 1);
                return TrgGoalIn_();
            case TRG_FREE_MODE:
                m_nTrgFlag |= 1 << (TRG_FREE_MODE + 1);
                return TrgFreeMode_();
            case TRG_CHR_HIT:
                m_nTrgFlag |= 1 << (TRG_CHR_HIT + 1);
                return TrgChrHit_();
            case TRG_ATTACH:
                m_nTrgFlag |= 1 << (TRG_ATTACH + 1);
                return TrgAttach_();
            case TRG_DAMAGE:
                m_nTrgFlag |= 1 << (TRG_DAMAGE + 1);
                return TrgDamage_();
            case TRG_DESTROY:
                return TrgDestroy_();
            case TRG_CONSTRUCT:
                return TrgConstruct_();
        }
        return false;
    }

    bool CItemBall::TrgShootStart_()
    {
        m_nTrgFlag |= 1 << (TRG_SHOOT_START + 1);
        return false;
    }

    bool CItemBall::TrgRebound_()
    {
        return true;
    }

    bool CItemBall::TrgGoalIn_()
    {
        return true;
    }

    // The next four hold once the ball has been shot.
    bool CItemBall::TrgReboundToFree_()
    {
        return (m_nTrgFlag >> (TRG_SHOOT_START + 1)) & 1;
    }

    bool CItemBall::TrgFreeMode_()
    {
        return (m_nTrgFlag >> (TRG_SHOOT_START + 1)) & 1;
    }

    bool CItemBall::TrgChrHit_()
    {
        return (m_nTrgFlag >> (TRG_SHOOT_START + 1)) & 1;
    }

    bool CItemBall::TrgAttach_()
    {
        return (m_nTrgFlag >> (TRG_SHOOT_START + 1)) & 1;
    }

    bool CItemBall::TrgDamage_()
    {
        return true;
    }

    // Detaches the models from the ball and lets Main delete the task.
    bool CItemBall::TrgDestroy_()
    {
        if (m_nStep == 0) {
            if (m_pMdl != 0) {
                m_pMdl->DetachFromParent();
            }
            if (m_pShadow != 0) {
                m_pShadow->DetachFromParent();
            }
            if (m_pRef != 0) {
                m_pRef->DetachFromParent();
            }
            m_nStep++;
            m_nTrgFlag |= 1;
        }
        return true;
    }

    bool CItemBall::TrgConstruct_()
    {
        return false;
    }

    int CItemBall::IsEnd() const
    {
        if (IsDelete()) {
            return 1;
        }
        return m_nTrgFlag & 1;
    }

    CBall *CItemBall::GetBall() const
    {
        if (m_nBallId == 0) {
            return 0;
        }
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0 && !pBall->IsDelete()) {
            return pBall;
        }
        return 0;
    }

    CItemBallShellWreck::CItemBallShellWreck(CBall *pBall, long nItemKind)
        : bcs::CUnivTaskEx(pBall, 0x17), m_nStep(0), m_fTimer(30.0f), m_pMdl(0)
    {
        nw4r::g3d::ResFile file = CItem::GetItemRsrcFile(nItemKind);
        nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
        m_pMdl = new (0x101) bcs::CMdlObj;
        m_vPos = pBall->GetPos();
        m_pMdl->Create(mdl, 0, 0);
        m_pMdl->m_vTrans = m_vPos;

        bcs::FVEC3 vel = pBall->GetVelocity();
        m_pMdl->SetRotY(WrapRad(nw4r::math::Atan2Rad(vel.x, vel.z)));
        m_pMdl->SetExBlendMode(1);
        m_pMdl->CalcRootMtx();
        bcs::CRenderMan::PushBack(m_pMdl, 2);

        // Thrown back and up.
        bcs::BCS_VecZ_RotY(m_vVel, -0.07f, m_pMdl->m_fRotY);
        m_vVel.y = 0.1f;
        m_fGravity = -0.00625f;
    }

    CItemBallShellWreck::~CItemBallShellWreck()
    {
        DeleteScnObj(m_pMdl);
    }

    void CItemBallShellWreck::Main()
    {
        switch (m_nStep) {
            case 0: {
                if (m_pMdl == 0) {
                    m_nStep++;
                    break;
                }
                float rate = GetFrameRateF();
                float fall = m_fGravity * rate;
                bcs::FVEC3 move;
                bcs::FVEC3Scale(&move, &m_vVel, rate);
                move.y += 0.5f * (fall * rate);
                m_vVel.y += fall;
                bcs::FVEC3Add(&m_vPos, &m_vPos, &move);
                if (m_pMdl != 0) {
                    m_pMdl->m_vTrans = m_vPos;
                    m_pMdl->CalcRootMtx();
                }

                if (m_fTimer > 0.0f) {
                    m_fTimer -= rate;
                }
                if (m_fTimer <= 0.0f) {
                    if (m_pMdl != 0) {
                        m_pMdl->DetachFromParent();
                        m_nStep++;
                    }
                } else if (m_fTimer <= 30.0f && m_pMdl != 0) {
                    // Tips over and fades out.
                    float t = Clamp(1.0f - (1.0f / 30.0f) * m_fTimer, 0.0f, 1.0f);
                    float rotX = -3.1415927f * nw4r::math::SinRad(1.5707964f * t);
                    m_pMdl->SetRotX(WrapRad(rotX));
                    m_pMdl->SetAlphaRateF((1.0f / 30.0f) * m_fTimer);
                }
                break;
            }
            case 1:
                m_nStep++;
                break;
            default:
                Delete();
                break;
        }
    }

    CItemBallChangeEffect::CItemBallChangeEffect(bcs::CTask *pParent, unsigned long nBallId)
        : bcs::CTask(pParent, 0x19), m_nBallId(nBallId), m_nStep(0), m_fTimer(0.0f), m_fBlinkTimer(0.0f)
    {
        ClearUnk08(4);
        m_fTime = 60.0f;
        CGame::StartStopEffect(m_fTime, 16.0f, 0);
    }

    CItemBallChangeEffect::~CItemBallChangeEffect()
    {
    }

    void CItemBallChangeEffect::UpdateItemDisp(int bEnd)
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall == 0) {
            return;
        }
        CItemBall *pItem = pBall->GetItemEffect_();
        if (pItem == 0) {
            return;
        }
        if (bEnd) {
            pItem->ChgDemoDisp(0);
            return;
        }
        if (m_fBlinkTimer >= 0.5f * m_fTime) {
            pItem->ChgDemoDisp(1);
        } else {
            pItem->ChgDemoDisp(0);
        }
        if (m_fBlinkTimer >= m_fTime) {
            m_fBlinkTimer -= m_fTime;
        }
    }

    void CItemBallChangeEffect::Main()
    {
        float rate = GetFrameRateF();
        m_fTimer += rate;
        m_fBlinkTimer += 4.0f * rate;
        switch (m_nStep) {
            case 0:
                m_nStep++;
                break;
            case 1:
                SetStartEffect();
                m_nStep++;
                break;
            case 2:
                if (m_fTimer >= m_fTime) {
                    UpdateItemDisp(1);
                    SetEndEffect();
                    Delete();
                    m_nStep++;
                } else {
                    UpdateItemDisp(0);
                }
                break;
            default:
                if (!IsDelete()) {
                    Delete();
                }
                break;
        }
    }
}
