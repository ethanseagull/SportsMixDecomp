#ifndef SELECTMENU_CFRIENDLIST_HPP_
#define SELECTMENU_CFRIENDLIST_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "nw4r/math/types.h"
#include "revolution/rfl.h"

namespace bcs {
    namespace dwc {
        class CDwcFriendList;
    }
}

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    namespace SelectMenu {

        // Size 0x6E8. The Wi-Fi friend roster screen ("me_f_list"): the
        // player's own entry first, then the friends, five rows at a time,
        // scrolled with two buttons, the stick or a slider bar. Its buttons
        // are read from "me_f_list.btn" by the CButtonEntry base at 0x5C (a
        // second base, as in CFriendCode). Menu object 0x28 of CSelectMenu.
        // Member names are guesses.
        class CFriendList : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    ROW_NUM = 5,      // rows of the list
                    FACE_NUM = 6,     // Mii pictures: the player's and one per friend row
                    NAME_LEN = 0x10   // characters kept of each row's name
                };

                // The slider bar's panes (two copies moved together).
                enum {
                    PANE_SLIDER_BAR_LIGHT = 0xA,
                    PANE_SLIDER_BAR = 0x35
                };

                // Values of m_nMoveState: how the list scrolls.
                enum {
                    MOVE_NONE = 0,
                    MOVE_DOWN = 1,      // one row down: start the animation
                    MOVE_DOWN_WAIT = 2, // wait for it
                    MOVE_JUMP = 3,      // to m_nTopIndex at once (slider bar)
                    MOVE_UP = 4,        // one row up
                    MOVE_UP_WAIT = 5
                };

                CFriendList(long nId);
                virtual ~CFriendList();
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

                // Scrolls the list one step at a time (m_nMoveState).
                void MoveList();
                // Moves the list to where the slider bar was dragged: at
                // once when bJump is set, else row by row.
                void MoveSliderBarPointer(int bJump);
                // Fills the five rows from entry nTop of the list (0 is the
                // player, then the friends in sorted order).
                void SetFriendData(long nTop);
                // Starts scrolling one row up (bUp) or down at speed fSpeed;
                // with bIdleOnly only when the list is not moving.
                void SetMoveButton(int bUp, float fSpeed, int bIdleOnly);
                // Sizes the slider bar for the number of friends.
                void SetUpSliderBar();

            private:
                // Inlined in the game, with no out-of-line copies left in
                // the module; only their local statics have symbols.
                void CheckButtonAnim();
                // Puts pText into text box nPane when both exist.
                void SetTextPaneString(long nPane, const wchar_t *pText);
                // Puts nValue as a number into text box nPane when it exists.
                void SetTextPaneString(long nPane, long nValue);
                // (no symbols; inline, names guessed) Selects button
                // nBtnKindId with the cursor; draws Mii rData into picture
                // nFace.
                void MoveCursor(long nBtnKindId);
                void SetFaceIcon(long nFace, RFLStoreData &rData);
                // (no symbol; inline, name guessed) Moves pPane under row
                // nRow's pane, at its origin.
                void AttachToRow(nw4r::lyt::Pane *pPane, long nRow);

            public:
                // 0x5A0. The texture memory of each Mii picture.
                unsigned char *m_pFaceTex[FACE_NUM];
                // 0x5B8. The picture panes made for them.
                nw4r::lyt::Pane *m_pFacePane[FACE_NUM];
                // 0x5D0. The entry shown in the first row.
                long m_nTopIndex;
                CSave::PlayerData *m_pPlayerData;          // 0x5D4
                bcs::dwc::CDwcFriendList *m_pFriendList;   // 0x5D8
                // 0x5DC to 0x5E7. Scrolling: the animation speed (negative
                // upwards), the state, and whether another step is queued.
                float m_fMoveSpeed;
                long m_nMoveState;
                long m_bMoveNext;
                // 0x5E8 to 0x603. The slider bar: the distance it can move,
                // its highest and lowest position, its position, and whether
                // it is being dragged with the pointer.
                float m_fSliderRange;
                float m_fSliderTop;
                float m_fSliderBottom;
                nw4r::math::VEC3 m_SliderPos;
                long m_bSliderDrag;
                // 0x604. 1 when the row holds a registered friend.
                long m_nRowKind[ROW_NUM];
                // 0x618. The friend of each row.
                dwc::CFriend *m_pRowFriend[ROW_NUM];
                // 0x62C. Frames before the rows are filled again.
                float m_fRefreshWait;
                // 0x630 to 0x645. Ids of the buttons of the player's row, of
                // the friends' rows and of the rows of unregistered codes
                // (from the type SetUpButton returns).
                unsigned short m_nSelfButton;
                unsigned short m_nFriendButton[ROW_NUM];
                unsigned short m_nCodeButton[ROW_NUM];
                // 0x646. The name shown in each row (for the help line).
                wchar_t m_Name[ROW_NUM][NAME_LEN];
                // 0x6E6. The row button the cursor is on.
                unsigned short m_nCursorBtnKindId;
        };
    }
}

#endif /* !SELECTMENU_CFRIENDLIST_HPP_ */
