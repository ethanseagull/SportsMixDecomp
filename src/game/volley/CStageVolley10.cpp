// msw::CStageVolley10: volley stage 10, the panel stage. The court is a grid
// of 3 x 6 panels (three rows per team); hitting the ball onto a panel
// scores by its type and, depending on the rule set, takes or paints it.
// See the header for the grid and the rule sets.
#include "game/msw/CStageVolley10.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Draw2D.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageAnm.hpp"
#include "game/msw/CStgMdl.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/math/types.h"

namespace msw {

    // The animations of the panel models: "01" to "03" for the panels of
    // rule set 3 (one model per local type), "01" to "23" for the shared
    // model of rule sets 2 and 4.
    static const char *s_PanelAnmSuffix01[] = { "01", "02", "03", 0 };
    static const char *s_PanelAnmSuffix02[] = { "01", "02", "03", 0 };
    static const char *s_PanelAnmSuffix03[] = { "01", "02", "03", 0 };
    static const char *s_PanelAnmSuffixB[] = { "01", "02", "03", "04", "05", "06", "07", "08",
                                               "09", "10", "11", "12", "13", "14", "15", "16",
                                               "17", "18", "19", "20", "21", "22", "23", 0 };

    // The panel model of each local type in rule set 3 (type 0 shows the
    // model of type 1 in its "02" animation).
    static const CStgMdl::MDL_PARAM s_PanelMdlParam[4] = {
        { "s10_panel_01", 0, 0, 0x30F, s_PanelAnmSuffix01, "s10_panel_01_m", 0, 0 },
        { "s10_panel_01", 0, 0, 0x30F, s_PanelAnmSuffix01, "s10_panel_01_m", 0, 0 },
        { "s10_panel_02", 0, 0, 0x30F, s_PanelAnmSuffix02, "s10_panel_02_m", 0, 0 },
        { "s10_panel_03", 0, 0, 0x30F, s_PanelAnmSuffix03, "s10_panel_03_m", 0, 0 },
    };

    // The panel model of rule sets 2 and 4.
    static const CStgMdl::MDL_PARAM s_PanelMdlParamB = {
        "s10_panel_04", 0, 0, 0x30F, s_PanelAnmSuffixB, "s10_panel_04_m", 0, 0
    };

    // The animation that changes a panel of rule sets 2 and 4 from local
    // type [from] to local type [to]; [n][n] is the one it stays in.
    static long s_ChangeAnm[5][5] = {
        { 0, 5, 15, 6, 16 },
        { 0, 1, 9, 7, 17 },
        { 0, 13, 19, 11, 4 },
        { 0, 8, 18, 2, 10 },
        { 0, 12, 3, 14, 20 },
    };
}

// Model kinds of the stage's MDL_GEN_RULE table.
enum {
    MDL_BG = 1,
    MDL_PANEL_EDGE = 2, // "s<n>_panel_edge": kept for the edge models
    MDL_FEVER = 0x65    // "s<n>_ground_03": the fever model
};

// Animations of the panel model (the "s10_panel_04" set).
enum {
    ANM_DOUBLE_1 = 0x13,  // local type 2 looping
    ANM_DOUBLE_2 = 0x14,  // local type 4 looping
    ANM_CENTER_1 = 0x15,  // the centre panel of a half, colour of types 1 and 2
    ANM_CENTER_2 = 0x16
};

// Sound slots and sounds of the stage; SE_GetSlotNum gives the four.
enum {
    SE_SLOT_AMBIENT0 = 0,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_AMBIENT0 = 0xD54,
    SE_AMBIENT2 = 0xD60,
    SE_EVENT = 0xD61,
    SE_PANEL_DOUBLE = 0xD53, // a doubled panel hit
    SE_PANEL_FEVER = 0xD55,  // a half full of doubled panels
    SE_PANEL_CHANGE = 0xD5A, // a panel changed
    SE_PANEL_STEAL = 0xD5D   // the mirror of a doubled panel changed team
};

msw::CStageVolley10::CStageVolley10(bcs::CTask *pParent, long nSetType)
    : CStageManVolley(pParent), m_nSetType(0), m_ppPanelMdl(0), m_pLocalType(0), m_pEdgeFlag(0), m_fAnmFrame(0.0f),
      m_pFeverMdl(0)
{
    m_ppPanelMdl = new (1) CStgMdl *[PANEL_NUM];
    m_ppEdgeMdl[0] = new (1) CStgMdl *[PANEL_NUM];
    m_ppEdgeMdl[1] = new (1) CStgMdl *[PANEL_NUM];
    m_pLocalType = new (1) long[PANEL_NUM];
    m_pEdgeFlag = new (1) unsigned long[PANEL_NUM];

    for (long i = 0; i < PANEL_NUM; i++) {
        m_ppPanelMdl[i] = 0;
        m_ppEdgeMdl[0][i] = 0;
        m_ppEdgeMdl[1][i] = 0;
        m_pLocalType[i] = 0;
        m_pEdgeFlag[i] = 0;
    }

    m_nSetType = nSetType;
}

msw::CStageVolley10::~CStageVolley10()
{
    DeletePanelAll();

    if (m_ppEdgeMdl[0] != 0) {
        delete[] m_ppEdgeMdl[0];
    }
    m_ppEdgeMdl[0] = 0;
    if (m_ppEdgeMdl[1] != 0) {
        delete[] m_ppEdgeMdl[1];
    }
    m_ppEdgeMdl[1] = 0;
    if (m_ppPanelMdl != 0) {
        delete[] m_ppPanelMdl;
    }
    m_ppPanelMdl = 0;
    if (m_pLocalType != 0) {
        delete[] m_pLocalType;
    }
    m_pLocalType = 0;
    if (m_pEdgeFlag != 0) {
        delete[] m_pEdgeFlag;
    }
    m_pEdgeFlag = 0;

    if (m_pFeverMdl != 0) {
        delete m_pFeverMdl;
    }
    m_pFeverMdl = 0;
}

