#ifndef CTOURNAMENTEXIT_HPP_
#define CTOURNAMENTEXIT_HPP_

#include "bcs/CTask.hpp"
#include "game/msw/CCockpitButtonInfo.hpp"

namespace msw {

    class CTournamentStep;

    // The "leave the tournament" menu of the map (module TourF, defined in
    // game/tour/CTournamentStep.cpp): a task that opens the pause window
    // when its pad presses the button, and asks the map step to end the
    // tournament when the player chooses so. 0x54 bytes (CTournamentStep::
    // CtorStateLoadEnd makes one). Member names guessed.
    class CTournamentExit : public bcs::CTask {
        public:
            CTournamentExit(CTournamentStep *pStep, long nPad);
            virtual ~CTournamentExit();
            virtual void Main();
            virtual void Disp();

            void MenuOutWindow();
            void RemoveWindow();
            int IsMenuOutWindow();
            void CreateButtonWindow2(const CCockpitButtonInfo::ButtonInfo *pInfo, const char *pFile,
                                     const char *pLabel);

            // (meaning guessed) Whether the menu may be opened; TourF's
            // Stage33 clears it while the map fades out for an airship
            // flight and sets it again afterwards. A byte.
            static bool m_bEnable;

        private:
            void _CheckResult(int bCancel);
            int _CanbeExit();
            int _InputCheck();
            void _PauseStart();
            void _PauseEnd();
            int SetState(long nState);

            CTournamentStep *m_pStep; // 0x34
            float m_fWait;            // 0x38
            long m_nWindow;           // 0x3C: cockpit object of the open window, -1 for none
            long m_nOutWindow;        // 0x40: the window that is closing, -1 for none
            long m_nPad;              // 0x44: the pad buttons that open the menu, -1 for none
            long m_nResult;           // 0x48: the cockpit's result
            long m_nPushNum;          // 0x4C: menu data pushed to CCokcpitMenuDataList
            signed char m_nState;     // 0x50: the window shown (SetState)
            signed char m_nStep;      // 0x51
            signed char m_bPause;     // 0x52: the game is paused by the menu
    }; // size 0x54
}

#endif /* !CTOURNAMENTEXIT_HPP_ */
