#ifndef MSW_CGAMESND_HPP_
#define MSW_CGAMESND_HPP_

#include "bcs/CLoadMan.hpp"

namespace bcs {
    struct FVEC3;
    class CSndGroupList;
}

namespace msw {

    // The sound interface of the games. Lives in the DOL; everything is
    // static. Names without a symbol in the DOL are guessed and say so.
    class CGameSnd {
        public:
            // Plays sound nSe, at *pPos when pPos is not null. What the other
            // arguments mean is not known; the callers seen so far pass
            // (nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f). Returns the id of the sound
            // task (msw::menu::CSndEff::Play keeps it for CTaskMan::GetTask).
            static unsigned long Play(long nSe, const bcs::FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                                      float fArg5, float fArg6);
            // The same with the position by reference (a second function in
            // the DOL).
            static unsigned long Play(long nSe, const bcs::FVEC3 &rPos, unsigned long nArg2, long nArg3, float fArg4,
                                      float fArg5, float fArg6);
            // Starts music nSeq; returns the id of its sound task.
            // msw::menu::CSndBGM passes (nSeq, 1, 1.0f, 0.0f). (argument
            // meanings not known)
            static unsigned long PlayBGM(long nSeq, unsigned long nArg1, float fArg2, float fArg3);

            // (name guessed; fn_80146548) Clears the voice slots and music
            // handles. Called once at boot, before CGameBGM::Create.
            static void Init();

            // (names guessed; fn_8014660C, fn_801466E8) Sound nKind/nVar of
            // the character-dependent sets: characters 0xC and 0x10 have
            // their own, the others share one (the first function has a
            // second set for the large characters, and halves the volume in
            // the volleyball party game).
            static unsigned long PlayChrSeA(long nChr, long nKind, long nVar, const bcs::FVEC3 &rPos,
                                            unsigned long nArg2, long nArg3, float fArg4, float fVolume, float fArg6);
            static unsigned long PlayChrSeB(long nChr, long nKind, long nVar, const bcs::FVEC3 &rPos,
                                            unsigned long nArg2, long nArg3, float fArg4, float fVolume, float fArg6);

            // (names guessed; fn_80146754, fn_801467B8) Plays voice nVoice of
            // character nChr in voice slot nSlot (0-15), replacing what the
            // slot played. Voices from 0x12 on have up to four variations,
            // picked with nRand (a random number in the first form).
            static void PlayVoice(long nSlot, long nChr, long nVoice, const bcs::FVEC3 &rPos, unsigned long nArg2,
                                  long nArg3);
            static void PlayVoice(long nSlot, long nChr, long nVoice, const bcs::FVEC3 &rPos, unsigned long nArg2,
                                  long nArg3, long nRand);
            // (names guessed; fn_8014691C, fn_80146968, fn_801469A4,
            // fn_80146A0C) The voice slots.
            static void StopVoice(long nSlot, float fFadeFrames);
            static int IsVoicePlaying(long nSlot);
            static void SetVoiceVolume(long nSlot, float fVolume, float fFadeFrames);
            static void ResetVoiceVolume(float fFadeFrames);

            // Plays the up to four sounds of action nAct of character nChr;
            // returns how many were started. The second form (fn_80146AF4,
            // unnamed) keeps their task ids in pIds.
            static long PlayWrapAct(long nChr, long nAct, const bcs::FVEC3 &rPos, unsigned long nArg2, long nArg3,
                                    float fArg4, float fVolume);
            static long PlayWrapAct(unsigned long *pIds, long nChr, long nAct, const bcs::FVEC3 &rPos,
                                    unsigned long nArg2, long nArg3, float fArg4, float fVolume);

            // (name guessed; fn_80146BF0) Marks the music files
            // aFile[nStart] to aFile[nEnd - 1] of a load table to be skipped;
            // in the volleyball party game the first is loaded after all,
            // as the music its party parameter selects.
            static void SetMusicFiles(bcs::CLoadMan::FILE_INPUT *aFile, long nStart, long nEnd);

            // The three tracks of a game's music: prepared, started together,
            // stopped together.
            static int PreparePlayMusic(long nMusic);
            static int StartPreparedMusic();
            static void StopMusic(float fFadeFrames);
            static void SetMusicVolume(long nTrack, float fVolume, float fFadeFrames);
            // Position of the first track, in frames; -1 without music.
            static float GetMusicFrame();

            // (name guessed; fn_80147278) Loads the sounds of the game
            // CGame::g_GameWork describes.
            static void LoadGameSnd();
            // (names guessed; fn_80147A7C, fn_80147B74) Records the sound
            // heap level the character sounds are loaded over (the first
            // only when bit 8 of GameWork word 0x40 is clear).
            static void SaveGameHeapState();
            static void SaveGameHeapStateAlways();
            // (name guessed; fn_80147AB8) Loads sound group 3.
            static void LoadCommonSnd();
            // (name guessed; fn_80147B70) Only calls LoadGameSnd.
            static void ReloadGameSnd();
            // (name guessed; fn_80147B9C) Loads the sounds of player
            // nPlayer's character over the saved heap level.
            static void LoadChrSnd(long nPlayer);
            // (name guessed; fn_80147C6C) Loads the menu's sounds.
            static void LoadMenuSnd();
            static void MenuSaveHeapState();
            // (name guessed; fn_80147D98, no symbol) Loads menu music
            // 0xBB7..0xBB9 into the sound heap; other numbers do nothing.
            static void MenuLoadSeq(unsigned long nSeq);
            // (name guessed; fn_80147E88) Loads the tournament's sounds.
            static void LoadTournamentSnd();
            // (names guessed; fn_801488DC, fn_801488E4)
            static void DestroyHeap();
            static void StopAllSound();
            // Empty in this build.
            static void GamePauseAll(int bPause, float fFadeFrames);

        private:
            // What LoadGameGroups needs to know of a game (names guessed).
            struct GAME_INFO {
                GAME_INFO(long nGameMode, long nGameType)
                    : m_nGameMode(nGameMode), m_nGameType(nGameType), m_nPartyParam(-1), m_nStage(0)
                {
                    for (long i = 0; i < 8; i++) {
                        m_aChr[i] = -1;
                    }
                }

                long m_nGameMode;   // 0x00, GameWork::m_nGameMode
                long m_nGameType;   // 0x04, GameWork::m_nGameType
                long m_nPartyParam; // 0x08, party parameter 0 in the volleyball party game
                long m_nStage;      // 0x0C
                long m_aChr[8];     // 0x10, CGame::GetCharType of each player
            };

            // (name guessed; fn_801466D8) GameWork::m_nGameType.
            static long GetGameType();
            // (name guessed; fn_80147160) The 3D listener settings of the
            // running game.
            static void SetGameListener();
            // (names guessed; fn_8014734C, fn_80147714) Collects the sound
            // groups of a game and, when bLoad is set, loads them into a new
            // heap; returns the heap size.
            static unsigned long LoadGameGroups(const GAME_INFO &rInfo, int bLoad);
            static void AddChrGroups(bcs::CSndGroupList &rList, const GAME_INFO &rInfo, short nFlag);
    };
}

#endif /* !MSW_CGAMESND_HPP_ */
