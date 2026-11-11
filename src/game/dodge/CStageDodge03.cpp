// msw::CStageDodge03: dodge stage 03, the fountain stage. The centre line
// between the two courts is a row of water jets. Every CENTER_CYCLE seconds
// the next row gushes up on another line (chosen at random among
// 2 * ORIFICE_NUM + 1 places) and becomes the centre line, so the courts
// grow and shrink; every BORDERLESS_CNT moves the court has no centre line
// for a while, and then nobody may cross back (CANNOT_COMEBACK).
#include "game/msw/CStageDodge03.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/DodgePrm.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, its address, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S03_PRM_TYPE {
        S03_PRM_TYPE_END,
        S03_PRM_TYPE_LONG,
        S03_PRM_TYPE_FLOAT
    };

    struct S03_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S03_PRM_TBL s_PrmTbl[] = {
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_CENTER_CYCLE, 0x00 },
        { S03_PRM_TYPE_LONG, &DODGE_PRM_S03_ORIFICE_NUM, 0x04 },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_ORIFICE_SPACE, 0x08 },
        { S03_PRM_TYPE_LONG, &DODGE_PRM_S03_BORDERLESS_CNT, 0x0C },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_BORDERLESS_TIME, 0x10 },
        { S03_PRM_TYPE_LONG, &DODGE_PRM_S03_CANNOT_COMEBACK, 0x14 },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_WATER_HEIGHT, 0x18 },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_WATER_PRESTART_TIME, 0x1C },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_PAT2_PRESTART_TIME, 0x20 },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_WATER_TALL_TIME, 0x24 },
        { S03_PRM_TYPE_FLOAT, &DODGE_PRM_S03_WATER_SPEED, 0x28 },
        { S03_PRM_TYPE_LONG, &DODGE_PRM_S03_WATER_NUM, 0x2C },
        { S03_PRM_TYPE_END, 0, 0 },
    };

    // (name guessed) v kept between lo and hi.
    template <typename T> static inline T Clamp(T v, T lo, T hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }

    // After the centre line has moved, the characters work out again which
    // area they stand in.
    static inline void UpdateInfieldAreaCodes()
    {
        long num = CChrMan::GetChrCoreNum();
        for (long i = 0; i < num; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && pChr->IsEnable() && pChr->IsDodgeInfield()) {
                pChr->DodgeForceUpdateAreaCode();
            }
        }
    }
}

// Model kinds of the stage's MDL_GEN_RULE table that are not made.
enum {
    MDL_SKIP_0 = 3,
    MDL_SKIP_1 = 4
};

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_AMBIENT = 1,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_AMBIENT = 0xD18,
    SE_AMBIENT2 = 0xD19,
    SE_EVENT = 0xD1A
};

// Stage messages handled here (sent through CStageMan::SendMessage).
enum {
    MSG_WATER = 0x25, // arg1 1: the court loses its centre line at once
    MSG_MODE = 0x27   // arg1 2: the stage runs without a centre line
};

const void *msw::CStageDodge03::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge03::CStageDodge03(bcs::CTask *pParent) : CStageManDodge(pParent)
{
    m_nCurSlot = 0;
    m_fCourtCenter = 0.0f;
    m_nCenterNo = 0;
    m_fTimer = 0.0f;
    m_nBorderlessCnt = 0;
    m_bNoBorderMode = 0;
}

msw::CStageDodge03::~CStageDodge03()
{
}

// Team 0 plays behind the centre line, team 1 in front of it.
float msw::CStageDodge03::GetTeamCourtFront(long team) const
{
    float center = GetCurCourtCenter_();
    if (team == 0) {
        return center;
    }
    return -GetBaseCourtDepth_();
}

float msw::CStageDodge03::GetTeamCourtBack(long team) const
{
    float center = GetCurCourtCenter_();
    if (team == 0) {
        return GetBaseCourtDepth_();
    }
    return center;
}

