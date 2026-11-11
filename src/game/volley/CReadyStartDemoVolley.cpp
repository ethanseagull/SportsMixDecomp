// msw::CReadyStartDemoVolley: the volley ready-start demo, an event scene of
// resource 0x60 that shows the serving character, then the serve telop.
// Its camera (CSceneCalcReadyStart) is moved by an offset that depends on
// the character shown.
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCameraAnm.hpp"
#include "bcs/CRenderMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitVolleyTelopServe.hpp"
#include "game/msw/CEventChrCore.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CEventDemoBase.hpp"
#include "game/msw/CEventDemoVolley.hpp"
#include "game/msw/CEventScene.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CNameMesg.hpp"
#include "game/msw/CSceneCalc.hpp"
#include "game/msw/EvDemo.hpp"

namespace msw {

    // Size 0x7C (CReadyStartDemoVolley's constructor). The scene camera,
    // moved from m_PosFrom/m_AtFrom to m_PosTo/m_AtTo over the animation
    // (z turned round for goal 1). Member names are guesses.
    class CSceneCalcReadyStart : public CSceneCalc {
        public:
            CSceneCalcReadyStart(CEventScene *pScene) : CSceneCalc(pScene)
            {
                m_nGoal = -1;
                m_PosFrom.Set(0.0f, 0.0f, 0.0f);
                m_AtFrom.Set(0.0f, 0.0f, 0.0f);
                m_PosTo.Set(0.0f, 0.0f, 0.0f);
                m_AtTo.Set(0.0f, 0.0f, 0.0f);
                m_fTan0 = 0.05f;
                m_fTan1 = 0.05f;
            }
            virtual ~CSceneCalcReadyStart();
            virtual void Calc(bcs::CCameraAnm &rAnm, bcs::FVEC3 &rPos, bcs::FVEC3 &rAt, float &rRoll);

            long m_nGoal;         // 0x40, the goal the team guards
            float m_fTan0;        // 0x44 \ the slopes of the curve at the start
            float m_fTan1;        // 0x48 / and at the end
            bcs::FVEC3 m_PosFrom; // 0x4C
            bcs::FVEC3 m_AtFrom;  // 0x58
            bcs::FVEC3 m_PosTo;   // 0x64
            bcs::FVEC3 m_AtTo;    // 0x70
    };

    // Size 0xC4 (CreateReadyStartDemo).
    class CReadyStartDemoVolley : public CEventScene {
        public:
            CReadyStartDemoVolley(const short *pMotTable, bcs::CTask *pParent, long nArg);
            virtual ~CReadyStartDemoVolley();

            virtual nw4r::g3d::ResFile GetRes();
            virtual int EventFirst();
            virtual void EventStart();
            virtual void EventKill() { Delete(); }
            virtual long EventPlay(CUT_WORK &rCut);
            virtual long EventCansel(CUT_WORK &rCut, long nRet);
            virtual void CheckEventSkip();
            virtual int IsEventSkip();
            virtual void EventTimer(short nId, short, unsigned long);
            virtual void EventTimerReset(short nId, short, unsigned long);
            virtual void EventFlags(unsigned long, unsigned long);

        private:
            const short *m_pMotTable;      // 0xA4: given to the character model
            CSceneCalcReadyStart *m_pCalc; // 0xA8
            long m_nPlayer;                // 0xAC: the player shown
            long m_nTeam;                  // 0xB0: its team
            long m_nPad;                   // 0xB4: its pad, -1 for the computer
            long m_nChrId;                 // 0xB8: its character
            float m_fCutFrame;             // 0xBC: length of the cut
            unsigned long m_nSkipBit;      // 0xC0: skip bits a pad sets
    };
}

