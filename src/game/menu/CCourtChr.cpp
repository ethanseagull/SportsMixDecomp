// msw::SelectMenu::CCourtChr, CCourtBall and CCourtChrBall: the court of
// the menu background with the characters, the balls of the sports and the
// player markers on it.
#include "game/msw/SelectMenu/CCourtChr.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/EF/CEffect.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CChrMenuCtrl.hpp"
#include "game/msw/CChrMenuObj.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuMdlTableSet.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/SelectMenu/CCourtBallMdlObj.hpp"
#include "game/msw/SelectMenu/CCourtCharaObj.hpp"
#include "game/msw/SelectMenu/CCourtTableSetObj.hpp"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The resource files of the screen, loaded into m_ResFile.
    const char *s_ResName[7] = {
        "chara_table.brres", "BG_ball_BA.brres", "BG_ball_VO.brres", "BG_ball_DO.brres",
        "BG_pack_HO.brres",  "BG_MX.brres",      "menu_chr_marker.brres",
    };

    // The models: the name and the entry of s_ResName the model is in.
    struct MdlInfo {
        const char *pName;
        unsigned char nRes;
    };

    const MdlInfo s_MdlTable[8] = {
        { "chara_set_tv", 0 },     // the table of CCourtChr
        { "chara_set_non_tv", 0 }, // the table of the ball screens
        { "BG_ball_BK", 1 },       // the balls
        { "BG_ball_VB", 2 },
        { "BG_ball_DB", 3 },
        { "BG_ball_HK", 4 },
        { "BG_MX", 5 },
        { "menu_chr_marker", 6 },
    };

    // The nodes of the table things stand on: the six places, the ball of
    // CCourtChrBall, then the balls of CCourtBall... by sport.
    const char *s_PosName[12] = {
        "pos_00",   "pos_01", "pos_02", "pos_03", "pos_04", "pos_05",
        "ball_pos", "BK_set", "VB_set", "DB_set", "HK_set", "MX_set",
    };

    // The number of animations of the table.
    const long ANIM_NUM = 42;

    // The table's animations: [0] with the layout as it is, [1] when
    // msw::CLytCtrl::IsLocationAdjust is set.
    const msw::CMenuMdlTableSet::AnimInfo s_AnimTable[2][ANIM_NUM] = {
        {
            { "m_cat03", 0, 0 },       { "m_cat03L", 1, 0 },     { "m_cat04", 0, 0 },
            { "m_cat04L", 1, 0 },      { "m_cat04L_T", 1, 0 },   { "m_cat05_S", 0, 0 },
            { "m_cat05_SL", 1, 0 },    { "m_cat05_D", 0, 0 },    { "m_cat05_DL", 1, 0 },
            { "m_cat06_S", 0, 0 },     { "m_cat06_D", 0, 0 },    { "m_cat05_S_2", 0, 0 },
            { "m_cat05_SL_2", 1, 0 },  { "m_cat05_D_2", 0, 0 },  { "m_cat05_DL_2", 1, 0 },
            { "m_cat06_S_2", 0, 0 },   { "m_cat06_D_2", 0, 0 },  { "m_cat07", 0, 0 },
            { "m_cat08", 0, 0 },       { "m_cat_ball", 1, 0 },   { "m_cat03_MIX", 0, 0 },
            { "m_cat03", 0, 1 },       { "m_cat03L", 1, 1 },     { "m_cat04", 0, 1 },
            { "m_cat04L", 1, 1 },      { "m_cat04L_T", 1, 1 },   { "m_cat05_S", 0, 1 },
            { "m_cat05_SL", 1, 1 },    { "m_cat05_D", 0, 1 },    { "m_cat05_DL", 1, 1 },
            { "m_cat06_S", 0, 1 },     { "m_cat06_D", 0, 1 },    { "m_cat05_S_2", 0, 1 },
            { "m_cat05_SL_2", 1, 1 },  { "m_cat05_D_2", 0, 1 },  { "m_cat05_DL_2", 1, 1 },
            { "m_cat06_S_2", 0, 1 },   { "m_cat06_D_2", 0, 1 },  { "m_cat07", 0, 1 },
            { "m_cat08", 0, 1 },       { "m_cat_ball", 1, 1 },   { "m_cat03_MIX", 0, 1 },
        },
        {
            { "cat03", 0, 0 },         { "cat03L", 1, 0 },       { "cat04", 0, 0 },
            { "cat04L", 1, 0 },        { "cat04L_T", 1, 0 },     { "cat05_S", 0, 0 },
            { "cat05_SL", 1, 0 },      { "cat05_D", 0, 0 },      { "cat05_DL", 1, 0 },
            { "cat06_S", 0, 0 },       { "cat06_D", 0, 0 },      { "cat05_S_2", 0, 0 },
            { "cat05_SL_2", 1, 0 },    { "cat05_D_2", 0, 0 },    { "cat05_DL_2", 1, 0 },
            { "cat06_S_2", 0, 0 },     { "cat06_D_2", 0, 0 },    { "cat07", 0, 0 },
            { "cat08", 0, 0 },         { "cat_ball", 1, 0 },     { "cat03_MIX", 0, 0 },
            { "cat03", 0, 1 },         { "cat03L", 1, 1 },       { "cat04", 0, 1 },
            { "cat04L", 1, 1 },        { "cat04L_T", 1, 1 },     { "cat05_S", 0, 1 },
            { "cat05_SL", 1, 1 },      { "cat05_D", 0, 1 },      { "cat05_DL", 1, 1 },
            { "cat06_S", 0, 1 },       { "cat06_D", 0, 1 },      { "cat05_S_2", 0, 1 },
            { "cat05_SL_2", 1, 1 },    { "cat05_D_2", 0, 1 },    { "cat05_DL_2", 1, 1 },
            { "cat06_S_2", 0, 1 },     { "cat06_D_2", 0, 1 },    { "cat07", 0, 1 },
            { "cat08", 0, 1 },         { "cat_ball", 1, 1 },     { "cat03_MIX", 0, 1 },
        },
    };

    // The playing speed of each animation of the table (all 1).
    struct AnimRate {
        unsigned short nAnim;
        unsigned short nUnk; // nAnim, from 21 on nAnim - 21
        float fRate;
    };

    const AnimRate s_AnimRate[ANIM_NUM] = {
        { 0, 0, 1.0f },   { 1, 1, 1.0f },   { 2, 2, 1.0f },   { 3, 3, 1.0f },   { 4, 4, 1.0f },
        { 5, 5, 1.0f },   { 6, 6, 1.0f },   { 7, 7, 1.0f },   { 8, 8, 1.0f },   { 9, 9, 1.0f },
        { 10, 10, 1.0f }, { 11, 11, 1.0f }, { 12, 12, 1.0f }, { 13, 13, 1.0f }, { 14, 14, 1.0f },
        { 15, 15, 1.0f }, { 16, 16, 1.0f }, { 17, 17, 1.0f }, { 18, 18, 1.0f }, { 19, 19, 1.0f },
        { 20, 20, 1.0f }, { 21, 0, 1.0f },  { 22, 1, 1.0f },  { 23, 2, 1.0f },  { 24, 3, 1.0f },
        { 25, 4, 1.0f },  { 26, 5, 1.0f },  { 27, 6, 1.0f },  { 28, 7, 1.0f },  { 29, 8, 1.0f },
        { 30, 9, 1.0f },  { 31, 10, 1.0f }, { 32, 11, 1.0f }, { 33, 12, 1.0f }, { 34, 13, 1.0f },
        { 35, 14, 1.0f }, { 36, 15, 1.0f }, { 37, 16, 1.0f }, { 38, 17, 1.0f }, { 39, 18, 1.0f },
        { 40, 19, 1.0f }, { 41, 20, 1.0f },
    };

    // The places 0 to 5 twice: InitShown takes the six entries from the
    // first 0 on.
    const long s_DefaultShown[12] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5 };

    // The colours of the markers.
    bcs::BCS_Color s_MarkerColor[5] = {
        bcs::BCS_Color(0x00, 0x64, 0xFF, 0xFF), bcs::BCS_Color(0xFF, 0x1E, 0x1E, 0xFF),
        bcs::BCS_Color(0x00, 0xFF, 0x00, 0xFF), bcs::BCS_Color(0xFF, 0xFF, 0x00, 0xFF),
        bcs::BCS_Color(0x80, 0x80, 0x80, 0xFF),
    };

    // The spin of each ball: its model entry in m_hMdl, the angles and the
    // top speed, in degrees.
    struct BallInfo {
        unsigned char nMdlId;
        bcs::FVEC3 vRot;
        float fRotSpeed;
    };

    // The names of the material colour of the markers and of the animations
    // of the table and the mixed sports' ball, then the balls' spin. One
    // object: the original's .data has the names at 0x50 and the ball table
    // right after them at 0x6C, where a separate array would be aligned
    // to 8.
    struct CourtParam {
        const char *pMarkerColorName;
        const char *pClrName[2];     // [0] the table, [1] the mixed sports' ball
        const char *pTsaName[2];     // the same
        const char *pBallMotName[2]; // the mixed sports' ball on CCourtChrBall, on CCourtBall
        BallInfo ball[5];
    };

    CourtParam s_Param = {
        "marker_color00",
        { "chara_set_tv", "BG_MX" },
        { "chara_set_tv", "BG_MX" },
        { "BG_MX", "BG_MX_WIFI" },
        {
            { 7, bcs::FVEC3(60.0f, 0.0f, -20.0f), 0.6f },
            { 8, bcs::FVEC3(60.0f, 0.0f, -20.0f), 0.6f },
            { 9, bcs::FVEC3(60.0f, 0.0f, -20.0f), 0.6f },
            { 10, bcs::FVEC3(65.0f, 0.0f, -20.0f), 1.5f },
            { 11, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0.0f },
        },
    };

    // The effect attached to a character: the Mii types 19 and 20, and
    // the characters 0 to 17. CreateChrMenuObj indexes s_ChrEffect for any
    // type below 19, so type 18 would read past the end (in the original,
    // into the next constant); no caller is known to pass it.
    const long s_MiiEffect[2] = { 0x4C7, 0x4C8 };
    const long s_ChrEffect[18] = {
        0x4B5, 0x4B6, 0x4B7, 0x4B8, 0x4B9, 0x4BA, 0x4BB, 0x4BC, 0x4BD,
        0x4BE, 0x4BF, 0x4C0, 0x4C1, 0x4C2, 0x4C3, 0x4C4, 0x4C5, 0x4C6,
    };

    // Degrees to radians.
    const float DEG_TO_RAD = 0.017453292f;

    // The Mii shown for a silhouette of a Mii type.
    long s_SilhouetteMii = 2;

    // The node of the table for position nPos, or null.
    inline const char *GetPosName(long nPos)
    {
        if ((unsigned long)nPos < 12) {
            return s_PosName[nPos];
        }
        return 0;
    }

    // Hangs pObj on node nPos of pTable.
    inline void AttachToPos(bcs::CScnObj *pObj, bcs::CMdlObj *pTable, long nPos)
    {
        if (pObj != 0 && pTable != 0) {
            pObj->AttachToObject(pTable, pTable->GetNodeID(GetPosName(nPos)), bcs::CScnObj::ATTACHMODE_0);
        }
    }

    // Hides or shows a model.
    inline void SetMdlHide(bcs::CScnObj *pMdl, int bHide)
    {
        if (bHide) {
            pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        } else {
            pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        }
    }

    // (guess) The destructors of CCourtBall and CCourtChrBall store their
    // own vtable pointers before the inlined CCourtChr destructor. The
    // compiler leaves those stores out when the body is empty and keeps
    // them when it calls an inline function, even one that compiles to
    // nothing (notes: CCockpitBasketLayoutBase has the same shape), so the
    // original called something that is empty in this build.
    inline void Nothing() {}

    // Detaches and deletes the animations of a model, then the model.
    inline void DeleteMdl(bcs::CMdlObj *pMdl)
    {
        bcs::CAnmObj *pAnmObj;

        pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_CHR);
        if (pAnmObj != 0) {
            delete pAnmObj;
        }
        pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_MATCLR);
        if (pAnmObj != 0) {
            delete pAnmObj;
        }
        pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
        if (pAnmObj != 0) {
            delete pAnmObj;
        }
        pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_3);
        if (pAnmObj != 0) {
            delete pAnmObj;
        }
        pMdl->DetachFromParent();
        if (pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(pMdl);
        }
    }
}

