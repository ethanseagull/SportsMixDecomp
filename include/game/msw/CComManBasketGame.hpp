#ifndef MSW_CCOMMANBASKETGAME_HPP_
#define MSW_CCOMMANBASKETGAME_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStage10Base.hpp"

namespace msw {

    class CChrCore;
    class CComDrvBasketGame;
    class CQPanel;
    class CStgGmk_Case;

    // (name from the mangled names; meaning guessed) The members of a team
    // as CComManBasketGame::GetMemBer lists them, at most six, with a score
    // per member that the computer driver fills in (CalcHoldBallScore).
    struct MEMBER_INFO {
        long m_aScore[6];             // 0x00
        const CChrCore *m_apChr[6];   // 0x18
    };

    // The computer players' manager of the normal basketball game (BasketF,
    // game/basket/CComManBasketGame.cpp). Size 0x20 (CreateInstance). It
    // derives from CComMan directly, as CComManVolleyGame does: the
    // original emits no CComManBasket vtable. Most functions are static:
    // the helpers the computer drivers ask about the court, the stage
    // gimmicks and the team members. Return types and parameter meanings
    // are guesses unless a caller shows them.
    class CComManBasketGame : public CComMan {
        public:
            static void CreateInstance();

            // Fills ppChr (nMax entries) with the characters of team nTeam
            // other than pIgnore, of driver type nDrvType (-1: any); returns
            // how many were found.
            static long GetMemBer(long nTeam, CChrCore **ppChr, long nMax, long nDrvType, const CChrCore *pIgnore);
            static long GetMemBer(long nTeam, const CChrCore **ppChr, long nMax, long nDrvType,
                                  const CChrCore *pIgnore);
            static long GetMemBer(long nTeam, MEMBER_INFO &rInfo, long nDrvType, const CChrCore *pIgnore);
            // The same, leaving out the characters in state 6 (a bad
            // condition).
            static long GetMemberNoBadCondition(long nTeam, const CChrCore **ppChr, long nMax, long nDrvType,
                                                const CChrCore *pIgnore);
            // The member of team nTeam nearest to the goal it guards that
            // can jump to block; null for none.
            static CChrCore *GetUrgentGuardCore(long nTeam, long nDrvType, const CChrCore *pIgnore);
            // pChr's computer driver; null when pChr is not computer-driven.
            static CComDrvBasketGame *GetComDrvBK(const CChrCore *pChr);
            // Not zero when rPos lies on the court (short of the invisible
            // wall at the view, if any).
            static int InMoveEnableArea(const bcs::FVEC3 &rPos);
            // Brings rPos onto the court when the court has an invisible
            // wall at the view.
            static void NormalizeCanMovePosToInvisibleWall(bcs::FVEC3 &rPos);
            // A score from the time left and the shot clock, between two
            // level parameters.
            static long GetBaseShootPoint();
            // The dunk frame rate of pChr's computer driver, 1 without one.
            static float GetDunkFrameRate(const CChrCore *pChr);
            // Stage 2 (the beach): the falling green shell nearest to rPos,
            // whether shell nId can no longer be taken, and its position.
            static unsigned long GetNBeachNearFallGShell(const bcs::FVEC3 &rPos);
            static int IsNBeachFallGSellDisable(unsigned long nId);
            static bcs::FVEC3 GetNBeachFallGShellPos(unsigned long nId);
            // Stage 7: the boss Packun's mud bombs.
            static CNpcBossPackun::CMudBomb *GetMudPtr(unsigned long nId);
            static const CNpcBossPackun::CMudBomb *GetMudFirst();
            static const CNpcBossPackun::CMudBomb *GetMudNext(const CNpcBossPackun::CMudBomb *pBomb);
            // The arrow panels among the "?" panels, and whether pPanel
            // points to the goal team nTeam attacks.
            static CQPanel *ArrowPanel_GetFirst();
            static CQPanel *ArrowPanel_GetNext(CQPanel *pPanel);
            static int IsTeamTargetDirArrowPanel(long nTeam, CQPanel *pPanel);
            // Stage 10 (the street): the panel a character stands on, the
            // panels, and the bonus time.
            static CStage10Base::CPanelInfoBase *GetJStreetPanelRide();
            static CStage10Base::CPanelInfoBase *GetJStreetPanelFirst();
            static CStage10Base::CPanelInfoBase *GetJStreetPanelNext(const CStage10Base::CPanelInfoBase *pPanel);
            static int IsJStreetBonus();
            // Stage 6: tells the computer drivers that the ghost caught a
            // shot.
            static void MsgShootCatchToGhost();
            // Stage 15: the cases.
            static long GetGmkCaseNum();
            static const CStgGmk_Case *GetGmkCase(long nIdx);
            static const CStgGmk_Case *GetGmkCasePtr(unsigned long nId);
            // Stage 17: whether the move from rFrom to rTo runs into the
            // train (rOut: where to stop), and whether the segment rPos0 to
            // rPos1 crosses the train's line.
            static int HitCheckTrain(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo);
            static int IsTrainCrossLine(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1);

            virtual ~CComManBasketGame();
            virtual void Clear();
            virtual void Main(float fFrame);

            // The computer level of team nTeam, and the same raised for a
            // computer team that trails.
            float GetBaseComLvF_(long nTeam) const;
            float GetComLvF_(long nTeam) const;
            // Not zero when pChr is behind the ball holder (m_nOnBallerBackBits)
            // or the holder has broken past it (m_nDefBreakthroughBits).
            int IsThanOnBallerBack_(const CChrCore *pChr) const;
            int IsDefBreakthrough_(const CChrCore *pChr) const;
            // Whether team nTeam may call a defender to the ball holder.
            long CanbeActCallGoDefOnBaller_(long nTeam) const;
            // Not zero the first time pChr asks after its dash ended.
            int CheckDashToTrickMoveActInter_(const CChrCore *pChr);
            // Whether a computer of team nTeam may shoot now.
            int CanbeShootState_(long nTeam) const;

        private:
            // Bits by player number (names guessed from the getters).
            unsigned long m_nDefBreakthroughBits;  // 0x04
            unsigned long m_nOnBallerBackBits;     // 0x08
            unsigned long m_nDashToTrickBits;      // 0x0C
            long m_bGoDefOnBaller[2];              // 0x10, per team
            float m_fGoDefTimer[2];                // 0x18, per team, frames to the next draw
    };
}

#endif /* !MSW_CCOMMANBASKETGAME_HPP_ */