// The outer fields are those of the other team (as in CStageManDodge),
// measured from the moving courts.
float msw::CStageDodge03::GetTeamOuterFieldFront(long team, long n) const
{
    long other = team ^ 1;
    if (other == 0) {
        if (n == 1) {
            return GetTeamCourtBack(other);
        }
        return GetBaseOuterFront_();
    }
    if (n == 2 || n == 3) {
        return GetTeamCourtFront(other);
    }
    float depth = GetBaseOuterDepth_();
    return GetTeamCourtFront(other) - depth;
}

float msw::CStageDodge03::GetTeamOuterFieldBack(long team, long n) const
{
    long other = team ^ 1;
    if (other == 0) {
        if (n == 2 || n == 3) {
            return GetTeamCourtBack(other);
        }
        float depth = GetBaseOuterDepth_();
        return GetTeamCourtBack(other) + depth;
    }
    if (n == 1) {
        return GetTeamCourtFront(other);
    }
    return -GetBaseOuterFront_();
}

void msw::CStageDodge03::Main_(float frame)
{
    UpdateCourtCenter_(frame);
    for (int i = 0; i < 3; i++) {
        m_aWater[i].Update(frame);
    }
    CStageManDodge::Main_(frame);
}

// When the timer runs out the current row stops (first rising tall when the
// court is about to lose its line), the next row becomes the centre line
// and the one after it is chosen.
void msw::CStageDodge03::UpdateCourtCenter_(float frame)
{
    bool bMoved = false;

    if (IsGimmickRunning() && !m_bNoBorderMode) {
        m_fTimer -= frame;
        if (m_fTimer <= 0.0f) {
            CWaterInfo &rWater = m_aWater[Water_GetCurSlot()];
            if (!rWater.IsSwooshTall() && IsNextNoBorder()) {
                rWater.TallStart();
                m_fTimer = DODGE_PRM_S03_WATER_TALL_TIME;
            } else {
                rWater.Stop();
                m_nCenterNo = m_nNextCenterNo;
                m_nNextCenterNo = NO_BORDER;
                m_nCurSlot = Water_GetNextSlot();
                if (!IsNoBorder()) {
                    SetCenterNo(m_nCenterNo, 60.0f * DODGE_PRM_S03_CENTER_CYCLE);
                    m_aWater[Water_GetCurSlot()].Start(m_nCenterNo, 1);
                } else {
                    SetNoBorder(60.0f * DODGE_PRM_S03_BORDERLESS_TIME);
                }
                SelectNextCourtCenter_();
                bMoved = true;
            }
        }
    }

    CommitCourtCenter_();

    if (bMoved) {
        UpdateInfieldAreaCodes();
    }
}

// Picks the line after the current one (a random line other than the
// current one, or none every BORDERLESS_CNT moves) and lets its row of jets
// gush up ahead of time.
void msw::CStageDodge03::SelectNextCourtCenter_()
{
    static const long nRetry = 1000;

    Water_GetNextSlot();

    m_nBorderlessCnt++;
    if (DODGE_PRM_S03_BORDERLESS_CNT > 0 && m_nBorderlessCnt >= DODGE_PRM_S03_BORDERLESS_CNT) {
        m_nNextCenterNo = NO_BORDER;
        m_nBorderlessCnt = 0;
    } else {
        long next = 0;
        long num = DODGE_PRM_S03_ORIFICE_NUM;
        if (num > 0) {
            if (IsNoBorder()) {
                m_nBorderlessCnt = 0;
            } else {
                bool bSame = true;
                for (long i = 0; i < nRetry; i++) {
                    next = bcs::BCS_RandRange(-num, num);
                    if (next != m_nCenterNo) {
                        bSame = false;
                        break;
                    }
                }
                if (bSame) {
                    next = (next + num + 1) % (num * 2 + 1) - num;
                }
            }
        }
        m_nNextCenterNo = next;
    }

    float time = m_fTimer;
    if (!IsNoBorder()) {
        time -= DODGE_PRM_S03_WATER_PRESTART_TIME;
    } else {
        time -= DODGE_PRM_S03_PAT2_PRESTART_TIME;
        time += DODGE_PRM_S03_WATER_TALL_TIME;
    }
    if (0.0f > time) {
        time = 0.0f;
    }

    long next = m_nNextCenterNo;
    if (next != NO_BORDER) {
        m_aWater[Water_GetNextSlot()].PreStart(next, time, 0);
    }
}

