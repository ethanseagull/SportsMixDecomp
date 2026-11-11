#ifndef SELECTMENU_CCHARASTATUS_HPP_
#define SELECTMENU_CCHARASTATUS_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {
    namespace SelectMenu {

        // Size 0xF4. The status windows of the character select screen
        // ("me_cha_status"): for each of up to six character slots a frame
        // with the player's label, the chosen character's name, type and
        // three gauges. Member names are guesses.
        class CCharaStatus : public CSelectMenuLayoutBase {
            public:
                enum {
                    SLOT_NUM = 6
                };

                CCharaStatus(long nId);
                virtual ~CCharaStatus();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                void Initialize();
                // The number of slots with a player, not counting the
                // computer's in modes 1 and 4.
                long GetArrangementCount();
                // Follows the character select screen's slots: names,
                // gauges and the court characters' motions.
                void DispCharaStatus();

            private:
                // Inline in the game: clears the slots' display state;
                // nSignChara goes into m_nSignChara.
                void ClearSlots(unsigned char nSignChara);

                long m_Unk5C;                    // 0x5C
                long m_Unk60;                    // 0x60
                long m_Unk64;                    // 0x64
                long m_Unk68;                    // 0x68
                unsigned char m_Unk6C[0xC];      // 0x6C, not touched here
                long m_Unk78;                    // 0x78
                long m_Unk7C;                    // 0x7C
                long m_nArrangeNum;              // 0x80, GetArrangementCount() while Set builds the layout
                long m_nDispStep[SLOT_NUM];      // 0x84, 0 to 2: what DispCharaStatus does next
                long m_nDispChara[SLOT_NUM];     // 0x9C, the character shown, 0xFF none
                long m_nDispState[SLOT_NUM];     // 0xB4, the state the court character was given
                float m_fGaugeTime[SLOT_NUM];    // 0xCC, frames until the gauge animations stop
                float m_fWait;                   // 0xE4, frames before Main starts
                unsigned char m_nSignChara[SLOT_NUM]; // 0xE8, the character the pad sign was set for
                unsigned char m_UnkEE[0x6];      // 0xEE
        };
    }
}

#endif /* !SELECTMENU_CCHARASTATUS_HPP_ */