msw::SelectMenu::CCourtChr::CCourtChr(long nId)
    : CSelectMenuLayoutBase(nId)
{
    Initialize();
}

msw::SelectMenu::CCourtChr::~CCourtChr()
{
    Remove();
    for (long i = 0; i < 6; i++) {
        if (m_pCharaObj[i] != 0) {
            delete m_pCharaObj[i];
            m_pCharaObj[i] = 0;
        }
    }
}

void msw::SelectMenu::CCourtChr::Initialize()
{
    static const long resLen = 7;

    m_Unk34 = 4;
    m_pTableSet = 0;
    for (long i = 0; i < MDL_NUM; i++) {
        m_hMdl[i] = 0;
    }
    m_bClose = 0;
    m_UnkC4 = 0;
    m_UnkD4 = 0.0f;
    m_fAnimSpeed = 1.0f;
    for (long i = 0; i < 6; i++) {
        m_pCharaObj[i] = new CCourtCharaObj(0);
    }
    bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
    for (long i = 0; i < resLen; i++) {
        m_ResFile[i] = pRes->GetG3DResFile(s_ResName[i]);
    }
    m_nAnim = -1;
    m_bMixMode = 0;
}

inline void msw::SelectMenu::CCourtChr::InitShown()
{
    for (long i = 0; i < 6; i++) {
        if (s_DefaultShown[i] == 0) {
            for (long j = 0; j < 6; j++) {
                m_nShown[j] = s_DefaultShown[i + j];
            }
        }
    }
}

