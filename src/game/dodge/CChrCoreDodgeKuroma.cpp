// msw::CChrCoreDodgeKuroma: the Black Mage in the dodgeball game.
//
// His special shot puts a lock-on mark (CC17_DODGE::CcKuromaLockonUnit) on
// every character of the other team and strikes them all a while later
// (CC17_DODGE::CcKuromaMagicMgr). Written after the same classes of
// HockeyF and VolleyF (game/hockey/CChrCoreHockeyKuroma.cpp).
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/ut/List.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // (guessed) The start of the work of state 0x22 (a throw, judging by
    // the shot effect it shows); only what CallbackMotTrg_ reads.
    struct STATE_WORK_22 {
        long m_nUnk00;                 // 0x00
        unsigned char m_Unk04[0x18 - 0x04];
        long m_nType;                  // 0x18
    };
}

namespace CC17_DODGE {

    class CcKuromaMagicMgr;

    // The mark on one character of the other team during the special shot:
    // an effect that follows the character. 0x48 bytes, kept in its
    // CcKuromaMagicMgr's list. (member names guessed)
    class CcKuromaLockonUnit : public bcs::CUnivTaskEx {
        public:
            inline CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, msw::CChrCore *pChr);
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

    // The special shot's strike: marks every character of the other team
    // and, once started, strikes them all after a wait. 0x48 bytes. (member
    // names guessed)
    class CcKuromaMagicMgr : public bcs::CUnivTaskEx {
        public:
            CcKuromaMagicMgr(bcs::CTask *pParent, long nTeam) : CUnivTaskEx(pParent, -1)
            {
                m_nStep = 0;
                m_fWait = 0.0f;
                nw4r::ut::List_Init(&m_List, 0x34);

                long nChr = msw::CChrMan::GetChrCoreNum();
                for (long i = 0; i < nChr; i++) {
                    msw::CChrCore *pChr = msw::CChrMan::GetChrCore(i);
                    if (!pChr->IsDisable() && nTeam != pChr->GetTeamNo()) {
                        new (0x100) CcKuromaLockonUnit(this, pChr);
                    }
                }
            }
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
    inline CcKuromaLockonUnit::CcKuromaLockonUnit(CcKuromaMagicMgr *pMgr, msw::CChrCore *pChr)
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
        m_hEffect = msw::CEffectCtrl::CreateAttachEffectAuto(0xB9, pChr->GetMdl(), 3);
    }
}

msw::CChrCoreDodgeKuroma::CChrCoreDodgeKuroma()
{
    m_hMagic = 0;
}

msw::CChrCoreDodgeKuroma::~CChrCoreDodgeKuroma()
{
}

inline void msw::CChrCoreDodgeKuroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

void msw::CChrCoreDodgeKuroma::Reset_()
{
    CChrCoreDodge::Reset_();
    ReleaseMagic_();
}

void msw::CChrCoreDodgeKuroma::DtorStateAirTrick()
{
    // Not used here. A guess (as in HockeyF's unit): the values are first
    // in .rodata, ahead of those of the trick functions (0 and the wait
    // CcKuromaMagicMgr gets in SubStateArtsShotIsReadyArtsShot_).
    float fUnusedZero = 0.0f;
    float fUnused = 10.0f;

    CChrCoreDodge::DtorStateAirTrick();
    ReleaseMagic_();
}

// Not zero once the trick is over. Step 0 is the jump: at its end the spell
// motion starts and the strike on the marked characters is set off; step 1
// waits for that motion and goes on to the loop motion and the second jump.
int msw::CChrCoreDodgeKuroma::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bReady = 0;

    switch (pWork->m_nTrickStep) {
        case 0:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount =
                pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : pWork->m_fTrickCount;
            SubStateArtsShotDoJumpMove_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                AnimPlay(3, 0x7B, 0, 4.0f, 0.0f);
                PlaySePos(0x4F1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0xB7, 3);
                ArtsOptCreateAttachEffectAuto(1, 0xB8, 3);
                CC17_DODGE::CcKuromaMagicMgr *pMagic =
                    static_cast<CC17_DODGE::CcKuromaMagicMgr *>(bcs::CTaskMan::GetTask(m_hMagic));
                if (pMagic && !pMagic->IsDelete()) {
                    pMagic->Start(10.0f);
                }
                pWork->m_fTrickCount = 0.0f;
                pWork->m_fTrickFrame = 60.0f + AnimGetResFrameNum(0x7B);
                pWork->m_nTrickStep++;
            }
            break;
        case 1:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount =
                pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : pWork->m_fTrickCount;
            if (IsAnimEnd(3)) {
                pWork->m_fAnimSpeed = 1.0f;
                SubStateArtsShotPlayLoopMotion_(pWork);
                pWork->m_nArtsLevel = 1;
                SubStateArtsShotSetTrickJumpParam_(pWork, 1);
                pWork->m_nTrickStep++;
            }
            break;
        default:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount =
                pWork->m_fTrickCount > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : pWork->m_fTrickCount;
            SubStateArtsShotDoJumpMove_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                bReady = 1;
            }
            break;
    }
    return bReady;
}

