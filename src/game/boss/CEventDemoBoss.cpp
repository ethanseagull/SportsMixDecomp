// The boss event demo: which demos boss plays, the telops (cockpit objects)
// they show, and the three classes that make the demo's models (players,
// objects and NPCs) for msw::CEvDemoRes. One file with
// msw::EvDemoBoss::CreateInstance and the weak inlines the vtable needs.
#include "game/msw/CEventDemoBoss.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitBoss.hpp"
#include "game/msw/CDemoManager.hpp"
#include "game/msw/CEvDemoMdlChr.hpp"
#include "game/msw/CEvDemoMdlObj.hpp"
#include "game/msw/CEvDemoRes.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/EvDemo.hpp"
#include "game/msw/IEvMdlCreater.hpp"

// The C library is not in include/ yet.
extern "C" int sprintf(char *, const char *, ...);

namespace msw {

    // What CEventDemoBoss puts in msw::CEvDemoRes, one per kind of model
    // (_CHR_INFO::nKind 0, 1, 2 and 3). No data members. Declared
    // here, after CEventDemoBoss: the vtables come out in reverse order of
    // declaration.
    class CEvMdlCreaterGmMdlChr : public IEvMdlCreater {
        public:
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile);
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo);
    };

    class CEvMdlCreaterGmMdlObj : public IEvMdlCreater {
        public:
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile);
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo);
    };

    class CEvMdlCreaterGmMdlNpc : public IEvMdlCreater {
        public:
            virtual int BindRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual long LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile);
            virtual void UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo);
            virtual CEventModel *CreateModel(CEvDemoRes::_CHR_INFO &rInfo);
    };
}

namespace {
    // The sound numbers of each character's voices (CEventModel's voice
    // table), indexed by character; the last characters share tables.
    enum { VOICE_NUM = 23 };