// A doubled panel (local type 2 or 4) of rule sets 2 and 4 loops its
// animation; all of them are kept in step: the frame of the one playing is
// remembered, and one that loops restarts at that frame.
inline void msw::CStageVolley10::SyncDoubleAnm_(long idx)
{
    long local = GetPanelLocalType_(idx);
    if (local == 2 || local == 4) {
        CStgMdl *pMdl = m_ppPanelMdl[idx];
        CStageAnm *pAnm = pMdl != 0 ? pMdl->GetAnmObj() : 0;
        if (pMdl != 0 && pAnm != 0) {
            unsigned long anm = pAnm->GetCurAnmId(CStageAnm::ANM_IDX(3));
            if (anm == ANM_DOUBLE_1 || anm == ANM_DOUBLE_2) {
                m_fAnmFrame = pAnm->GetFrame(CStageAnm::ANM_IDX(3));
            }
            if (pAnm->IsLooped(-1)) {
                pMdl->SetAnm(local == 2 ? ANM_DOUBLE_1 : ANM_DOUBLE_2, 0, 1.0f, 0.0f, 0.0f, -1);
                pAnm->SetFrame(-1, PanelSetB_GetAnmFrame());
            }
        }
    }
}

// Keeps the doubled panels in step, places the panel models, updates the
// edges and keeps the ambient sound going.
void msw::CStageVolley10::Main_(float frame)
{
    for (long i = 0; i < PANEL_NUM; i++) {
        switch (GetPanelSetType()) {
            case 3:
                break;
            case 2:
                SyncDoubleAnm_(i);
                break;
            case 4:
                SyncDoubleAnm_(i);
                break;
        }
    }

    for (long i = 0; i < PANEL_NUM; i++) {
        CStgMdl *pMdl = m_ppPanelMdl[i];
        if (pMdl != 0) {
            MTX mtx;
            mtx = GetPanelMtx_(i);
            pMdl->SetMtx(mtx);
            pMdl->Commit();
        }
    }

    switch (GetPanelSetType()) {
        case 2:
        case 4:
            UpdateEdge_();
            break;
    }

    if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
        SE_PlayEx(SE_SLOT_AMBIENT0, SE_AMBIENT0, 3, 1, -1, 0.0f, 1.0f, 0.0f);
    }

    CStageManVolley::Main_(frame);
}

long msw::CStageVolley10::GetPanelSetType() const
{
    return m_nSetType;
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley10::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManVolley.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg", MDL_BG, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_panel_edge", MDL_PANEL_EDGE, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ground_03", MDL_FEVER, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageManVolley::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

// The edge model is only remembered (InitPanelAll makes two per panel); the
// fever model is made here, hidden until the fever starts.
bool msw::CStageVolley10::OnPreCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, nw4r::g3d::ResFile file,
                                         CArn &rArn, long n)
{
    switch (pRule->m_nId) {
        case MDL_PANEL_EDGE:
            m_pEdgeArn = &rArn;
            m_nEdgeIdx = n;
            return false;
        case MDL_FEVER:
            if (m_pFeverMdl == 0) {
                CStgMdl *pMdl = new (1) CStgMdl(0);
                m_pFeverMdl = pMdl;
                if (pMdl != 0) {
                    nw4r::g3d::ResFile res = CLoadData::GetRes(0);
                    pMdl->Create(CStgMdl::CGenericCreator(res, &rArn, n));

                    MTX mtx;
                    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                    PSMTXTransApply(mtx.m, mtx.m, pos.x, pos.y, pos.z);
                    pMdl->SetMtx(mtx);
                    pMdl->SetLayer(1);

                    bcs::CScnObj *pObj = pMdl->GetMdlObj();
                    if (pObj != 0) {
                        pObj->SetDrawPriorityOpa(7);
                        pObj->SetDrawPriorityXlu(7);
                    }
                }
            }
            return false;
        default:
            return true;
    }
}

void msw::CStageVolley10::OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CMdlPtrPool::Handle &rHandle, CArn &rArn, long n)
{
}

void msw::CStageVolley10::OnPostInitialize()
{
    CStageManVolley::OnPostInitialize();

    // Projection camera of view 0: orthographic, 4 units high, looking down
    // from straight above with +X as up (as in CStageVolley42).
    nw4r::g3d::CameraData data;
    nw4r::g3d::Camera camera(&data);
    camera.Init();
    float aspect = 1.0f;
    float height = 4.0f;
    float width = height * aspect;
    camera.SetOrtho(0.5f * height, 0.5f * -height, 0.5f * -width, 0.5f * width, 0.5f, 10000.0f);
    camera.SetPosition(nw4r::math::VEC3(0.0f, 1.0f, 0.0f));

    nw4r::g3d::Camera::PostureInfo posture;
    posture.tp = nw4r::g3d::Camera::POSTURE_LOOKAT;
    posture.cameraUp = nw4r::math::VEC3(1.0f, 0.0f, 0.0f);
    posture.cameraTarget = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    camera.SetPosture(posture);

    bcs::CWorldEnv::SetProjCameraData(0, data);
}

void msw::CStageVolley10::OnSceneEntry()
{
    InitPanelAll(m_nSetType);
}

void msw::CStageVolley10::OnEntranceEventStart()
{
    InitPanelAll(m_nSetType);
}

void msw::CStageVolley10::OnPeriodInit()
{
    InitPanelAll(GetPanelSetType());
}

void msw::CStageVolley10::OnTurnInit()
{
    ResetPanelAnimAll();
}

void msw::CStageVolley10::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
                SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