// The trick motion: its effects and sound, the option model, and the marks
// on the other team.
void msw::CChrCoreDodgeKuroma::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0xB5, 3);
    PlaySePos(0x48D, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    ArtsOptCreate(1, 0, 1);
    ArtsOptCreateAttachEffectAuto(0, 0xB6, 3);
    ArtsOptFadeIn(5.0f, 10.0f, 1);

    ReleaseMagic_();
    CC17_DODGE::CcKuromaMagicMgr *pMagic =
        new (0) CC17_DODGE::CcKuromaMagicMgr(CGame::GetTempParent(), GetTeamNo());
    m_hMagic = pMagic->GetId();
}

void msw::CChrCoreDodgeKuroma::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(pWork);
    ArtsOptCreateAttachEffectAuto(2, 0xBA, 3);
}

void msw::CChrCoreDodgeKuroma::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0xBC, 3);
    ArtsOptStopEffect(2);
    ArtsOptCreateAttachEffectAuto(3, 0xBB, 3);
    ArtsOptSetFadeOutSE(0x48F);
    ArtsOptFadeOut(5.0f, 59.0f, 1);
}

void msw::CChrCoreDodgeKuroma::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CChrCoreDodge::SubStateArtsShotPlayLandMotion_(pWork);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

void msw::CChrCoreDodgeKuroma::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *)
{
    Eff_CreateAttachAuto(0x16, 0xBD, 3);
    SE_PlayArtsLandSE();
}

CC17_DODGE::CcKuromaMagicMgr::~CcKuromaMagicMgr()
{
    DeleteAllChild();
    nw4r::ut::List_Init(&m_List, 0x34);
}

// Strikes every marked character still in the game when the wait is over:
// an effect at its feet, a sound and a hit that knocks it down.
void CC17_DODGE::CcKuromaMagicMgr::Main()
{
    if (m_nStep == 1 && m_fWait > 0.0f) {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            CcKuromaLockonUnit *pUnit = GetNext(0);
            while (pUnit) {
                long nChr = pUnit->GetChrIdx();
                msw::CChrCore *pChr;
                if (nChr >= 0 && nChr < msw::CChrMan::GetChrCoreNum() &&
                    (pChr = msw::CChrMan::GetChrCore(nChr)) != 0 && pChr->IsEnable()) {
                    pUnit->ClearEffect();
                    bcs::FVEC3 pos;
                    pChr->GetTransNodePos(pos);
                    pos.y = 0.0f;
                    msw::CEffectCtrl::SimpleEffect(0x5EC, pos, 3, 0, 0);
                    pChr->PlaySePos(0x48E, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                    pChr->SetHookDamage(0xB0D, pChr->GetRotY(), 0.0f, 240.0f, 0x27, 0.0f, 0, 0, -1, -1);
                }
                pUnit->Delete();
                pUnit = pUnit ? GetNext(pUnit) : 0;
            }
            m_nStep++;
        }
    }
}

CC17_DODGE::CcKuromaLockonUnit::~CcKuromaLockonUnit()
{
    if (m_pMgr) {
        m_pMgr->Remove(this);
    }
    if (m_hEffect) {
        msw::CEffectCtrl::StopAttachEffect(m_hEffect);
    }
}

// Keeps the mark on the character.
void CC17_DODGE::CcKuromaLockonUnit::Main()
{
    msw::CChrCore *pChr;
    if (m_hEffect && m_nChr >= 0 && m_nChr < msw::CChrMan::GetChrCoreNum() &&
        (pChr = msw::CChrMan::GetChrCore(m_nChr)) != 0 && pChr->IsEnable()) {
        msw::CEffectCluster *pCluster = msw::CEffectCtrl::GetObject(m_hEffect);
        if (pCluster) {
            bcs::FVEC3 pos;
            pChr->GetTransNodePos(pos);
            pCluster->SetTranslate(&pos);
        }
    }
}

// At trigger 0x21 of the motion of state 0x22 (kinds 2, 5 and 6 of it,
// unless status bit 0x400 is set), the special item effect on the option
// model.
void msw::CChrCoreDodgeKuroma::CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg)
{
    // Not used here. A guess: the original's .rodata has a -1 after the
    // strike's constants that nothing left in the file reads.
    float fUnused = -1.0f;

    CChrCore::CallbackMotTrg_(pTrg);
    if (pTrg) {
        switch (GetState()) {
            case 0x22: {
                STATE_WORK_22 *pWork = static_cast<STATE_WORK_22 *>(FsmqGetWork_());
                if (pWork->m_nUnk00 == 0 && !(m_nStatusFlag & 0x400) && pTrg->m_nType == 0x21) {
                    switch (pWork->m_nType) {
                        case 2:
                        case 5:
                        case 6:
                            ArtsOptCreateAttachEffect(3, 0x66A, "SP_item_02", 0, 0, 3);
                            break;
                    }
                }
                break;
            }
        }
    }
}
