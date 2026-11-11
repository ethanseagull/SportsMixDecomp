#ifndef SELECTMENU_CFRIENDINFO_HPP_
#define SELECTMENU_CFRIENDINFO_HPP_

#include "bcs/G2D/CLineStripDL.hpp"
#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "nw4r/ut/Color.hpp"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    class CPlayerInfoRender;
    struct WiFiSkill;

    namespace dwc {
        class CFriend;
    }

    namespace SelectMenu {

        // Size 0x9C0. The information window of a Wi-Fi friend, or of the
        // player ("me_p_inf_a"): five tabs (the four sports and an overall
        // one), each with the rating, its history drawn as a line graph, and
        // the win counts; the friend's Mii, trophies and online state. Its
        // buttons are read by the CButtonEntry base at 0x5C (a second base,
        // as in CTeam). Menu object 0x29 of CSelectMenu;
        // CSelectMenu::FriendPlayerInfo_* and FriendInfo_* drive it. Member
        // names are guesses.
        class CFriendInfo : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                typedef bcs::G2D::CLineStripDL::VERTEX_INFO VERTEX_INFO;

                enum {
                    TAB_NUM = 5,
                    // Points of the rating graph.
                    GRAPH_POINT_NUM = 30,
                    GRAPH_VERTEX_MAX = 32,
                    // Horizontal lines at fixed ratings.
                    BORDER_LINE_NUM = 4,
                    // Copies of the digit panes for the graph's scale.
                    ZERO_PANE_NUM = 3,
                    NINE_PANE_NUM = 4
                };

                CFriendInfo(long nId);
                virtual ~CFriendInfo();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Fills the window: the friend's (bFriend not zero) or the
                // player's own data.
                void SetUpDisp(int bFriend);
                // Follows the friend's online state and shows it.
                void CheckFrindStat();
                // Selects tab nTab and shows its data, the friend's when
                // bFriend is not zero.
                void SetTabSelect(long nTab, int bFriend);
                // Lays out the graph of pSkill.
                void SetScorePos(WiFiSkill *pSkill);
                // Shows the scale of the graph up to nMax.
                void DispNumberPane(unsigned short nMax);
                void SetSkillLine(WiFiSkill *pSkill, nw4r::math::VEC2 &rOrigin, float fStepX, float fMax,
                                  float fHeight);
                // Border line nIndex at rating nValue, on a scale up to fMax.
                void SetBorderSkillLine(long nIndex, long nValue, float fMax);

            private:
                // Inlined in the game; no out-of-line copies, only their
                // local statics have symbols.
                void CheckButtonAnim();
                void SetTextPaneString(long nPane, const wchar_t *pText);
                void SetTextPaneString(long nPane, long nValue);
                nw4r::lyt::Pane *DuplicateDigitPane(long nDigit, const char *pName, const char *pLayoutName);
                bcs::G2D::CLineStripDL *AppendLineStrip(const char *pName);

                // CSelectMenu's steps read the delete button's state.
                friend class msw::CSelectMenu;

                // 0x5A0. The friend shown (CSelectMenu::m_Unk708), null for
                // the player's own data.
                dwc::CFriend *m_pFriend;
                long m_bFriend;                         // 0x5A4

            public:
                // 0x5A8. The selected tab.
                long m_nTabSelect;

            private:
                long m_bTabHit[TAB_NUM];                // 0x5AC, the pointer is on the tab
                CPlayerInfoRender *m_pMiiRender;        // 0x5C0
                unsigned long m_nGraphPointNum;         // 0x5C4
                // 0x5C8. Copies of the digit panes: three zeros, a five and
                // four nines.
                nw4r::lyt::Pane *m_pZeroPane[ZERO_PANE_NUM];
                nw4r::lyt::Pane *m_pFivePane;           // 0x5D4
                nw4r::lyt::Pane *m_pNinePane[NINE_PANE_NUM]; // 0x5D8
                unsigned char m_Unk5E8[0x8];            // 0x5E8
                nw4r::ut::Color m_GraphColor;           // 0x5F0, the colour of the selected tab
                unsigned char m_nOnlineStat;            // 0x5F4, the state last shown
                unsigned char m_nDeleteAnim;            // 0x5F5, the delete button's state
                unsigned char m_Unk5F6[0x2];            // 0x5F6
                VERTEX_INFO m_PrimLine[GRAPH_VERTEX_MAX];   // 0x5F8
                VERTEX_INFO m_PrimShadow[GRAPH_VERTEX_MAX]; // 0x778
                VERTEX_INFO m_NumLine[2];               // 0x8F8
                VERTEX_INFO m_ShadowLine[2];            // 0x910
                bcs::G2D::CLineStripDL *m_pPrimLine;    // 0x928
                bcs::G2D::CLineStripDL *m_pPrimShadow;  // 0x92C
                bcs::G2D::CLineStripDL *m_pNumLine;     // 0x930
                bcs::G2D::CLineStripDL *m_pShadowLine;  // 0x934
                VERTEX_INFO m_BorderLine[BORDER_LINE_NUM][2];          // 0x938
                bcs::G2D::CLineStripDL *m_pBorderLine[BORDER_LINE_NUM];  // 0x998
                bcs::G2D::CLineStripDL *m_pSBorderLine[BORDER_LINE_NUM]; // 0x9A8
                long m_nPrevTab;                        // 0x9B8, -1: none
                float m_fTrofyWait;                     // 0x9BC
        };

        // Size 0x5A0. The window shown for a friend who has not registered
        // the player back ("me_p_inf_b"): the friend code and a delete
        // button. Menu object 0x2A of CSelectMenu; CSelectMenu::NoFriendInfo_*
        // drive it.
        class CNoFriendInfo : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CNoFriendInfo(long nId);
                virtual ~CNoFriendInfo();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long PushButton(long nPad, long nBtnKindId);

            private:
                // Inlined in the game; only its local static has a symbol.
                void CheckButtonAnim();
        };
    }
}

#endif /* !SELECTMENU_CFRIENDINFO_HPP_ */