    const long sVoice00[VOICE_NUM] = {
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x83, 0x84, 0x85, 0x86, 0x82, 0x80, 0x81
    };
    const long sVoice01[VOICE_NUM] = {
        0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90,
        0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x9C, 0x9D, 0x9E, 0x9F, 0x9B, 0x99, 0x9A
    };
    const long sVoice02[VOICE_NUM] = {
        0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9,
        0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1,
        0xB5, 0xB6, 0xB7, 0xB8, 0xB4, 0xB2, 0xB3
    };
    const long sVoice03[VOICE_NUM] = {
        0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2,
        0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA,
        0xCE, 0xCF, 0xD0, 0xD1, 0xCD, 0xCB, 0xCC
    };
    const long sVoice04[VOICE_NUM] = {
        0xD4, 0xD5, 0xD6, 0xD7, 0xDC, 0xE2, 0xE3, 0xDD,
        0xDE, 0xDF, 0xE0, 0xDB, 0xE1, 0xD8, 0xD9, 0xDA,
        0xE4, 0xE5, 0xE8, 0xE9, 0xE6, 0xE7, 0xEA
    };
    const long sVoice05[VOICE_NUM] = {
        0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4,
        0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC,
        0x100, 0x101, 0x102, 0x103, 0xFF, 0xFD, 0xFE
    };
    const long sVoice06[VOICE_NUM] = {
        0x106, 0x107, 0x108, 0x109, 0x10A, 0x10B, 0x10C, 0x10D,
        0x10E, 0x10F, 0x110, 0x111, 0x112, 0x113, 0x114, 0x115,
        0x119, 0x11A, 0x11B, 0x11C, 0x118, 0x116, 0x117
    };
    const long sVoice07[VOICE_NUM] = {
        0x11F, 0x120, 0x121, 0x122, 0x123, 0x124, 0x125, 0x126,
        0x127, 0x128, 0x129, 0x12A, 0x12B, 0x12C, 0x12D, 0x12E,
        0x132, 0x133, 0x134, 0x135, 0x131, 0x12F, 0x130
    };
    const long sVoice08[VOICE_NUM] = {
        0x138, 0x139, 0x13A, 0x13B, 0x13C, 0x13D, 0x13E, 0x13F,
        0x140, 0x141, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147,
        0x14B, 0x14C, 0x14D, 0x14E, 0x14A, 0x148, 0x149
    };
    const long sVoice09[VOICE_NUM] = {
        0x151, 0x152, 0x153, 0x154, 0x155, 0x156, 0x157, 0x158,
        0x159, 0x15A, 0x15B, 0x15C, 0x15D, 0x15E, 0x15F, 0x160,
        0x164, 0x165, 0x166, 0x167, 0x163, 0x161, 0x162
    };
    const long sVoice10[VOICE_NUM] = {
        0x16A, 0x16B, 0x16C, 0x16D, 0x16E, 0x16F, 0x170, 0x171,
        0x172, 0x173, 0x174, 0x175, 0x176, 0x177, 0x178, 0x179,
        0x17D, 0x17E, 0x17F, 0x180, 0x17C, 0x17A, 0x17B
    };
    const long sVoice11[VOICE_NUM] = {
        0x183, 0x184, 0x185, 0x186, 0x187, 0x188, 0x189, 0x18A,
        0x18B, 0x18C, 0x18D, 0x18E, 0x18F, 0x190, 0x191, 0x192,
        0x196, 0x197, 0x198, 0x199, 0x195, 0x193, 0x194
    };
    const long sVoice12[VOICE_NUM] = {
        0x19C, 0x19D, 0x19E, 0x19F, 0x1A4, 0x1AA, 0x1AB, 0x1A5,
        0x1A6, 0x1A7, 0x1A8, 0x1A3, 0x1A9, 0x1A0, 0x1A1, 0x1A2,
        0x1AC, 0x1AD, 0x1B0, 0x1B1, 0x1AE, 0x1AF, 0x1B2
    };
    const long sVoice13[VOICE_NUM] = {
        0x1B5, 0x1B6, 0x1B7, 0x1B8, 0x1BD, 0x1C3, 0x1C4, 0x1BE,
        0x1BF, 0x1C0, 0x1C1, 0x1BC, 0x1C2, 0x1B9, 0x1BA, 0x1BB,
        0x1C5, 0x1C6, 0x1C9, 0x1CA, 0x1C7, 0x1C8, 0x1CB
    };
    const long sVoice14[VOICE_NUM] = {
        0x1CE, 0x1CF, 0x1D0, 0x1D1, 0x1D6, 0x1DC, 0x1DD, 0x1D7,
        0x1D8, 0x1D9, 0x1DA, 0x1D5, 0x1DB, 0x1D2, 0x1D3, 0x1D4,
        0x1DE, 0x1DF, 0x1E2, 0x1E3, 0x1E0, 0x1E1, 0x1E4
    };
    const long sVoice15[VOICE_NUM] = {
        0x1E7, 0x1E8, 0x1E9, 0x1EA, 0x1EF, 0x1F5, 0x1F6, 0x1F0,
        0x1F1, 0x1F2, 0x1F3, 0x1EE, 0x1F4, 0x1EB, 0x1EC, 0x1ED,
        0x1F7, 0x1F8, 0x1FB, 0x1FC, 0x1F9, 0x1FA, 0x1FD
    };
    const long sVoice16[VOICE_NUM] = {
        0x200, 0x201, 0x202, 0x203, 0x208, 0x20E, 0x20F, 0x209,
        0x20A, 0x20B, 0x20C, 0x207, 0x20D, 0x204, 0x205, 0x206,
        0x210, 0x211, 0x214, 0x215, 0x212, 0x213, 0x216
    };
    const long sVoice17[VOICE_NUM] = {
        0x219, 0x21A, 0x21B, 0x21C, 0x221, 0x227, 0x228, 0x222,
        0x223, 0x224, 0x225, 0x220, 0x226, 0x21D, 0x21E, 0x21F,
        0x229, 0x22A, 0x22D, 0x22E, 0x22B, 0x22C, 0x22F
    };
    const long sVoice18[VOICE_NUM] = {
        0x232, 0x233, 0x234, 0x235, 0x23D, 0x23E, 0x23F, 0x240,
        0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x247, 0x248,
        0x239, 0x23A, 0x23B, 0x23C, 0x238, 0x236, 0x237
    };
    const long sVoice19[VOICE_NUM] = {
        0x24B, 0x24C, 0x24D, 0x24E, 0x256, 0x257, 0x258, 0x259,
        0x25A, 0x25B, 0x25C, 0x25D, 0x25E, 0x25F, 0x260, 0x261,
        0x252, 0x253, 0x254, 0x255, 0x251, 0x24F, 0x250
    };

