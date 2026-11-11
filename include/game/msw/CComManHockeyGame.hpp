#ifndef MSW_CCOMMANHOCKEYGAME_HPP_
#define MSW_CCOMMANHOCKEYGAME_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CComManHockey.hpp"

namespace msw {

    class CChrCore;

    // Helpers of the computer players in the hockey game (HockeyF,
    // game/hockey/CComManHockeyGame.cpp). All static; whether the class
    // derives from CComManHockey is a guess. Return types and argument
    // meanings are guesses unless a body shows them.
    class CComManHockeyGame : public CComManHockey {
        public:
            // The player number of another character, of those whose bit
            // is set in nChrBits, that stands in the sector from pChr
            // towards rTarget (fWidth, fLength) within fAngle of the
            // direction to rTarget; -1 for none. The first takes rTarget
            // in the court's frame, the second in the world.
            static long IsExistChrToTargetHockey(const CChrCore *pChr, float fAngle, const bcs::FVEC3 &rTarget,
                                                 float fWidth, float fLength, unsigned long nChrBits);
            static long IsExistChrToTargetWorldHockey(const CChrCore *pChr, float fAngle,
                                                      const bcs::FVEC3 &rTarget, float fWidth, float fLength,
                                                      unsigned long nChrBits);
            // Whether the "?" panel task nTask is gone or cannot be taken.
            static int IsQPanelDisable(unsigned long nTask);
            // The direction (radians, 0 to 2 pi) from pChr to the ball, both
            // fTime frames ahead; false without a ball in play.
            static int GetAngleFromBall(const CChrCore *pChr, float fTime, float &rAngle);
            // The player number of the opponent pChr should mark, -1 for
            // none (keepers left out).
            static long DefenseShouldGetCharaCoreIndex(const CChrCore *pChr);
            // The nearest "?" panel of kind nKind (0 to 2), its distance from
            // pos in the XZ plane (-1 for none) and its position; 0 for none.
            static unsigned long GetExisQPanelDist(long nKind, bcs::FVEC3 pos, float &rDist,
                                                   bcs::FVEC3 &rPanelPos);
            // The same for the nearest falling green shell.
            static unsigned long GetExisGreenShellDist(bcs::FVEC3 pos, float &rDist, bcs::FVEC3 &rShellPos);
            // Whether stage 10's fever time is on (false on other stages).
            static int IsNowFeverTime();
            // On stage 10: whether a panel of side nSide (0: z <= -8, else
            // z >= 8) lies within fMaxDist of pos, with the nearest one's
            // position and radius; with nMode 1 only panels that score.
            static int isExistScorePanel(long nMode, long nSide, bcs::FVEC3 pos, float fMaxDist,
                                         bcs::FVEC3 &rPanelPos, float &rRadius);
            static int GetPanelMaxNum_Stage10(long nMode, long nSide, bcs::FVEC3 pos, float fMaxDist,
                                              bcs::FVEC3 &rPanelPos, float &rRadius);
            // On stage 10: 1 when pos is on a panel that scores, 0 on one
            // that does not, -1 on none (or another stage).
            static long isExistScorePanelNowPos(bcs::FVEC3 pos);
    };
}

#endif /* !MSW_CCOMMANHOCKEYGAME_HPP_ */
