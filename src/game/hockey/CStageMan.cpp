// msw::CStageHockey02: hockey stage 02, the wave stage (the unit name
// CStageMan.cpp is the splitter's guess). CStage02Base (a DOL
// class) does the work; this class hands the stage manager's hooks on to it
// and gives it the hockey wave cluster and the HOCKEY_PRM_S02_* tuning.
#include "game/msw/CStageHockey02.hpp"

#include "game/msw/HockeyPrm.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgGmk_Wave_Hockey.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, the value, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;    // 0x00
        void *m_pValue;  // 0x04
        long m_nOffset;  // 0x08
    };

    static const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_PART_INTERVAL, 0x00 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_BIG_INTERVAL, 0x04 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_BIG_INTERVAL_RND, 0x08 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_ANIM_TIME, 0x0C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_BIG_ANIM_TIME, 0x10 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_OFFSET_X, 0x14 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_BIG_OFFSET_X, 0x18 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_PUSH_POWER, 0x1C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_PULL_POWER, 0x20 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_WAVE_POS_MARGIN, 0x24 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S02_COIN_POP_NUM, 0x28 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S02_BIG_COIN_POP_NUM, 0x2C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S02_ITEM_POP_CYCLE, 0x30 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S02_ITEM_POP_NUM, 0x34 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_ITEM_POP_DELAY, 0x38 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_ITEM_POP_COME_FRM, 0x3C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_ITEM_POP_DIST, 0x40 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_ITEM_POP_POS_X, 0x44 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S02_BIG_ITEM_POP_CYCLE, 0x48 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S02_BIG_ITEM_POP_NUM, 0x4C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_BIG_ITEM_POP_DELAY, 0x50 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_BIG_ITEM_POP_COME_FRM, 0x54 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_BIG_ITEM_POP_DIST, 0x58 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S02_BIG_ITEM_POP_POS_X, 0x5C },
        { PRM_TYPE_END, 0, 0 },
    };
}

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_WAVE_0 = 0,
    SE_SLOT_WAVE_1 = 1,
    SE_SLOT_WAVE_2 = 2,
    SE_SLOT_SPLASH = 3,
    SE_WAVE_0 = 0xD0E,
    SE_WAVE_1 = 0xD0F,
    SE_WAVE_2 = 0xD10,
    SE_SPLASH = 0xD11
};

// The model rule of the parasols.
enum {
    RULE_PARASOL = 0x32
};

// What GetHitMaterial reports on a wave.
enum {
    HIT_MATERIAL_WAVE = 3
};

const void *msw::CStageHockey02::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey02::CStageHockey02(bcs::CTask *pParent) : CStageManHockey(pParent), CStage02Base(pParent)
{
}

msw::CStageHockey02::~CStageHockey02()
{
}

void msw::CStageHockey02::Main_(float frame)
{
    Stage02_Main(frame);
    CStageManHockey::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey02::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_exnode_([0-9]+)_sibu00$", 0xA, 1, 0, NewMdl },
        { "^s[0-9]+_exnode_([0-9]+)_sibu01$", 0xB, 1, 0, NewMdl },
        { "^s[0-9]+_exnode_([0-9]+)_sibu02$", 0xC, 1, 0, NewMdl },
        { "^s[0-9]+_exnode_([0-9]+)_sibu03$", 0xD, 1, 0, NewMdl },
        { "^s[0-9]+_exnode_([0-9]+)_sibu04$", 0xE, 1, 0, NewMdl },
        { "^s[0-9]+_exnode_([0-9]+)_sibu05$", 0xF, 1, 0, NewMdl },
        { "^s[0-9]+_g_parasol_([0-9]+)$", RULE_PARASOL, 1, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

bool msw::CStageHockey02::OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         nw4r::g3d::ResFile file, CArn &rArn, long n)
{
    return Stage02_OnPreCreateMdl(pRule, rRegex, file, rArn, n);
}

// The parasols get slot 0x170 of CFgMdl with 1 (what it does is not known).
void msw::CStageHockey02::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
{
    if (pRule->m_nId == RULE_PARASOL && pNode->GetObj() != 0) {
        static_cast<CFgMdl *>(pNode->GetObj())->Unk170(1);
    }
    CStageManHockey::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageHockey02::OnPostInitialize()
{
    CStageManHockey::OnPostInitialize();
    Stage02_OnPostInitialize();
}

void msw::CStageHockey02::OnSceneEntry()
{
    Stage02_OnSceneEntry();
}

long msw::CStageHockey02::GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    return Stage02_GetFloorAttribute(rPos);
}

void msw::CStageHockey02::GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const
{
    Stage02_GetFloorFlow(rFlow, rPos, type);
}

long msw::CStageHockey02::GetHitMaterial(const bcs::FVEC3 &rPos) const
{
    if (Stage02_IsOnWave(rPos)) {
        return HIT_MATERIAL_WAVE;
    }
    return CStageManHockey::GetHitMaterial(rPos);
}

void msw::CStageHockey02::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManHockey::SendMessage(msg, arg1, arg2, arg3);
}

void msw::CStageHockey02::OnGimmickReset()
{
    Stage02_OnGimmickReset();
}

void msw::CStageHockey02::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_WAVE_0)) {
                SE_Play(SE_SLOT_WAVE_0, SE_WAVE_0, 0.0f, 3);
            }
            if (!SE_IsAlive(SE_SLOT_WAVE_1)) {
                SE_RepeatPlay(SE_SLOT_WAVE_1, SE_WAVE_1, 180.0f, 420.0f, -1.0f, 0.0f, 12);
            }
            if (!SE_IsAlive(SE_SLOT_WAVE_2)) {
                SE_Play(SE_SLOT_WAVE_2, SE_WAVE_2, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            SE_Play(SE_SLOT_SPLASH, SE_SPLASH, 0.0f, 0);
            break;
    }
}

float msw::CStageHockey02::GetParam_PushPower() const
{
    return HOCKEY_PRM_S02_WAVE_PUSH_POWER;
}

float msw::CStageHockey02::GetParam_PullPower() const
{
    return HOCKEY_PRM_S02_WAVE_PULL_POWER;
}

float msw::CStageHockey02::GetParam_WavePosMargin() const
{
    return HOCKEY_PRM_S02_WAVE_POS_MARGIN;
}

msw::CStgGmk_WaveCluster *msw::CStageHockey02::NewWaveCluster(bcs::CTask *pParent) const
{
    return new (1) CStgGmk_WaveCluster_Hockey(pParent);
}