// The camera offsets of the ready-start cut for each character
// (CChrCore::GetChrTypeID): where the camera and its target end.
static bcs::FVEC3 s_PosTo[25] = {
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.15f, 0.35f, 0.35f),
    bcs::FVEC3(0.15f, 0.35f, 0.35f),
    bcs::FVEC3(0.15f, 0.35f, 0.35f),
    bcs::FVEC3(0.25f, 0.3f, 0.25f),
    bcs::FVEC3(0.0f, 0.35f, 0.5f),
    bcs::FVEC3(0.05f, 0.35f, 0.45f),
    bcs::FVEC3(0.2f, 0.35f, 0.3f),
    bcs::FVEC3(0.3f, 0.3f, 0.19999999f),
    bcs::FVEC3(0.05f, 0.35f, 0.45f),
    bcs::FVEC3(0.3f, 0.3f, 0.19999999f),
    bcs::FVEC3(0.3f, 0.3f, 0.19999999f),
    bcs::FVEC3(0.1f, 0.35f, 0.4f),
    bcs::FVEC3(0.25f, 0.3f, 0.25f),
    bcs::FVEC3(0.15f, 0.35f, 0.35f),
    bcs::FVEC3(0.1f, 0.4f, 0.4f),
    bcs::FVEC3(0.15f, 0.35f, 0.35f),
    bcs::FVEC3(0.3f, 0.3f, 0.19999999f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
    bcs::FVEC3(0.2f, 0.3f, 0.3f),
};

static bcs::FVEC3 s_AtTo[25] = {
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.05f, 0.5f),
    bcs::FVEC3(0.0f, -0.05f, 0.5f),
    bcs::FVEC3(0.05f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.05f, -0.0f, 0.5f),
    bcs::FVEC3(0.0f, -0.0f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.05f, -0.35f, 0.5f),
    bcs::FVEC3(0.05f, -0.0f, 0.5f),
    bcs::FVEC3(0.1f, -0.35f, 0.5f),
    bcs::FVEC3(0.0f, -0.35f, 0.5f),
    bcs::FVEC3(0.0f, -0.05f, 0.5f),
    bcs::FVEC3(0.05f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.0f, 0.5f),
    bcs::FVEC3(-0.6f, -0.5f, 1.05f),
    bcs::FVEC3(-0.05f, -0.05f, 0.5f),
    bcs::FVEC3(0.0f, -0.35f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
    bcs::FVEC3(0.0f, -0.15f, 0.5f),
};

msw::CReadyStartDemoVolley::CReadyStartDemoVolley(const short *pMotTable, bcs::CTask *pParent, long nArg)
    : CEventScene(8, pParent, nArg)
{
    m_pMotTable = pMotTable;
    m_nSkipBit = 0;
    m_nChrNum = CChrMan::GetChrCoreNum();
    m_pCalc = new CSceneCalcReadyStart(this);

    m_nPlayer = EvDemo::GetTargetReadyStart();
    m_nChrId = 0;
    m_nTeam = 0;
    m_nPad = -1;
    CChrCore *pChr = CChrMan::GetChrCore(m_nPlayer);
    if (pChr != 0) {
        m_nTeam = pChr->GetTeamNo();
        m_nChrId = pChr->GetChrTypeID();
        if (pChr->IsPadCtrl()) {
            m_nPad = pChr->GetPadNo();
        }
    }

    m_pCalc->m_PosTo = s_PosTo[m_nChrId];
    m_pCalc->m_AtTo = s_AtTo[m_nChrId];
    m_pCalc->m_nGoal = CGame::GetGuardGoalIdx(m_nTeam);
}

msw::CReadyStartDemoVolley::~CReadyStartDemoVolley()
{
    CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
    if (pDemo != 0) {
        pDemo->RemoveTelopAll();
    }
    bcs::CRenderMan::EnableDepthOfFieldEffect(0);
}

nw4r::g3d::ResFile msw::CReadyStartDemoVolley::GetRes()
{
    return CLoadData::GetRes(0x60);
}