int msw::SelectMenu::CCourtChr::Set()
{
    if (m_Unk2C != 0) {
        return 0;
    }

    nw4r::g3d::ResFile file = m_ResFile[0];
    bcs::CMdlObj *pMdl = CreateMdlObj(s_MdlTable[0].pName, file);
    if (pMdl != 0) {
        CCourtTableSetObj *pObj = new CCourtTableSetObj(pMdl);
        if (pObj != 0) {
            CMenuMdlTableSet *pTableSet = new CMenuMdlTableSet;
            if (pTableSet != 0) {
                pTableSet->SetObj(pObj);
                pTableSet->SetResFile(file);
                pTableSet->SetAnimTable(s_AnimTable[CLytCtrl::IsLocationAdjust() != 0], ANIM_NUM);
                pObj->Play(pTableSet->GetResAnimClr(s_Param.pClrName[0], file), 1.0f);
                pObj->Play(pTableSet->GetResAnimTsa(s_Param.pTsaName[0], file), 1.0f);
            }
            m_pTableSet = pTableSet;
            pObj->SetVisible(0);
        }
    }

    // The markers, one on each place.
    bcs::CMdlObj *pTable = 0;
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
        pTable = m_pTableSet->GetObj()->GetMdl();
    }
    if (pTable != 0) {
        const bcs::FVEC3 pos(0.0f, 0.01f, 0.0f);
        for (long i = 0; i < 6; i++) {
            nw4r::g3d::ResFile markerFile = m_ResFile[6];
            if (!markerFile.IsValid()) {
                continue;
            }
            bcs::CMdlObj *pMarker = CreateMdlObj(s_MdlTable[7].pName, markerFile);
            if (pMarker == 0) {
                continue;
            }
            m_hMdl[MDL_MARKER + i] = pMarker->GetHandle();
            AttachToPos(pMarker, pTable, i);
            SetMarkerColor(i, 4);
            pMarker->m_vTrans = pos;
            pMarker->CalcRootMtx();
        }
    }
    for (long i = 0; i < 6; i++) {
        SetMarkerDisp(i, 0);
    }

    InitShown();
    if (m_pTableSet != 0) {
        m_nAnim = -1;
    }
    m_bClose = 0;
    m_Unk2C = 1;
    m_Unk30 = STATE_NONE;
    return 0;
}