// Keeps the centre line inside the places and works out its z.
void msw::CStageDodge03::CommitCourtCenter_()
{
    long num = DODGE_PRM_S03_ORIFICE_NUM;

    if (!IsNoBorder()) {
        m_nCenterNo = Clamp(m_nCenterNo, -num, num);
        m_fCourtCenter = CalcCourtCenterZ_(m_nCenterNo);
    } else {
        m_fCourtCenter = 0.0f;
    }
}

float msw::CStageDodge03::CalcCourtCenterZ_(long centerNo) const
{
    static const float fMargin = 1.0f;

    if (centerNo == NO_BORDER) {
        return 0.0f;
    }

    float z = centerNo * DODGE_PRM_S03_ORIFICE_SPACE;
    float front = GetCourtFront() + fMargin;
    float back = GetCourtBack() - fMargin;
    return Clamp(z, front, back);
}

void msw::CStageDodge03::SetCenterNo(long centerNo, float time)
{
    m_fTimer = time;
    m_nCenterNo = centerNo;
    CommitCourtCenter_();
}

// Back to the middle line (or to no line in the mode without one).
void msw::CStageDodge03::ResetCenter()
{
    Water_ResetAll();
    m_nCurSlot = Water_GetNextSlot();
    if (!m_bNoBorderMode) {
        SetCenterNo(0, 60.0f * DODGE_PRM_S03_CENTER_CYCLE);
        m_aWater[Water_GetCurSlot()].Start(m_nCenterNo, 1);
        m_nBorderlessCnt = 0;
        SelectNextCourtCenter_();
    } else {
        m_nCurSlot = 0;
        m_nCenterNo = NO_BORDER;
        m_nNextCenterNo = NO_BORDER;
    }
}

void msw::CStageDodge03::SetNoBorder(float time)
{
    m_fTimer = time;
    m_nCenterNo = NO_BORDER;
    CommitCourtCenter_();
}

// Without a centre line, a character who left the infield may not come back
// (DODGE_PRM_S03_CANNOT_COMEBACK).
bool msw::CStageDodge03::CanbeComeback() const
{
    if (DODGE_PRM_S03_CANNOT_COMEBACK && !IsNoBorder()) {
        return false;
    }
    return true;
}

bool msw::CStageDodge03::CanbeJump(const bcs::FVEC3 &) const
{
    return false;
}

void msw::CStageDodge03::UpdateWaterPattern_(float)
{
}

void msw::CStageDodge03::UpdateWaterPos_(float)
{
}

void msw::CStageDodge03::Water_PreStart(long centerNo, float time)
{
    if (centerNo != NO_BORDER) {
        m_aWater[m_nCurSlot].PreStart(centerNo, time, 0);
    }
}

void msw::CStageDodge03::Water_Start(long centerNo)
{
    if (centerNo != NO_BORDER) {
        m_aWater[m_nCurSlot].Start(centerNo, 0);
    }
}

void msw::CStageDodge03::Water_StopAll()
{
    for (int i = 0; i < 3; i++) {
        m_aWater[i].Stop();
    }
}

void msw::CStageDodge03::Water_TallStartAll()
{
    for (int i = 0; i < 3; i++) {
        m_aWater[i].TallStart();
    }
}

