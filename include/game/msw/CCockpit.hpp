#ifndef CCOCKPIT_HPP_
#define CCOCKPIT_HPP_

#include "game/msw/CMenuTask.hpp"

struct _GXTexObj;

namespace bcs {
    struct FVEC3;
}

namespace nw4r {
    namespace lyt {
        struct Size;
    }
}

namespace msw {
    class CCockpitLayoutBase;
    class CCockpitPointer;
    class CCockpitResult;
    class CCockpitResultPartyVS;
    class CCockpitResultPartyTeam;

    // The task that owns the in-game displays. Lives in the DOL; vptr at 0.
    // The virtual functions are declared in the order of its vtable
    // (lbl_804710C0): bcs::CTask's four, then thirteen of its own. Those
    // with no name in the DOL are placeholders.
    class CCockpit : public CMenuTask {
        public:
            CCockpit(bcs::CTask *pParent);
            virtual ~CCockpit();
            virtual void Main();
            virtual void Disp();

            // Creates display object nObjId; the result of CMenuTask::AddObj.
            virtual long CreateObjImpl(long nObjId, long nParam) = 0;
            virtual void SetTeamScoreCoder(long, long);
            virtual void AddTeamScoreCoder(long, long);
            virtual void SetTeamScoreColorCoder(long, int);
            virtual void SetTeamCoinCoder(long, long);
            virtual void AddTeamCoinCoder(long, long, long);
            // The next four are inline: each module that derives a class
            // emits them (the DOL's copies have no name). Return types and
            // what they return a pointer to are guesses.
            virtual void SetIconFaceImpl() {}
            virtual void SetSPGaugeDispEnableImpl(int) {}
            virtual long GetCoinEffectTarget(long, bcs::FVEC3 &, nw4r::lyt::Size &) { return -1; }
            virtual bcs::CTask *Get3DCoinTaskImpl(long) { return 0; }
            virtual void SetItemRouletteStateImpl(long, long, long);
            // The archive that holds layout file nFile (0 to 7).
            virtual const char *GetArchiveFileName(long nFile);
            virtual const char *GetArchiveRootFolderName(long nFile);
            // Pure here (null in the DOL's vtable); a derived class also
            // derives bcs::CFsm and forwards to its ChangeState.
            virtual long ChangeState(long nState) = 0;

            // Looks the task up in the task manager by s_TaskHandle; null
            // when there is none.
            static CCockpit *GetTask();
            // (DOL, static; added for BossF game/boss/GameBoss.cpp; argument
            // meanings guessed) The numbers of the result screen: periods
            // to show, the current period, then six arrays of period
            // values (the boss game passes each team's score per period and
            // which team won each), and a last word (-1).
            static void SetResultParameter(long nPeriodNum, long nPeriod, long *, long *, long *, long *, long *,
                                           long *, long);
            // (return type guessed) Removes the cockpit task, if there is one.
            static void Remove();

