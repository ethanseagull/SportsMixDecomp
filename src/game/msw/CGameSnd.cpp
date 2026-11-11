// msw::CGameSnd, the sound interface of the games (sound effects, voices,
// the three-track game music, loading the sound groups of a game), and
// msw::CGameBGM, the music of a game as a task with one state per music
// situation (stage music, star, bonus time, jingles, hurry-up).
//
// One file in the original (0x80146548..0x8014A618): the two share the
// .sdata2 constants 1.0f and 0.0f, CGameBGM calls CGameSnd::PlayBGM inlined,
// and .data, .bss and .sbss run on from one to the other.
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameBGM.hpp"

#include <stdio.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "nw4r/snd/StrmSoundHandle.hpp"

namespace msw {

    // Per-character tables defined with the character data, in other files
    // (names guessed; numbered after the first character using them).
    // Variations of each voice from 0x12 on: up to four offsets from the
    // character's first voice, -1 ending the list.
    extern const signed char g_aChrVoice00[][4], g_aChrVoice01[][4], g_aChrVoice02[][4], g_aChrVoice03[][4],
        g_aChrVoice04[][4], g_aChrVoice05[][4], g_aChrVoice06[][4], g_aChrVoice07[][4], g_aChrVoice08[][4],
        g_aChrVoice09[][4], g_aChrVoice10[][4], g_aChrVoice11[][4], g_aChrVoice12[][4], g_aChrVoice13[][4],
        g_aChrVoice14[][4], g_aChrVoice15[][4], g_aChrVoice16[][4], g_aChrVoice17[][4], g_aChrVoice18[][4],
        g_aChrVoice19[][4], g_aChrVoice20[][4];
    // The up to four sounds of each action PlayWrapAct plays, -1 ending the
    // list.
    extern const short g_aChrWrapAct00[][4], g_aChrWrapAct01[][4], g_aChrWrapAct02[][4], g_aChrWrapAct03[][4],
        g_aChrWrapAct04[][4], g_aChrWrapAct05[][4], g_aChrWrapAct06[][4], g_aChrWrapAct07[][4],
        g_aChrWrapAct08[][4], g_aChrWrapAct09[][4], g_aChrWrapAct10[][4], g_aChrWrapAct11[][4],
        g_aChrWrapAct12[][4], g_aChrWrapAct13[][4], g_aChrWrapAct14[][4], g_aChrWrapAct15[][4],
        g_aChrWrapAct16[][4], g_aChrWrapAct17[][4], g_aChrWrapAct18[][4], g_aChrWrapAct19[][4],
        g_aChrWrapAct20[][4];

    namespace {
        // Counts the callers check against; unsigned, so a negative number
        // fails the check too.
        const unsigned long CHR_NUM = 25;
        const unsigned long VOICE_NUM = 0x96;  // voices of a character
        const unsigned long WRAP_ACT_NUM = 24; // actions of PlayWrapAct
        // The checks compare against the last valid number (`> last`, not
        // `>= count`).
        const unsigned long CHR_LAST = CHR_NUM - 1;
        const unsigned long VOICE_LAST = VOICE_NUM - 1;
        const unsigned long WRAP_ACT_LAST = WRAP_ACT_NUM - 1;

        enum {
            VOICE_SLOT_NUM = 16,
            VOICE_VAR_FIRST = 0x12, // the first voice with variations
            VOICE_VAR_MAX = 4,
            WRAP_ACT_SND_MAX = 4,
            MUSIC_TRACK_NUM = 3,
            SND_HEAP = 1 // the sound heap the groups go into
        };

        // Size class of each character for PlayChrSeA (2 and up: large).
        const unsigned char s_aChrSeClass[CHR_NUM] = {
            1, 1, 0, 0, 1, 2, 1, 2, 0, 0, 2, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 2, 1,
        };

        // PlayChrSeA: [nKind][nVar] for the ordinary characters, the large
        // ones, character 0xC and character 0x10.
        const long s_aaChrSeA[4][4] = {
            { 0xCB6, 0xCB8, 0xCBA, 0xCBC },
            { 0xCBE, 0xCC0, 0xCC2, 0xCC4 },
            { 0xCC6, 0xCC8, 0xCCA, 0xCCC },
            { 0xCCE, 0xCCF, 0xCD0, 0xCD1 },
        };
        const long s_aaChrSeALarge[4][4] = {
            { 0xCB7, 0xCB9, 0xCBB, 0xCBD },
            { 0xCBF, 0xCC1, 0xCC3, 0xCC5 },
            { 0xCC7, 0xCC9, 0xCCB, 0xCCD },
            { 0xCCE, 0xCCF, 0xCD0, 0xCD1 },
        };
        const long s_aaChrSeA0C[4][4] = {
            { 0xCD2, 0xCD3, 0xCD4, 0xCBC },
            { 0xCD2, 0xCD3, 0xCD4, 0xCC4 },
            { 0xCD2, 0xCD3, 0xCD4, 0xCCC },
            { 0xCD5, 0xCD6, 0xCD7, 0xCD1 },
        };
        const long s_aaChrSeA10[4][4] = {
            { 0xCD8, 0xCD9, 0xCDA, 0xCD8 },
            { 0xCD8, 0xCD9, 0xCDA, 0xCD8 },
            { 0xCD8, 0xCD9, 0xCDA, 0xCD8 },
            { 0xCDB, 0xCDC, 0xCDD, 0xCDB },
        };