// Makes the panels for the rule set. Set 3 deals each half two panels of
// type 1, two of type 2 and one of type 3 on random places; sets 2 and 4
// start empty (set 4 then paints the centre panel of each half for its
// attacking team).
void msw::CStageVolley10::InitPanelAll(long nSetType)
{
    DeletePanelAll();

    switch (GetPanelSetType()) {
        case 3: {
            long types[2][PANEL_COL_NUM * TEAM_ROW_NUM];
            for (long team = 0; team < 2; team++) {
                long count[4] = { 0, 2, 2, 1 };
                long *pTypes = types[team];

                long type = 1;
                for (long i = 0; i < PANEL_COL_NUM * TEAM_ROW_NUM; i++) {
                    while (type < 4 && count[type] <= 0) {
                        type++;
                    }
                    if (type < 4) {
                        pTypes[i] = type;
                        count[type]--;
                    } else {
                        pTypes[i] = 0;
                    }
                }

                for (long i = 1; i < PANEL_COL_NUM * TEAM_ROW_NUM; i++) {
                    long j = bcs::BCS_RandRange(0L, i);
                    long tmp = pTypes[i];
                    pTypes[i] = pTypes[j];
                    pTypes[j] = tmp;
                }

                for (long i = 0; i < PANEL_COL_NUM * TEAM_ROW_NUM; i++) {
                    m_pLocalType[team * PANEL_COL_NUM * TEAM_ROW_NUM + i] = pTypes[i];
                }
            }
            break;
        }
        case 2:
        case 4:
            for (long i = 0; i < PANEL_NUM; i++) {
                m_pLocalType[i] = 0;
            }
            PanelSetB_FeverCancel_();
            break;
        default:
            return;
    }

    for (long i = 0; i < PANEL_NUM; i++) {
        long local = m_pLocalType[i];
        const CStgMdl::MDL_PARAM *pParam;
        switch (GetPanelSetType()) {
            case 3:
                pParam = &s_PanelMdlParam[local];
                break;
            case 2:
            case 4:
                pParam = &s_PanelMdlParamB;
                break;
            default:
                return;
        }

        CStgMdl *pMdl = new (1) CStgMdl(0);
        pMdl->Create(0, pParam);
        pMdl->SetDrawPriority(5, 5);
        pMdl->SetLayer(1);
        pMdl->SceneEntry();
        m_ppPanelMdl[i] = pMdl;
    }

    for (long i = 0; i < PANEL_NUM; i++) {
        CStgMdl *pMdl = m_ppPanelMdl[i];
        long local = m_pLocalType[i];
        switch (GetPanelSetType()) {
            case 3:
                if (local != 0) {
                    pMdl->SetAnm(0, 0, 1.0f, 0.0f, 0.0f, -1);
                } else {
                    pMdl->SetAnm(1, 0, 1.0f, 0.0f, 0.0f, -1);
                }
                break;
            case 2:
            case 4: {
                pMdl->SetAnm(0, 0, 1.0f, 0.0f, 0.0f, -1);

                nw4r::g3d::ResFile res = CLoadData::GetRes(0);
                CStgMdl::CGenericCreator creator(res, m_pEdgeArn, m_nEdgeIdx);
                for (long j = 0; j < 2; j++) {
                    CStgMdl *pEdge = new (1) CStgMdl(0);
                    pEdge->Create(creator);
                    pEdge->SetAnm(0, 2, 1.0f, 0.0f, 0.0f, -1);
                    pEdge->SetDrawPriority(6, 6);
                    pEdge->SetLayer(1);
                    pEdge->SceneEntry();
                    m_ppEdgeMdl[j][i] = pEdge;
                    pEdge->GetMdlObj();
                }
                break;
            }
            default:
                return;
        }
    }

    if (GetPanelSetType() == 4) {
        for (long team = 0; team < 2; team++) {
            long offense = GetOffenseTeamNo(team);
            long local = GetPanelTypeByTeamNo_(offense, 1);
            GetPanelTypeByTeamNo_(offense ^ 1, 1);
            PanelSetB_SetPanelLocalType_(offense, 1, 1, team, local);
        }
    }
}

void msw::CStageVolley10::DeletePanelAll()
{
    if (m_ppPanelMdl != 0) {
        for (long i = 0; i < PANEL_NUM; i++) {
            if (m_ppPanelMdl[i] != 0) {
                delete m_ppPanelMdl[i];
            }
            m_ppPanelMdl[i] = 0;
        }
    }

    for (long j = 0; j < 2; j++) {
        if (m_ppEdgeMdl[j] != 0) {
            for (long i = 0; i < PANEL_NUM; i++) {
                if (m_ppEdgeMdl[j][i] != 0) {
                    delete m_ppEdgeMdl[j][i];
                }
                m_ppEdgeMdl[j][i] = 0;
            }
        }
    }
}

void msw::CStageVolley10::ResetPanelAll()
{
}

// Rule set 4: every panel back to the animation of its local type (the
// centre panels and the doubled ones to their own).
void msw::CStageVolley10::ResetPanelAnimAll()
{
    if (GetPanelSetType() != 4) {
        return;
    }

    for (long i = 0; i < PANEL_NUM; i++) {
        long local = GetPanelLocalType_(i);
        long anm = s_ChangeAnm[local][local];
        if (i == GetPanelIdxColRow_(1, 1, 0) || i == GetPanelIdxColRow_(1, 1, 1)) {
            if (local == 1) {
                anm = ANM_CENTER_1;
            } else if (local == 3) {
                anm = ANM_CENTER_2;
            } else if (local == 2) {
                anm = ANM_CENTER_1;
            } else if (local == 4) {
                anm = ANM_CENTER_2;
            }
        } else if (local == 2) {
            anm = ANM_CENTER_1;
        } else if (local == 4) {
            anm = ANM_CENTER_2;
        }

        if (anm >= 0) {
            m_ppPanelMdl[i]->SetAnm(anm, 0, 1.0f, 0.0f, 0.0f, -1);
        }
    }
}

long msw::CStageVolley10::GetPanelType(const bcs::FVEC3 &rPos) const
{
    return GetPanelType(GetPanelIdx_(rPos));
}

long msw::CStageVolley10::GetPanelType(long idx) const
{
    if (idx < 0) {
        return 0;
    }
    return GetPanelTypeByLocalType_(GetPanelLocalType_(idx));
}