bcs::CMdlObj *msw::SelectMenu::CCourtChr::CreateMdlObj(const char *pName, nw4r::g3d::ResFile file)
{
    if (!file.IsValid()) {
        return 0;
    }
    nw4r::g3d::ResMdl mdl = file.GetResMdl(pName);
    if (!mdl.IsValid()) {
        return 0;
    }

    bcs::CMdlObj *pMdl = new bcs::CMdlObj;
    if (pMdl == 0) {
        return 0;
    }
    pMdl->Create(mdl, 0x103, 0);
    bcs::CRenderMan::PushBack(pMdl, 2);
    pMdl->OnFlag(0x2000);
    pMdl->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    pMdl->SetRot(0.0f, 0.0f, 0.0f);
    pMdl->m_vScale.Set(1.0f, 1.0f, 1.0f);

    bcs::CAnmMotion *pAnm = new bcs::CAnmMotion;
    if (pAnm != 0) {
        pAnm->Create(pMdl, file, 0, 3, 0);
        pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
    }
    return pMdl;
}

msw::CChrMdlObj *msw::SelectMenu::CCourtChr::CreateChrMenuObj(long nId, long nChr, long nSub, long bEffect,
                                                              RFLDataSource source, long nMii, long nLayer)
{
    if (source == RFLDataSource_Default && nMii >= 6) {
        nMii %= 6;
    }
    long nType = nChr;
    if (nChr >= 19) {
        nType = CGame::GetMiiChrType(source, nMii);
        if (nSub == 0xFF) {
            nMii = s_SilhouetteMii;
        }
    }

    CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.CreateChrMenuObj(nType, nSub >= 6 ? 0 : nSub, source, nMii, nLayer);
    if (pObj == 0) {
        return 0;
    }
    pObj->SetId(nId);
    pObj->SetSub(nSub);

    switch (nSub) {
        case 0xFF:
            pObj->SetSilhouette(1);
            break;
        case 0xFE:
            pObj->SetSilhouette(2);
            break;
        case 0xFD:
            pObj->SetSilhouette(3);
            break;
        default:
            pObj->SetSilhouette(0);
            break;
    }
    if (nType >= 19 && nSub == 0xFF) {
        pObj->SetMiiNoface(1);
    }

    if (bEffect == 1) {
        pObj->SetAttachEffect(nType < 19 ? s_ChrEffect[nType] : s_MiiEffect[nType - 19]);
        unsigned long hEffect = pObj->GetEffectHandle();
        if (hEffect != 0) {
            CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
            if (pCluster != 0) {
                pCluster->CallEffectFunc(&bcs::EF::CEffect::SetDelayTime, 4.0f);
            }
        }
    } else {
        pObj->StopAttachEffect();
    }
    pObj->SetEffectFlag(bEffect);
    return pObj->GetMdl();
}

msw::SelectMenu::CCourtBallMdlObj *msw::SelectMenu::CCourtChr::CreateBallMdlObj(const char *pName,
                                                                                nw4r::g3d::ResFile file)
{
    if (!file.IsValid()) {
        return 0;
    }
    nw4r::g3d::ResMdl mdl = file.GetResMdl(pName);
    if (!mdl.IsValid()) {
        return 0;
    }

    CCourtBallMdlObj *pMdl = new CCourtBallMdlObj;
    if (pMdl == 0) {
        return 0;
    }
    pMdl->Create(mdl, 0x103, 0);
    bcs::CRenderMan::PushBack(pMdl, 2);
    pMdl->OnFlag(0x2000);
    pMdl->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    pMdl->SetRot(0.0f, 0.0f, 0.0f);
    pMdl->m_vScale.Set(1.0f, 1.0f, 1.0f);

    bcs::CAnmMotion *pAnm = new bcs::CAnmMotion;
    if (pAnm != 0) {
        pAnm->Create(pMdl, file, 0, 3, 0);
        pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
    }
    return pMdl;
}

void msw::SelectMenu::CCourtChr::Remove()
{
    if (m_Unk2C == 0) {
        return;
    }

    for (long i = 0; i < 6; i++) {
        unsigned long &hMdl = m_hMdl[GetMdlObjIdFromCharaId(i)];
        if (hMdl != 0) {
            CChrMenuCtrl::s_Instance.DeleteChrMenuObj(hMdl);
        }
        hMdl = 0;
    }
    for (long i = 0; i < 5; i++) {
        unsigned long &hMdl = m_hMdl[GetMdlObjIdFromBallId(i)];
        bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(hMdl));
        if (pMdl != 0) {
            DeleteMdl(pMdl);
        }
        hMdl = 0;
    }
    for (long i = 0; i < 6; i++) {
        unsigned long &hMdl = m_hMdl[GetMdlObjIdFromMarkerId(i)];
        bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(hMdl));
        if (pMdl != 0) {
            DeleteMdl(pMdl);
        }
        hMdl = 0;
    }

    CMenuMdlTableSet *pTableSet = m_pTableSet;
    if (pTableSet != 0) {
        if (pTableSet->GetObj() != 0) {
            delete pTableSet->GetObj();
        }
        delete pTableSet;
        m_pTableSet = 0;
    }
    m_bClose = 0;
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
}

inline int msw::SelectMenu::CCourtChr::IsAnimEnd()
{
    if (m_pTableSet == 0) {
        return 1;
    }
    return m_pTableSet->GetObj()->IsAnmEnd();
}

inline int msw::SelectMenu::CCourtChr::IsAnimLooped()
{
    if (m_pTableSet == 0) {
        return 1;
    }
    return m_pTableSet->GetObj()->IsAnmLooped();
}

