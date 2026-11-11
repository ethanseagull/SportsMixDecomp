// msw::CChrCoreHockeyKuroma (the Black Mage) in the hockey game.
//
// His shot trick puts a lock-on mark (CC17_HOCKEY::CcKuromaLockonUnit) on
// every character of the other team but the keeper, and strikes them all a
// while later (CC17_HOCKEY::CcKuromaMagicMgr).
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {
    namespace CC17_HOCKEY {

        class CcKuromaMagicMgr;

        // The mark on one character of the other team during the shot
        // trick: an effect that follows the character. 0x48 bytes, kept in
        // its CcKuromaMagicMgr's list. (member names guessed)
        class CcKuromaLockonUnit : public bcs::CUnivTaskEx {
            public:
                inline CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, CChrCore *pChr);
                virtual ~CcKuromaLockonUnit();
                virtual void Main();

                // The index of the character marked.
                long GetChrIdx() const { return m_nChr; }
                // The mark stays without its effect.
                void ClearEffect() { m_hEffect = 0; }

            private:
                nw4r::ut::Link m_Link;      // 0x34
                CcKuromaMagicMgr *m_pMgr;   // 0x3C
                unsigned long m_hEffect;    // 0x40
                long m_nChr;                // 0x44, the character index
        };

        // The shot trick's strike: marks every character of the other team
        // but the keeper and, once started, strikes them all after a wait.
        // 0x48 bytes. (member names guessed)
        class CcKuromaMagicMgr : public bcs::CUnivTaskEx {
            public:
                CcKuromaMagicMgr(bcs::CTask *pParent, long nTeam);
                virtual ~CcKuromaMagicMgr();
                virtual void Main();

                // Starts the wait before the strike (once).
                void Start(float fWait)
                {
                    if (m_nStep == 0) {
                        m_fWait = fWait;
                        m_nStep++;
                    }
                }

                void Append(CcKuromaLockonUnit *pUnit) { nw4r::ut::List_Append(&m_List, pUnit); }
                void Remove(CcKuromaLockonUnit *pUnit) { nw4r::ut::List_Remove(&m_List, pUnit); }

            private:
                CcKuromaLockonUnit *GetNext(CcKuromaLockonUnit *pUnit)
                {
                    return static_cast<CcKuromaLockonUnit *>(nw4r::ut::List_GetNext(&m_List, pUnit));
                }

                nw4r::ut::List m_List;      // 0x34, of CcKuromaLockonUnit
                long m_nStep;               // 0x40: 0 marked, 1 waiting, 2 done
                float m_fWait;              // 0x44
        };

        // Puts the mark on pChr.
        inline CcKuromaLockonUnit::CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, CChrCore *pChr)
            : CUnivTaskEx(pMgr, -1)
        {
            m_pMgr = pMgr;
            m_hEffect = 0;
            m_nChr = -1;
            m_Link.pPrev = 0;
            m_Link.pNext = 0;
            if (pMgr) {
                pMgr->Append(this);
            }
            m_nChr = pChr->GetPlayerNo();
            m_hEffect = CEffectCtrl::CreateAttachEffectAuto(0xD7, pChr->GetMdl(), 3);
        }
    }
}

msw::CChrCoreHockeyKuroma::CChrCoreHockeyKuroma()
{
    m_hMagic = 0;
}

msw::CChrCoreHockeyKuroma::~CChrCoreHockeyKuroma()
{
}

inline void msw::CChrCoreHockeyKuroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

void msw::CChrCoreHockeyKuroma::Reset_()
{
    CChrCoreHockey::Reset_();
    ReleaseMagic_();
}

void msw::CChrCoreHockeyKuroma::DtorStateAirTrick()
{
    // Not used here. A guess: the value is first in .rodata, ahead of
    // those of the trick functions (it is the wait CcKuromaMagicMgr gets in
    // SubStateArtsShotIsReadyArtsShot_; VolleyF's unit has the same).
    float fUnused = 10.0f;

    CChrCoreHockey::DtorStateAirTrick();
    ReleaseMagic_();
}

// The trick motion: its effects and sounds, and the marks on the other team.
void msw::CChrCoreHockeyKuroma::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xD5, 3);
    SE_PlayArtsJumpSE(0);
    PlaySePos(0x48D, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    PlaySePos(0x4F1, 0, 0, -1, 0.0f, 1.0f, 0.0f);

    ReleaseMagic_();
    CC17_HOCKEY::CcKuromaMagicMgr *pMagic =
        new (0x100) CC17_HOCKEY::CcKuromaMagicMgr(CGame::GetTempParent(), GetTeamNo());
    m_hMagic = pMagic->GetId();
}

