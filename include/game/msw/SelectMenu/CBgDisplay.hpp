#ifndef SELECTMENU_CBGDISPLAY_HPP_
#define SELECTMENU_CBGDISPLAY_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

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

        // Size 0x24E8 (layout.py). The movie and texts behind the select
        // menu's screens; menu object 0x3E of CSelectMenu. Only what
        // game/menu/CCourtSelect.cpp needs.
        class CBgDisplay : public CSelectMenuLayoutBase {
            public:
                CBgDisplay(long nId);
                virtual ~CBgDisplay();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                void SetParameter(const BgDispParam *pParam);
                // (argument meaning guessed)
                void SetMessageTextNum(long nNum);
        };
    }
}

#endif /* !SELECTMENU_CBGDISPLAY_HPP_ */
