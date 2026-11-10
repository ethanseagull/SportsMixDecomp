// The two pending headers stand in for nw4r/g3d/ResFile.hpp and
// game/msw/CLoadData.hpp, so they come before any header that includes
// those.
#include "nw4r/g3d/ResFile.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/TournamentWork.hpp"

#include "game/msw/TournamentData.hpp"

#include "game/msw/CEvDemoMdlChr.hpp"
#include "game/msw/CEvDemoRes.hpp"
#include "game/msw/CEvMdlCreaterTmMdlChr.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/TournamentSe.hpp"

namespace msw {

    namespace {

        // Motion numbers of the characters on the tournament map: indices
        // into a model's motion table. (names guessed from the functions
        // below that play them)
        enum {
            MOT_NONE,
            MOT_IDLE,
            MOT_02,
            MOT_03,
            MOT_JUMP,
            MOT_BATTLE,
            MOT_BATTLE_LOOP,
            MOT_WIN,
            MOT_LOSE,
            MOT_LOSE_LOOP,
            MOT_CUPUP,
            MOT_CUPUP_LOOP,
            MOT_SUPPORT1,
            MOT_SUPPORT1_LOOP,
            MOT_SUPPORT2,
            MOT_SUPPORT2_LOOP,
            MOT_SECRETOPEN,
            MOT_SECRETOPEN_LOOP,
            MOT_18,
            MOT_BIGGUN,
            MOT_GYAGYA,
            MOT_21,
            MOT_MAX
        };

        // Second argument of CEventModel::AnimPlay. (name guessed: the
        // motions that keep running are played with it)
        const unsigned long ANIM_LOOP = 2;

        // Triggers of the jump motion read by GetJumpFrame. (names guessed)
        const unsigned long JUMP_TRG_DOWN = 0x11;
        const unsigned long JUMP_TRG_UP = 0x12;

        // msw::CLoadData entries. (names guessed)
        const long LOADDATA_CHR_MDL = 0x25;
        const long LOADDATA_CHR_MOT = 0x26;

        // The model's foot sound table: sound numbers, -1 for none.
        const long s_FootSeTable[4] = { -1, -1, -1, 0xE29 };

        // Animation number of each motion in a character's own motion file.
        const short s_ChrMotTable[MOT_MAX] = {
            1,  // MOT_NONE
            1,  // MOT_IDLE
            2,  // MOT_02
            3,  // MOT_03
            4,  // MOT_JUMP
            5,  // MOT_BATTLE
            6,  // MOT_BATTLE_LOOP
            7,  // MOT_WIN
            8,  // MOT_LOSE
            9,  // MOT_LOSE_LOOP
            0,  // MOT_CUPUP
            0,  // MOT_CUPUP_LOOP
            0,  // MOT_SUPPORT1
            0,  // MOT_SUPPORT1_LOOP
            0,  // MOT_SUPPORT2
            0,  // MOT_SUPPORT2_LOOP
            10, // MOT_SECRETOPEN
            11, // MOT_SECRETOPEN_LOOP
            12, // MOT_18
            13, // MOT_BIGGUN
            14, // MOT_GYAGYA
            0,  // MOT_21
        };

        // The same for an event motion file, such as the victory scene's.
        const short s_EventMotTable[MOT_MAX] = {
            8,  // MOT_NONE
            8,  // MOT_IDLE
            8,  // MOT_02
            9,  // MOT_03
            1,  // MOT_JUMP
            10, // MOT_BATTLE
            11, // MOT_BATTLE_LOOP
            0,  // MOT_WIN
            0,  // MOT_LOSE
            0,  // MOT_LOSE_LOOP
            2,  // MOT_CUPUP
            3,  // MOT_CUPUP_LOOP
            4,  // MOT_SUPPORT1
            5,  // MOT_SUPPORT1_LOOP
            6,  // MOT_SUPPORT2
            7,  // MOT_SUPPORT2_LOOP
            0,  // MOT_SECRETOPEN
            0,  // MOT_SECRETOPEN_LOOP
            0,  // MOT_18
            0,  // MOT_BIGGUN
            0,  // MOT_GYAGYA
            0,  // MOT_21
        };
    }