// Makes the seven timers, one cut per scene of the camera animation and the
// cut of the side the team plays on, then the model of the character shown.
int msw::CReadyStartDemoVolley::EventFirst()
{
    CEventDemoBase::GetInstance();
    CreateTimer(7);

    long num = m_pCamera->GetSceneNum();
    if (num > 0) {
        if (m_pCut != 0) {
            delete[] m_pCut;
        }
        m_pCut = 0;
        m_pCut = new CUT_WORK[num];
        for (long i = 0; i < num; i++) {
            CUT_WORK &rCut = m_pCut[i];
            rCut.pName = "NoData";
            rCut.nUnk04 = -1;
            rCut.nUnk06 = -1;
            rCut.fUnk1C = 0.0f;
            rCut.fUnk20 = 1.0f;
            rCut.nUnk08 = 0x7F;
            rCut.nUnk0A = 0;
            rCut.fUnk10 = 0.025f;
            rCut.fUnk18 = 0.025f;
            rCut.fUnk14 = 20.0f;
            rCut.nUnk0C = -1;
            m_pCut[i].fUnk20 = 2.0f;
        }

        float fStart = 180.0f;
        switch (CGame::GetGuardGoalIdx(m_nTeam)) {
            case 1:
                m_fCutFrame = AddCut("cut_Ready_01", 11, -1, fStart) - fStart;
                break;
            case 0:
                m_fCutFrame = AddCut("cut_Ready_00", 10, -1, fStart) - fStart;
                break;
        }

        if (m_nCutNum > 0) {
            CUT_WORK &rFirst = m_pCut[0];
            rFirst.nUnk08 = 0x7E;
            rFirst.nUnk0A = 2;

            long nPlayer = m_nPlayer;
            CGame::GetCharColor(nPlayer);
            long type = CGame::GetCharType(nPlayer);
            if (type != -1) {
                m_ppChr[nPlayer] = new CEventChrCore(type, nPlayer, m_pMotTable);
                if (m_ppChr[nPlayer]->IsObj()) {
                    m_ppChr[nPlayer]->Create();
                    m_ppChr[nPlayer]->SetFloorMove(1);
                    m_nChrMask |= 1 << nPlayer;
                } else {
                    if (m_ppChr[nPlayer] != 0) {
                        delete m_ppChr[nPlayer];
                    }
                    m_ppChr[nPlayer] = 0;
                }
            }
        }
    }
    return 1;
}

void msw::CReadyStartDemoVolley::EventStart()
{
    bcs::CRenderMan::EnableDepthOfFieldEffect(1);
    bcs::CRenderMan::SetDepthOfFieldParam(1, 8.0f, 5.0f, 2.0f);
    m_pCalc->SetCalc(1);

    AddTimer(0, 0.0f, 0, 0);
    AddTimer(2, m_fUnk34, 0, 0);
    AddTimer(5, -30.0f + m_fCutFrame, 0, 0);
    AddTimer(6, m_fUnk34, 0, 0);
    m_nSkipBit = 1;
}

long msw::CReadyStartDemoVolley::EventPlay(CUT_WORK &rCut)
{
    if (IsFadeOutOnly(rCut.nUnk08)) {
        return StartFadeOut(rCut, rCut.nUnk08);
    }

    m_fUnk3C = 0.0f;
    if (m_nUnk9D == 0) {
        // The first cut: follow the character and start its motion.
        m_pCalc->SetFloorChr(m_nPlayer);
        CGame::SetStartFadeType(1);
        SetFloorMove(CGame::g_GameWork.m_nStage, m_nTeam, 0);
        m_ppChr[m_nPlayer]->AnimPlay(1L, 0, 0.0f, 0.0f, 1);
        AddTimerRel(3, 180.0f, 0, 0);
    }
    return StartFadeOut(rCut, rCut.nUnk08);
}

// Timer 0 lets the camera play on, timer 2 sets the scene's byte at 0x9E to
// 7, timer 3 starts the character's second motion, timers 5 and 6 show and
// close the serve telop; the others go to the demo.
void msw::CReadyStartDemoVolley::EventTimer(short nId, short nArg, unsigned long ulArg)
{
    CEventDemoBase *pDemo = CEventDemoBase::GetInstance();
    switch (nId) {
        case 0:
            m_pCamera->Resume();
            break;
        case 2:
            m_nUnk9E = 7;
            break;
        case 3:
            m_ppChr[m_nPlayer]->AnimPlay(0L, 2, 8.0f, 0.0f, 1);
            break;
        case 5: {
            pDemo->CreateTelop(pDemo->GetTelopId(3), 0);
            CGame::SetCockpitPartMove(0, 0);
            long type = CGame::GetCharType(m_nPlayer);
            const wchar_t *pName = CNameMesg::GetCharName(type, CGame::GetCharColor(m_nPlayer), 0);
            if (type >= 0x13) {
                pName = CGame::GetCharName(m_nPlayer);
            }
            CCockpitVolleyTelopServe::SetParameter(CCockpit::GetTask(), pDemo->GetTelopId(3), pName,
                                                   CGame::g_GameWork.m_Unk0B, CGame::g_GameWork.m_nUnk44);
            break;
        }
        case 6:
            pDemo->PlayEndTelop(pDemo->GetTelopId(3));
            break;
        default:
            pDemo->TimerEvent(nId, nArg, ulArg);
            break;
    }
}

