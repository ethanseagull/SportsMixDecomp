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
            unsigned long m_Unk08;              // 0x08
            long m_nId;                         // 0x0C, the constructor argument
            CMenuPartList *m_pList;             // 0x10
            unsigned short m_nBtnKindId;        // 0x14
            unsigned short m_Unk16;             // 0x16
            unsigned short m_nCurrentBtnKindId; // 0x18
            unsigned short m_nNextBtnKindId[5]; // 0x1A
            unsigned short m_Unk24;             // 0x24
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
                    unsigned short m_Unk40_0018 : 2;
                    unsigned short m_bUnk40_0004 : 1;
                    unsigned short m_bUnk40_0002 : 1;
                    unsigned short m_bUnk40_0001 : 1;
                };
                unsigned short m_nFlags40;
            };
            // 0x42: the constructor clears the low four bits.
            unsigned char m_Unk42_F0 : 4;
            unsigned char m_Unk42_0F : 4;

            // vptr at 0x44
            CMenuPartBase(long nId);
            virtual ~CMenuPartBase();
            virtual void Main();
            virtual void Unk10();
            virtual void SetButtonIn();
            virtual void SetButtonOut();
            virtual void SetButtonPush();
            virtual void SetButtonPushCancel();
            virtual void SetButtonMultiIn(long);
            virtual void SetButtonMultiOut(long);
            virtual void ResetButton();
            virtual void SetButtonAnimWait(long, int);
            virtual void InitButtonAnim();
            virtual void SetCursorAnimByPadNoBits(unsigned long);
            virtual void SetButtonAnim(long, int, long);

            // (name guessed; no symbol) The 0x0020 flag as an int: the
            // SelectMenu screens compare it signed, which the bit-field
            // read directly does not give.
            int IsStickMode() const { return m_bUnk40_0020; }
    };
}

#endif /* !CMENUPARTBASE_HPP_ */
