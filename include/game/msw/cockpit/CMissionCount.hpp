#ifndef MSW_COCKPIT_CMISSIONCOUNT_HPP_
#define MSW_COCKPIT_CMISSIONCOUNT_HPP_

#include "nw4r/ut/List.hpp"

namespace msw {

    class CMenuPaneAccessor;

    namespace cockpit {

        // The cockpit's mission counter ("n / m"): two text boxes that count
        // up with an animation and an effect, queued while the animation
        // still plays. Lives in the DOL, in the file named after it
        // (game/msw/cockpit/CMissionCount.cpp). Size 0x1C, vptr at 0x18.
        // The panes and animations are indices set by SetParameter, in its
        // argument order; their names are guesses from use.
        class CMissionCount {
            public:
                CMissionCount();

                void SetParameter(unsigned char nPane, unsigned char nNumPane, unsigned char nNextPane,
                                  unsigned char nMaxPane, unsigned char nEffectPane, unsigned char nAnim05,
                                  unsigned char nRollAnim, unsigned char nRollEndAnim, unsigned char nWaitAnim,
                                  unsigned char nInAnim, unsigned char nOutAnim);
                // Starts the next queued count once the count animation ended.
                void MainLoop(CMenuPaneAccessor *pAccessor);
                // Plays the in (bDisp set) or out animation of the counter.
                void SetDisp(CMenuPaneAccessor *pAccessor, int bDisp);
                // Stops the counter's animations and plays nAnim.
                void SetMove(CMenuPaneAccessor *pAccessor, long nAnim);
                // Sets the count (both count boxes) and its maximum at once.
                void SetNum(CMenuPaneAccessor *pAccessor, long nNum, long nMax);
                // Rolls the count from nFrom to nTo, now or, while a roll
                // still plays, after it.
                void SetCountUpNum(CMenuPaneAccessor *pAccessor, long nFrom, long nTo);
                // Does nothing.
                void SetActiveColor(CMenuPaneAccessor *pAccessor, int bActive);

            private:
                // A count waiting for the animation (allocated, 0x10 bytes).
                struct CountUp {
                    nw4r::ut::Link m_Link; // 0x00
                    long m_nFrom;          // 0x08
                    long m_nTo;            // 0x0C
                };

                // (name guessed; the DOL's fn_801BCF68) Puts nFrom and nTo
                // into the two count boxes and starts the roll and its effect.
                void StartCountUp(CMenuPaneAccessor *pAccessor, long nFrom, long nTo);
                // (name guessed; inline) Not zero while the parent of the
                // second count box plays the roll.
                inline int IsCountUpAnimPlay(CMenuPaneAccessor *pAccessor);

                unsigned char m_nPane;       // 0x00
                unsigned char m_nNumPane;    // 0x01, the first count box
                unsigned char m_nNextPane;   // 0x02, the second count box
                unsigned char m_nMaxPane;    // 0x03
                unsigned char m_nEffectPane; // 0x04
                unsigned char m_nAnim05;     // 0x05, only stopped (by SetMove)
                unsigned char m_nRollAnim;   // 0x06
                unsigned char m_nRollEndAnim; // 0x07
                unsigned char m_nWaitAnim;   // 0x08
                unsigned char m_nInAnim;     // 0x09
                unsigned char m_nOutAnim;    // 0x0A
                nw4r::ut::List m_List;       // 0x0C, of CountUp

            public:
                // vptr at 0x18: declared after the data.
                virtual ~CMissionCount();
        };
    }
}

#endif /* !MSW_COCKPIT_CMISSIONCOUNT_HPP_ */