            // All static: each loads the task from s_TaskHandle itself.
            static long CreateObj(long nObjId, long nParam); // -1 without a task
            static long RemoveObj(long nObjId);               // -1 without a task
            static long IsEndObj(long nObjId);                // 1 without a task
            static void SetAnimEndClose(long nObjId, int bClose); // return type guessed
            // The word at 0x4C + 4 * nFile of the task (nFile 0 to 7); null
            // without a task or for another nFile.
            static const char *GetResourceFileName(long nFile);
            static void PlayEndAnim(long nObjId);                 // return type guessed
            // (DOL, static; return type guessed) Resets the characters' face
            // icons (HockeyF's CChrManHockey::AllReset_).
            static void InitFaceIcon();
            // (added for DOL game/msw/CGame.cpp; static, return types
            // guessed) Shows nCoin coins on team nTeam's counter; whether
            // telop nObjId has played its start animation; shows or hides
            // the special gauges.
            static void SetTeamCoin(long nTeam, long nCoin);
            static int IsEndOfStartAnim(long nObjId);
            static void SetSPGaugeDispEnable(int bEnable);
            // CMenuTask::SendObjEventTrigger on the task; -1 without a task.
            static long SendObjEventTrigger(long nObjId, long nEvent, void *pParam);
            // (added for MenuF game/menu/CWiFiFrindVS.cpp) The task's result
            // word (m_Result); static like the above. The friend match
            // screen waits for 1000.
            static long GetResult();
            // (name and argument guessed; DOL fn_8013A534, static) Closes
            // the mini telop of every split view (objects 0x100D on).
            // msw::CStage10Base::CTelopDispBase::Erase passes 1.
            static void CloseTelopMiniPos(int bUnk);
            // (DOL; added for BasketF game/basket/CStageBasket10.cpp; return
            // type guessed) Shows the points icon nKind for team nTeam at *pPos.
            static void SetShootPtsIcon(long nTeam, const bcs::FVEC3 *pPos, long nKind);
            // (DOL, static; added for BasketF game/basket/CChrManBasket.cpp;
            // parameter meanings guessed) The head icon of human player
            // nCtrlPlayer over player nPlayer: at *pPos plus *pOfs, with the
            // texture pTex (null: the default); and its removal.
            static void SetChrHeadIcon(long nCtrlPlayer, long nPlayer, const bcs::FVEC3 *pOfs, long,
                                       const bcs::FVEC3 *pPos, const _GXTexObj *pTex);
            static void ClearChrHeadIcon(long nCtrlPlayer, long nPlayer, int);
            // (DOL, static; added for BasketF
            // game/basket/CGameBasketHungryBossPackun.cpp; parameter meanings
            // guessed) The party game's result screen: nNum players with
            // their scores, (optional) times, teams and ranks; bDiffRank is
            // set when not every player has the same rank.
            static void SetResultPartyVSParameter(long nNum, long *pScore, float *pTime, long *pTeam, long *pRank,
                                                  int bDiffRank);
            // (name guessed; DOL fn_80137A34, not inline) The result word
            // at 0x70.
            long GetUnk70() const;
            // (names guessed; inline, no symbol; added for
            // game/msw/CCokcpitMenuDataList.cpp) Set the result words:
            // SetResults both of them, like the static ClearResult does on
            // the task; SetUnk70 the one at 0x70 alone.
            void SetResults(long nResult)
            {
                m_Unk70 = nResult;
                m_Result = nResult;
            }
            void SetUnk70(long nValue) { m_Unk70 = nValue; }
            // (added for DodgeF game/dodge/CCockpitDodge.cpp; static, return
            // type and argument names guessed) Places mini telop nObjId (a
            // CCockpitTelopMiniPos) at world position rPos as seen by camera
            // nCamNo.
            static long CreateTelopMiniPos(long nObjId, unsigned char nCamNo, const bcs::FVEC3 &rPos);

            // (added for DOL game/msw/CCockpitMenuBase.cpp; static, return
            // types guessed) Set both result words of the task to nResult.
            static void ClearResult(long nResult);
            // The pointer (cursor) display, object 0x1016: created with a
            // sort z, removed, and found (DOL fn_8013A928, name
            // guessed; null without a task or the object).
            static void CreatePointerObj(float fSortZ);
            static void RemovePointerObj();
            static CCockpitPointer *GetPointerObj();
            // (added for DOL game/msw/CGameGuideMan.cpp; names and argument
            // meanings guessed) The guide telop object nObjId (0x1001 or
            // 0x1002) closes; the line message box (object 0x1015) shows
            // the state nMode (0 to 3) and the text pText.
            static void CloseGuideTelop(long nObjId, int);
            static void SetLineMsgMode(int nMode);
            static void SetLineMsgText(const wchar_t *pText);
            // (DOL fn_8013A688; name, argument meanings and
            // return type guessed) Asks pPointer for pad nPad's pointer
            // position and, when it is over a button of pMenu that
            // OnCursorCheck accepts, marks the pad's pointer as in use.
            // Returns 1 then, else 0.
            static int PointerCursorCheck(long nPad, CCockpitLayoutBase *pMenu, CCockpitPointer *pPointer);
            // (name guessed; inline, no symbol) Makes the word at 0x70 the
            // result (CCockpitMenuBase's buttons, when one is decided).
            void CommitResult() { m_Result = m_Unk70; }