void msw::CStageDodge03::Water_ResetAll()
{
    for (int i = 0; i < 3; i++) {
        m_aWater[i].Reset();
    }
}

bool msw::CStageDodge03::Water_IsSwooshAll() const
{
    return false;
}

void msw::CStageDodge03::Water_NextPattern()
{
}

void msw::CStageDodge03::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManDodge::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case MSG_WATER:
            if (arg1 == 1) {
                Water_StopAll();
                SetNoBorder(60.0f * DODGE_PRM_S03_BORDERLESS_TIME);
                SelectNextCourtCenter_();
                CommitCourtCenter_();
                UpdateInfieldAreaCodes();
            }
            break;
        case MSG_MODE:
            if (arg1 == 2) {
                m_bNoBorderMode = 1;
            }
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge03::GetMdlGenRule_() const
{
    // Only the end entry, which chains to the table of CStageMan.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

bool msw::CStageDodge03::OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, nw4r::g3d::ResFile,
                                        CArn &rArn, long n)
{
    rArn.GetName(n); // (result not used: probably a stripped debug print)

    switch (pRule->m_nId) {
        case MDL_SKIP_0:
            return false;
        case MDL_SKIP_1:
            return false;
    }
    return true;
}

void msw::CStageDodge03::OnPostInitialize()
{
    CStageManDodge::OnPostInitialize();
    for (int i = 0; i < 3; i++) {
        m_aWater[i].Init(this);
    }
}

void msw::CStageDodge03::OnSceneEntry()
{
}

void msw::CStageDodge03::OnGimmickReset()
{
    ResetCenter();
}

void msw::CStageDodge03::OnGimmickStop()
{
    Water_ResetAll();
    m_nCurSlot = Water_GetNextSlot();
}

void msw::CStageDodge03::OnGimmickPause()
{
}

void msw::CStageDodge03::OnGimmickResume()
{
}

void msw::CStageDodge03::OnArtsEventStart()
{
    Water_ResetAll();
}

void msw::CStageDodge03::OnArtsEventEnd()
{
    if (!m_bNoBorderMode) {
        ResetCenter();
    }
}

// In the mode without a centre line, a special shot still gets one in the
// middle for its length.
void msw::CStageDodge03::OnArtsShootStart()
{
    if (m_bNoBorderMode) {
        Water_ResetAll();
        m_nCurSlot = 0;
        SetCenterNo(0, 600.0f);
        m_aWater[Water_GetCurSlot()].Start(m_nCenterNo, 1);
        m_nBorderlessCnt = 0;
        m_nNextCenterNo = NO_BORDER;
    }
}

void msw::CStageDodge03::OnArtsShootEnd()
{
    if (m_bNoBorderMode) {
        m_aWater[Water_GetCurSlot()].Stop();
        m_nCenterNo = NO_BORDER;
    }
}

void msw::CStageDodge03::OnEntranceEventGmkStart()
{
}

void msw::CStageDodge03::OnEntranceEventGmkEnd()
{
    Water_ResetAll();
    m_nCurSlot = Water_GetNextSlot();
}

// The centre line's state, with the z taken as its bits.
unsigned long msw::CStageDodge03::OnCalcSum() const
{
    unsigned long sum = m_nCurSlot;
    sum += *reinterpret_cast<const unsigned long *>(&m_fCourtCenter);
    sum += m_nNextCenterNo;
    return sum;
}

void msw::CStageDodge03::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
                SE_RepeatPlay(SE_SLOT_AMBIENT, SE_AMBIENT, 180.0f, 420.0f, -1.0f, 0.0f, 0xC);
            }
            if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
                SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

void msw::CStageDodge03::Disp3D(long n)
{
    CStageManDodge::Disp3D(n);
}

msw::CStageDodge03::CWaterInfo::CWaterInfo()
{
    m_pStage = 0;
    m_nCenterNo = 0;
    for (int i = 0; i < 6; i++) {
        m_apWater[i] = 0;
    }
}