    const long *sVoiceTable[] = {
        sVoice00, sVoice01, sVoice02, sVoice03, sVoice04, sVoice05, sVoice06,
        sVoice07, sVoice08, sVoice09, sVoice10, sVoice11, sVoice12, sVoice13,
        sVoice14, sVoice15, sVoice16, sVoice17, sVoice00, sVoice18, sVoice19,
        sVoice18, sVoice19, sVoice18, sVoice19, 0,
    };

    // The players' foot step sounds: none.
    const long sFootSeTable[4] = { -1, -1, -1, -1 };
    // The motion tables of the players with their own motions and with the
    // demo's motions (what the values mean is not known).
    const short sChrMotTable[2] = { 1, 0 };
    const short sEventMotTable[2] = { 8, 0 };

    // msw::CLoadData entries of the files: the players' models from 3 and
    // their own motions from 0x13, one per player; the objects' archive
    // at 3 and its motions at 0x13; the NPCs' from 0x0B and 0x1B, one per
    // NPC slot.
    const long LOADDATA_CHR_MDL = 3;
    const long LOADDATA_CHR_MOT = 0x13;
    const long LOADDATA_NPC_MDL = 0x0B;
    const long LOADDATA_NPC_MOT = 0x1B;
    const long NPC_MAX = 8;
}

long msw::CEvMdlCreaterGmMdlChr::LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile)
{
    rInfo.nMdlFile = LOADDATA_CHR_MDL + rInfo.nIndex;

    // Selection 0 is the player's own motion file; selection n is event
    // motion file n - 1, which takes an entry of its own.
    long nMot = rInfo.nMotSel - 1;
    if (nMot < 0) {
        rInfo.nMotFile[rInfo.nMotSel] = LOADDATA_CHR_MOT + rInfo.nIndex;
        return nFile;
    }

    if (rInfo.nMotFile[rInfo.nMotSel] >= 0) {
        return nFile;
    }

    long nLoaded = CLoadData::LoadEventMotion(nFile, rInfo.nId & 0xFF, nMot, 1);
    if (nLoaded != nFile) {
        // Another entry already has the file.
        rInfo.nMotFile[rInfo.nMotSel] = nLoaded;
        return nFile;
    }

    rInfo.SetBind(0);
    rInfo.nMotFile[rInfo.nMotSel] = nFile;
    return nFile + 1;
}

void msw::CEvMdlCreaterGmMdlChr::UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo)
{
    if (rInfo.nMotSel - 1 >= 0 && rInfo.nMotFile[rInfo.nMotSel] >= 0) {
        rInfo.nMotFile[rInfo.nMotSel] = -1;
        rInfo.nMotSel = 0;
    }
}

int msw::CEvMdlCreaterGmMdlChr::BindRes(CEvDemoRes::_CHR_INFO &rInfo)
{
    nw4r::g3d::ResFile res = CLoadData::GetRes(rInfo.nMotFile[rInfo.nMotSel]);

    if (res.IsValid()) {
        res.CheckRevision();
        res.Init();
        res.Bind(res);
    }
    return 0;
}