            // Id of the cockpit task, set by the constructor; 0 when there
            // is none.
            // (added for TourF game/tour/CTournamentStep.cpp) The id for a
            // new menu object.
            long GetNextCockpitMenuObjId();

            static unsigned long s_TaskHandle;

            // (added for BasketF game/basket/CCockpitBasket.cpp; return
            // types and argument names guessed) What every player of each
            // team played, for GetResultMemberData: nUnk is passed through,
            // then up to three characters per team.
            struct ResultMemberInfo {
                long nUnk;          // 0x00
                long aChr[2][3];    // 0x04
            };

            // (added for BasketF game/basket/CCockpitBasket.cpp; return types
            // guessed) Shows or hides the displays this class owns.
            void SetDisp(int bDisp);
            // The time left on the result screen's countdown (frames).
            float CalcTimeCount();
            void CreateDemoTelop();
            void RemoveDemoTelop();
            // Fill the result displays: the team results (bUnk: whether
            // the cockpit is in its first result state, CCockpitBasket's
            // state 4; pTotal gets the two teams' totals), the players, the
            // party VS result.
            void GetResultData(CCockpitResult *pResult, int bUnk, long *pTotal);
            void GetResultMemberData(ResultMemberInfo *pInfo);
            void GetResultPartyVSParameter(CCockpitResultPartyVS *pResult);
            // (added for VolleyF game/volley/CCockpitVolley.cpp) The same for
            // the team party result.
            void GetResultPartyTeamParameter(CCockpitResultPartyTeam *pResult);

        protected:
            // Runs the parts every cockpit shares, at the start of Main.
            void MainLoopCommon();
            // Creates the display objects every cockpit knows; null for an
            // unknown nObjId.
            CMenuLinkBase *CreateObjCommonImpl(long nObjId, long nParam);

        protected:
            // 0x44: CMenuTask ends here (0x44 bytes). msw::CCockpitTournament's
            // constructor clears this word. msw::CCockpitBasket's result
            // states step through it (0, 1, 2; compared signed).
            long m_Unk44;

            // 0x48: set to 2 by msw::CCockpitBasket::GamePartyResult_Loop
            // once the party result has closed (meaning not known).
            long m_Unk48;

        private:
            // Not mapped. From the constructor in the DOL: 0x4C holds the
            // eight archive paths (CLoadData::GetFilePath), 0x6C a short,
            // 0x1CC and 0x1D0 words. The size is where
            // CCockpitTournament's bcs::CFsm base starts.
            unsigned char m_Unk4C[0x70 - 0x4C];

        protected:
            // 0x70 and 0x74: the result words. ClearResult(n) sets both to
            // n, GetResult returns 0x74, an unnamed DOL getter returns 0x70.
            // msw::CCockpitMenu's constructor sets both to 0x10000.
            long m_Unk70;
            long m_Result; // 0x74

        public:
            // 0x78 to 0xBF: the score of each period of the match (names
            // guessed). GetResultData sums the period scores of each team;
            // the pause score boards (BasketF CCockpitBasketPause::
            // ScoreBoard_Loop) read all three members.
            long m_nPeriod;            // 0x78, the period being played (guess)
            long m_nPeriodNum;         // 0x7C, the number of periods (at most 8)
            long m_aPeriodScore[2][8]; // 0x80, per team, per period
            // 0xC0 and 0x100: two more per-team, per-period tables, read
            // only by the pause score boards (DodgeF and VolleyF
            // SetCountPoint, VolleyF SetPerfectIcon). Names guessed.
            long m_aPeriodPoint[2][8];   // 0xC0
            long m_aPeriodPerfect[2][8]; // 0x100

        private:
            unsigned char m_Unk140[0x1C8 - 0x140];

        protected:
            // 0x1C8: the result screen's countdown in frames
            // (msw::CCockpitBasket::GameResult_Loop sets it; name guessed).
            float m_fTimeCount;

        private:
            unsigned char m_Unk1CC[0x1D4 - 0x1CC];
    };
}

#endif /* !CCOCKPIT_HPP_ */