void msw::SelectMenu::CCourtChr::StateControl()
{
    static const float speed = 1.0f;

    switch (m_Unk30) {
        case STATE_NONE:
            // Back to normal speed once the table's animation is over.
            if (IsAnimEnd() || IsAnimLooped()) {
                m_fAnimSpeed = speed;
                SetTableAnimSpeed(m_fAnimSpeed);
            }
            break;
    }
}

void msw::SelectMenu::CCourtChr::Main()
{
    StateControl();
    if (m_Unk2C == 1) {
        for (long i = 0; i < 5; i++) {
            CCourtBallMdlObj *pBall =
                static_cast<CCourtBallMdlObj *>(bcs::CScnObj::GetScnObj(m_hMdl[GetMdlObjIdFromBallId(i)]));
            if (pBall != 0) {
                pBall->Main();
            }
        }
    }
    if (m_bClose) {
        m_Unk30 = STATE_CLOSE;
    }
    for (long i = 0; i < 6; i++) {
        if (m_pCharaObj[i] != 0) {
            m_pCharaObj[i]->StateCheck();
        }
    }
    if (m_pTableSet != 0) {
        m_pTableSet->MainLoop();
    }
}

inline float msw::SelectMenu::CCourtChr::GetAnimRate(long nAnim)
{
    for (long i = 0; i < ANIM_NUM; i++) {
        if (nAnim == s_AnimRate[i].nAnim) {
            return s_AnimRate[i].fRate;
        }
    }
    return 1.0f;
}

inline void msw::SelectMenu::CCourtChr::PlayAnim(long nAnim)
{
    float rate = GetAnimRate(nAnim) * m_fAnimSpeed;
    if (m_pTableSet != 0) {
        if (nAnim != -1) {
            m_pTableSet->Play(nAnim, rate, 0.0f, 0, -1.0f);
        }
        m_nAnim = nAnim;
    }
}

inline void msw::SelectMenu::CCourtChr::PushAnim(long nAnim)
{
    float rate = GetAnimRate(nAnim) * m_fAnimSpeed;
    if (m_pTableSet != 0 && nAnim != -1) {
        m_pTableSet->PushBack(nAnim, rate, 0.0f);
    }
}

inline void msw::SelectMenu::CCourtChr::SelectBall(long nBall)
{
    for (long i = 0; i < 5; i++) {
        bcs::CScnObj *pBall = bcs::CScnObj::GetScnObj(m_hMdl[GetMdlObjIdFromBallId(i)]);
        if (pBall != 0) {
            SetMdlHide(pBall, i != nBall);
        }
    }
}

inline void msw::SelectMenu::CCourtChr::SetBallDispEnable(int bDisp)
{
    int bHide = (bDisp == 0);
    for (long i = 0; i < 5; i++) {
        bcs::CScnObj *pBall = bcs::CScnObj::GetScnObj(m_hMdl[GetMdlObjIdFromBallId(i)]);
        if (pBall != 0) {
            SetMdlHide(pBall, bHide);
        }
    }
}

long msw::SelectMenu::CCourtChr::SendEvent(long nEvent, void *pParam)
{
    long nAnim = (long)pParam;
    long result = 0;

    if (m_Unk2C == 0) {
        return 0;
    }

    switch (nEvent) {
        case EVENT_DISP:
            SetDispEnable((int)pParam);
            SetCharaDispEnable((int)pParam);
            if (pParam == 0) {
                for (long i = 0; i < 6; i++) {
                    SetCharaModel(i, 0xFF, 0xFF, 0, -1, RFLDataSource_Default, 0, 0);
                }
                m_nAnim = -1;
            }
            break;
        case EVENT_PLAY_ANIM:
            if (nAnim == m_nAnim) {
                break;
            }
            // fall through
        case EVENT_PLAY_ANIM_FORCE: {
            PlayAnim(nAnim);

            // The loop that follows each opening animation.
            long nNext;
            switch (nAnim) {
                case 0:
                    nNext = 1;
                    break;
                case 20:
                    nNext = 1;
                    break;
                case 2:
                    nNext = 3;
                    break;
                case 7:
                    nNext = 8;
                    break;
                case 5:
                    nNext = 6;
                    break;
                case 13:
                    nNext = 14;
                    break;
                case 11:
                    nNext = 12;
                    break;
                default:
                    nNext = -1;
                    break;
            }
            if (nNext >= 0) {
                PushAnim(nNext);
            }
            break;
        }
        case EVENT_PUSH_ANIM:
            PushAnim(nAnim);
            break;
        case EVENT_CHANGE_ANIM:
            WaitToPlayAnim(CCourtCharaObj::s_FadeInOutTime, nAnim, GetAnimRate(nAnim) * m_fAnimSpeed);
            break;
        case EVENT_ANIM_SPEED:
            m_fAnimSpeed = *(float *)pParam;
            SetTableAnimSpeed(m_fAnimSpeed);
            break;
        case EVENT_BALL_SELECT:
            SelectBall(nAnim);
            break;
        case EVENT_GET_ANIM:
            result = m_nAnim;
            break;
    }
    return result;
}

void msw::SelectMenu::CCourtChr::WaitToPlayAnim(float fWait, long nAnim, float fSpeed)
{
    CMenuMdlTableSet *pTableSet = m_pTableSet;
    if (pTableSet != 0) {
        pTableSet->SetWait(fWait);
        pTableSet->ClearList();
        pTableSet->PushBack(nAnim, fSpeed, 0.0f);
    }
}

void msw::SelectMenu::CCourtChr::SetDispEnable(int bDisp)
{
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
        m_pTableSet->GetObj()->SetVisible(bDisp);
    }
}

