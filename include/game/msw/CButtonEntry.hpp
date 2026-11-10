#ifndef CBUTTONENTRY_HPP_
#define CBUTTONENTRY_HPP_

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {
    class CMenuPartButton;

    // Sound effect codes the button descriptions index (8 entries; defined
    // with the CCharaStatus unit's data).
    extern long ButtonEntrySETable[];

    // Size 0x5A0, no vtable. Lives in MenuF, embedded in the SelectMenu
    // screens (CEntry has one at 0x5C). Reads the button descriptions of a
    // screen from "menu/bin/General_pack.bin" and fills CMenuPartButtons from
    // them. All names are guesses.
    class CButtonEntry {
        public:
            enum {
                NAME_MAX = 64,
                ANIM_MAX = 64
            };

            // Size 0x68. One button in the file.
            struct Button {
                unsigned char m_Unk00[8];           // 0x00
                unsigned short m_nNameId;           // 0x08, ArchiveBuildLoader writes the index of m_Name
                unsigned short m_nName2Id;          // 0x0A, the same for m_Name2
                unsigned char m_nAnim[5];           // 0x0C
                unsigned char m_Unk11;              // 0x11
                unsigned short m_nNextBtnKindId[5]; // 0x12; 0x8000 in the first: no button
                long m_nSe[2];                      // 0x1C, indices into ButtonEntrySETable
                unsigned char m_Unk24[4];           // 0x24
                char m_Name[0x20];                  // 0x28
                char m_Name2[0x20];                 // 0x48
            };

            // Start of the resource the file holds. m_nButtonNum Buttons
            // follow it; m_nAnimNum animation names of ANIM_NAME_SIZE bytes
            // start at m_nAnimOffset.
            struct Header {
                unsigned char m_Unk00[8];    // 0x00
                unsigned long m_nAnimOffset; // 0x08, from the start of the header
                unsigned short m_nButtonNum; // 0x0C
                unsigned short m_nAnimNum;   // 0x0E
            };

            enum {
                ANIM_NAME_SIZE = 0x40
            };

            CButtonEntry();
            ~CButtonEntry();

            void ArchiveBuildLoader(const char *pName);
            void RemoveList();
            void SetUpAnim(bcs::G2D::CPaneCtrl *pPaneCtrl);
            int SetUpButton(CMenuPartButton *pButton, long nIndex, int bUnk);
            void ResetButtonID(CMenuPartButton *pButton);
            // The fourth id argument goes to m_nNextBtnKindId[4], the fifth to [3].
            void ReFineButtonID(CMenuPartButton *pButton, unsigned short nId0, unsigned short nId1,
                                unsigned short nId2, unsigned short nId4, unsigned short nId3);

        private:
            // Button nIndex of the file. Inline in the game (name guessed).
            Button *GetButton(long nIndex)
            {
                return reinterpret_cast<Button *>(m_pData + 1) + nIndex;
            }

            // Index of pName in m_pName, adding it at the end when it is new;
            // 0 for an empty name. Inline in the game (name guessed).
            long EntryName(const char *pName);

            char m_ArchiveName[0x40];       // 0x000, released by RemoveList when not empty
            Header *m_pData;                // 0x040
            const char *m_pName[NAME_MAX];  // 0x044
            unsigned char m_Unk144[0x2FC];  // 0x144, not touched by this unit
            const char *m_pAnimName[ANIM_MAX]; // 0x440
            long m_nNameNum;                // 0x540
            unsigned char m_Unk544[0x5C];   // 0x544
    };
}

#endif /* !CBUTTONENTRY_HPP_ */