        // PlayChrSeB: [nKind][nVar] for the other characters, character 0xC
        // and character 0x10.
        const long s_aaChrSeB[5][6] = {
            { 0xCDE, 0xCDF, 0xCE0, 0xCE1, 0xCE2, 0xCDE },
            { 0xCE3, 0xCE4, 0xCE5, 0xCE6, 0xCE7, 0xCE3 },
            { 0xCE8, 0xCE9, 0xCEA, 0xCEB, 0xCEC, 0xCE8 },
            { 0xCED, 0xCEE, 0xCEF, 0xCF0, 0xCF1, 0xCED },
            { 0xCF2, 0xCF3, 0xCF4, 0xCF5, 0xCF6, 0xCF2 },
        };
        const long s_aaChrSeB0C[5][6] = {
            { 0xCF7, 0xCF8, 0xCF8, 0xCF9, 0xCFA, 0xCF7 },
            { 0xCF7, 0xCF8, 0xCF8, 0xCF9, 0xCFA, 0xCF7 },
            { 0xCF7, 0xCF8, 0xCF8, 0xCF9, 0xCFA, 0xCF7 },
            { 0xCF7, 0xCF8, 0xCF8, 0xCF9, 0xCFA, 0xCF7 },
            { 0xCFB, 0xCFC, 0xCFC, 0xCFD, 0xCFE, 0xCFB },
        };
        const long s_aaChrSeB10[5][6] = {
            { 0xCFF, 0xD00, 0xD00, 0xD01, 0xD02, 0xCFF },
            { 0xCFF, 0xD00, 0xD00, 0xD01, 0xD02, 0xCFF },
            { 0xCFF, 0xD00, 0xD00, 0xD01, 0xD02, 0xCFF },
            { 0xCFF, 0xD00, 0xD00, 0xD01, 0xD02, 0xCFF },
            { 0xD03, 0xD04, 0xD04, 0xD05, 0xD06, 0xD03 },
        };

        // First voice of each character.
        const short s_aChrVoiceBase[CHR_NUM] = {
            623, 641, 659, 677, 695, 713, 731, 749, 767, 785, 803, 821, 839,
            857, 875, 893, 911, 929, 947, 965, 983, 965, 983, 965, 983,
        };

        // 3D listener settings (names of the three values guessed from the
        // nw4r calls they go to).
        struct LISTENER_PARAM {
            float m_fMaxVolumeDistance;
            float m_fUnitDistance;
            float m_fInteriorSize;
        };
        enum {
            LISTENER_MENU,
            LISTENER_TOURNAMENT,
            LISTENER_TOURNAMENT_STAGE,
            LISTENER_GAME,           // 3 to 7: each sport (+ GAME_MODE)
            LISTENER_PARTY_GAME = 8, // 8 to 11: each sport's party game
            LISTENER_NUM = 12
        };
        const LISTENER_PARAM s_aListener[LISTENER_NUM] = {
            { 2.5f, 5.0f, 2.0f },   { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f },
            { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f },
            { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f }, { 10.0f, 20.0f, 2.0f }, { 20.0f, 30.0f, 2.0f },
        };

        // The three tracks of each game music.
        // One table per track (three objects: PreparePlayMusic addresses
        // each from the start of the section, which the compiler does for
        // separate objects but not for rows of one array).
        const short s_aMusicTrack0[12] = { 64, 67, 70, 73, 76, 79, 82, 85, 88, 91, 94, 97 };
        const short s_aMusicTrack1[12] = { 65, 68, 71, 74, 77, 80, 83, 86, 89, 92, 95, 98 };
        const short s_aMusicTrack2[12] = { 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 99 };

        // Sound group of each stage (indexed by GameWork::m_nStage), -1 for
        // none.
        const short s_aStageGroup[100] = {
            0x26, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, -1,   0x2D,
            0x2E, 0x2F, 0x30, -1,   -1,   0x31, 0x32, 0x33, -1,   -1,
            -1,   -1,   -1,   -1,   -1,   0x26, -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   0x34, 0x35, 0x36, -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
            0x37, 0x38, 0x39, -1,   -1,   -1,   -1,   -1,   -1,   -1,
            -1,   -1,   -1,   -1,   -1,   0x3A, 0x3B, 0x3C, -1,   -1,
            -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        };

        // Sound groups of each character in a game (two) and the one
        // LoadChrSnd loads; -1 for none.
        const short s_aChrGroupA[CHR_NUM] = {
            68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 87, 88, 87, 88,
        };
        const short s_aChrGroupB[CHR_NUM] = {
            109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
            122, 123, 124, 125, 126, -1,  127, 128, 127, 128, 127, 128,
        };
        const short s_aChrGroupC[CHR_NUM] = {
            89,  90,  91,  92,  93,  94,  95,  96, 97,  98,  99,  100, 101,
            102, 103, 104, 105, 106, -1, 107, 108, 107, 108, 107, 108,
        };

        // Sound group of each volleyball party game (party parameter 0).
        const short s_aPartyGroup[12] = {
            129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
        };

        // The per-character tables, indexed by character.
        const signed char (*s_apChrVoice[CHR_NUM])[4] = {
            g_aChrVoice00, g_aChrVoice01, g_aChrVoice02, g_aChrVoice03, g_aChrVoice04,
            g_aChrVoice05, g_aChrVoice06, g_aChrVoice07, g_aChrVoice08, g_aChrVoice09,
            g_aChrVoice10, g_aChrVoice11, g_aChrVoice12, g_aChrVoice13, g_aChrVoice14,
            g_aChrVoice15, g_aChrVoice16, g_aChrVoice17, g_aChrVoice18, g_aChrVoice19,
            g_aChrVoice20, g_aChrVoice19, g_aChrVoice20, g_aChrVoice19, g_aChrVoice20,
        };
        const short (*s_apChrWrapAct[CHR_NUM])[4] = {
            g_aChrWrapAct00, g_aChrWrapAct01, g_aChrWrapAct02, g_aChrWrapAct03, g_aChrWrapAct04,
            g_aChrWrapAct05, g_aChrWrapAct06, g_aChrWrapAct07, g_aChrWrapAct08, g_aChrWrapAct09,
            g_aChrWrapAct10, g_aChrWrapAct11, g_aChrWrapAct12, g_aChrWrapAct13, g_aChrWrapAct14,
            g_aChrWrapAct15, g_aChrWrapAct16, g_aChrWrapAct17, g_aChrWrapAct18, g_aChrWrapAct19,
            g_aChrWrapAct20, g_aChrWrapAct19, g_aChrWrapAct20, g_aChrWrapAct19, g_aChrWrapAct20,
        };

        unsigned long s_ahVoice[VOICE_SLOT_NUM];  // task ids of the voice slots
        float s_afVoiceVolume[VOICE_SLOT_NUM];    // volume of each voice slot
        unsigned long s_ahMusic[MUSIC_TRACK_NUM]; // task ids of the music tracks
        long s_nGameHeapState; // heap level LoadChrSnd loads over
        long s_nMenuHeapState; // heap level MenuLoadSeq loads over
        long s_nUnk;           // only cleared (meaning not known)

