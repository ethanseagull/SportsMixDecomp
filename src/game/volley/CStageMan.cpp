// msw::CStageManVolley: the stage manager of the volley game.
#include "game/msw/CStageManVolley.hpp"

#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageFactoryVolley.hpp"
#include "game/msw/CStageNetVolley.hpp"

namespace msw {

    namespace {

        // (name guessed) A figure with its shadow that stands at one end of
        // the court of stage 7; OnPostInitialize places two. It plays motion
        // 1 and (guessed from CFgMdl) fades by the direction it is seen from.
        // The class has no name in the module (its functions and vtable are
        // not exported), so it was most likely local to this file.
        class CStageVolleyFigure : public bcs::CTask {
            public:
                CStageVolleyFigure(bcs::CTask *pParent, nw4r::g3d::ResFile res, const bcs::FVEC3 &rPos,
                                   const bcs::FVEC3 &rRot, const bcs::FVEC3 &rScale);
                virtual ~CStageVolleyFigure();
                virtual void Main();

            private:
                CFgMdl *m_pMdl;              // 0x34
                bcs::CMdlRef *m_pRef;        // 0x38
                bcs::CMdlShadow *m_pShadow;  // 0x3C
                bcs::CAnmMotion *m_pMotion;  // 0x40
                float m_Unk44;               // 0x44 (not used)
        };

        // An angle brought into [0, 2 pi).
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

        // Sets the rotation of pObj, each angle brought into [0, 2 pi).
        inline void SetRot(bcs::CScnObj *pObj, const bcs::FVEC3 &rRot)
        {
            float x = rRot.x;
            float y = rRot.y;
            float z = rRot.z;

            pObj->m_fRotX = WrapRad(x);
            pObj->m_fRotY = WrapRad(y);
            pObj->m_fRotZ = WrapRad(z);
        }
    }
}

// `res` is not used: the figure's model and motion files are fixed.
msw::CStageVolleyFigure::CStageVolleyFigure(bcs::CTask *pParent, nw4r::g3d::ResFile res, const bcs::FVEC3 &rPos,
                                            const bcs::FVEC3 &rRot, const bcs::FVEC3 &rScale)
    : bcs::CTask(pParent, -1), m_pMdl(0), m_pRef(0), m_pShadow(0), m_pMotion(0), m_Unk44(0.0f)
{
    nw4r::g3d::ResFile mdlRes = CLoadData::GetRes(0xC);
    nw4r::g3d::ResFile motRes = CLoadData::GetRes(0x1C);

    m_pMdl = new (0) CFgMdl;
    m_pMdl->Create(mdlRes.GetResMdl("body_h"), 0, 0);
    m_pMdl->m_vTrans = rPos;
    SetRot(m_pMdl, rRot);
    m_pMdl->m_vScale = rScale;
    m_pMdl->CalcRootMtx();
    m_pMdl->SetExBlendMode(1);

    m_pMotion = new (0) bcs::CAnmMotion;
    m_pMotion->Create(m_pMdl, motRes, CC_DATA::GetNpcMdl_NodeExInfo(2), 3, 0);
    m_pMdl->AttachAnmObj(m_pMotion, bcs::CMdlObj::ANMOBJTYPE_CHR);
    m_pMotion->Play(1, 2, 1.0f, 0.0f, 0.0f);
    m_pMdl->OnFlag(0x2000);
    bcs::CRenderMan::PushBack(m_pMdl, 2);

    m_pShadow = new (0) bcs::CMdlShadow;
    m_pShadow->Create(mdlRes.GetResMdl("body_h"), 0x40, 0, 0);
    m_pShadow->SetLink(m_pMdl);
    bcs::CRenderMan::PushBack(m_pShadow, 2);

    m_pRef = new (0) bcs::CMdlRef;
    m_pRef->Create(mdlRes.GetResMdl("body_h"), 0, 0);
    m_pRef->SetLink(m_pMdl, 0);
    bcs::CRenderMan::PushBack(m_pRef, 4);
}

// The motion object is not deleted here (as in the original).
msw::CStageVolleyFigure::~CStageVolleyFigure()
{
    if (m_pShadow != 0) {
        delete m_pShadow;
    }
    m_pShadow = 0;
    if (m_pRef != 0) {
        delete m_pRef;
    }
    m_pRef = 0;
    if (m_pMdl != 0) {
        delete m_pMdl;
    }
    m_pMdl = 0;
}