void msw::SelectMenu::CCourtChr::SetCharaDispEnable(int bDisp)
{
    int bHide = (bDisp == 0);
    for (long i = 0; i < 6; i++) {
        // m_nShown holds places, not m_hMdl entries (GetMdlObjIdFromCharaId
        // is missing here in the original).
        CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[m_nShown[i]]);
        if (pObj != 0 && pObj->GetMdl() != 0) {
            SetMdlHide(pObj->GetMdl(), bHide);
        }
    }
}

unsigned long msw::SelectMenu::CCourtChr::SetCharaModel(long nIndex, long nChr, long nSub, long bEffect, long nSlot,
                                                        RFLDataSource source, long nMii, int bUnused)
{
    unsigned char nMdlId = GetMdlObjIdFromCharaId(nIndex);
    unsigned long hMdl = 0;
    CChrMdlObj *pMdl = 0;

    if (nSlot < 0 || nIndex == m_nShown[nSlot]) {
        // Nothing to do if this character is shown there already.
        CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[nMdlId]);
        if (pObj != 0 && pObj->GetMdl() != 0 && !(pObj->GetMdl()->GetFlag() & bcs::CScnObj::FLAG_HIDE_40)) {
            if (nChr >= 19 && nChr <= 24) {
                if (pObj->GetChr() >= 19 && nSub == pObj->GetSub() && bEffect == pObj->GetEffectFlag() &&
                    source == pObj->GetSource() && nMii == pObj->GetMiiIndex()) {
                    return 0;
                }
            } else if (nChr == pObj->GetChr() && nSub == pObj->GetSub() && bEffect == pObj->GetEffectFlag()) {
                return 0;
            }
        }
    }

    if (nChr < 25) {
        pMdl = CreateChrMenuObj(nMdlId, nChr, nSub, bEffect, source, nMii, 2);
    }
    if (pMdl != 0) {
        hMdl = pMdl->GetHandle();
    }
    m_hMdl[nMdlId] = hMdl;

    CCourtCharaObj *pChara = m_pCharaObj[nIndex];
    if (pChara != 0) {
        pChara->SetNextMdlHandle(hMdl);
        if (pMdl != 0) {
            pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            pMdl->SetAlphaRateF(0.0f);
        }
        if (nChr < 25) {
            pChara->ChangeState(pChara->GetMdlHandle() == 0 ? CCourtCharaObj::STATE_IN
                                                             : CCourtCharaObj::STATE_CHARA_CHANGE,
                                1);
        } else {
            // No character: the one shown leaves.
            pChara->ChangeState(CCourtCharaObj::STATE_OUT, 1);
            CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(pChara->GetMdlHandle());
            if (pObj != 0) {
                pObj->StopAttachEffect();
            }
        }
        if (nSlot >= 0 && nSlot <= 5) {
            m_nShown[nSlot] = nIndex;
        }
    }

    // Every character shown goes on its place of the table.
    bcs::CMdlObj *pTable = m_pTableSet->GetObj()->GetMdl();
    for (long i = 0; i < 6; i++) {
        CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[GetMdlObjIdFromCharaId(m_nShown[i])]);
        if (pObj != 0 && pObj->GetMdl() != 0 && pTable != 0) {
            pObj->GetMdl()->AttachToObject(pTable, pTable->GetNodeID(GetPosName(i)), bcs::CScnObj::ATTACHMODE_0);
        }
    }
    return 1;
}

void msw::SelectMenu::CCourtChr::SetCharaMotion(long nIndex, long nMotion, long nFlag, float fRate, float fBlend,
                                                float fFrame, int bUnused)
{
    CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[GetMdlObjIdFromCharaId(nIndex)]);
    if (pObj != 0) {
        // Not again if it is playing already.
        if (nMotion != pObj->GetMot() || pObj->GetMotFlag() != 0 || fRate != pObj->GetMotRate()) {
            pObj->AnimPlayMot(nMotion, nFlag, fRate, fBlend, fFrame);
        }
    }
}

unsigned long msw::SelectMenu::CCourtChr::GetMdlHandleByCharaId(long nIndex)
{
    CCourtCharaObj *pChara = m_pCharaObj[nIndex];
    if (pChara == 0) {
        return 0;
    }
    return pChara->GetMdlHandle();
}

unsigned long msw::SelectMenu::CCourtChr::GetNextMdlHandleByCharaId(long nIndex)
{
    CCourtCharaObj *pChara = m_pCharaObj[nIndex];
    if (pChara == 0) {
        return 0;
    }
    return pChara->GetNextMdlHandle();
}

nw4r::g3d::ResTexObj msw::SelectMenu::CCourtChr::GetResTexObj(const char *pName, unsigned long nMat)
{
    if (m_pTableSet == 0) {
        return nw4r::g3d::ResTexObj(0);
    }
    if (m_pTableSet->GetObj() == 0) {
        return nw4r::g3d::ResTexObj(0);
    }
    bcs::CMdlObj *pMdl = m_pTableSet->GetObj()->GetMdl();
    if (pMdl == 0) {
        return nw4r::g3d::ResTexObj(0);
    }
    nw4r::g3d::ResFile file = pMdl->GetResMdl().GetParent();
    if (!file.IsValid()) {
        return nw4r::g3d::ResTexObj(0);
    }
    if (file.GetResTexNumEntries() == 0) {
        return nw4r::g3d::ResTexObj(0);
    }
    if (!file.GetResTex(pName).IsValid()) {
        return nw4r::g3d::ResTexObj(0);
    }
    bcs::CMdlObj::CopiedMatAccess access(pMdl, nMat);
    if (!access.IsValid()) {
        return nw4r::g3d::ResTexObj(0);
    }
    return access.GetResTexObj(0);
}