        inline bcs::CSndTask *GetSndTask(unsigned long id)
        {
            return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(id));
        }

        inline void SetListener(const LISTENER_PARAM &rParam)
        {
            float fMaxVolumeDistance = rParam.m_fMaxVolumeDistance;
            float fUnitDistance = rParam.m_fUnitDistance;
            float fInteriorSize = rParam.m_fInteriorSize;
            bcs::CSnd::SetListenerMaxVolumeDistance(fMaxVolumeDistance);
            bcs::CSnd::SetListenerUnitDistance(fUnitDistance);
            bcs::CSnd::SetListenerInteriorSize(fInteriorSize);
        }

        // LoadMenuSnd goes through this one and the others call the
        // reference form directly: the float registers of the inlined body
        // swap with the inline depth, and the original has them one way in
        // LoadMenuSnd and the other in SetGameListener/LoadTournamentSnd
        // (which caller used which form is a guess).
        inline void SetListener(long nListener)
        {
            SetListener(s_aListener[nListener]);
        }

        // Makes the sound heap for rList, nExtra bytes larger, and loads the
        // groups into it.
        inline void CreateHeapAndLoad(const bcs::CSndGroupList &rList, unsigned long nExtra)
        {
            unsigned long nSize = bcs::CSnd::GetGroupListSize(rList);
            bcs::CSnd::CreateHeap(SND_HEAP, nSize + nExtra, 1, -0x20);
            bcs::CSnd::LoadGroupList(rList);
        }