void msw::CStageVolleyFigure::Main()
{
    CFgMdl *pMdl = m_pMdl;

    if (pMdl != 0) {
        // (meaning guessed) The fade is centred on the figure's back, with
        // two angles of 100 degrees.
        pMdl->SetFadeDir(pMdl->m_fRotY + 3.1415927f);
        pMdl->SetFadeAngle0(1.7453293f);
        pMdl->SetFadeAngle1(1.7453293f);
        pMdl->SetFadeEnable(1);
    }
}

msw::CStageManVolley::CStageManVolley(bcs::CTask *pParent) : CStageMan(pParent), m_pNet(0)
{
}

msw::CStageManVolley::~CStageManVolley()
{
}

msw::CStageManVolley *msw::CStageManVolley::CreateInstance(bcs::CTask *pParent, long stageNo)
{
    CStageManVolley *pStageMan = CStageFactoryVolley::Create(stageNo, pParent);

    pStageMan->SetStageNo(stageNo);
    return pStageMan;
}

float msw::CStageManVolley::GetBaseCourtWidth_() const
{
    return 11.6f;
}

float msw::CStageManVolley::GetBaseCourtDepth_() const
{
    return 10.8f;
}

float msw::CStageManVolley::GetBaseCourtFrontZoneDepth_() const
{
    return 1.8f;
}

float msw::CStageManVolley::GetBaseGroundWidth_() const
{
    return 3.0f;
}

float msw::CStageManVolley::GetBaseGroundDepth_() const
{
    return 3.3f;
}

float msw::CStageManVolley::GetBaseOuterWidth_() const
{
    return 3.0f;
}

float msw::CStageManVolley::GetBaseOuterDepth_() const
{
    return 3.3f;
}

float msw::CStageManVolley::GetBaseOuterFront_() const
{
    return 0.0f;
}

float msw::CStageManVolley::GetBaseNetWidth_() const
{
    return 13.0f;
}

float msw::CStageManVolley::GetBaseNetHeightTop_() const
{
    return 2.4f;
}

float msw::CStageManVolley::GetBaseNetHeightBottom_() const
{
    return 1.0f;
}

void msw::CStageManVolley::OnPostInitialize()
{
    if (GetStageNo() == 7) {
        nw4r::g3d::ResFile res = CLoadData::GetRes(0);
        bcs::FVEC3 posBack(0.0f, 0.0f, -14.5f);
        bcs::FVEC3 posFront(0.0f, 0.0f, 14.5f);
        bcs::FVEC3 rotBack(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 rotFront(0.0f, 3.1415927f, 0.0f);
        bcs::FVEC3 scale(1.0f, 1.0f, 1.0f);

        new CStageVolleyFigure(this, res, posBack, rotBack, scale);
        new CStageVolleyFigure(this, res, posFront, rotFront, scale);
    }

    CStageNet *pNet = NewNet(this);
    if (pNet != 0) {
        pNet->Create();
        pNet->SetPos(bcs::FVEC3(0.0f, 0.0f, 0.0f));
        pNet->SceneEntry();
        m_pNet = pNet;
    }
}

msw::CStageNet *msw::CStageManVolley::NewNet(bcs::CTask *pParent)
{
    return new CStageNetVolley(pParent);
}

long msw::CStageManVolley::GetGoalSE(GOAL_SE_TYPE type) const
{
    long se = -1;

    if (type == 1) {
        se = (GetStageNo() == 11) + 0xC97;
    }
    return se;
}

void msw::CStageManVolley::SendMessage(long msg, long a, long arg, long b)
{
    CStageMan::SendMessage(msg, a, arg, b);

    switch (msg) {
        case 3:
        case 0x13:
            PlayAnim_AudienceFlash(1, -1.0f);
            InitAnim_AudienceFlag();
            break;
        case 0x54:
            PlayAnim_AudienceFlash(2, 300.0f);
            break;
        case 5:
            PlayAnim_AudienceFlash(2, 300.0f);
            InitAnim_AudienceFlag();
            break;
    }

    switch (msg) {
        case 3:
        case 0x13:
            OnSoundEvent(0);
            break;
        case 0x54:
            if (arg == 9) {
                OnSoundEvent(2);
            } else {
                OnSoundEvent(1);
            }
            break;
        case 0x57:
            OnSoundEvent(3);
            break;
        case 0x4B:
            OnSoundEvent(4);
            break;
        case 0x53:
            OnSoundEvent(5);
            break;
    }
}

void msw::CStageManVolley::OnSoundEvent(long)
{
}
