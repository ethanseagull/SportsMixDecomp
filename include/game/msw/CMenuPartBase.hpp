#ifndef CMENUPARTBASE_HPP_
#define CMENUPARTBASE_HPP_

namespace msw {
    class CMenuPartList;

    // Size 0x48, vptr at 0x44. One button of a menu; CMenuPartList holds
    // them. Lives in the DOL. The layout is read from its constructor; the
    // member names are guesses.
    class CMenuPartBase {
        public:
            // (name guessed) Size 0x16. The constant description of a button
            // that each menu's Set function copies into the part.
            struct Param {
                unsigned short m_nBtnKindId;        // 0x00
                unsigned short m_Unk02;             // 0x02
                unsigned short m_Unk04;             // 0x04
                unsigned char m_nAnim[5];           // 0x06
                unsigned short m_nNextBtnKindId[5]; // 0x0C
            };

            unsigned char m_Unk00[8];           // 0x00
            // 0x08: the button state, compared signed (1 in, 3 out,
            // 5 pushed, 7 waiting on an animation).
            union {
                unsigned long m_Unk08;
                long m_nState;
            };
            long m_nId;                         // 0x0C, the constructor argument
            CMenuPartList *m_pList;             // 0x10
            unsigned short m_nBtnKindId;        // 0x14
            unsigned short m_Unk16;             // 0x16
            unsigned short m_nCurrentBtnKindId; // 0x18
            unsigned short m_nNextBtnKindId[5]; // 0x1A
            unsigned short m_Unk24;             // 0x24
            // 0x26: animation numbers, 0xFF for none: [0] in, [1] after in,
            // [2] out, [3] after out, [4] push, [5] after push, [6] played
            // with each of them, [7..10] cursor of pad 0..3, [11] no cursor.
            unsigned char m_nAnim[12];          // 0x26
            long m_nSeCode[3];                  // 0x34
            // 0x40: sixteen flag bits; the constructor leaves only 0x0002 set.
            // m_nFlags40 is the same word whole: CButtonEntry::SetUpButton
            // sets 0x0004 with a ternary on it, which the bit-fields do not
            // compile to.
            union {
                struct {
                    unsigned short m_Unk40_FFC0 : 10;
                    unsigned short m_bUnk40_0020 : 1;
                    // (split for CMenuPartBase::Main, which tests 0x0010 alone)
                    unsigned short m_bUnk40_0010 : 1;
                    unsigned short m_bUnk40_0008 : 1;
                    unsigned short m_bUnk40_0004 : 1;
                    unsigned short m_bUnk40_0002 : 1;
                    unsigned short m_bUnk40_0001 : 1;
                };
                unsigned short m_nFlags40;
            };
            // 0x42: the constructor clears the low four bits (one per pad
            // whose cursor is on the button); ClearPadNoBits and
            // ResetButton clear the whole byte.
            union {
                struct {
                    unsigned char m_Unk42_F0 : 4;
                    unsigned char m_Unk42_0F : 4;
                };
                unsigned char m_nPadNoBits;
            };

            // vptr at 0x44
            CMenuPartBase(long nId);
            // Inline and empty: MenuF's ~CMenuPartButton inlines it, and the
            // DOL emits it as a weak copy.
            virtual ~CMenuPartBase() {}
            // The Set* functions return 0, SetButtonPush the part's id (-1
            // when it has no pane); CMenuPartButton passes the result on.
            virtual void Main();
            // Named by the RSO import of MenuF's CMenuPartButton vtable.
            // Inline and empty: MenuF's CSelectMenuLayoutBase.cpp emits a
            // weak copy at the head of its weak tail.
            virtual void Disp() {}
            virtual long SetButtonIn();
            virtual long SetButtonOut();
            virtual long SetButtonPush();
            virtual long SetButtonPushCancel();
            virtual long SetButtonMultiIn(long);
            virtual long SetButtonMultiOut(long);
            virtual long ResetButton();
            virtual long SetButtonAnimWait(long, int);
            virtual long InitButtonAnim();
            virtual long SetCursorAnimByPadNoBits(unsigned long);
            virtual long SetButtonAnim(long, int, long);

            // (added for game/menu/CCourtTab.cpp) Clears the per-pad
            // cursor bits.
            void ClearPadNoBits();

            // (name guessed; no symbol) The 0x0020 flag as an int: the
            // SelectMenu screens compare it signed, which the bit-field
            // read directly does not give.
            int IsStickMode() const { return m_bUnk40_0020; }

            // (names guessed; no symbols) Groups of m_nState values.
            static int IsStateIn(long nState)
            {
                int bResult = 1;
                if (nState != 1 && nState != 2) {
                    bResult = 0;
                }
                return bResult;
            }

            static int IsStateOut(long nState)
            {
                return nState == 3 || nState == 4;
            }

            static int IsStatePush(long nState)
            {
                int bResult = 1;
                if (nState != 5 && nState != 6) {
                    bResult = 0;
                }
                return bResult;
            }

            int IsIn() const { return IsStateIn(m_nState); }
            int IsOut() const { return IsStateOut(m_nState); }
            int IsPush() const { return IsStatePush(m_nState); }
    };
}

#endif /* !CMENUPARTBASE_HPP_ */