msw::CStageDodge03::CWaterInfo::~CWaterInfo()
{
    for (int i = 0; i < 6; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Delete();
        }
        m_apWater[i] = 0;
    }
}

void msw::CStageDodge03::CWaterInfo::Init(CStageDodge03 *pStage)
{
    m_pStage = pStage;
    for (int i = 0; i < 6; i++) {
        CStgGmk_WaterWorks_Dodge *pWater = new CStgGmk_WaterWorks_Dodge(m_pStage, 0);
        if (pWater != 0) {
            pWater->Create();
            pWater->SceneEntry();
            m_apWater[i] = pWater;
        }
    }
    UpdatePos_();
}

void msw::CStageDodge03::CWaterInfo::Update(float)
{
    UpdatePos_();
}

void msw::CStageDodge03::CWaterInfo::UpdatePos_()
{
    CStageDodge03 *pStage = m_pStage;
    // (not used afterwards; the original builds it too)
    bcs::FVEC3 vCenter(0.0f, 0.0f, 0.0f);
    vCenter.z = pStage->CalcCourtCenterZ_(GetCurCenterNo());
    bcs::FVEC3 vPos(0.0f, 0.0f, 0.0f);

    float left = pStage->GetCourtLeft();
    float width = pStage->GetCourtRight() - left;
    vPos.z = m_pStage->CalcCourtCenterZ_(GetCurCenterNo());

    float x0 = vPos.x - 0.5f * width;
    float x1 = vPos.x + 0.5f * width;
    long num = DODGE_PRM_S03_WATER_NUM;
    vPos.y = 0.0f;

    // Jet i stands in the middle of the i-th of num equal parts.
    long i;
    for (i = 0; i < num; i++) {
        CStgGmk_WaterWorks_Dodge *pWater = m_apWater[i];
        if (pWater != 0) {
            bcs::FVEC3 vWater;
            vWater.x = x0 + (x1 - x0) * ((float)i / num + 1.0f / (num * 2));
            vWater.y = vPos.y;
            vWater.z = vPos.z;
            pWater->SetDefaultPos(vWater);
            pWater->SetPos(vWater);
            pWater->Activate();
        }
    }
    for (; i < 6; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Inactivate();
        }
    }
}

void msw::CStageDodge03::CWaterInfo::PreStart(long centerNo, float time, int)
{
    m_nCenterNo = centerNo;
    long num = DODGE_PRM_S03_WATER_NUM;
    if (num > 0) {
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterWorks_Dodge *pWater = m_apWater[i];
            if (pWater != 0) {
                pWater->SetParam_Height(DODGE_PRM_S03_WATER_HEIGHT);
                pWater->Start(time, 0);
            }
        }
    }
}

void msw::CStageDodge03::CWaterInfo::Start(long centerNo, int)
{
    m_nCenterNo = centerNo;
    long num = DODGE_PRM_S03_WATER_NUM;
    if (num > 0) {
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterWorks_Dodge *pWater = m_apWater[i];
            if (pWater != 0) {
                pWater->SetParam_Height(DODGE_PRM_S03_WATER_HEIGHT);
                pWater->Start(-1.0f, 1);
            }
        }
    }
}

void msw::CStageDodge03::CWaterInfo::Stop()
{
    for (int i = 0; i < 6; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Stop();
        }
    }
}

void msw::CStageDodge03::CWaterInfo::TallStart()
{
    for (int i = 0; i < 6; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->TallStart();
        }
    }
}

void msw::CStageDodge03::CWaterInfo::Reset()
{
    for (int i = 0; i < 6; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Reset();
        }
    }
}

bool msw::CStageDodge03::CWaterInfo::IsSwooshTall() const
{
    for (int i = 0; i < 6; i++) {
        if (m_apWater[i] != 0 && m_apWater[i]->IsSwooshTall()) {
            return true;
        }
    }
    return false;
}
