#ifndef MSW_CMENUMDLTABLESET_HPP_
#define MSW_CMENUMDLTABLESET_HPP_

#include "game/msw/CTableSetMdlObj.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {

    // An animation waiting in msw::CMenuMdlTableSet's queue. Size 0x18,
    // vptr at 0x14. Member names are guessed.
    class CChrMenuAnimInfoLink {
        public:
            CChrMenuAnimInfoLink(long nAnim, float rate, float frame)
            {
                m_nAnim = nAnim;
                m_fRate = rate;
                m_fFrame = frame;
            }

            nw4r::ut::Link m_Link; // 0x00
            long m_nAnim;          // 0x08, index into the animation table
            float m_fRate;         // 0x0C
            float m_fFrame;        // 0x10

            // Inline: the compiler then emits it right after
            // CMenuMdlTableSet::MainLoop, the first function that deletes a
            // link in its own body, which is where the original has it.
            virtual ~CChrMenuAnimInfoLink() {}
    };

    // Plays the animations of a table on a msw::CTableSetMdlObj, one after
    // another. Size 0x28, vptr at 0x24. Member names are guessed.
    class CMenuMdlTableSet {
        public:
            // One animation of the table. Size 8. Member names are guessed.
            struct AnimInfo {
                const char *pName;      // 0x00, name of the resource in the file
                unsigned char bLoop;    // 0x04, adds flag 2 when playing
                unsigned char bReverse; // 0x05, plays backwards from the end
            };

            CMenuMdlTableSet();

            void MainLoop();
            int SetAnimTable(const AnimInfo *pTable, long num); // returns 0
            // Both return 0, -1 without a model or -2 for a bad index.
            long Play(long nAnim, float rate, float frame, unsigned long flag, float blend);
            long PlayImpl(long nAnim, float rate, float frame, unsigned long flag, float blend);
            void PushBack(long nAnim, float rate, float frame);
            void ClearList();
            nw4r::g3d::ResAnmChr GetResAnimMot(const char *pName, nw4r::g3d::ResFile file);
            nw4r::g3d::ResAnmClr GetResAnimClr(const char *pName, nw4r::g3d::ResFile file);
            nw4r::g3d::ResAnmTexSrt GetResAnimTsa(const char *pName, nw4r::g3d::ResFile file);

            // (guessed) No copy of its own in MenuF. MainLoop loads the
            // arguments of its PlayImpl call in the order an inline function
            // with these three parameters gives; a direct call does not.
            long PlayQueued(long nAnim, float rate, float frame) { return PlayImpl(nAnim, rate, frame, 0, 0.0f); }

        private:
            CTableSetMdlObj *m_pObj;       // 0x00
            const AnimInfo *m_pAnimTable;  // 0x04
            long m_nAnimNum;               // 0x08
            nw4r::ut::List m_List;         // 0x0C, of CChrMenuAnimInfoLink
            nw4r::g3d::ResFile m_ResFile;  // 0x18
            int m_bOwnObj;                 // 0x1C, the destructor deletes m_pObj if set
            float m_fWait;                 // 0x20, time left before the animation is stopped

        public:
            // Declared after the data: the vptr is at 0x24.
            virtual ~CMenuMdlTableSet();
    };
}

#endif /* !MSW_CMENUMDLTABLESET_HPP_ */