    void TournamentData::SetVictoryRes(CEventModel *pModel, long nRes)
    {
        if (pModel != 0) {
            pModel->ChangeAnimFile(CLoadData::GetRes(nRes));
            pModel->SetMotTable(s_EventMotTable);
            pModel->SetAnimSpeed(1.0f);
            pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, 0.0f, 1);
        }
    }

    namespace {

        // The motion sequences of the poses: a motion and the one that
        // follows it. The "Rev" ones are the mirror images, played for
        // side 1.
        const CEventModel::MOT_DATA s_MotBattleRev[] = {
            { -MOT_BATTLE, 0, 8.0f, 0.0f },
            { -MOT_BATTLE_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotBattle[] = {
            { MOT_BATTLE, 0, 8.0f, 0.0f },
            { MOT_BATTLE_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotSecretOpenRev[] = {
            { -MOT_SECRETOPEN, 0, 8.0f, 0.0f },
            { -MOT_SECRETOPEN_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotSecretOpen[] = {
            { MOT_SECRETOPEN, 0, 8.0f, 0.0f },
            { MOT_SECRETOPEN_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotLose[] = {
            { MOT_LOSE, 0, 8.0f, 0.0f },
            { MOT_LOSE_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotLoseRev[] = {
            { -MOT_LOSE, 0, 8.0f, 0.0f },
            { -MOT_LOSE_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotWin[] = {
            { MOT_WIN, 0, 8.0f, 0.0f },
            { MOT_IDLE, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotWinRev[] = {
            { -MOT_WIN, 0, 8.0f, 0.0f },
            { MOT_IDLE, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotJump[] = {
            { MOT_JUMP, 0, 8.0f, 0.0f },
            { MOT_IDLE, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotBigGun[] = {
            { MOT_BIGGUN, 0, 8.0f, 0.0f },
            { MOT_IDLE, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotCupUp[] = {
            { MOT_CUPUP, 0, 0.0f, 0.0f },
            { MOT_CUPUP_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotSupport1Rev[] = {
            { -MOT_SUPPORT1, 0, 8.0f, 0.0f },
            { -MOT_SUPPORT1_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotSupport2Rev[] = {
            { -MOT_SUPPORT2, 0, 8.0f, 0.0f },
            { -MOT_SUPPORT2_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotSupport1[] = {
            { MOT_SUPPORT1, 0, 8.0f, 0.0f },
            { MOT_SUPPORT1_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };

        const CEventModel::MOT_DATA s_MotSupport2[] = {
            { MOT_SUPPORT2, 0, 8.0f, 0.0f },
            { MOT_SUPPORT2_LOOP, ANIM_LOOP, 8.0f, 0.0f },
            { MOT_NONE, 0, 0.0f, 0.0f },
        };
    }

    void TournamentData::SetIdlePause(CEventModel *pModel, float fFrame)
    {
        if (pModel != 0) {
            pModel->SetAnimSpeed(1.0f);
            pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
        }
    }

    void TournamentData::SetGyaGyaPause(CEventModel *pModel, float fFrame)
    {
        if (pModel != 0) {
            pModel->SetAnimSpeed(1.0f);
            pModel->AnimPlay(MOT_GYAGYA, ANIM_LOOP, 4.0f, fFrame, 1);
        }
    }

    float TournamentData::SetBattlePause(CEventModel *pModel, long nSide, float fFrame)
    {
        float fLength = 0.0f;

        if (pModel == 0) {
            return fLength;
        }

        pModel->SetAnimSpeed(1.0f);

        switch (nSide) {
            case 0:
                pModel->AnimPlay(s_MotBattle, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(MOT_BATTLE);
                break;
            case 1:
                pModel->AnimPlay(s_MotBattleRev, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(-MOT_BATTLE);
                break;
            default:
                pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
                fLength = pModel->AnimResFrame(MOT_IDLE);
                break;
        }

        return fLength - fFrame;
    }

    float TournamentData::SetSecretOpenPause(CEventModel *pModel, long nSide, float fFrame)
    {
        float fLength = 0.0f;

        if (pModel == 0) {
            return fLength;
        }

        pModel->SetAnimSpeed(1.0f);

        switch (nSide) {
            case 0:
                pModel->AnimPlay(s_MotSecretOpen, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(MOT_SECRETOPEN);
                break;
            case 1:
                pModel->AnimPlay(s_MotSecretOpenRev, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(-MOT_SECRETOPEN);
                break;
            default:
                pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
                fLength = pModel->AnimResFrame(MOT_IDLE);
                break;
        }

        return fLength - fFrame;
    }

    float TournamentData::SetLosePause(CEventModel *pModel, long nSide, float fFrame)
    {
        float fLength = 0.0f;

        if (pModel == 0) {
            return fLength;
        }

        pModel->SetAnimSpeed(1.0f);

        switch (nSide) {
            case 0:
                pModel->AnimPlay(s_MotLose, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(MOT_LOSE);
                break;
            case 1:
                pModel->AnimPlay(s_MotLoseRev, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(-MOT_LOSE);
                break;
            default:
                pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
                fLength = pModel->AnimResFrame(MOT_IDLE);
                break;
        }

        return fLength - fFrame;
    }

    float TournamentData::SetWinPause(CEventModel *pModel, long nSide, float fFrame)
    {
        float fLength = 0.0f;

        if (pModel == 0) {
            return fLength;
        }

        pModel->SetAnimSpeed(1.0f);

        switch (nSide) {
            case 0:
                pModel->AnimPlay(s_MotWin, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(MOT_WIN);
                break;
            case 1:
                pModel->AnimPlay(s_MotWinRev, 0, fFrame, -1);
                fLength = pModel->AnimResFrame(-MOT_WIN);
                break;
            default:
                pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
                fLength = pModel->AnimResFrame(MOT_IDLE);
                break;
        }

        return fLength - fFrame;
    }

    float TournamentData::SetJumpPause(CEventModel *pModel, float fFrame)
    {
        if (pModel == 0) {
            return 0.0f;
        }

        pModel->SetAnimSpeed(1.0f);
        pModel->AnimPlay(s_MotJump, 0, fFrame, -1);
        return pModel->AnimResFrame(MOT_JUMP);
    }

    float TournamentData::SetBigGunPause(CEventModel *pModel, float fFrame)
    {
        if (pModel == 0) {
            return 0.0f;
        }

        pModel->SetAnimSpeed(1.0f);
        pModel->AnimPlay(s_MotBigGun, 0, fFrame, -1);
        // The length asked for is motion 18's, not that of the motion
        // played (19).
        return pModel->AnimResFrame(MOT_18);
    }

    void TournamentData::SetVicotoryCupUpPause(CEventModel *pModel, float fFrame)
    {
        if (pModel != 0) {
            pModel->SetAnimSpeed(1.0f);
            pModel->AnimPlay(s_MotCupUp, 0, fFrame, -1);
        }
    }

    void TournamentData::SetVicotorySupport1Pause(CEventModel *pModel, long nSide, float fFrame)
    {
        if (pModel != 0) {
            pModel->SetAnimSpeed(1.0f);

            switch (nSide) {
                case 0:
                    pModel->AnimPlay(s_MotSupport1, 0, fFrame, -1);
                    break;
                case 1:
                    pModel->AnimPlay(s_MotSupport1Rev, 0, fFrame, -1);
                    break;
                default:
                    pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
                    break;
            }
        }
    }

    void TournamentData::SetVicotorySupport2Pause(CEventModel *pModel, long nSide, float fFrame)
    {
        if (pModel != 0) {
            pModel->SetAnimSpeed(1.0f);

            switch (nSide) {
                case 0:
                    pModel->AnimPlay(s_MotSupport2, 0, fFrame, -1);
                    break;
                case 1:
                    pModel->AnimPlay(s_MotSupport2Rev, 0, fFrame, -1);
                    break;
                default:
                    pModel->AnimPlay(MOT_IDLE, ANIM_LOOP, 4.0f, fFrame, 1);
                    break;
            }
        }
    }

    float TournamentData::GetJumpFrame(CEventModel *pModel, float &rUp, float &rDown)
    {
        float fEnd = 61.0f;
        float fUp = 11.0f;
        float fDown = 51.0f;

        if (pModel != 0) {
            fEnd = pModel->AnimResFrame(MOT_JUMP);
            fUp = pModel->AnimResTrgFrame(MOT_JUMP, JUMP_TRG_UP, 0.0f);
            fDown = pModel->AnimResTrgFrame(MOT_JUMP, JUMP_TRG_DOWN, 0.0f);
        }

        if (fUp < 0.0f) {
            fUp = 11.0f;
        }
        if (fDown < 0.0f) {
            fDown = 51.0f;
        }

        rUp = fUp;
        rDown = fEnd - fDown;
        return (fEnd - fUp) - (fEnd - fDown);
    }

    namespace {

        const int CHAR_NUM = 25;

        const TournamentData::CHAR_INFO s_CharInfo[CHAR_NUM] = {
            { 1.0f, 1.1f },
            { 1.0f, 1.2f },
            { 1.0f, 1.2f },
            { 1.0f, 1.2f },
            { 1.0f, 1.2f },
            { 1.0f, 1.1f },
            { 1.0f, 1.1f },
            { 1.07f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 1.4f },
            { 1.0f, 1.1f },
            { 1.03f, 1.1f },
            { 1.04f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 0.8f },
            { 1.12f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 1.1f },
            { 1.0f, 0.9f },
            { 1.0f, 0.9f },
            { 1.0f, 1.3f },
            { 1.0f, 1.3f },
        };
    }

    const TournamentData::CHAR_INFO &TournamentData::GetCharInfo(long nChr)
    {
        const CHAR_INFO *pInfo = &s_CharInfo[0];

        if (nChr >= 0 && nChr < CHAR_NUM) {
            pInfo = &s_CharInfo[nChr];
        }
        return *pInfo;
    }

    long CEvMdlCreaterTmMdlChr::LoadRes(CEvDemoRes::_CHR_INFO &rInfo, bcs::CLoadMan::FILE_INPUT *, long nFile)
    {
        rInfo.nMdlFile = LOADDATA_CHR_MDL;

        // Selection 0 is the character's own motion file; selection n is
        // event motion file n - 1, which takes an entry of its own.
        long nMot = rInfo.nMotSel - 1;
        if (nMot < 0) {
            rInfo.nMotFile[rInfo.nMotSel] = LOADDATA_CHR_MOT;
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

    void CEvMdlCreaterTmMdlChr::UnLoadRes(CEvDemoRes::_CHR_INFO &rInfo)
    {
        if (rInfo.nMotSel - 1 >= 0 && rInfo.nMotFile[rInfo.nMotSel] >= 0) {
            rInfo.nMotFile[rInfo.nMotSel] = -1;
            rInfo.nMotSel = 0;

            // A model that exists goes back to the character's own motions.
            if (rInfo.pModel != 0) {
                rInfo.pModel->ChangeAnimFile(CLoadData::GetChrMotRes(LOADDATA_CHR_MOT, rInfo.nId & 0xFF));
                rInfo.pModel->SetMotTable(s_ChrMotTable);
                rInfo.pModel->AnimPlay(MOT_NONE, 0, 0.0f, 0.0f, 1);
            }
        }
    }

    int CEvMdlCreaterTmMdlChr::BindRes(CEvDemoRes::_CHR_INFO &rInfo)
    {
        nw4r::g3d::ResFile res = CLoadData::GetRes(rInfo.nMotFile[rInfo.nMotSel]);

        if (res.IsValid()) {
            res.CheckRevision();
            res.Init();
            res.Bind(res);
        }
        return 0;
    }

    CEventModel *CEvMdlCreaterTmMdlChr::CreateModel(CEvDemoRes::_CHR_INFO &rInfo)
    {
        nw4r::g3d::ResFile mdlRes;
        nw4r::g3d::ResFile motRes;
        int bReverse;
        const short *pMotTable;
        long nChr = rInfo.nId & 0xFF;

        mdlRes = CLoadData::GetChrMdlRes(rInfo.nMdlFile, nChr, (rInfo.nId >> 8) & 0xFF);

        if (rInfo.nMotSel != 0) {
            motRes = CLoadData::GetRes(rInfo.nMotFile[rInfo.nMotSel]);
            pMotTable = s_EventMotTable;
        } else {
            motRes = CLoadData::GetChrMotRes(rInfo.nMotFile[rInfo.nMotSel], nChr);
            pMotTable = s_ChrMotTable;
        }

        bReverse = (CEvDemoRes::GetInstance()->GetFlag() >> 2) & 1;

        if (rInfo.pModel != 0) {
            if (rInfo.IsCreate()) {
                CTournamentMan::GetInstance()->CleanChrModel(rInfo.nIndex, 0);
            } else {
                // The model is kept: it only changes its animations.
                long nAnim = rInfo.pModel->GetAnimNo();

                rInfo.pModel->ChangeAnimFile(motRes);
                rInfo.pModel->SetMotTable(pMotTable);
                rInfo.pModel->SetMotionReverse(bReverse);

                long nDemoType = TournamentWork::GetTournamentInfo().nDemoType;
                if (nDemoType == 0x46 || nDemoType == 0x47) {
                    rInfo.pModel->AnimPlay(nAnim, ANIM_LOOP, 8.0f, 0.0f, 1);
                } else {
                    rInfo.pModel->AnimPlay(MOT_NONE, 0, 0.0f, 0.0f, 1);
                }
                return rInfo.pModel;
            }
        }

        CEvDemoMdlChr::_SETUP setup(mdlRes, motRes, rInfo.IsDisp());
        setup.pInit = rInfo.pInit;
        setup.nType = nChr;
        setup.nIndex = rInfo.nIndex;
        setup.pMotTable = pMotTable;

        CEvDemoMdlChr *pModel = CEvDemoMdlChr::CreateModel(setup, 0);
        pModel->SetMotionReverse(bReverse);
        pModel->SetFootSeTable(s_FootSeTable);
        pModel->SetVoiceTable(TournamentSe::GetVoiceTable(nChr));
        return pModel;
    }
}
