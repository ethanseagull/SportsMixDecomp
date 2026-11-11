#ifndef MSW_CFIGHTING_HPP_
#define MSW_CFIGHTING_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // The state of a fight over the puck in the hockey game (HockeyF
    // game/hockey/CChrManHockey.cpp), 0x84 bytes. CFighting keeps it at 0x34
    // and CChrManHockey::GetFightingWork_ hands out a copy. The copy moves
    // the members one by one (floats through float registers, the arrays as
    // words), which the compiler does for a struct with a constructor of its
    // own. Names are guesses from the use.
    struct FIGHTING_WORK {
        FIGHTING_WORK() {}

        float m_fTime;        // 0x00, frames left in the current phase
        long m_Unk04;         // 0x04, only cleared
        long m_bJoin[8];      // 0x08, per character: taking part in the fight
        long m_nChr2;         // 0x28, the second character that started it
        long m_bActive;       // 0x2C, a fight is on
        long m_bCanJoin;      // 0x30, characters near the puck may join
        float m_fPower[8];    // 0x34, per character: the shaking it has put in
        float m_fWinPoint;    // 0x54, the power a team needs to win
        long m_nState;        // 0x58, see CFighting::MainFunc
        long m_nFirst;        // 0x5C, the first character to act in phase 2, -1 for none
        long m_nChr1;         // 0x60, the first character that started it
        float m_fWinMotFrm;   // 0x64, frame of the winning motion to wait for
        long m_nWinner;       // 0x68, the winning character, -1 for none
        // 0x6C (meaning not known): only ever cleared here; when it is not
        // zero the fight goes on to phase 2 instead of ending.
        long m_Unk6C;
        long m_nRound;        // 0x70, how many times the fight was restarted
        float m_fJoinWait;    // 0x74, frames before characters may join
        float m_fCallWait;    // 0x78, frames before phase 2's cockpit call
        float m_fEndWait;     // 0x7C, frames before the fight may end
        long m_bShakeIcon;    // 0x80, the "shake" icon is shown
    };

    // The fight over the puck: a task that CChrManHockey::CreateMarker_
    // makes (0xB8 bytes, priority 0x19) and keeps at 0xCC. Characters near
    // the puck join; each team's shaking (the per-character power) fills its
    // gauge, and the first team past m_fWinPoint wins it.
    class CFighting : public bcs::CTask, public FIGHTING_WORK {
        public:
            // (guessed inline: CreateMarker_ builds it in place)
            CFighting(bcs::CTask *pParent) : bcs::CTask(pParent, 0x19) { Reset(); }
            virtual ~CFighting();
            virtual void Main();
            virtual void MainFunc(float fRate);
            virtual void UpdateFunc(float fRate);

            // Ends any fight and clears the cockpit's fight display.
            void Reset();
            // The frame of character nChr's winning motion to wait for.
            float GetNowWinMotFrm(long nChr);
            // Shows the "shake" icon while a human player takes part.
            void CheckbShakeIconDisp();
            // Not zero when the fight must end (a fighter is hit or too far
            // from the puck).
            int CheckEndFighting();
            // Lets the characters near the puck join the fight.
            void CheckFightingStart();
    };
}

#endif /* !MSW_CFIGHTING_HPP_ */