// The ball hit the court at rPos: returns the type of the panel there (4
// when it starts the fever, 0 for a panel set 4 left unchanged) and
// changes the panels by the rule set.
long msw::CStageVolley10::HitPanel(const bcs::FVEC3 &rPos, int)
{
    long idx = GetPanelIdx_(rPos);
    long result = 0;
    if (static_cast<unsigned long>(idx) > PANEL_NUM - 1) {
        return 0;
    }

    long hitLocal = GetPanelLocalType_(idx);
    switch (GetPanelSetType()) {
        case 3:
            // The panel is taken.
            result = GetPanelType(idx);
            SetPanelLocalType_(idx, 0);
            break;
        case 2: {
            // The attacking team paints the panel and its mirror.
            long oldLocal = GetPanelLocalType_(idx);
            long negLocal = GetPanelLocalType_(GetPanelNegIdx_(idx));
            long col;
            long row;
            long team;
            if (GetPanelColRow_(idx, &col, &row, &team)) {
                long offense = GetOffenseTeamNo(team);
                if (offense != GetTeamNoByPanelType_(hitLocal)) {
                    PanelSetB_SetPanelLocalType_(offense, col, row, team, GetPanelTypeByTeamNo_(offense, 1));
                    PanelSetB_UpDownCheck_(offense, team);

                    long negIdx = GetPanelNegIdx_(idx);
                    long other = offense >= 0 ? offense ^ 1 : -1;
                    long negCol;
                    long negRow;
                    long negTeam;
                    GetPanelColRow_(negIdx, &negCol, &negRow, &negTeam);
                    PanelSetB_SetPanelLocalType_(offense, negCol, negRow, negTeam, GetPanelTypeByTeamNo_(offense, 1));
                    PanelSetB_UpDownCheck_(other, negTeam);
                }
            }

            result = GetPanelType(idx);
            if (PanelSetB_IsFever_(team)) {
                PanelSetB_FeverStart_(team);
                result = 4;
            }

            long newLocal = GetPanelLocalType_(idx);
            if (result == 4) {
                CGameSnd::Play(SE_PANEL_FEVER, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            } else if (GetMulRateByPanelType_(GetPanelTypeByLocalType_(newLocal)) > 1) {
                CGameSnd::Play(SE_PANEL_DOUBLE, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            } else if (GetMulRateByPanelType_(GetPanelTypeByLocalType_(negLocal)) > 1 &&
                       GetTeamNoByPanelType_(newLocal) != GetTeamNoByPanelType_(oldLocal)) {
                CGameSnd::Play(SE_PANEL_STEAL, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            } else if (oldLocal != newLocal) {
                CGameSnd::Play(SE_PANEL_CHANGE, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            break;
        }
        case 4: {
            // The attacking team paints the panel (no mirror).
            long oldLocal = GetPanelLocalType_(idx);
            long negLocal = GetPanelLocalType_(GetPanelNegIdx_(idx));
            long col;
            long row;
            long team;
            if (GetPanelColRow_(idx, &col, &row, &team)) {
                long offense = GetOffenseTeamNo(team);
                if (offense != GetTeamNoByPanelType_(hitLocal)) {
                    PanelSetB_SetPanelLocalType_(offense, col, row, team, GetPanelTypeByTeamNo_(offense, 1));
                    PanelSetC_UpDownCheck_(offense, team, col, row);
                }
            }

            result = GetPanelType(idx);
            if (PanelSetB_IsFever_(team)) {
                PanelSetB_FeverStart_(team);
                result = 4;
            }

            long newLocal = GetPanelLocalType_(idx);
            if (result == 4) {
                CGameSnd::Play(SE_PANEL_FEVER, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            } else if (GetMulRateByPanelType_(GetPanelTypeByLocalType_(newLocal)) > 1 && oldLocal != newLocal) {
                CGameSnd::Play(SE_PANEL_DOUBLE, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            } else if (GetMulRateByPanelType_(GetPanelTypeByLocalType_(negLocal)) > 1 &&
                       GetTeamNoByPanelType_(newLocal) != GetTeamNoByPanelType_(oldLocal)) {
                // (no sound here, unlike rule set 2)
            } else if (oldLocal != newLocal) {
                CGameSnd::Play(SE_PANEL_CHANGE, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }

            if (oldLocal == newLocal) {
                result = 0;
            }
            break;
        }
    }
    return result;
}

long msw::CStageVolley10::GetPanelIdx_(long col, long row) const
{
    col = col > PANEL_COL_NUM - 1 ? PANEL_COL_NUM - 1 : (col < 0 ? 0 : col);
    row = row > PANEL_ROW_NUM - 1 ? PANEL_ROW_NUM - 1 : (row < 0 ? 0 : row);
    return col + row * PANEL_COL_NUM;
}

long msw::CStageVolley10::GetPanelIdxColRow_(long col, long row, long nTeam) const
{
    return GetPanelIdx_(col, row + nTeam * TEAM_ROW_NUM);
}

// The court is taken to be centred on the origin; row 0 is at the back.
long msw::CStageVolley10::GetPanelIdx_(const bcs::FVEC3 &rPos) const
{
    float left;
    float right;
    float front;
    float back;
    GetCourtRect(&left, &right, &front, &back);
    if (rPos.x < left || rPos.x > right || rPos.z < front || rPos.z > back) {
        return -1;
    }

    float depth = back - front;
    float width = right - left;
    long col = static_cast<long>(3.0f * ((0.5f * width + rPos.x) / width));
    long row = 5 - static_cast<long>(6.0f * ((0.5f * depth + rPos.z) / depth));
    return GetPanelIdx_(col, row);
}

long msw::CStageVolley10::GetPanelNegIdx_(long idx) const
{
    if (static_cast<unsigned long>(idx) > PANEL_NUM - 1) {
        return -1;
    }
    long col = idx % PANEL_COL_NUM;
    long row = idx / PANEL_COL_NUM;
    // Both arms are 5 - row; the original computes them apart.
    long negRow = row < TEAM_ROW_NUM ? (PANEL_ROW_NUM - 1) - row : -(row - (PANEL_ROW_NUM - 1));
    return GetPanelIdx_(col, negRow);
}

// (the check lets 18 through, as in the original)
bool msw::CStageVolley10::GetPanelColRow_(long idx, long *pCol, long *pRow, long *pTeam) const
{
    if (static_cast<unsigned long>(idx) > PANEL_NUM) {
        return false;
    }

    long row = idx / PANEL_COL_NUM;
    long col = idx % PANEL_COL_NUM;
    long team = row / TEAM_ROW_NUM;
    row -= team * TEAM_ROW_NUM;
    if (pCol != 0) {
        *pCol = col;
    }
    if (pRow != 0) {
        *pRow = row;
    }
    if (pTeam != 0) {
        *pTeam = team;
    }
    return true;
}

bcs::FVEC3 msw::CStageVolley10::GetPanelPos_(long idx) const
{
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    float width = GetPanelWidth_();
    float depth = GetPanelDepth_();
    pos.x = 0.5f * width + (idx % PANEL_COL_NUM) * width + GetCourtLeft();
    pos.y = 0.0f;
    pos.z = GetCourtBack() - (0.5f * depth + (idx / PANEL_COL_NUM) * depth);
    return pos;
}

// The panel model's matrix: at its place, turned half round in team 0's
// half, scaled from the model's size (3.8 x 3.6) to the panel's.
msw::CStageVolley10::MTX msw::CStageVolley10::GetPanelMtx_(long idx) const
{
    bcs::FVEC3 pos = GetPanelPos_(idx);
    MTX mtx;
    PSMTXTransApply(mtx.m, mtx.m, pos.x, pos.y, pos.z);

    if (idx / (PANEL_COL_NUM * TEAM_ROW_NUM) <= 0) {
        bcs::FMTX34 rot;
        nw4r::math::MTX34RotXYZFIdx(reinterpret_cast<nw4r::math::MTX34 *>(&rot), 0.0f, 128.0f, 0.0f);
        PSMTXConcat(mtx.m, rot.m, mtx.m);
    }

    float left = GetCourtLeft();
    float width = GetCourtRight() - left;
    float front = GetCourtFront();
    float depth = GetCourtBack() - front;
    nw4r::math::VEC3 scale;
    scale.x = width / 3.0f / 3.8f;
    scale.y = 1.0f;
    scale.z = depth / 6.0f / 3.6f;
    nw4r::math::MTX34Scale(reinterpret_cast<nw4r::math::MTX34 *>(&mtx),
                           reinterpret_cast<const nw4r::math::MTX34 *>(&mtx), &scale);
    return mtx;
}

float msw::CStageVolley10::GetPanelWidth_() const
{
    float left;
    float right;
    GetCourtRect(&left, &right, 0, 0);
    return (right - left) / 3.0f;
}

float msw::CStageVolley10::GetPanelDepth_() const
{
    float front;
    float back;
    GetCourtRect(0, 0, &front, &back);
    return (back - front) / 6.0f;
}

long msw::CStageVolley10::GetOffenseTeamNo(long nTeam) const
{
    return CGame::GetTeamIdxByGoalIdx(nTeam) ^ 1;
}

long msw::CStageVolley10::GetTeamNoByPanel_(long col, long row, long nTeam) const
{
    return GetTeamNoByPanelType_(GetPanelLocalTypeColRow_(col, row, nTeam));
}

long msw::CStageVolley10::GetTeamNoByPanelType_(long type) const
{
    // The colour of the type: 1 for types 1 and 2, 0 for 3 and 4.
    bool color1 = false;
    if (type == 1 || type == 2) {
        color1 = true;
    }
    long color;
    if (color1) {
        color = 1;
    } else {
        bool color0 = false;
        if (type == 3 || type == 4) {
            color0 = true;
        }
        color = color0 ? 0 : -1;
    }

    if (color < 0) {
        return -1;
    }
    if (color == (CGame::GetTeamColorIdx(0) & 1)) {
        return 0;
    }
    return color == (CGame::GetTeamColorIdx(1) & 1) ? 1 : -1;
}

long msw::CStageVolley10::GetMulRateByPanel_(long col, long row, long nTeam) const
{
    return GetMulRateByPanelType_(GetPanelType(GetPanelIdxColRow_(col, row, nTeam)));
}

long msw::CStageVolley10::GetMulRateByPanelType_(long type) const
{
    return (type == 2) + 1;
}

long msw::CStageVolley10::GetPanelTypeByTeamNo_(long nTeam, long kind) const
{
    long color = CGame::GetTeamColorIdx(nTeam) & 1;
    long type;
    if (color == 0) {
        type = 3;
        if (kind == 2) {
            type = 4;
        }
    } else if (color == 1) {
        type = 1;
        if (kind == 2) {
            type = 2;
        }
    } else {
        type = 0;
    }
    return type;
}

long msw::CStageVolley10::GetPanelLocalTypeColRow_(long col, long row, long nTeam) const
{
    return GetPanelLocalType_(GetPanelIdxColRow_(col, row, nTeam));
}

long msw::CStageVolley10::GetPanelLocalType_(long idx) const
{
    if (static_cast<unsigned long>(idx) <= PANEL_NUM - 1) {
        return m_pLocalType[idx];
    }
    return 0;
}

long msw::CStageVolley10::GetPanelTypeByLocalType_(long local) const
{
    long type = 0;
    switch (GetPanelSetType()) {
        case 3:
            type = local;
            break;
        case 2:
        case 4:
            if (local == 2 || local == 4) {
                type = 2;
            } else {
                type = 0;
            }
            break;
    }
    return type;
}

void msw::CStageVolley10::SetPanelLocalType_(long idx, long local) const
{
    if (static_cast<unsigned long>(idx) > PANEL_NUM - 1) {
        return;
    }

    long oldLocal = GetPanelLocalType_(idx);
    switch (GetPanelSetType()) {
        case 3:
            if (oldLocal != local) {
                m_ppPanelMdl[idx]->SetAnm(2, 0, 1.0f, 0.0f, 0.0f, -1);
            }
            break;
        case 2:
            if (s_ChangeAnm[oldLocal][local] >= 0) {
                m_ppPanelMdl[idx]->SetAnm(s_ChangeAnm[oldLocal][local], 0, 1.0f, 0.0f, 0.0f, -1);
            }
            break;
        case 4: {
            long anm = s_ChangeAnm[oldLocal][local];
            if (idx == GetPanelIdxColRow_(1, 1, 0) || idx == GetPanelIdxColRow_(1, 1, 1)) {
                if (local == 1) {
                    anm = ANM_CENTER_1;
                } else if (local == 3) {
                    anm = ANM_CENTER_2;
                } else if (local == 2) {
                    anm = ANM_DOUBLE_1;
                } else if (local == 4) {
                    anm = ANM_DOUBLE_2;
                }
            }
            if (anm >= 0) {
                CStgMdl *pMdl = m_ppPanelMdl[idx];
                pMdl->SetAnm(anm, 0, 1.0f, 0.0f, 0.0f, -1);
                CStageAnm *pAnm = pMdl != 0 ? pMdl->GetAnmObj() : 0;
                if (pAnm != 0 && (anm == ANM_DOUBLE_1 || anm == ANM_DOUBLE_2)) {
                    pAnm->SetFrame(-1, PanelSetB_GetAnmFrame());
                }
            }
            break;
        }
    }

    SetPanelLocalTypeImm_(idx, local);
}

void msw::CStageVolley10::SetPanelLocalTypeImm_(long idx, long local) const
{
    if (static_cast<unsigned long>(idx) > PANEL_NUM - 1) {
        return;
    }
    m_pLocalType[idx] = local;
}

// Paints the panel at col, row of team nTeam's half with `local`. In rule
// set 2, painting over a doubled panel (seen from the attacking side, i.e.
// in the mirror when nOffense attacks that half) clears its whole group
// instead.
void msw::CStageVolley10::PanelSetB_SetPanelLocalType_(long nOffense, long col, long row, long nTeam, long local)
{
    long idx = GetPanelIdxColRow_(col, row, nTeam);
    long negIdx = GetPanelNegIdx_(idx);
    long checkCol;
    long checkRow;
    long checkTeam;
    if (nOffense != GetOffenseTeamNo(nTeam)) {
        checkCol = col;
        checkRow = row;
        checkTeam = nTeam;
    } else {
        GetPanelColRow_(negIdx, &checkCol, &checkRow, &checkTeam);
    }

    long checkLocal = GetPanelLocalTypeColRow_(checkCol, checkRow, checkTeam);
    if ((checkLocal == 2 || checkLocal == 4) && GetPanelSetType() == 2) {
        PanelSetB_SetPanelLocalTypeSub_(nOffense, col, row, nTeam, checkLocal);
    } else {
        SetPanelLocalType_(GetPanelIdxColRow_(col, row, nTeam), local);
    }
}

// Clears the panel and, recursively, its neighbours of the same doubled
// type and team.
void msw::CStageVolley10::PanelSetB_SetPanelLocalTypeSub_(long nOffense, long col, long row, long nTeam, long local)
{
    long idx = GetPanelIdxColRow_(col, row, nTeam);
    long negIdx = GetPanelNegIdx_(idx);
    long checkCol;
    long checkRow;
    long checkTeam;
    if (nOffense != GetOffenseTeamNo(nTeam)) {
        checkCol = col;
        checkRow = row;
        checkTeam = nTeam;
    } else {
        GetPanelColRow_(negIdx, &checkCol, &checkRow, &checkTeam);
    }

    if (GetPanelLocalTypeColRow_(checkCol, checkRow, checkTeam) == local) {
        long teamNo = GetTeamNoByPanelType_(local);
        if (GetTeamNoByPanel_(col, row, nTeam) == teamNo) {
            SetPanelLocalType_(GetPanelIdxColRow_(col, row, nTeam), 0);
            if (col - 1 >= 0) {
                PanelSetB_SetPanelLocalTypeSub_(nOffense, col - 1, row, nTeam, local);
            }
            if (col + 1 < PANEL_COL_NUM) {
                PanelSetB_SetPanelLocalTypeSub_(nOffense, col + 1, row, nTeam, local);
            }
            if (row - 1 >= 0) {
                PanelSetB_SetPanelLocalTypeSub_(nOffense, col, row - 1, nTeam, local);
            }
            if (row + 1 < TEAM_ROW_NUM) {
                PanelSetB_SetPanelLocalTypeSub_(nOffense, col, row + 1, nTeam, local);
            }
        }
    }
}

void msw::CStageVolley10::PanelSetB_UpDownCheck_(long nOffense, long nTeam)
{
    for (long row = 0; row < TEAM_ROW_NUM; row++) {
        for (long col = 0; col < PANEL_COL_NUM; col++) {
            PanelSetB_UpDownCheckSub_(nOffense, col, row, nTeam);
        }
    }
}

// A panel of nOffense's colour in a full line (row or column) of its
// colour is doubled; a doubled panel in no line goes back to plain.
void msw::CStageVolley10::PanelSetB_UpDownCheckSub_(long nOffense, long col, long row, long nTeam)
{
    if (nOffense < 0) {
        return;
    }

    bool rowLine = true;
    bool colLine = true;
    bool rowBroken = false;
    bool colBroken = false;
    long idx = GetPanelIdxColRow_(col, row, nTeam);
    long teamNo = GetTeamNoByPanel_(col, row, nTeam);
    long rate = GetMulRateByPanel_(col, row, nTeam);

    for (long c = 0; c < PANEL_COL_NUM; c++) {
        if (c != col) {
            long other = GetTeamNoByPanel_(c, row, nTeam);
            if (other < 0 || other != teamNo) {
                rowBroken = true;
                rowLine = false;
            }
        }
    }
    for (long r = 0; r < TEAM_ROW_NUM; r++) {
        if (r != row) {
            long other = GetTeamNoByPanel_(col, r, nTeam);
            if (other < 0 || other != teamNo) {
                colBroken = true;
                colLine = false;
            }
        }
    }

    if ((rowLine || colLine) && teamNo == nOffense && rate < 2) {
        SetPanelLocalType_(idx, GetPanelTypeByTeamNo_(teamNo, 2));
    } else if (rowBroken && colBroken && rate >= 2) {
        SetPanelLocalType_(idx, GetPanelTypeByTeamNo_(teamNo, 1));
    }
}

float msw::CStageVolley10::PanelSetB_GetAnmFrame() const
{
    return m_fAnmFrame;
}

void msw::CStageVolley10::PanelSetC_UpDownCheck_(long nOffense, long nTeam, long hitCol, long hitRow)
{
    for (long row = 0; row < TEAM_ROW_NUM; row++) {
        for (long col = 0; col < PANEL_COL_NUM; col++) {
            PanelSetC_UpDownCheckSub_(nOffense, col, row, nTeam, hitCol, hitRow);
        }
    }
}

// As PanelSetB_UpDownCheckSub_, and a doubled panel in a line through the
// panel just hit is doubled again (its animation restarts).
void msw::CStageVolley10::PanelSetC_UpDownCheckSub_(long nOffense, long col, long row, long nTeam, long hitCol,
                                                     long hitRow)
{
    if (nOffense < 0) {
        return;
    }

    bool rowLine = true;
    bool colLine = true;
    bool rowBroken = false;
    bool colBroken = false;
    long idx = GetPanelIdxColRow_(col, row, nTeam);
    long teamNo = GetTeamNoByPanel_(col, row, nTeam);
    long rate = GetMulRateByPanel_(col, row, nTeam);

    for (long c = 0; c < PANEL_COL_NUM; c++) {
        if (c != col) {
            long other = GetTeamNoByPanel_(c, row, nTeam);
            if (other < 0 || other != teamNo) {
                rowBroken = true;
                rowLine = false;
            }
        }
    }
    for (long r = 0; r < TEAM_ROW_NUM; r++) {
        if (r != row) {
            long other = GetTeamNoByPanel_(col, r, nTeam);
            if (other < 0 || other != teamNo) {
                colBroken = true;
                colLine = false;
            }
        }
    }

    if ((rowLine || colLine) && teamNo == nOffense &&
        (rate < 2 || (rowLine && row == hitRow) || (colLine && col == hitCol))) {
        SetPanelLocalType_(idx, GetPanelTypeByTeamNo_(teamNo, 2));
    } else if (rowBroken && colBroken && rate >= 2) {
        SetPanelLocalType_(idx, GetPanelTypeByTeamNo_(teamNo, 1));
    }
}

bool msw::CStageVolley10::PanelSetB_IsFever_(long nTeam) const
{
    for (long row = 0; row < TEAM_ROW_NUM; row++) {
        for (long col = 0; col < PANEL_COL_NUM; col++) {
            long local = GetPanelLocalTypeColRow_(col, row, nTeam);
            if (local != 2 && local != 4) {
                return false;
            }
        }
    }
    return true;
}

void msw::CStageVolley10::PanelSetB_FeverStart_(long)
{
    if (m_pFeverMdl != 0) {
        m_pFeverMdl->SceneEntry();
        m_pFeverMdl->SetAnm(1, 0, 1.0f, 0.0f, 0.0f, -1);
    }
}

void msw::CStageVolley10::PanelSetB_FeverCancel_()
{
    if (m_pFeverMdl != 0) {
        m_pFeverMdl->SceneLeave();
    }
}

// Edge bits of a panel: one per side (0x80 left, 0x08 right, 0x20 back,
// 0x02 front) and per corner (0x40, 0x10, 0x04, 0x01), set where the
// neighbour is not of the same local type. A side sets the corners at its
// ends. The low four bits pick the animation of the first edge model, the
// high four that of the second. In rule set 2 the mirrored panel gets the
// same edges flipped front to back.
void msw::CStageVolley10::UpdateEdge_()
{
    for (long team = 0; team < 2; team++) {
        long offense = GetOffenseTeamNo(team);
        for (long row = 0; row < TEAM_ROW_NUM; row++) {
            for (long col = 0; col < PANEL_COL_NUM; col++) {
                long idx = GetPanelIdxColRow_(col, row, team);
                long local = GetPanelLocalTypeColRow_(col, row, team);
                long teamNo = GetTeamNoByPanel_(col, row, team);
                if (teamNo < 0) {
                    m_pEdgeFlag[idx] = 0;
                } else if (teamNo != offense) {
                    if (GetPanelSetType() != 2) {
                        m_pEdgeFlag[idx] = 0;
                    }
                } else {
                    long negIdx = GetPanelNegIdx_(idx);
                    if (local != 2 && local != 4) {
                        m_pEdgeFlag[idx] = 0;
                        if (GetPanelSetType() == 2) {
                            m_pEdgeFlag[negIdx] = 0;
                        }
                    } else {
                        unsigned long flag = 0;
                        unsigned long negFlag = 0;
                        if (col <= 0 || local != GetPanelLocalTypeColRow_(col - 1, row, team)) {
                            flag |= 0x80;
                            negFlag |= 0x80;
                        }
                        if (col >= 2 || local != GetPanelLocalTypeColRow_(col + 1, row, team)) {
                            flag |= 0x08;
                            negFlag |= 0x08;
                        }
                        if (row <= 0 || local != GetPanelLocalTypeColRow_(col, row - 1, team)) {
                            flag |= 0x20;
                            negFlag |= 0x02;
                        }
                        if (row >= 2 || local != GetPanelLocalTypeColRow_(col, row + 1, team)) {
                            flag |= 0x02;
                            negFlag |= 0x20;
                        }
                        if (col <= 0 || row <= 0 || local != GetPanelLocalTypeColRow_(col - 1, row - 1, team)) {
                            flag |= 0x40;
                            negFlag |= 0x01;
                        }
                        if (col >= 2 || row <= 0 || local != GetPanelLocalTypeColRow_(col + 1, row - 1, team)) {
                            flag |= 0x10;
                            negFlag |= 0x04;
                        }
                        if (col <= 0 || row >= 2 || local != GetPanelLocalTypeColRow_(col - 1, row + 1, team)) {
                            flag |= 0x01;
                            negFlag |= 0x40;
                        }
                        if (col >= 2 || row >= 2 || local != GetPanelLocalTypeColRow_(col + 1, row + 1, team)) {
                            flag |= 0x04;
                            negFlag |= 0x10;
                        }

                        if (flag & 0x20) {
                            flag |= 0x50;
                            negFlag |= 0x05;
                        }
                        if (flag & 0x08) {
                            flag |= 0x14;
                            negFlag |= 0x14;
                        }
                        if (flag & 0x02) {
                            flag |= 0x05;
                            negFlag |= 0x50;
                        }
                        if (flag & 0x80) {
                            flag |= 0x41;
                            negFlag |= 0x41;
                        }

                        m_pEdgeFlag[idx] = flag;
                        if (GetPanelSetType() == 2) {
                            m_pEdgeFlag[negIdx] = negFlag;
                        }
                    }
                }
            }
        }
    }

    for (long i = 0; i < PANEL_NUM; i++) {
        CStgMdl *pEdge0 = m_ppEdgeMdl[0][i];
        CStgMdl *pEdge1 = m_ppEdgeMdl[1][i];
        const bcs::FMTX34 &rMtx = GetPanelMtx_(i);
        bcs::FMTX34 mtx0 = rMtx;
        bcs::FMTX34 mtx1 = rMtx;

        long team;
        GetPanelColRow_(i, 0, 0, &team);
        if (team == 0) {
            bcs::FMTX34 rot;
            nw4r::math::MTX34RotXYZFIdx(reinterpret_cast<nw4r::math::MTX34 *>(&rot), 0.0f, 128.0f, 0.0f);
            PSMTXConcat(mtx0.m, rot.m, mtx0.m);
        } else {
            bcs::FMTX34 rot;
            nw4r::math::MTX34RotXYZFIdx(reinterpret_cast<nw4r::math::MTX34 *>(&rot), 0.0f, 128.0f, 0.0f);
            PSMTXConcat(mtx1.m, rot.m, mtx1.m);
        }

        pEdge0->SetMtx(mtx0);
        pEdge0->Commit();
        pEdge1->SetMtx(mtx1);
        pEdge1->Commit();
        pEdge0->SetAnm(m_pEdgeFlag[i] & 0xF, 2, 1.0f, 0.0f, 0.0f, -1);
        pEdge1->SetAnm((m_pEdgeFlag[i] >> 4) & 0xF, 2, 1.0f, 0.0f, 0.0f, -1);
    }
}

void msw::CStageVolley10::DrawEdge_(long)
{
    for (long team = 0; team < 2; team++) {
        for (long row = 0; row < TEAM_ROW_NUM; row++) {
            for (long col = 0; col < PANEL_COL_NUM; col++) {
                DrawEdgeOne_(col, row, team);
            }
        }
    }
}

// Draws the edge bits of one panel: a black line per side and a small
// black square per corner.
void msw::CStageVolley10::DrawEdgeOne_(long col, long row, long nTeam)
{
    long idx = GetPanelIdxColRow_(col, row, nTeam);
    unsigned long flag = m_pEdgeFlag[idx];
    bcs::FVEC3 pos = GetPanelPos_(idx);
    float width = GetPanelWidth_();
    float depth = GetPanelDepth_();

    bcs::FVEC3 corner[4] = {
        bcs::FVEC3(-(0.5f * width - pos.x), 0.0f, 0.5f * depth + pos.z),
        bcs::FVEC3(0.5f * width + pos.x, 0.0f, 0.5f * depth + pos.z),
        bcs::FVEC3(0.5f * width + pos.x, 0.0f, -(0.5f * depth - pos.z)),
        bcs::FVEC3(-(0.5f * width - pos.x), 0.0f, -(0.5f * depth - pos.z)),
    };

    const unsigned long sideBit[4] = { 0x20, 0x08, 0x02, 0x80 };
    const long sideCorner[4][2] = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 } };
    for (long i = 0; i < 4; i++) {
        if (flag & sideBit[i]) {
            const bcs::FVEC3 &rFrom = corner[sideCorner[i][0]];
            const bcs::FVEC3 &rTo = corner[sideCorner[i][1]];
            bcs::BCS_GX_DrawLineEx(rFrom.x, rFrom.y, rFrom.z, rTo.x, rTo.y, rTo.z, bcs::BCS_Color(0x000000FF),
                                   bcs::BCS_Color(0x000000FF));
        }
    }

    const unsigned long cornerBit[4] = { 0x40, 0x10, 0x04, 0x01 };
    bcs::FVEC3 size(0.2f, 0.0f, 0.2f);
    bcs::FVEC3 square[4] = {
        bcs::FVEC3(-0.5f, 1.0f, 0.5f),
        bcs::FVEC3(0.5f, 1.0f, 0.5f),
        bcs::FVEC3(0.5f, 1.0f, -0.5f),
        bcs::FVEC3(-0.5f, 1.0f, -0.5f),
    };
    for (long i = 0; i < 4; i++) {
        if (flag & cornerBit[i]) {
            bcs::FVEC3 quad[4];
            for (long j = 0; j < 4; j++) {
                quad[j] = bcs::FVEC3(square[j].x * size.x, square[j].y * size.y, square[j].z * size.z);
                bcs::FVEC3Add(&quad[j], &quad[j], &corner[i]);
            }
            bcs::BCS_GX_DrawQuadEx(quad[0], quad[1], quad[2], quad[3], bcs::BCS_Color(0x000000FF),
                                   bcs::BCS_Color(0x000000FF), bcs::BCS_Color(0x000000FF),
                                   bcs::BCS_Color(0x000000FF));
        }
    }
}

void msw::CStageVolley10::Disp3D(long)
{
}

void msw::CStageVolley10::Disp()
{
    CStageManVolley::Disp();
}

bcs::FVEC3 msw::CStageVolley10::GetPanelPos_CR(long nTeam, long col, long row)
{
    CStageVolley10 *pStage = static_cast<CStageVolley10 *>(CStageMan::GetInstance());
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    long idx = pStage->GetPanelIdx_(col, row);
    if (nTeam != 0) {
        idx = pStage->GetPanelNegIdx_(idx);
    }
    if (idx >= 0) {
        pos = pStage->GetPanelPos_(idx);
    }
    return pos;
}

float msw::CStageVolley10::GetPanelWidth()
{
    return static_cast<CStageVolley10 *>(CStageMan::GetInstance())->GetPanelWidth_();
}

float msw::CStageVolley10::GetPanelDepth()
{
    return static_cast<CStageVolley10 *>(CStageMan::GetInstance())->GetPanelDepth_();
}

long msw::CStageVolley10::GetPanelType_CR(long nTeam, long col, long row)
{
    CStageVolley10 *pStage = static_cast<CStageVolley10 *>(CStageMan::GetInstance());
    if (static_cast<unsigned long>(col) > 2 || static_cast<unsigned long>(row) > 2) {
        return 0;
    }

    bcs::FVEC3 pos = GetPanelPos_CR(nTeam, col, row);
    long local = pStage->GetPanelLocalType_(pStage->GetPanelIdx_(pos));
    long type = 0;
    long color = CGame::GetTeamColorIdx(pStage->GetOffenseTeamNo(nTeam)) & 1;
    switch (local) {
        case 1:
            type = 1;
            if (color != 0) {
                type = 2;
            }
            break;
        case 2:
            type = 1;
            if (color != 0) {
                type = 3;
            }
            break;
        case 3:
            type = 2;
            if (color != 0) {
                type = 1;
            }
            break;
        case 4:
            type = 3;
            if (color != 0) {
                type = 1;
            }
            break;
    }
    return type;
}