msw::CEventModel *msw::CEvMdlCreaterGmMdlChr::CreateModel(CEvDemoRes::_CHR_INFO &rInfo)
{
    nw4r::g3d::ResFile mdlRes;
    nw4r::g3d::ResFile motRes;
    long nChr = rInfo.nId & 0xFF;

    mdlRes = CLoadData::GetRes(rInfo.nMdlFile);
    motRes = CLoadData::GetRes(rInfo.nMotFile[rInfo.nMotSel]);

    const short *pMotTable = rInfo.nMotSel != 0 ? sEventMotTable : sChrMotTable;
    int bReverse = CEvDemoRes::GetInstance()->IsReverse();

    CEvDemoMdlChr::_SETUP setup(mdlRes, motRes, rInfo.IsDisp());
    setup.pInit = rInfo.pInit;
    setup.nType = nChr;
    setup.nIndex = rInfo.nIndex;
    setup.pMotTable = pMotTable;

    CEvDemoMdlChr *pModel = CEvDemoMdlChr::CreateModel(setup, 0);
    pModel->SetMotionReverse(bReverse);
    pModel->SetFootSeTable(sFootSeTable);
    pModel->SetVoiceTable(sVoiceTable[nChr]);
    pModel->SetEnableFootSe(1);
    return pModel;
}

long msw::CEvMdlCreaterGmMdlObj::LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile)
{
    if (rInfo.nMotSel - 1 < 0) {
        rInfo.nMotFile[rInfo.nMotSel] = LOADDATA_CHR_MOT;
        rInfo.nMdlFile = LOADDATA_CHR_MDL;
        return nFile;
    }

    if (rInfo.nMotFile[rInfo.nMotSel] >= 0) {
        return nFile;
    }

    long nLoaded = CLoadData::LoadEventData(nFile, "event/demos", CEvDemoRes::GetInstance()->GetDemoName(),
                                            rInfo.nEvDataNo, 1, rInfo.nEvDataKind);
    if (nLoaded != nFile) {
        // Another entry already has the file.
        rInfo.nMotFile[rInfo.nMotSel] = nLoaded;
        rInfo.nMdlFile = nLoaded;
        return nFile;
    }

    rInfo.SetBind(0);
    rInfo.nMotFile[rInfo.nMotSel] = nFile;
    rInfo.nMdlFile = nFile;
    return nFile + 1;
}

void msw::CEvMdlCreaterGmMdlObj::UnLoadRes(CEvDemoRes::_CHR_INFO &)
{
}

int msw::CEvMdlCreaterGmMdlObj::BindRes(CEvDemoRes::_CHR_INFO &rInfo)
{
    if (CLoadData::IsComplete(rInfo.nMdlFile)) {
        EvDemo::InitArcResData(CLoadData::GetPtr(rInfo.nMdlFile), 0);
    }
    return 0;
}

msw::CEventModel *msw::CEvMdlCreaterGmMdlObj::CreateModel(CEvDemoRes::_CHR_INFO &rInfo)
{
    CEvDemoMdlObj::_SETUP setup(rInfo.IsDisp());
    char name[32];

    setup.nIndex = rInfo.nIndex;
    setup.nDrawLayer = rInfo.nDrawLayer;
    setup.nShadowSize = rInfo.GetShadowSize();
    setup.nLightSelector = rInfo.nLightSelector;

    sprintf(name, "m%02d", rInfo.nId);
    return CEvDemoMdlObj::CreateModel(setup, rInfo.nMdlFile, static_cast<const char *>(rInfo.pInit), name);
}

long msw::CEvMdlCreaterGmMdlNpc::LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile)
{
    if (rInfo.nMotSel - 1 < 0) {
        // Which NPC slot has this NPC.
        long nNpc = -1;
        for (long i = 0; i < NPC_MAX; i++) {
            if (rInfo.nId == CGame::GetNpcType(i)) {
                nNpc = i;
                break;
            }
        }
        rInfo.nMotFile[rInfo.nMotSel] = LOADDATA_NPC_MOT + nNpc;
        rInfo.nMdlFile = LOADDATA_NPC_MDL + nNpc;
        return nFile;
    }
    return nFile;
}

