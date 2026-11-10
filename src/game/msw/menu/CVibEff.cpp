// In the order of the file (the order of the classes' vtables in .data
// follows it).
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CSndBGM.hpp"
#include "game/msw/menu/CVibEff.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameVib.hpp"
#include "nw4r/ut/List.hpp"

// The menu's sound effects, music and rumble (CSndEff, CSndBGM, CVibEff):
// one file, as the 0.0f/1.0f constants they share and the one static
// initialiser for all three instances show.

namespace {
    using namespace msw;

    // (name guessed; no symbol) One sound effect CSndEff::Play started.
    // Size 0x14, vptr at 0x10.
    class CSndEffEntry {
        public:
            CSndEffEntry(unsigned long nTaskId, unsigned long nSe) : m_nTaskId(nTaskId), m_nSe(nSe) {}

            nw4r::ut::Link m_Link;   // 0x00
            unsigned long m_nTaskId; // 0x08, the id CGameSnd::Play returned
            unsigned long m_nSe;     // 0x0C

            // Declared after the data: the vptr is at 0x10. Stops the sound
            // if its task still exists.
            virtual ~CSndEffEntry();
    };

    // (name guessed; no symbol) The list of CSndEffEntry. Size 0x10, vptr at
    // 0x0C.
    class CSndEffEntryList {
        public:
            CSndEffEntryList() { nw4r::ut::List_Init(&m_List, 0); }

            CSndEffEntry *GetPrev(CSndEffEntry *pEntry)
            {
                return static_cast<CSndEffEntry *>(nw4r::ut::List_GetPrev(&m_List, pEntry));
            }

            // Deletes every entry, newest first, which stops its sound.
            void DeleteAll()
            {
                CSndEffEntry *pEntry = GetPrev(0);
                while (pEntry != 0) {
                    CSndEffEntry *pPrev = GetPrev(pEntry);
                    nw4r::ut::List_Remove(&m_List, pEntry);
                    delete pEntry;
                    pEntry = pPrev;
                }
            }

            // Deletes every entry of effect nSe, which stops its sound. (guess)
            // Calls List_GetPrev itself where DeleteAll goes through
            // GetPrev: with the helper, the registers of the two pointers
            // come out swapped in Play and Stop.
            void Delete(unsigned long nSe)
            {
                CSndEffEntry *pEntry = static_cast<CSndEffEntry *>(nw4r::ut::List_GetPrev(&m_List, 0));
                while (pEntry != 0) {
                    CSndEffEntry *pPrev = static_cast<CSndEffEntry *>(nw4r::ut::List_GetPrev(&m_List, pEntry));
                    if (nSe == pEntry->m_nSe) {
                        nw4r::ut::List_Remove(&m_List, pEntry);
                        delete pEntry;
                    }
                    pEntry = pPrev;
                }
            }

            nw4r::ut::List m_List; // 0x00

            // Declared after the data: the vptr is at 0x0C.
            virtual ~CSndEffEntryList();
    };

    // The effects CSndEff::Play started. Defined at the end of the file:
    // __sinit constructs it after the three instances.
    extern CSndEffEntryList s_SndEffList;
}

namespace msw {
    namespace menu {

        CSndEff CSndEff::s_Instance;

        CSndEff::~CSndEff() {}

        void CSndEff::Initialize()
        {
            m_bEnable = 1;
            s_SndEffList.DeleteAll();
            nw4r::ut::List_Init(&s_SndEffList.m_List, 0);
        }

        void CSndEff::Finalize()
        {
            m_bEnable = 0;
            s_SndEffList.DeleteAll();
        }

        unsigned long CSndEff::Play(unsigned long nSe, unsigned long nUnk)
        {
            if (!m_bEnable) {
                return 0;
            }
            unsigned long nTaskId = CGameSnd::Play(nSe, 0, nUnk, -1, 0.0f, 1.0f, 0.0f);
            s_SndEffList.Delete(nSe);
            CSndEffEntry *pEntry = new CSndEffEntry(nTaskId, nSe);
            if (pEntry != 0) {
                nw4r::ut::List_Append(&s_SndEffList.m_List, pEntry);
            }
            return nTaskId;
        }

        void CSndEff::Stop(unsigned long nSe)
        {
            s_SndEffList.Delete(nSe);
        }

        CSndBGM CSndBGM::s_Instance;

        CSndBGM::~CSndBGM() {}

        void CSndBGM::Initialize()
        {
            if (bcs::CSnd::IsReady()) {
                CGameSnd::MenuSaveHeapState();
            }
            m_nState = STATE_READY;
            m_fFade = 0.0f;
            m_nSeq = -1;
            m_nNextSeq = -1;
        }