        inline int IsMusicPrepared()
        {
            for (long i = 0; i < MUSIC_TRACK_NUM; i++) {
                bcs::CSndTask *pTask = GetSndTask(s_ahMusic[i]);
                if (pTask == 0) {
                    return 0;
                }
                if (!pTask->IsPrepared()) {
                    return 0;
                }
            }
            return 1;
        }
    }

    void CGameSnd::Init()
    {
        for (long i = 0; i < VOICE_SLOT_NUM; i++) {
            s_ahVoice[i] = 0;
            s_afVoiceVolume[i] = 1.0f;
        }
        for (long i = 0; i < MUSIC_TRACK_NUM; i++) {
            s_ahMusic[i] = 0;
        }
        s_nGameHeapState = 0;
        s_nMenuHeapState = 0;
        s_nUnk = 0;
    }

    unsigned long CGameSnd::Play(long nSe, const bcs::FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                                 float fArg5, float fArg6)
    {
        return bcs::CSndTask::Play(nSe, pPos, nArg2, nArg3, fArg4, fArg5, fArg6);
    }

    unsigned long CGameSnd::Play(long nSe, const bcs::FVEC3 &rPos, unsigned long nArg2, long nArg3, float fArg4,
                                 float fArg5, float fArg6)
    {
        return bcs::CSndTask::Play(nSe, rPos, nArg2, nArg3, fArg4, fArg5, fArg6);
    }

    unsigned long CGameSnd::PlayBGM(long nSeq, unsigned long nArg1, float fArg2, float fArg3)
    {
        return bcs::CSndTask::PlayBGM(nSeq, nArg1, fArg2, fArg3);
    }

    unsigned long CGameSnd::PlayChrSeA(long nChr, long nKind, long nVar, const bcs::FVEC3 &rPos,
                                       unsigned long nArg2, long nArg3, float fArg4, float fVolume, float fArg6)
    {
        long nClass = nChr <= CHR_LAST ? s_aChrSeClass[nChr] : 1;

        long nSe;
        if (nChr == 0xC) {
            nSe = s_aaChrSeA0C[nKind][nVar];
        } else if (nChr == 0x10) {
            nSe = s_aaChrSeA10[nKind][nVar];
        } else if (nClass >= 2) {
            nSe = s_aaChrSeALarge[nKind][nVar];
        } else {
            nSe = s_aaChrSeA[nKind][nVar];
        }

        CGame::GameWork &work = CGame::g_GameWork;
        if (work.GetGameMode() == CGame::GAME_MODE_VOLLEY && work.GetGameType() == 2) {
            fVolume *= 0.5f;
        }
        return bcs::CSndTask::Play(nSe, rPos, nArg2, nArg3, fArg4, fVolume, fArg6);
    }

    long CGameSnd::GetGameType()
    {
        return CGame::g_GameWork.m_nGameType;
    }

    unsigned long CGameSnd::PlayChrSeB(long nChr, long nKind, long nVar, const bcs::FVEC3 &rPos,
                                       unsigned long nArg2, long nArg3, float fArg4, float fVolume, float fArg6)
    {
        long nSe;
        if (nChr == 0xC) {
            nSe = s_aaChrSeB0C[nKind][nVar];
        } else if (nChr == 0x10) {
            nSe = s_aaChrSeB10[nKind][nVar];
        } else {
            nSe = s_aaChrSeB[nKind][nVar];
        }
        return bcs::CSndTask::Play(nSe, rPos, nArg2, nArg3, fArg4, fVolume, fArg6);
    }

    void CGameSnd::PlayVoice(long nSlot, long nChr, long nVoice, const bcs::FVEC3 &rPos, unsigned long nArg2,
                             long nArg3)
    {
        PlayVoice(nSlot, nChr, nVoice, rPos, nArg2, nArg3, bcs::BCS_RandMax(0xFFFL));
    }

    void CGameSnd::PlayVoice(long nSlot, long nChr, long nVoice, const bcs::FVEC3 &rPos, unsigned long nArg2,
                             long nArg3, long nRand)
    {
        StopVoice(nSlot, 0.0f);
        if (nVoice == -1) {
            return;
        }
        if (nChr > CHR_LAST) {
            return;
        }
        if (nVoice > VOICE_LAST) {
            return;
        }

        long nSe = -1;
        short nBase = s_aChrVoiceBase[nChr];
        if (nVoice >= VOICE_VAR_FIRST) {
            const signed char *pVar = s_apChrVoice[nChr][nVoice - VOICE_VAR_FIRST];
            long nVarNum;
            for (nVarNum = 0; nVarNum < VOICE_VAR_MAX; nVarNum++) {
                if (pVar[nVarNum] == -1) {
                    break;
                }
            }
            if (nVarNum >= 1) {
                nSe = nBase + pVar[nRand % nVarNum];
            }
        } else {
            nSe = nVoice + nBase;
        }

        if (nSe >= 0) {
            s_ahVoice[nSlot] = bcs::CSndTask::Play(nSe, rPos, nArg2, nArg3, 0.0f, s_afVoiceVolume[nSlot], 0.0f);
        }
    }

    void CGameSnd::StopVoice(long nSlot, float fFadeFrames)
    {
        bcs::CSndTask *pTask = GetSndTask(s_ahVoice[nSlot]);
        if (pTask != 0) {
            pTask->Stop(fFadeFrames);
        }
    }

    int CGameSnd::IsVoicePlaying(long nSlot)
    {
        return GetSndTask(s_ahVoice[nSlot]) != 0;
    }

    void CGameSnd::SetVoiceVolume(long nSlot, float fVolume, float fFadeFrames)
    {
        s_afVoiceVolume[nSlot] = fVolume;
        bcs::CSndTask *pTask = GetSndTask(s_ahVoice[nSlot]);
        if (pTask != 0) {
            pTask->SetVolume(fVolume, fFadeFrames);
        }
    }

    void CGameSnd::ResetVoiceVolume(float fFadeFrames)
    {
        for (long i = 0; i < VOICE_SLOT_NUM; i++) {
            SetVoiceVolume(i, 1.0f, fFadeFrames);
        }
    }

    long CGameSnd::PlayWrapAct(long nChr, long nAct, const bcs::FVEC3 &rPos, unsigned long nArg2, long nArg3,
                               float fArg4, float fVolume)
    {
        unsigned long ahSnd[WRAP_ACT_SND_MAX];
        return PlayWrapAct(ahSnd, nChr, nAct, rPos, nArg2, nArg3, fArg4, fVolume);
    }

    long CGameSnd::PlayWrapAct(unsigned long *pIds, long nChr, long nAct, const bcs::FVEC3 &rPos,
                               unsigned long nArg2, long nArg3, float fArg4, float fVolume)
    {
        if (nAct == -1) {
            return 0;
        }
        if (nChr > CHR_LAST) {
            return 0;
        }
        if (nAct > WRAP_ACT_LAST) {
            return 0;
        }

        const short *pSe = s_apChrWrapAct[nChr][nAct];
        long nNum = 0;
        for (long i = 0; i < WRAP_ACT_SND_MAX; i++) {
            if (pSe[i] < 0) {
                break;
            }
            pIds[i] = bcs::CSndTask::Play(pSe[i], rPos, nArg2, nArg3, fArg4, fVolume, 0.0f);
            nNum++;
        }
        return nNum;
    }

    void CGameSnd::SetMusicFiles(bcs::CLoadMan::FILE_INPUT *aFile, long nStart, long nEnd)
    {
        for (long i = nStart; i < nEnd; i++) {
            aFile[i].m_nFlags |= 1;
        }

        CGame::GameWork &work = CGame::g_GameWork;
        if (work.GetGameMode() == CGame::GAME_MODE_VOLLEY && work.GetGameType() == 2) {
            long nParam = CGame::GetPartyGameParam(0);
            aFile[nStart].m_nFlags &= ~1;
            sprintf(aFile[nStart].m_szPath, "snd/Music%.2d.mus", nParam);
        }
    }

    int CGameSnd::PreparePlayMusic(long nMusic)
    {
        s_ahMusic[0] = bcs::CSndTask::PrepareBGM(s_aMusicTrack0[nMusic], 1, 1.0f, 0.0f);
        s_ahMusic[1] = bcs::CSndTask::PrepareBGM(s_aMusicTrack1[nMusic], 1, 1.0f, 0.0f);
        s_ahMusic[2] = bcs::CSndTask::PrepareBGM(s_aMusicTrack2[nMusic], 1, 1.0f, 0.0f);
        // Only the first track is heard at first.
        SetMusicVolume(1, 0.0f, 0.0f);
        SetMusicVolume(2, 0.0f, 0.0f);
        return 1;
    }

    int CGameSnd::StartPreparedMusic()
    {
        if (!IsMusicPrepared()) {
            return 0;
        }
        for (long i = 0; i < MUSIC_TRACK_NUM; i++) {
            bcs::CSndTask *pTask = GetSndTask(s_ahMusic[i]);
            if (pTask == 0) {
                return 0;
            }
            pTask->StartPrepared();
        }
        return 1;
    }

    void CGameSnd::StopMusic(float fFadeFrames)
    {
        for (long i = 0; i < MUSIC_TRACK_NUM; i++) {
            bcs::CSndTask *pTask = GetSndTask(s_ahMusic[i]);
            if (pTask != 0) {
                pTask->Stop(fFadeFrames);
            }
        }
    }

    void CGameSnd::SetMusicVolume(long nTrack, float fVolume, float fFadeFrames)
    {
        bcs::CSndTask *pTask = GetSndTask(s_ahMusic[nTrack]);
        if (pTask != 0) {
            pTask->SetVolume(fVolume, fFadeFrames);
        }
    }

    float CGameSnd::GetMusicFrame()
    {
        bcs::CSndTask *pTask = GetSndTask(s_ahMusic[0]);
        if (pTask == 0) {
            return -1.0f;
        }

        nw4r::snd::StrmSoundHandle handle(&pTask->m_Handle);
        nw4r::snd::StrmDataInfo info;
        if (!handle.ReadStrmDataInfo(&info)) {
            return -1.0f;
        }
        return 60.0f * handle.GetPlaySamplePosition() / info.m_nSampleRate;
    }

    void CGameSnd::SetGameListener()
    {
        CGame::GameWork &work = CGame::g_GameWork;
        long nListener = LISTENER_GAME;
        switch (work.m_nGameMode) {
        case CGame::GAME_MODE_BASKET:
            if (work.GetGameType() == 2) {
                nListener = LISTENER_PARTY_GAME + 0;
            } else {
                nListener = LISTENER_GAME + 0;
            }
            break;
        case CGame::GAME_MODE_VOLLEY:
            if (work.GetGameType() == 2) {
                nListener = LISTENER_PARTY_GAME + 1;
            } else {
                nListener = LISTENER_GAME + 1;
            }
            break;
        case CGame::GAME_MODE_DODGE:
            if (work.GetGameType() == 2) {
                nListener = LISTENER_PARTY_GAME + 2;
            } else {
                nListener = LISTENER_GAME + 2;
            }
            break;
        case CGame::GAME_MODE_HOCKEY:
            if (work.GetGameType() == 2) {
                nListener = LISTENER_PARTY_GAME + 3;
            } else {
                nListener = LISTENER_GAME + 3;
            }
            break;
        case CGame::GAME_MODE_BOSS:
            nListener = LISTENER_GAME + 4;
            break;
        }
        const LISTENER_PARAM &rParam = s_aListener[nListener];
        SetListener(rParam);
    }

    void CGameSnd::LoadGameSnd()
    {
        SetGameListener();

        GAME_INFO info(CGame::g_GameWork.GetGameMode(), CGame::g_GameWork.GetGameType());
        if (info.m_nGameMode == CGame::GAME_MODE_VOLLEY && info.m_nGameType == 2) {
            info.m_nPartyParam = CGame::GetPartyGameParam(0);
        }
        info.m_nStage = CGame::g_GameWork.m_nStage;
        for (long i = 0; i < 8; i++) {
            info.m_aChr[i] = CGame::GetCharType(i);
        }
        LoadGameGroups(info, 1);
    }

    unsigned long CGameSnd::LoadGameGroups(const GAME_INFO &rInfo, int bLoad)
    {
        bcs::CSndGroupList list;
        long nGameMode = rInfo.m_nGameMode;
        long nGameType = rInfo.m_nGameType;
        long nPartyParam = rInfo.m_nPartyParam;

        list.Add(6, 1);
        list.Add(7, 1);
        list.Add(8, 1);
        list.Add(9, 1);
        list.Add(10, 1);
        list.Add(11, 1);
        list.Add(12, 1);
        list.Add(13, 1);
        list.Add(15, 1);
        if (nGameType == 2) {
            list.Add(14, 1);
        }

        switch (nGameMode) {
        case CGame::GAME_MODE_VOLLEY:
            list.Add(0x16, 1);
            if (nPartyParam >= 0) {
                list.Add(s_aPartyGroup[nPartyParam], 1);
            }
            if (nGameType == 2) {
                list.Add(0x3D, 1);
            }
            break;
        case CGame::GAME_MODE_DODGE:
            list.Add(0x17, 1);
            if (nGameType == 2) {
                list.Add(0x40, 1);
            }
            break;
        case CGame::GAME_MODE_HOCKEY:
            list.Add(0x18, 1);
            if (nGameType == 2) {
                list.Add(0x3F, 1);
            }
            break;
        case CGame::GAME_MODE_BOSS:
            list.Add(0x16, 1);
            list.Add(0x17, 1);
            list.Add(0x18, 1);
            list.Add(0x15, 1);
            list.Add(0x41, 1);
            list.Add(0x42, 1);
            list.Add(0x43, 1);
            break;
        default:
            list.Add(0x15, 1);
            if (nGameType == 2) {
                list.Add(0x3E, 1);
            }
            break;
        }

        if (nGameMode != CGame::GAME_MODE_HOCKEY) {
            list.Add(0x19, 1);
            list.Add(0x1A, 1);
            list.Add(0x1B, 1);
            list.Add(0x1C, 1);
        } else {
            list.Add(0x1F, 1);
            list.Add(0x20, 1);
            list.Add(0x21, 1);
            list.Add(0x22, 1);
            list.Add(0x23, 1);
        }

        AddChrGroups(list, rInfo, 1);

        unsigned long nSize = bcs::CSnd::GetGroupListSize(list);
        if (GetGameType() == 0) {
            nSize += 0x80000;
        }
        if (bLoad) {
            bcs::CSnd::CreateHeap(SND_HEAP, nSize, 1, -0x20);
            bcs::CSnd::LoadGroupList(list);
        }
        return nSize;
    }

    void CGameSnd::AddChrGroups(bcs::CSndGroupList &rList, const GAME_INFO &rInfo, short nFlag)
    {
        short nStageGroup = s_aStageGroup[rInfo.m_nStage];
        if (nStageGroup >= 0) {
            rList.Add(nStageGroup, nFlag);
        }

        for (long i = 0; i < 8; i++) {
            long nChr = rInfo.m_aChr[i];
            if (nChr < 0) {
                continue;
            }

            short nGroup = s_aChrGroupA[nChr];
            if (nGroup >= 0) {
                rList.Add(nGroup, nFlag);
            }
            nGroup = s_aChrGroupB[nChr];
            if (nGroup >= 0) {
                rList.Add(nGroup, nFlag);
            }

            // Characters 0xC and 0x10 have one more group, another one in
            // hockey.
            if (nChr == 0xC) {
                if (rInfo.m_nGameMode != CGame::GAME_MODE_HOCKEY) {
                    rList.Add(0x1D, nFlag);
                } else {
                    rList.Add(0x24, nFlag);
                }
            }
            if (nChr == 0x10) {
                if (rInfo.m_nGameMode != CGame::GAME_MODE_HOCKEY) {
                    rList.Add(0x1E, nFlag);
                } else {
                    rList.Add(0x25, nFlag);
                }
            }
        }
    }

    void CGameSnd::SaveGameHeapState()
    {
        if (!(CGame::g_GameWork.m_nUnk40 & 0x100)) {
            s_nGameHeapState = bcs::CSnd::SaveHeapState(SND_HEAP);
        }
    }

    void CGameSnd::LoadCommonSnd()
    {
        bcs::CSndGroupList list;
        list.Add(3, 1);
        CreateHeapAndLoad(list, 0);
    }

    void CGameSnd::ReloadGameSnd()
    {
        LoadGameSnd();
    }

    void CGameSnd::SaveGameHeapStateAlways()
    {
        s_nGameHeapState = bcs::CSnd::SaveHeapState(SND_HEAP);
    }

    void CGameSnd::LoadChrSnd(long nPlayer)
    {
        bcs::CSndGroupList list;
        long nGroup = s_aChrGroupC[CGame::GetCharType(nPlayer)];
        if (bcs::CSnd::IsGroupLoaded(nGroup)) {
            return;
        }
        list.Add(nGroup, 1);
        bcs::CSnd::LoadHeapState(SND_HEAP, s_nGameHeapState);
        bcs::CSnd::LoadGroupList(list);
    }

    void CGameSnd::LoadMenuSnd()
    {
        SetListener(LISTENER_MENU);
        bcs::CSndGroupList list;
        list.Add(5, 1);
        CreateHeapAndLoad(list, 0x200000);
    }

    void CGameSnd::MenuSaveHeapState()
    {
        s_nMenuHeapState = bcs::CSnd::SaveHeapState(SND_HEAP);
    }

    void CGameSnd::MenuLoadSeq(unsigned long nSeq)
    {
        bcs::CSndGroupList list;
        long nGroup;
        if (nSeq == 0xBB7) {
            nGroup = 0;
        } else if (nSeq == 0xBB8) {
            nGroup = 1;
        } else if (nSeq == 0xBB9) {
            nGroup = 2;
        } else {
            return;
        }
        if (bcs::CSnd::IsGroupLoaded(nGroup)) {
            return;
        }
        list.Add(nGroup, 1);
        bcs::CSnd::LoadHeapState(SND_HEAP, s_nMenuHeapState);
        bcs::CSnd::LoadGroupList(list);
    }

    void CGameSnd::LoadTournamentSnd()
    {
        if (CTournament::IsSndStage()) {
            SetListener(s_aListener[LISTENER_TOURNAMENT_STAGE]);
        } else {
            SetListener(s_aListener[LISTENER_TOURNAMENT]);
        }

        bcs::CSndGroupList list;
        long nStage = -1;
        if (CTournament::IsSndStage()) {
            nStage = CTournament::GetSndStage();
        }

        list.Add(0x14, 1);
        list.Add(0x19, 1);
        list.Add(0x1D, 1);
        list.Add(0x1E, 1);
        if (nStage == -1) {
            list.Add(0x10, 1);
            list.Add(0x11, 1);
            list.Add(0x12, 1);
            list.Add(0x13, 1);
            long nUnk07 = CGame::g_GameWork.m_nUnk07;
            if (nUnk07 == 1) {
                list.Add(0x8D, 1);
            } else if (nUnk07 == 2) {
                list.Add(0x8E, 1);
            } else if (nUnk07 == 3) {
                list.Add(0x8F, 1);
            }
        } else if (nStage == 10) {
            list.Add(0x90, 1);
        } else if (nStage == 0x47) {
            list.Add(0x91, 1);
        } else if (nStage == 0x79) {
            list.Add(0x92, 1);
        } else if (nStage == 0x82) {
            list.Add(0x93, 1);
        } else if (nStage == 0x50 || nStage == 0x51) {
            list.Add(0x94, 1);
        } else if (nStage == 0x6E) {
            list.Add(0x95, 1);
        } else if (nStage == 0x83) {
            list.Add(0x96, 1);
        } else if (nStage == 0x8C) {
            list.Add(0x97, 1);
        } else if (nStage >= 1000) {
            list.Add(3, 1);
        } else {
            return;
        }
        CreateHeapAndLoad(list, 0);
    }

    void CGameSnd::DestroyHeap()
    {
        bcs::CSnd::DestroyHeap(SND_HEAP);
    }

    void CGameSnd::StopAllSound()
    {
        bcs::CSnd::StopAllSound();
    }

    void CGameSnd::GamePauseAll(int bPause, float fFadeFrames)
    {
        bcs::CSnd::PauseAll(bPause, fFadeFrames);
    }

    namespace {
        // Fade lengths, in frames.
        const float FADE_NONE = 0.0f;
        const float FADE_SLOW = 30.0f;

        // Music of each stage (indexed by GameWork::m_nStage) and its fast
        // version after the hurry-up jingle.
        short s_aStageBgm[100] = {
            0x04, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x04, 0x12,
            0x14, 0x16, 0x18, 0x04, 0x04, 0x1A, 0x1C, 0x1E, 0x04, 0x04,
            0x20, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
            0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x28, 0x28, 0x28, 0x28, 0x28,
            0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
            0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25,
            0x25, 0x25, 0x25, 0x25, 0x25, 0x23, 0x23, 0x23, 0x23, 0x23,
            0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
        };
        short s_aStageBgmFast[100] = {
            0x05, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x05, 0x13,
            0x15, 0x17, 0x19, 0x05, 0x05, 0x1B, 0x1D, 0x1F, 0x05, 0x05,
            0x21, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
            0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
            0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
            0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x29, 0x29, 0x29, 0x29, 0x29,
            0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
            0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27, 0x27,
            0x27, 0x27, 0x27, 0x27, 0x27, 0x24, 0x24, 0x24, 0x24, 0x24,
            0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
        };

        // Sequences (names guessed).
        enum {
            SEQ_JINGLE_2 = 0x2,
            SEQ_BONUS = 0x26,
            SEQ_JINGLE_37 = 0x37,
            SEQ_JINGLE_38 = 0x38,
            SEQ_MUSIC_3C = 0x3C,
            SEQ_STAR = 0x3E,
            SEQ_INTRO = 0xBBA,
            SEQ_HURRYUP = 0xBFD
        };
    }

    CGameBGM *CGameBGM::s_pInstance;

    inline bcs::CSndTask *CGameBGM::GetSnd(long nSnd)
    {
        return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_ahSnd[nSnd]));
    }

    inline void CGameBGM::StopSnd(long nSnd, float fFadeFrames)
    {
        bcs::CSndTask *pSnd = GetSnd(nSnd);
        if (pSnd != 0) {
            pSnd->Stop(fFadeFrames);
        }
        m_ahSnd[nSnd] = 0;
    }

    inline void CGameBGM::PlaySnd(long nSnd, long nSeq)
    {
        StopSnd(nSnd, FADE_NONE);
        m_ahSnd[nSnd] = CGameSnd::PlayBGM(nSeq, 1, 1.0f, 0.0f);
    }

    inline void CGameBGM::PauseSnd(long nSnd, int bPause, float fFadeFrames)
    {
        bcs::CSndTask *pSnd = GetSnd(nSnd);
        if (pSnd != 0) {
            pSnd->Pause(fFadeFrames, bPause);
        }
    }

    inline void CGameBGM::StopAllSnd(float fFadeFrames)
    {
        for (long i = 0; i < SND_NUM; i++) {
            StopSnd(i, fFadeFrames);
        }
    }

    void CGameBGM::Create()
    {
        if (s_pInstance == 0) {
            s_pInstance = new CGameBGM;
        }
    }

    long CGameBGM::GetStageBgm(long nStage, int bFast)
    {
        if (s_aStageBgm[nStage] < 0) {
            return -1;
        }

        CGame::GameWork &work = CGame::g_GameWork;
        if (work.m_nGameType == 2) {
            // Party games: basketball, dodgeball and hockey have their own
            // music when party parameter 0 is set.
            long nMode = work.m_nGameMode;
            if (nMode == CGame::GAME_MODE_BASKET) {
                if (CGame::GetPartyGameParam(0) != 0) {
                    return bFast == 0 ? 0x2E : 0x2F;
                }
            } else if (nMode == CGame::GAME_MODE_DODGE) {
                if (CGame::GetPartyGameParam(0) != 0) {
                    return bFast == 0 ? 0x30 : 0x31;
                }
            } else if (nMode == CGame::GAME_MODE_HOCKEY) {
                if (CGame::GetPartyGameParam(0) != 0) {
                    return bFast == 0 ? 0x2C : 0x2D;
                }
            }
        }

        long nBgm;
        if (bFast == 0) {
            nBgm = s_aStageBgm[nStage];
        } else {
            nBgm = s_aStageBgmFast[nStage];
        }
        return nBgm;
    }

    void CGameBGM::SetBgmState(long state)
    {
        if (s_pInstance == 0) {
            return;
        }

        long cur = GetBgmState();
        // States 4 and 5 neither restart themselves nor replace states 6 to
        // 8.
        if (state == 4) {
            if (cur == 4) {
                return;
            }
            if (cur == 6) {
                return;
            }
            if (cur == 7) {
                return;
            }
            if (cur == 8) {
                return;
            }
        }
        if (state == 5) {
            if (cur == 5) {
                return;
            }
            if (cur == 6) {
                return;
            }
            if (cur == 7) {
                return;
            }
            if (cur == 8) {
                return;
            }
        }
        s_pInstance->RequestState(state);
    }

    long CGameBGM::GetBgmState()
    {
        if (s_pInstance != 0) {
            return s_pInstance->m_nState;
        }
        return 0;
    }

    inline int CGameBGM::StartHurryUp()
    {
        // Only while the stage, star or bonus music plays.
        if ((unsigned long)(m_nState - STATE_PLAY) > STATE_BONUS - STATE_PLAY) {
            return 0;
        }
        if (m_bHurryUpLock != 0 || m_bWaitFastBgm != 0) {
            return 0;
        }

        PlaySnd(SND_HURRYUP, SEQ_HURRYUP);
        StopSnd(SND_MAIN, FADE_SLOW);
        PauseSnd(SND_STAR, 1, FADE_SLOW);
        PauseSnd(SND_BONUS, 1, FADE_SLOW);
        m_bWaitFastBgm = 1;
        m_bHurryUpLock = 1;
        return 1;
    }

    int CGameBGM::HurryUp()
    {
        if (s_pInstance != 0) {
            return s_pInstance->StartHurryUp();
        }
        return 0;
    }

    CGameBGM::CGameBGM()
        : CTask(0, 0x3E)
    {
        SetUnk08(0xFC01);
        m_ahSnd[SND_MAIN] = 0;
        m_ahSnd[SND_STAR] = 0;
        m_ahSnd[SND_BONUS] = 0;
        m_ahSnd[SND_JINGLE] = 0;
        m_ahSnd[SND_HURRYUP] = 0;
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;

        InitFsm(this, STATE_NUM);
        SetState(STATE_STOP, &CGameBGM::CtorStateStop, &CGameBGM::LoopStateStop, &CGameBGM::DtorStateStop);
        SetState(STATE_INTRO, &CGameBGM::CtorStateIntro, &CGameBGM::LoopStateIntro, &CGameBGM::DtorStateIntro);
        SetState(STATE_START, &CGameBGM::CtorStateStart, &CGameBGM::LoopStateStart, &CGameBGM::DtorStateStart);
        SetState(STATE_PLAY, &CGameBGM::CtorStatePlay, &CGameBGM::LoopStatePlay, &CGameBGM::DtorStatePlay);
        SetState(STATE_STAR, &CGameBGM::CtorStateStar, &CGameBGM::LoopStateStar, &CGameBGM::DtorStateStar);
        SetState(STATE_BONUS, &CGameBGM::CtorStateBonus, &CGameBGM::LoopStateBonus, &CGameBGM::DtorStateBonus);
        SetState(STATE_JINGLE_37, &CGameBGM::CtorStateJingle37, &CGameBGM::LoopStateJingle37,
                 &CGameBGM::DtorStateJingle37);
        SetState(STATE_JINGLE_38, &CGameBGM::CtorStateJingle38, &CGameBGM::LoopStateJingle38,
                 &CGameBGM::DtorStateJingle38);
        SetState(STATE_JINGLE_38B, &CGameBGM::CtorStateJingle38B, &CGameBGM::LoopStateJingle38B,
                 &CGameBGM::DtorStateJingle38B);
        SetState(STATE_JINGLE_2, &CGameBGM::CtorStateJingle2, &CGameBGM::LoopStateJingle2,
                 &CGameBGM::DtorStateJingle2);
        SetState(STATE_MUSIC_3C, &CGameBGM::CtorStateMusic3C, &CGameBGM::LoopStateMusic3C,
                 &CGameBGM::DtorStateMusic3C);
        SetState(STATE_FADE_OUT, &CGameBGM::CtorStateFadeOut, &CGameBGM::LoopStateFadeOut,
                 &CGameBGM::DtorStateFadeOut);
        SetState(STATE_CUT, &CGameBGM::CtorStateCut, &CGameBGM::LoopStateCut, &CGameBGM::DtorStateCut);
        ChangeState(STATE_STOP);
    }

    CGameBGM::~CGameBGM()
    {
        StopAllSnd(FADE_NONE);
    }

    void CGameBGM::Main()
    {
        ExecState();
    }

    void CGameBGM::CtorStateStop()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopAllSnd(FADE_NONE);
    }

    void CGameBGM::LoopStateStop() {}

    void CGameBGM::DtorStateStop() {}

    void CGameBGM::CtorStateIntro()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopAllSnd(FADE_NONE);
        PlaySnd(SND_MAIN, SEQ_INTRO);
        ChangeState(STATE_PLAY);
    }

    void CGameBGM::LoopStateIntro() {}

    void CGameBGM::DtorStateIntro() {}

    void CGameBGM::CtorStateStart()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 0;
        StopAllSnd(FADE_SLOW);
        PlaySnd(SND_MAIN, GetStageBgm(CGame::g_GameWork.m_nStage, 0));
        ChangeState(STATE_PLAY);
    }

    void CGameBGM::LoopStateStart() {}

    void CGameBGM::DtorStateStart() {}

    void CGameBGM::CtorStatePlay() {}

    void CGameBGM::LoopStatePlay()
    {
        // After the hurry-up jingle, the fast version of the stage music.
        if (m_bWaitFastBgm != 0 && GetSnd(SND_HURRYUP) == 0) {
            long nSeq = GetStageBgm(CGame::g_GameWork.m_nStage, 1);
            PlaySnd(SND_MAIN, nSeq);
            m_bWaitFastBgm = 0;
        }
    }

    void CGameBGM::DtorStatePlay() {}

    void CGameBGM::CtorStateStar()
    {
        PauseSnd(SND_MAIN, 1, FADE_SLOW);
        PlaySnd(SND_STAR, SEQ_STAR);
        // The hurry-up jingle has priority.
        if (m_bWaitFastBgm != 0) {
            PauseSnd(SND_STAR, 1, FADE_NONE);
        }
    }

    void CGameBGM::LoopStateStar()
    {
        int bEnd = 1;
        if (CChrMan::IsExistStarChr()) {
            bEnd = 0;
        }
        if (CBallMan::IsExistStarBall()) {
            bEnd = 0;
        }
        if (bEnd) {
            ChangeState(STATE_PLAY);
        } else if (m_bWaitFastBgm != 0 && GetSnd(SND_HURRYUP) == 0) {
            // The jingle has ended: the star music goes on, the fast stage
            // music waits paused under it.
            PauseSnd(SND_STAR, 0, FADE_SLOW);
            long nSeq = GetStageBgm(CGame::g_GameWork.m_nStage, 1);
            PlaySnd(SND_MAIN, nSeq);
            PauseSnd(SND_MAIN, 1, FADE_NONE);
            m_bWaitFastBgm = 0;
        }
    }

    void CGameBGM::DtorStateStar()
    {
        PauseSnd(SND_MAIN, 0, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
    }

    void CGameBGM::CtorStateBonus()
    {
        PauseSnd(SND_MAIN, 1, FADE_SLOW);
        PlaySnd(SND_BONUS, SEQ_BONUS);
        if (m_bWaitFastBgm != 0) {
            PauseSnd(SND_BONUS, 1, FADE_NONE);
        }
    }

    void CGameBGM::LoopStateBonus()
    {
        int bEnd = 1;
        if (CGame::IsNowBonusTime()) {
            bEnd = 0;
        }
        if (bEnd) {
            ChangeState(STATE_PLAY);
        } else if (m_bWaitFastBgm != 0 && GetSnd(SND_HURRYUP) == 0) {
            PauseSnd(SND_BONUS, 0, FADE_SLOW);
            long nSeq = GetStageBgm(CGame::g_GameWork.m_nStage, 1);
            PlaySnd(SND_MAIN, nSeq);
            PauseSnd(SND_MAIN, 1, FADE_NONE);
            m_bWaitFastBgm = 0;
        }
    }

    void CGameBGM::DtorStateBonus()
    {
        PauseSnd(SND_MAIN, 0, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
    }

    void CGameBGM::CtorStateJingle37()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_37);
    }

    void CGameBGM::LoopStateJingle37()
    {
        if (GetSnd(SND_JINGLE) == 0) {
            ChangeState(STATE_PLAY);
        }
    }

    void CGameBGM::DtorStateJingle37() {}

    void CGameBGM::CtorStateJingle38()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_38);
    }

    void CGameBGM::LoopStateJingle38()
    {
        if (GetSnd(SND_JINGLE) == 0) {
            ChangeState(STATE_PLAY);
        }
    }

    void CGameBGM::DtorStateJingle38() {}

    void CGameBGM::CtorStateJingle38B()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_38);
    }

    void CGameBGM::LoopStateJingle38B()
    {
        if (GetSnd(SND_JINGLE) == 0) {
            ChangeState(STATE_PLAY);
        }
    }

    void CGameBGM::DtorStateJingle38B() {}

    void CGameBGM::CtorStateJingle2()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        StopSnd(SND_MAIN, FADE_SLOW);
        StopSnd(SND_STAR, FADE_SLOW);
        StopSnd(SND_BONUS, FADE_SLOW);
        StopSnd(SND_HURRYUP, FADE_SLOW);
        PlaySnd(SND_JINGLE, SEQ_JINGLE_2);
    }

    void CGameBGM::LoopStateJingle2() {}

    void CGameBGM::DtorStateJingle2() {}

    void CGameBGM::CtorStateMusic3C()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 1;
        PlaySnd(SND_MAIN, SEQ_MUSIC_3C);
    }

    void CGameBGM::LoopStateMusic3C() {}

    void CGameBGM::DtorStateMusic3C() {}

    void CGameBGM::CtorStateFadeOut()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 0;
        StopAllSnd(FADE_SLOW);
    }

    void CGameBGM::LoopStateFadeOut() {}

    void CGameBGM::DtorStateFadeOut() {}

    void CGameBGM::CtorStateCut()
    {
        m_bWaitFastBgm = 0;
        m_bHurryUpLock = 0;
        StopAllSnd(FADE_NONE);
    }

    void CGameBGM::LoopStateCut() {}

    void CGameBGM::DtorStateCut() {}
}