void msw::CEvMdlCreaterGmMdlNpc::UnLoadRes(CEvDemoRes::_CHR_INFO &)
{
}

int msw::CEvMdlCreaterGmMdlNpc::BindRes(CEvDemoRes::_CHR_INFO &)
{
    return 0;
}

msw::CEventModel *msw::CEvMdlCreaterGmMdlNpc::CreateModel(CEvDemoRes::_CHR_INFO &rInfo)
{
    nw4r::g3d::ResFile mdlRes;
    nw4r::g3d::ResFile motRes;

    mdlRes = CLoadData::GetRes(rInfo.nMdlFile);
    motRes = CLoadData::GetRes(rInfo.nMotFile[rInfo.nMotSel]);

    CEvDemoMdlObj::_SETUP setup(rInfo.IsDisp());
    setup.nIndex = rInfo.nIndex;
    setup.nType = rInfo.nId;
    setup.pMdlRes = &mdlRes;
    setup.pMotRes = &motRes;
    setup.pInit = rInfo.pInit;
    setup.nDrawLayer = rInfo.nDrawLayer;
    setup.nShadowSize = rInfo.GetShadowSize();
    setup.nLightSelector = rInfo.nLightSelector;

    return CEvDemoMdlObj::CreateModel(setup, 0);
}

msw::CEventDemoBoss::CEventDemoBoss(bcs::CTask *pParent) : CEventDemoBase(pParent)
{
    for (int i = 0; i < TELOP_MAX; i++) {
        m_nTelop[i] = -1;
    }
    m_pInstance = this;
    _Init();

    CEvDemoRes *pRes = CEvDemoRes::GetInstance();
    pRes->SetCreater(0, new CEvMdlCreaterGmMdlChr);
    pRes->SetCreater(1, new CEvMdlCreaterGmMdlNpc);
    pRes->SetCreater(2, new CEvMdlCreaterGmMdlObj);
    pRes->SetCreater(3, new CEvMdlCreaterGmMdlObj);
    pRes->SetUnk62(0);
    pRes->SetUnk64(1);
    pRes->SetFlag(0);
}

msw::CEventDemoBoss::~CEventDemoBoss()
{
    _Destroy();
}

