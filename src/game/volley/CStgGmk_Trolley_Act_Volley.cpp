// msw::CStgGmk_Trolley_Act_Volley: the actor of the trolley of volley
// stage 17, with the VOLLEY_PRM_S17_* tuning for msw::CStgGmk_Trolley_Act
// (a DOL class). Every car shown gets a defence collision box that
// reflects the ball; a reflection plays the hit effect and keeps the
// trolley from reflecting again for ten frames. The cars,
// msw::CStgGmk_Trolley_Mdl_Volley, have their own size.
#include "game/msw/CStgGmk_Trolley_Volley.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CStgSrt.hpp"

float msw::CStgGmk_Trolley_Mdl_Volley::GetParam_MdlWidth() const
{
    return 4.0f;
}

float msw::CStgGmk_Trolley_Mdl_Volley::GetParam_MdlHeight() const
{
    return 3.5f;
}

float msw::CStgGmk_Trolley_Mdl_Volley::GetParam_MdlDepth() const
{
    return 5.0f;
}

msw::CStgGmk_Trolley_Act_Volley::CStgGmk_Trolley_Act_Volley(CStgGmk *pParent) : CStgGmk_Trolley_Act(pParent)
{
    m_fReflectWait = 0.0f;
}

msw::CStgGmk_Trolley_Act_Volley::~CStgGmk_Trolley_Act_Volley()
{
}

void msw::CStgGmk_Trolley_Act_Volley::OnInit()
{
    CStgGmk_Trolley_Act::OnInit();
}

// Counts the reflect wait down to zero.
void msw::CStgGmk_Trolley_Act_Volley::OnUpdate(float rate)
{
    CStgGmk_Trolley_Act::OnUpdate(rate);
    if (m_fReflectWait > 0.0f) {
        m_fReflectWait -= rate;
        float wait = m_fReflectWait;
        if (0.0f > wait) {
            wait = 0.0f;
        }
        m_fReflectWait = wait;
    }
}

// The trolley does not push the characters while it runs.
void msw::CStgGmk_Trolley_Act_Volley::UpdateColl()
{
    CStgGmk_Trolley_Act::UpdateColl();
    if (IsAlive()) {
        m_BodyColl.SetPushDisable(0);
    }
}

// Twenty defence units, all off until UpdateDefColl places them.
void msw::CStgGmk_Trolley_Act_Volley::InitDefColl()
{
    m_DefColl.Create(20, 5, 4, 0);
    m_DefColl.GetColl(0)->SetCollType(2);
    m_DefColl.GetColl(0)->SetShape(4);
    m_DefColl.GetColl(0)->OffFlagA0(1);
    RegistReflectHitCallback(m_DefColl);
}

// One box per car shown, the cars one depth apart behind the trolley's
// position along its facing; the units of the cars not shown are off.
// (rPos and the two middle arguments are not used.)
void msw::CStgGmk_Trolley_Act_Volley::UpdateDefColl(const bcs::FVEC3 &rPos, float width, float, float,
                                                   float rotY)
{
    if (IsEnableDefColl()) {
        long num = GetDispMdlNum();
        bcs::FMTX34 mtx;
        bcs::FMTX34Copy(&mtx, &GetSrt()->GetMtx());
        bcs::FVEC3 pos = GetSrt()->GetPos();
        float depth = GetParam_MdlDepth();

        bcs::FVEC3 dir(mtx.m[0][2], mtx.m[1][2], mtx.m[2][2]);
        bcs::BCS_Vec_Normalize(dir, dir);

        long i;
        for (i = 0; i < num; i++) {
            CStgGmk_Trolley_Mdl *pMdl = static_cast<CStgGmk_Trolley_Mdl *>(GetMdl(i));
            bcs::FVEC3 carPos = dir;
            float height = GetParam_DefHeight(pMdl->GetMdlType());
            bcs::FVEC3Scale(&carPos, &carPos, -depth * i);
            bcs::FVEC3Add(&carPos, &carPos, &pos);

            m_DefColl.GetColl(i)->SetCollType(2);
            m_DefColl.GetColl(i)->SetShape(4);
            m_DefColl.GetColl(i)->SetPos(carPos);
            m_DefColl.GetColl(i)->SetSize(0.5f * width, height, 0.5f * depth);

            float rot = rotY;
            while (rot > 6.2831855f) {
                rot -= 6.2831855f;
            }
            while (rot < 0.0f) {
                rot += 6.2831855f;
            }
            m_DefColl.GetColl(i)->SetRotY(rot);
            m_DefColl.GetColl(i)->OnFlag10(1);
            m_DefColl.GetColl(i)->OnFlagA0(1);
        }
        for (; i < 20; i++) {
            m_DefColl.GetColl(i)->OffFlag10(1);
            m_DefColl.GetColl(i)->OffFlagA0(1);
        }
    } else {
        m_DefColl.SetDisableAll();
        m_DefColl.SetReflectDisableAll();
    }
}

// Plays the hit effect where the ball struck, then ignores the ball for
// ten frames. The ball bounces off as the collision computed (0x400).
unsigned long msw::CStgGmk_Trolley_Act_Volley::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                           bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
{
    if (m_fReflectWait > 0.0f) {
        return 0;
    }

    long effect = GetParam_Effect(2);
    if (effect >= 0) {
        CEffectCtrl::SimpleEffect(effect, 0, &rPos, 0, 3, 0, 0, 0);
    }
    m_fReflectWait = 10.0f;
    return 1 | 0x400;
}

void msw::CStgGmk_Trolley_Act_Volley::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
{
}

void msw::CStgGmk_Trolley_Act_Volley::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
{
    pBall->SetVolleyAnyHitMode(0, 0);
}

long msw::CStgGmk_Trolley_Act_Volley::GetParam_Effect(long type) const
{
    return -1;
}

long msw::CStgGmk_Trolley_Act_Volley::GetParam_TrolleyPatValidNum() const
{
    return 0;
}

// (meaning guessed) Index of a raw location: each of the ten rows starts
// 20 entries after the previous one, the first at 169. The original checks
// the row against patnum (40), not 10, so rows 10 to 39 read past the
// table. (The "@0" on patnum's symbol says another patnum was declared
// before it in the original file; colnum is not read.)
long msw::CStgGmk_Trolley_Act_Volley::GetLocRawTblIdx(long row, long idx) const
{
    const long tbl[] = { 169, 189, 209, 229, 249, 269, 289, 309, 329, 349 };
    static const long patnum = 40;
    static const long colnum = 1;
    long rowLen = tbl[1] - tbl[0];

    if (row < 0 || row >= patnum || idx < 0 || idx >= rowLen * colnum) {
        return -1;
    }
    return idx + tbl[row];
}
