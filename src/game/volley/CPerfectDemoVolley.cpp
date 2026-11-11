// msw::CPerfectDemoVolley: the volley "perfect" demo (CEventDemoVolley's
// demo 7), an event scene of resource 0x60 that shows every character.
#include "game/msw/CEventDemoVolley.hpp"
#include "bcs/CCameraAnm.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEventChrCore.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CEventDemoBase.hpp"
#include "game/msw/CEventScene.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CSceneCalc.hpp"

namespace msw {

    // Size 0xC4 (CreatePerfectDemo).
    class CPerfectDemoVolley : public CEventScene {
        public:
            CPerfectDemoVolley(bcs::CTask *pParent, const short *pMotTable) : CEventScene(8, pParent, 0)
            {
                m_pMotTable = pMotTable;
                m_nChrNum = CChrMan::GetChrCoreNum();
                m_pCalc = new CSceneCalc(this);
            }
            virtual ~CPerfectDemoVolley();

            virtual nw4r::g3d::ResFile GetRes();
            virtual int EventFirst();
            virtual void EventStart();
            virtual void EventKill() { Delete(); }
            virtual long EventPlay(CUT_WORK &rCut);
            virtual void EventTimer(short nId, short, unsigned long);
            virtual void EventTimerReset(short nId, short, unsigned long);
            virtual void EventFlags(unsigned long, unsigned long);

        private:
            float m_fCutFrame[3];     // 0xA4: length of each scene cut (AddSceneCut)
            long m_nCutIdx[3];        // 0xB0: index of each scene's first cut
            CSceneCalc *m_pCalc;      // 0xBC
            const short *m_pMotTable; // 0xC0: given to the character models
    };
}

// For the cuts of the third scene: the model slot the node follows and the
// model whose floor the camera follows.
static const short s_NodeChr[] = { 0, 1 };
static const short s_FloorChr[] = { 0, 1 };

msw::CPerfectDemoVolley::~CPerfectDemoVolley()
{
    CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
    if (pDemo != 0) {
        pDemo->RemoveTelopAll();
    }
}

nw4r::g3d::ResFile msw::CPerfectDemoVolley::GetRes()
{
    return CLoadData::GetRes(0x60);
}

// Stops the game, makes the four timers and one cut per scene of the camera
// animation, then a model for every character that has one.
int msw::CPerfectDemoVolley::EventFirst()
{
    CEventDemoBase::GetInstance();
    CChrMan::SetChrMarkerDisp(0);
    CBallMan::AllReset();
    CBallMan::AllSleep();
    CreateTimer(4);

    long num = m_pCamera->GetSceneNum();
    if (num > 0) {
        if (m_pCut != 0) {
            delete[] m_pCut;
        }
        m_pCut = 0;
        m_pCut = new CUT_WORK[num];
        const char *pNoData = "NoData";
        for (long i = 0; i < num; i++) {
            CUT_WORK &rCut = m_pCut[i];
            rCut.pName = pNoData;
            rCut.nUnk04 = -1;
            rCut.nUnk06 = -1;
            rCut.fUnk1C = 0.0f;
            rCut.fUnk20 = 1.0f;
            rCut.nUnk08 = 0x7C;
            rCut.nUnk0A = 0;
            rCut.fUnk10 = 0.025f;
            rCut.fUnk18 = 0.025f;
            rCut.fUnk14 = 30.0f;
            rCut.nUnk0C = -1;
        }

        for (long i = 0; i < 3; i++) {
            m_nCutIdx[i] = m_nCutNum;
            m_fCutFrame[i] = AddSceneCut(i);
        }

        if (m_nCutNum > 0) {
            CUT_WORK &rFirst = m_pCut[0];
            rFirst.nUnk08 = 0x7E;
            rFirst.nUnk0A = 2;
            for (long i = 0; i < m_nChrNum; i++) {
                long color = CGame::GetCharColor(i);
                long type = CGame::GetCharType(i);
                if (type != -1) {
                    m_ppChr[i] = new CEventChrCore(type, i, m_pMotTable);
                    if (m_ppChr[i]->IsObj()) {
                        m_ppChr[i]->Create();
                        m_ppChr[i]->SetDisp(0);
                        m_ppChr[i]->SetFloorMove(1);
                        if (color < 4) {
                            m_nChrMask |= 1 << i;
                        }
                    } else {
                        if (m_ppChr[i] != 0) {
                            delete m_ppChr[i];
                        }
                        m_ppChr[i] = 0;
                    }
                }
            }
        }
    }
    return 1;
}

// Timer 3 runs out 50 frames before the end of the first two scenes, but
// not before frame 40.
void msw::CPerfectDemoVolley::EventStart()
{
    float frame = 0.0f;
    for (long i = 0; i < 2; i++) {
        frame += m_fCutFrame[i];
    }
    frame -= 50.0f;
    if (frame <= 40.0f) {
        frame = 40.0f;
    }

    AddTimer(0, 10.0f, 0, 0);
    AddTimer(1, m_fUnk34, 0, 0);
    AddTimer(2, 40.0f, 0, 0);
    AddTimer(3, frame, 0, 0);
    m_pCalc->SetCalc(1);
}

long msw::CPerfectDemoVolley::EventPlay(CUT_WORK &rCut)
{
    if (IsFadeOutOnly(rCut.nUnk08)) {
        return StartFadeOut(rCut, rCut.nUnk08);
    }

    if (rCut.nUnk04 == 2) {
        long idx = rCut.nUnk06 - m_nCutIdx[2];
        SetDispChr(0);
        SetNodeChr(rCut.pName, s_NodeChr[idx], 0, 2);
        m_pCalc->SetFloorChr(s_FloorChr[idx]);
    }
    return StartFadeOut(rCut, rCut.nUnk08);
}

// Timer 0 lets the camera play on, timer 1 sets the scene's byte at 0x9E to
// 7; timers 2 and 3 do nothing here, the others go to the demo.
void msw::CPerfectDemoVolley::EventTimer(short nId, short nArg, unsigned long ulArg)
{
    CEventDemoBase *pDemo = CEventDemoBase::GetInstance();
    switch (nId) {
        case 0:
            m_pCamera->Resume();
            break;
        case 1:
            m_nUnk9E = 7;
            break;
        case 2:
        case 3:
            break;
        default:
            pDemo->TimerEvent(nId, nArg, ulArg);
            break;
    }
}

void msw::CPerfectDemoVolley::EventTimerReset(short nId, short, unsigned long)
{
}

void msw::CPerfectDemoVolley::EventFlags(unsigned long, unsigned long)
{
}

void msw::EvDemoVolley::CreatePerfectDemo(bcs::CTask *pParent, const short *pData)
{
    if (!CEventDemo::IsEnable()) {
        CEventDemo::m_Work.bSkip = 1;
        return;
    }
    new CPerfectDemoVolley(pParent, pData);
}