// Only the entrance (0) and victory (1) demos are played, from the demo
// files. The two cases repeat the same code; written as one inline helper
// the compiler allocates the registers of the first case differently.
void msw::CEventDemoBoss::CreateDemo(long nDemo, bcs::CTask *pParent)
{
    CGame::SetStartFadeType(0);

    switch (nDemo) {
        case 0: {
            if (!CEventDemo::IsEnable()) {
                CEventDemo::m_Work.bSkip = 1;
                break;
            }

            // Every player on the court goes into the demo as a model; the
            // player itself is hidden.
            CEvDemoRes *pRes = CEvDemoRes::GetInstance();
            long nNum = CChrMan::GetChrCoreNum();
            long nTeam0 = 0;
            long nTeam1 = 0;

            for (long i = 0; i < nNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && pChr->IsEnable()) {
                    pChr->OnDispFlag(4);

                    CEvDemoRes::_CHR_INFO &rInfo = pRes->FindIndexNew(i, 0);
                    pRes->SetInit(rInfo, i);
                    rInfo.SetCreate(1);
                    rInfo.SetRelease(1);
                    rInfo.SetChrInit(CGame::GetCharType(i), CGame::GetCharColor(i), CGame::GetMiiStoreData(i), 0);
                    if (pChr->GetTeamNo() != 0) {
                        rInfo.SetTeam(1, nTeam1);
                        nTeam1++;
                    } else {
                        rInfo.SetTeam(0, nTeam0);
                        nTeam0++;
                    }
                    rInfo.SetDispInfo(1, 2, -1, 0);
                    pRes->_CreateRes(rInfo);
                }
            }

            if (CGame::GetPartyGameParam(0) >= 2) {
                new CDemoManager("demo091", pParent);
            } else {
                new CDemoManager("demo090", pParent);
            }
            CChrMan::SetChrMarkerDisp(0);
            break;
        }
        case 1: {
            if (!CEventDemo::IsEnable()) {
                CEventDemo::m_Work.bSkip = 1;
                break;
            }

            // Every player on the court goes into the demo as a model; the
            // player itself is hidden.
            CEvDemoRes *pRes = CEvDemoRes::GetInstance();
            long nNum = CChrMan::GetChrCoreNum();
            long nTeam0 = 0;
            long nTeam1 = 0;

            for (long i = 0; i < nNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && pChr->IsEnable()) {
                    pChr->OnDispFlag(4);

                    CEvDemoRes::_CHR_INFO &rInfo = pRes->FindIndexNew(i, 0);
                    pRes->SetInit(rInfo, i);
                    rInfo.SetCreate(1);
                    rInfo.SetRelease(1);
                    rInfo.SetChrInit(CGame::GetCharType(i), CGame::GetCharColor(i), CGame::GetMiiStoreData(i), 0);
                    if (pChr->GetTeamNo() != 0) {
                        rInfo.SetTeam(1, nTeam1);
                        nTeam1++;
                    } else {
                        rInfo.SetTeam(0, nTeam0);
                        nTeam0++;
                    }
                    rInfo.SetDispInfo(1, 2, -1, 0);
                    pRes->_CreateRes(rInfo);
                }
            }

            if (CGame::GetPartyGameParam(0) >= 2) {
                new CDemoManager("demo101", pParent);
            } else {
                new CDemoManager("demo100", pParent);
            }
            CChrMan::SetChrMarkerDisp(0);
            break;
        }
        default:
            CEventDemo::m_Work.bSkip = 1;
            break;
    }
}

int msw::CEventDemoBoss::IsDemoPlay(long nDemo) const
{
    switch (nDemo) {
        case 0:
        case 1:
            return 1;
    }
    return 0;
}

void msw::CEventDemoBoss::CreateTelop(long nObjId, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitBoss::CreateObj(nObjId, 0);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoBoss::CreateTelop(long nObjId, long nParam, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitBoss::CreateObj(nObjId, nParam);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoBoss::PlayEndTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::PlayEndAnim(nObjId);
        ClearTelop(nObjId);
    }
}

int msw::CEventDemoBoss::IsEndTelop(long nObjId)
{
    if (nObjId >= 0 && IsTelop(nObjId)) {
        return CCockpit::IsEndObj(nObjId);
    }
    return 1;
}

void msw::CEventDemoBoss::RemoveTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::RemoveObj(nObjId);
        ClearTelop(nObjId);
    }
}

void msw::CEventDemoBoss::PlayEndTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::PlayEndAnim(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

void msw::CEventDemoBoss::RemoveTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::RemoveObj(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

long msw::CEventDemoBoss::GetTelopId(long)
{
    return -1;
}

void msw::CEventDemoBoss::SetCockpitState(long nPart, long nState)
{
    if (nPart == 0) {
        if (nState == 2) {
            CCockpitBoss::SetPartMove(0, 3);
            CCockpitBoss::SetRadarIconDisp(0);
        } else if (nState == 1) {
            CCockpitBoss::SetPartMove(0, 1);
            CCockpitBoss::SetRadarIconDisp(1);
        }
    }
}

void msw::CEventDemoBoss::TimerEvent(short, short, unsigned long)
{
}

void msw::CEventDemoBoss::SendMessage(long, long, long, long)
{
}

void msw::EvDemoBoss::CreateInstance(bcs::CTask *pParent)
{
    new CEventDemoBoss(pParent);
}