        void CSndBGM::Finalize()
        {
            Stop(m_nTaskId, 1.0f);
            m_nState = STATE_NONE;
        }

        void CSndBGM::SeqLoad(long nSeq, int bPlay)
        {
            if (nSeq >= 0xBB7 && nSeq <= 0xBB9) {
                m_nNextSeq = nSeq;
                m_nState = bPlay ? STATE_WAIT_LOAD_PLAY : STATE_WAIT_LOAD;
            } else {
                if (bPlay) {
                    Play(nSeq);
                }
                m_nState = STATE_READY;
            }
        }

        void CSndBGM::Play(long nSeq)
        {
            m_nSeq = nSeq;
            m_nTaskId = CGameSnd::PlayBGM(nSeq, 1, 1.0f, 0.0f);
        }

        void CSndBGM::Stop(unsigned long nTaskId, float fFade)
        {
            bcs::CSndTask *pTask = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(nTaskId));
            if (pTask != 0 && !pTask->IsDelete()) {
                pTask->Stop(fFade);
                m_fFade = fFade;
            }
            m_nSeq = -1;
            m_nTaskId = 0;
        }

        int CSndBGM::IsPlayNow(unsigned long nTaskId)
        {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(nTaskId);
            if (pTask != 0 && !pTask->IsDelete()) {
                return 1;
            }
            return 0;
        }

        void CSndBGM::MainLoop()
        {
            switch (m_nState) {
                case STATE_WAIT_LOAD:
                case STATE_WAIT_LOAD_PLAY: {
                    // Read before the call, as the original does.
                    float fFade = m_fFade;
                    fFade -= bcs::CMainApp::GetAppFrameRateF();
                    if (fFade <= 0.0f) {
                        fFade = 0.0f;
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nTaskId);
                        if (bcs::CSnd::IsReady() && pTask == 0) {
                            CGameSnd::MenuLoadSeq(m_nNextSeq);
                            m_nState = m_nState == STATE_WAIT_LOAD_PLAY ? STATE_WAIT_PLAY : STATE_READY;
                        }
                    }
                    m_fFade = fFade;
                    break;
                }
                case STATE_WAIT_PLAY: {
                    // Read before the call, as the original does.
                    float fFade = m_fFade;
                    fFade -= bcs::CMainApp::GetAppFrameRateF();
                    if (fFade <= 0.0f) {
                        fFade = 0.0f;
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_nTaskId);
                        if (bcs::CSnd::IsReady() && pTask == 0) {
                            Play(m_nNextSeq);
                            m_nState = STATE_READY;
                        }
                    }
                    m_fFade = fFade;
                    break;
                }
            }
        }

        CVibEff CVibEff::s_Instance;

        CVibEff::~CVibEff() {}

        void CVibEff::Initialize()
        {
            m_bEnable = 1;
            m_fWait[0] = 0.0f;
            m_fWait[1] = 0.0f;
            m_fWait[2] = 0.0f;
            m_fWait[3] = 0.0f;
            m_fInterval = 15.0f;
        }

        void CVibEff::Finalize()
        {
            m_bEnable = 0;
        }

        void CVibEff::Play(long nPattern, long nUnk, unsigned long nPadMask)
        {
            if (m_bEnable) {
                for (int i = 0; i < 4; i++) {
                    if ((nPadMask & (1 << i)) && m_fWait[i] == 0.0f) {
                        CGameVib::Start(0, nPattern, nUnk, i);
                        m_fWait[i] = m_fInterval;
                    }
                }
            }
        }

        void CVibEff::Stop(unsigned long nPadMask)
        {
            for (int i = 0; i < 4; i++) {
                if (nPadMask & (1 << i)) {
                    CGameVib::Stop(i);
                }
            }
        }

        void CVibEff::MainLoop()
        {
            float fRate = bcs::CMainApp::GetAppFrameRateF();
            for (int i = 0; i < 4; i++) {
                float fWait = m_fWait[i] - fRate;
                if (fWait < 0.0f) {
                    fWait = 0.0f;
                }
                m_fWait[i] = fWait;
            }
        }
    }
}

namespace {
    CSndEffEntry::~CSndEffEntry()
    {
        bcs::CSndTask *pTask = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_nTaskId));
        if (pTask != 0) {
            pTask->Stop(0.0f);
            pTask->Delete();
        }
    }

    CSndEffEntryList::~CSndEffEntryList()
    {
        DeleteAll();
    }

    CSndEffEntryList s_SndEffList;
}