void msw::SelectMenu::CCourtChr::SetMarkerColor(long nMarker, long nColor)
{
    if (nMarker >= 0 && nMarker <= 5 && nColor >= 0 && nColor <= 4) {
        bcs::CScnObj *pMarker = bcs::CScnObj::GetScnObj(m_hMdl[GetMdlObjIdFromMarkerId(nMarker)]);
        if (pMarker != 0) {
            pMarker->SetMatTevColor(s_Param.pMarkerColorName, GX_TEVREG0, s_MarkerColor[nColor]);
        }
    }
}

void msw::SelectMenu::CCourtChr::SetMarkerDisp(long nMarker, int bDisp)
{
    bcs::CScnObj *pMarker = bcs::CScnObj::GetScnObj(m_hMdl[GetMdlObjIdFromMarkerId(nMarker)]);
    if (pMarker != 0) {
        SetMdlHide(pMarker, !bDisp);
    }
}

void msw::SelectMenu::CCourtChr::SetMarkerAlpha(float fAlpha)
{
    if (fAlpha > 1.0f) {
        fAlpha = 1.0f;
    }
    for (long i = 0; i < 6; i++) {
        bcs::CScnObj *pMarker = bcs::CScnObj::GetScnObj(m_hMdl[GetMdlObjIdFromMarkerId(i)]);
        if (pMarker != 0) {
            pMarker->SetAlphaRateF(fAlpha);
        }
    }
}

long msw::SelectMenu::CCourtChr::GetNodePositionByPosId(long nPos, bcs::FVEC3 &rPos)
{
    const char *pName = GetPosName(nPos);
    if (pName == 0) {
        return -1;
    }
    if (m_pTableSet == 0) {
        return -2;
    }
    if (m_pTableSet->GetObj() == 0) {
        return -3;
    }
    m_pTableSet->GetObj()->GetNodePosition(pName, rPos);
    return 0;
}

void msw::SelectMenu::CCourtChr::SetTableAnimSpeed(float fSpeed)
{
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0 && m_pTableSet->GetObj()->GetMdl() != 0) {
        bcs::CAnmObj *pAnm = m_pTableSet->GetObj()->GetMdl()->GetAnmObjChr();
        if (pAnm != 0) {
            pAnm->SetRate(fSpeed);
        }
    }
}

float msw::SelectMenu::CCourtChr::GetTableAnimSpeed()
{
    if (m_pTableSet == 0) {
        return 0.0f;
    }
    if (m_pTableSet->GetObj() == 0) {
        return 0.0f;
    }
    if (m_pTableSet->GetObj()->GetMdl() == 0) {
        return 0.0f;
    }
    bcs::CAnmObj *pAnm = m_pTableSet->GetObj()->GetMdl()->GetAnmObjChr();
    if (pAnm == 0) {
        return 0.0f;
    }
    return pAnm->GetRate();
}

unsigned char msw::SelectMenu::CCourtChr::GetMdlObjIdFromCharaId(unsigned char nChara)
{
    static const unsigned char convertTbl[6] = { 1, 2, 3, 4, 5, 6 };
    return convertTbl[nChara];
}

unsigned char msw::SelectMenu::CCourtChr::GetMdlObjIdFromBallId(unsigned char nBall)
{
    static const unsigned char convertTbl[5] = { 7, 8, 9, 10, 11 };
    return convertTbl[nBall];
}

unsigned char msw::SelectMenu::CCourtChr::GetMdlObjIdFromMarkerId(unsigned char nMarker)
{
    static const unsigned char convertTbl[6] = { 12, 13, 14, 15, 16, 17 };
    return convertTbl[nMarker];
}

msw::SelectMenu::CCourtBall::CCourtBall(long nId)
    : CCourtChr(nId)
{
}

msw::SelectMenu::CCourtBall::~CCourtBall()
{
    Nothing();
}

