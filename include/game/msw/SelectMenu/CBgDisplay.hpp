#ifndef SELECTMENU_CBGDISPLAY_HPP_
#define SELECTMENU_CBGDISPLAY_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "wchar.h"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {

    // Size 0x3C (names guessed; added for game/menu/CCourtSelect.cpp).
    // What the background display of the select menu shows for a stage or
    // a cup: its movie and its texts. The owner fills it on the stack and
    // passes it to CBgDisplay::SetParameter.
    struct BgDispParam {
        // m_nType is left to the owner, as in the game.
        BgDispParam()
            : m_pMovieName(0), m_nStage(-1), m_pStageName(0), m_pIntroText(0), m_pCupText(0),
              m_pRuleText0(0), m_pRuleText1(0), m_pRuleText2(0), m_pRuleText3(0), m_pRuleText4(0),
              m_pScoreText(0), m_pScoreLabel(0), m_bStage(0), m_nGameKind(0),
              m_nRuleFlag(-1), m_nCup(-1), m_nPartyCup(-1), m_nPartyCup2(-1), m_nPartyStage(-1),
              m_nPartyLevel(-1)
        {
        }

        const char *m_pMovieName;          // 0x00
        long m_nStage;                     // 0x04, -1 for none
        const wchar_t *m_pStageName;       // 0x08
        const wchar_t *m_pIntroText;       // 0x0C
        const wchar_t *m_pCupText;         // 0x10
        const wchar_t *m_pRuleText0;       // 0x14
        const wchar_t *m_pRuleText1;       // 0x18
        const wchar_t *m_pRuleText2;       // 0x1C
        const wchar_t *m_pRuleText3;       // 0x20
        const wchar_t *m_pRuleText4;       // 0x24
        const wchar_t *m_pScoreText;       // 0x28
        const wchar_t *m_pScoreLabel;      // 0x2C
        // 0x30 to 0x38. -1 for none in the signed ones.
        unsigned char m_nType;             // 0x30, 0 the sports, 1 and 2 party games
        unsigned char m_bStage;            // 0x31
        unsigned char m_nGameKind;         // 0x32
        signed char m_nRuleFlag;       // 0x33
        signed char m_nCup;            // 0x34
        signed char m_nPartyCup;       // 0x35
        signed char m_nPartyCup2;      // 0x36
        signed char m_nPartyStage;     // 0x37
        signed char m_nPartyLevel;     // 0x38
    };

    namespace SelectMenu {

        class CNwLayout;

        // Size 0x24E8 (layout.py). The movie and texts behind the select
        // menu's screens; menu object 0x3E of CSelectMenu. Defined in
        // game/menu/CBgDisplay.cpp (the original file is
        // SelectMenuBgDisp.cpp, the name its assertion passes). It plays
        // the stage movie of the current choice in a picture pane of its
        // own layout (a CNwLayout), or the menu music, and shows the texts
        // of a BgDispParam. Member names are guesses.
        class CBgDisplay : public CSelectMenuLayoutBase {
            public:
                // Events of SendEvent (names guessed).
                enum {
                    EVENT_SET_PARAM = 0x4A,  // pParam: const BgDispParam *
                    EVENT_SET_MOVIE = 0x4B,  // pParam: the movie file name
                    EVENT_SET_MODE = 0x4D,   // pParam: MODE_*
                    EVENT_SET_BGM = 0x4E     // pParam: the music, negative for none
                };

                // Values of m_nMode.
                enum {
                    MODE_MOVIE = 0,
                    MODE_BGM = 1
                };

                CBgDisplay(long nId);
                virtual ~CBgDisplay();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                void Initialize();
                CNwLayout *NwLayout();
                void MovieControl();
                void BgmControl();
                long SetParam(const BgDispParam *pParam);
                void SetParameter(const BgDispParam *pParam);
                // (argument meaning guessed)
                void SetMessageTextNum(long nNum);
                void SetDispParameter();
                // Shows pane nSel of the nNum panes whose name numbers are in
                // pPanes and hides the others (0xFF hides them all).
                void SetDispPanes(const unsigned char *pPanes, unsigned char nNum, unsigned char nSel);
                long PlayPaneAnim(nw4r::lyt::Pane *pPane, long nAnim, int bEnable, int bRecursive, int bReset);
                long SetAnimBack(long nAnim);
                long StopAllAnimBack();
                // Shows or hides pane nPane of the layout, when there is one.
                // Defined in the .cpp.
                inline void SetPaneVisible(long nPane, bool bVisible);

            private:
                long m_nMode;                   // 0x5C, MODE_*
                long m_Unk60;                   // 0x60
                long m_nMovieStep;              // 0x64
                long m_nBgmStep;                // 0x68
                const char *m_pMovieName;       // 0x6C, the movie asked for
                const char *m_pOpenMovieName;   // 0x70, the movie opened
                const char *m_pWaitMovieName;   // 0x74, the movie asked for when m_fMovieWait started
                float m_fMovieWait;             // 0x78
                long m_nBgm;                    // 0x7C, the music asked for
                long m_nPlayBgm;                // 0x80, the music playing
                long m_nWaitBgm;                // 0x84, the music asked for when m_fBgmWait started
                float m_fBgmWait;               // 0x88
                unsigned char m_nLayout;        // 0x8C, which layout file
                CNwLayout *m_pNwLayout;         // 0x90
                long m_Unk94;                   // 0x94
                long m_Unk98;                   // 0x98
                wchar_t m_StageName[0x100];     // 0x9C
                wchar_t m_IntroText[0x800];     // 0x29C
                wchar_t m_CupText[0x800];       // 0x129C
                wchar_t m_RuleText[5][0x20];    // 0x229C
                wchar_t m_ScoreText[0x40];      // 0x23DC
                wchar_t m_ScoreLabel[0x40];     // 0x245C
                // 0x24DC to 0x24E4: BgDispParam's bytes, read as signed.
                signed char m_nType;            // 0x24DC
                signed char m_bStage;           // 0x24DD
                signed char m_nGameKind;        // 0x24DE
                signed char m_nRuleFlag;        // 0x24DF
                signed char m_nCup;             // 0x24E0
                signed char m_nPartyCup;        // 0x24E1
                signed char m_nPartyCup2;       // 0x24E2
                signed char m_nPartyStage;      // 0x24E3
                signed char m_nPartyLevel;      // 0x24E4
        };
    }
}

#endif /* !SELECTMENU_CBGDISPLAY_HPP_ */