// Not zero once the trick is over: three motions one after the other; at
// the end of the first the strike on the marked characters is set off.
int msw::CChrCoreHockeyKuroma::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9D, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xD6, 3);
                CC17_HOCKEY::CcKuromaMagicMgr *pMagic =
                    static_cast<CC17_HOCKEY::CcKuromaMagicMgr *>(bcs::CTaskMan::GetTask(m_hMagic));
                if (pMagic && !pMagic->IsDelete()) {
                    pMagic->Start(10.0f);
                }
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9E, 0, 0.0f, 0.0f);
                pWork->m_nTrickStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x9F, 0, 0.0f, 0.0f);
                Eff_CreateAttachAuto(0x15, 0xD8, 3);
                Eff_CreateAttachAuto(0x16, 0xD9, 3);
                pWork->m_nTrickStep++;
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                bReady = 1;
            }
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeyKuroma::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x17, 0xDA, 3);
}

void msw::CChrCoreHockeyKuroma::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_Stop(0x17);
    Eff_Stop(0x16);
    Eff_CreateAttachAuto(0x18, 0xDB, 3);
}

void msw::CChrCoreHockeyKuroma::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
}

// Marks every character of the other team but the keeper.
msw::CC17_HOCKEY::CcKuromaMagicMgr::CcKuromaMagicMgr(bcs::CTask *pParent, long nTeam)
    : CUnivTaskEx(pParent, -1)
{
    m_nStep = 0;
    m_fWait = 0.0f;
    nw4r::ut::List_Init(&m_List, 0x34);

    long nChr = CChrMan::GetChrCoreNum();
    for (long i = 0; i < nChr; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (!pChr->IsDisable() && nTeam != pChr->GetTeamNo() && !pChr->IsKeeper()) {
            new (0x100) CcKuromaLockonUnit(this, pChr);
        }
    }
}

msw::CC17_HOCKEY::CcKuromaMagicMgr::~CcKuromaMagicMgr()
{
    DeleteAllChild();
    nw4r::ut::List_Init(&m_List, 0x34);
}

// Strikes every marked character still in the game when the wait is over:
// an effect at its feet, a sound and a hit that knocks it down.
void msw::CC17_HOCKEY::CcKuromaMagicMgr::Main()
{
    if (m_nStep == 1 && m_fWait > 0.0f) {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            CcKuromaLockonUnit *pUnit = GetNext(0);
            while (pUnit) {
                long nChr = pUnit->GetChrIdx();
                CChrCore *pChr;
                if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum() && (pChr = CChrMan::GetChrCore(nChr)) != 0 &&
                    pChr->IsEnable()) {
                    pUnit->ClearEffect();
                    bcs::FVEC3 pos;
                    pChr->GetTransNodePos(pos);
                    pos.y = 0.0f;
                    CEffectCtrl::SimpleEffect(0x648, pos, 3, 0, 0);
                    pChr->PlaySePos(0x48E, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                    pChr->SetHookDamage(0xB0D, pChr->GetRotY(), 0.0f, 220.0f, 0x27, 0.0f, 0, 0, -1, -1);
                }
                pUnit->Delete();
                pUnit = pUnit ? GetNext(pUnit) : 0;
            }
            m_nStep++;
        }
    }
}

msw::CC17_HOCKEY::CcKuromaLockonUnit::~CcKuromaLockonUnit()
{
    if (m_pMgr) {
        m_pMgr->Remove(this);
    }
    if (m_hEffect) {
        CEffectCtrl::StopAttachEffect(m_hEffect);
    }
}

// Keeps the mark on the character.
void msw::CC17_HOCKEY::CcKuromaLockonUnit::Main()
{
    CChrCore *pChr;
    if (m_hEffect && m_nChr >= 0 && m_nChr < CChrMan::GetChrCoreNum() &&
        (pChr = CChrMan::GetChrCore(m_nChr)) != 0 && pChr->IsEnable()) {
        CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
        if (pCluster) {
            bcs::FVEC3 pos;
            pChr->GetTransNodePos(pos);
            pCluster->SetTranslate(&pos);
        }
    }
}