int msw::SelectMenu::CCourtBall::Set()
{
    if (m_Unk2C != 0) {
        return 0;
    }

    nw4r::g3d::ResFile file = m_ResFile[0];
    bcs::CMdlObj *pMdl = CreateMdlObj(s_MdlTable[1].pName, file);
    if (pMdl != 0) {
        CCourtTableSetObj *pObj = new CCourtTableSetObj(pMdl);
        if (pObj != 0) {
            CMenuMdlTableSet *pTableSet = new CMenuMdlTableSet;
            if (pTableSet != 0) {
                pTableSet->SetObj(pObj);
                pTableSet->SetResFile(file);
                pTableSet->SetAnimTable(s_AnimTable[CLytCtrl::IsLocationAdjust() != 0], ANIM_NUM);
            }
            m_pTableSet = pTableSet;
        }
    }

    // The balls, one on the node of each sport.
    unsigned long nGold = 0;
    bcs::CMdlObj *pTable = 0;
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
        pTable = m_pTableSet->GetObj()->GetMdl();
    }
    if (pTable != 0) {
        for (long i = 0; i < 5; i++) {
            nw4r::g3d::ResFile ballFile = m_ResFile[i + 1];
            if (!ballFile.IsValid()) {
                continue;
            }
            // The mixed sports' ball only when they are unlocked.
            if (i == 4 && !m_bMixMode) {
                continue;
            }
            CCourtBallMdlObj *pBall = CreateBallMdlObj(s_MdlTable[i + 2].pName, ballFile);
            if (pBall == 0) {
                continue;
            }
            m_hMdl[MDL_BALL + i] = pBall->GetHandle();
            AttachToPos(pBall, pTable, i);

            const BallInfo &info = s_Param.ball[i];
            pBall->SetSpin(bcs::FVEC3(info.vRot.x * DEG_TO_RAD, info.vRot.y * DEG_TO_RAD, info.vRot.z * DEG_TO_RAD),
                           info.fRotSpeed * DEG_TO_RAD);

            // A sport's ball is gold once its top-menu character is
            // unlocked.
            int bGold;
            if (i != 4) {
                if (CTournament::IsUnlockTopMenuChr(i, -1)) {
                    nGold |= 1 << i;
                }
                bGold = (nGold & (1 << i)) != 0;
            } else {
                bGold = 0;
            }
            pBall->SetGoldColor(bGold);

            if (i == 4) {
                CMenuMdlTableSet *pTableSet = m_pTableSet;
                if (pTableSet != 0) {
                    pBall->Play(pTableSet->GetResAnimMot(s_Param.pBallMotName[1], ballFile), 1.0f, 2);
                    pBall->Play(pTableSet->GetResAnimClr(s_Param.pClrName[1], ballFile), 1.0f);
                    pBall->Play(pTableSet->GetResAnimTsa(s_Param.pTsaName[1], ballFile), 1.0f);
                }
            }
        }
    }
    SetDispEnable(0);
    SetBallDispEnable(0);

    InitShown();
    PlayAnim(19);
    m_bClose = 0;
    m_Unk2C = 1;
    m_Unk30 = STATE_NONE;
    return 0;
}

long msw::SelectMenu::CCourtBall::SendEvent(long nEvent, void *pParam)
{
    if (m_Unk2C == 0) {
        return 0;
    }
    if (nEvent == EVENT_BALL_DISP) {
        SetBallDispEnable((int)pParam);
    }
    return 0;
}

msw::SelectMenu::CCourtChrBall::CCourtChrBall(long nId)
    : CCourtChr(nId)
{
}

msw::SelectMenu::CCourtChrBall::~CCourtChrBall()
{
    Nothing();
}

int msw::SelectMenu::CCourtChrBall::Set()
{
    if (m_Unk2C != 0) {
        return 0;
    }

    nw4r::g3d::ResFile file = m_ResFile[0];
    bcs::CMdlObj *pMdl = CreateMdlObj(s_MdlTable[1].pName, file);
    if (pMdl != 0) {
        CCourtTableSetObj *pObj = new CCourtTableSetObj(pMdl);
        if (pObj != 0) {
            CMenuMdlTableSet *pTableSet = new CMenuMdlTableSet;
            if (pTableSet != 0) {
                pTableSet->SetObj(pObj);
                pTableSet->SetResFile(file);
                pTableSet->SetAnimTable(s_AnimTable[CLytCtrl::IsLocationAdjust() != 0], ANIM_NUM);
            }
            m_pTableSet = pTableSet;
        }
    }

    // The balls, all on the one ball node.
    unsigned long nGold = 0;
    bcs::CMdlObj *pTable = 0;
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
        pTable = m_pTableSet->GetObj()->GetMdl();
    }
    if (pTable != 0) {
        for (long i = 0; i < 5; i++) {
            nw4r::g3d::ResFile ballFile = m_ResFile[i + 1];
            if (!ballFile.IsValid()) {
                continue;
            }
            CCourtBallMdlObj *pBall = CreateBallMdlObj(s_MdlTable[i + 2].pName, ballFile);
            if (pBall == 0) {
                continue;
            }
            m_hMdl[MDL_BALL + i] = pBall->GetHandle();
            AttachToPos(pBall, pTable, 6);

            const BallInfo &info = s_Param.ball[i];
            pBall->SetSpin(bcs::FVEC3(info.vRot.x * DEG_TO_RAD, info.vRot.y * DEG_TO_RAD, info.vRot.z * DEG_TO_RAD),
                           info.fRotSpeed * DEG_TO_RAD);

            int bGold;
            if (i != 4) {
                if (CTournament::IsUnlockTopMenuChr(i, -1)) {
                    nGold |= 1 << i;
                }
                bGold = (nGold & (1 << i)) != 0;
            } else {
                bGold = 0;
            }
            pBall->SetGoldColor(bGold);

            if (i == 4) {
                CMenuMdlTableSet *pTableSet = m_pTableSet;
                pBall->Play(pTableSet->GetResAnimMot(s_Param.pBallMotName[0], ballFile), 1.0f, 2);
                pBall->Play(pTableSet->GetResAnimClr(s_Param.pClrName[1], ballFile), 1.0f);
                pBall->Play(pTableSet->GetResAnimTsa(s_Param.pTsaName[1], ballFile), 1.0f);
            }
        }
    }
    SelectBall(-1);
    SetDispEnable(0);

    InitShown();
    PlayAnim(19);
    m_bClose = 0;
    m_Unk2C = 1;
    m_Unk30 = STATE_NONE;
    return 0;
}

long msw::SelectMenu::CCourtChrBall::SendEvent(long nEvent, void *pParam)
{
    long result = 0;

    if (m_Unk2C == 0) {
        return 0;
    }
    if (nEvent == EVENT_DISP) {
        SetDispEnable((int)pParam);
        if (pParam == 0) {
            m_nAnim = -1;
        }
    } else {
        result = CCourtChr::SendEvent(nEvent, pParam);
    }
    return result;
}