void msw::CReadyStartDemoVolley::EventTimerReset(short nId, short, unsigned long)
{
}

void msw::CReadyStartDemoVolley::EventFlags(unsigned long, unsigned long)
{
}

long msw::CReadyStartDemoVolley::EventCansel(CUT_WORK &rCut, long nRet)
{
    if (m_nSkip & 1) {
        if (m_nUnk9F) {
            _StartFadeOut();
        }
        CEventDemo::m_Work.bCancel = 1;
        ResetTimer();
        return 5;
    }
    return nRet;
}

void msw::CReadyStartDemoVolley::CheckEventSkip()
{
    if (CGame::DemoSkipInputCheck(-1) >= 0) {
        m_nSkip |= m_nSkipBit;
    }
}

int msw::CReadyStartDemoVolley::IsEventSkip()
{
    if (!_IsPlayFade()) {
        if (CEventDemo::m_Work.bCancel) {
            m_nSkip |= 1;
        }
        if (m_nSkip != m_nSkipNone) {
            return 1;
        }
    }
    return 0;
}

msw::CSceneCalcReadyStart::~CSceneCalcReadyStart()
{
}

namespace {
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        if (fValue > fMax) {
            return fMax;
        }
        if (fValue < fMin) {
            return fMin;
        }
        return fValue;
    }

    // (guessed helper) A cubic from fFrom to fTo: fRate is the smooth step
    // of the time, fRate0/fRate1 weigh the slopes at the two ends.
    inline float Curve(float fFrom, float fTo, float fRate, float fRate0, float fTan0, float fRate1, float fTan1)
    {
        return fFrom - fRate * fFrom + fRate * fTo + fRate0 * fTan0 + fRate1 * fTan1;
    }
}

void msw::CSceneCalcReadyStart::Calc(bcs::CCameraAnm &rAnm, bcs::FVEC3 &rPos, bcs::FVEC3 &rAt, float &rRoll)
{
    bcs::FVEC3 posTo = m_PosTo;
    bcs::FVEC3 atTo = m_AtTo;

    float t = Clamp(rAnm.GetFrame() / (rAnm.GetEndFrame() - 1.0f), 0.0f, 1.0f);
    float t2 = t * t;
    float t21 = t2 - t;
    float t32 = t21 * t;
    float fRate = t2 - 2.0f * t32;
    float fRate0 = t32 - t21;

    bcs::FVEC3 posOff;
    posOff.x = Curve(m_PosFrom.x, posTo.x, fRate, fRate0, m_fTan0, t32, m_fTan1);
    posOff.y = Curve(m_PosFrom.y, posTo.y, fRate, fRate0, m_fTan0, t32, m_fTan1);
    if (m_nGoal != 0) {
        posOff.z = Curve(-m_PosFrom.z, -posTo.z, fRate, fRate0, m_fTan0, t32, m_fTan1);
    } else {
        posOff.z = Curve(m_PosFrom.z, posTo.z, fRate, fRate0, m_fTan0, t32, m_fTan1);
    }

    bcs::FVEC3 atOff;
    atOff.x = Curve(m_AtFrom.x, atTo.x, fRate, fRate0, m_fTan0, t32, m_fTan1);
    atOff.y = Curve(m_AtFrom.y, atTo.y, fRate, fRate0, m_fTan0, t32, m_fTan1);
    if (m_nGoal != 0) {
        atOff.z = Curve(-m_AtFrom.z, -atTo.z, fRate, fRate0, m_fTan0, t32, m_fTan1);
    } else {
        atOff.z = Curve(m_AtFrom.z, atTo.z, fRate, fRate0, m_fTan0, t32, m_fTan1);
    }

    _CalcRotOffset(rAnm, rPos, rAt, rRoll);
    bcs::FVEC3Add(&rPos, &rPos, &posOff);
    bcs::FVEC3Add(&rAt, &rAt, &atOff);
    _CalcFloor(rPos, rAt);
    _CalcRev(rAnm, rPos, rAt, rRoll);
}

void msw::EvDemoVolley::CreateReadyStartDemo(bcs::CTask *pParent, const short *pData)
{
    CGame::SetStartFadeType(0);
    if (!CEventDemo::IsEnable()) {
        CEventDemo::m_Work.bSkip = 1;
        return;
    }
    new CReadyStartDemoVolley(pData, pParent, 0);
}
