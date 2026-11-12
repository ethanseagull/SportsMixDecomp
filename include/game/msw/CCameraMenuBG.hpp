#ifndef MSW_CCAMERAMENUBG_HPP_
#define MSW_CCAMERAMENUBG_HPP_

#include "game/msw/CCameraAnimMenu.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {

    // A camera animation waiting in msw::CCameraMenuBG's queue. Size 0x18,
    // vptr at 0x14. The same shape as msw::CChrMenuAnimInfoLink. Member
    // names are guessed.
    class CCameraMenuAnimInfoLink {
        public:
            CCameraMenuAnimInfoLink(long nAnim, float rate, float frame)
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
            // CCameraMenuBG::MainLoop, as in the original.
            virtual ~CCameraMenuAnimInfoLink() {}
    };

    // The camera of the menus' 3D background: plays the camera animations
    // of a table one after another. One instance, s_Instance. Size 0x28,
    // vptr at 0x24. Member names are guessed.
    class CCameraMenuBG {
        public:
            // One camera animation of the table. Size 0x28. Member names are
            // guessed; the words from 0x14 on are not read by this class.
            struct AnimInfo {
                // 0x00: scene animation names, [variant][adjusted]. The
                // variant is chosen with SetAnimTable, the second index is 1
                // when CLytCtrl::IsLocationAdjust is set.
                const char *pName[2][2];
                unsigned char nCamera;    // 0x10, camera of the scene animation
                unsigned char bFlag1;     // 0x11, Play flag bit 0
                unsigned char bFlag2;     // 0x12, Play flag bit 1
                unsigned char m_Unk13;    // 0x13 (0 in the tables seen)
                unsigned char m_Unk14;    // 0x14
                long m_Unk18;             // 0x18
                float m_Unk1C;            // 0x1C
                float m_Unk20;            // 0x20
                float m_Unk24;            // 0x24
            };

            CCameraMenuBG()
                : m_pCamera(0)
                , m_nView(0)
            {
                nw4r::ut::List_Init(&m_List, 0);
            }

            // Inline: the copy at MenuF 0x573C is game/menu/CSelectMenu.cpp's.
            static CCameraMenuBG *GetInstance() { return &s_Instance; }

            void Initialize(long view, nw4r::g3d::ResFile file);
            void Finalize();
            void MainLoop();
            nw4r::g3d::ResFile GetResFileFromEfResource(const char *pName);
            int SetAnimTable(const AnimInfo *pTable, long num, long nVariant); // returns 0
            // Both return 0, -1 without a camera or -2 for a bad index.
            long Play(long nAnim, float rate, float frame);
            long PlayImpl(long nAnim, float rate, float frame);
            int IsEndAnim();
            void PushBack(long nAnim, float rate, float frame);
            void ClearList();
            // Queues nAnim unless it is playing or queued already; returns 1
            // if it was queued.
            int PushBackWithoutReady(long nAnim, float rate, float frame);
            float GetSpeed();
            void SetSpeed(float speed);

            static CCameraMenuBG s_Instance;

        private:
            // (names guessed) Inline helpers of PushBackWithoutReady: the
            // original materialises the result of the test as 0 or 1 before
            // testing it again.
            CCameraMenuAnimInfoLink *FindLink(long nAnim)
            {
                CCameraMenuAnimInfoLink *pLink = static_cast<CCameraMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, 0));
                while (pLink != 0) {
                    if (nAnim == pLink->m_nAnim) {
                        return pLink;
                    }
                    pLink = static_cast<CCameraMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
                }
                return 0;
            }
            bool IsReady(long nAnim)
            {
                if (nAnim == m_nCurrent || FindLink(nAnim) != 0) {
                    return true;
                }
                return false;
            }
            // (guessed) MainLoop loads the arguments of its PlayImpl call in
            // the order an inline function with these parameters gives, as
            // in msw::CMenuMdlTableSet::MainLoop.
            long PlayQueued(long nAnim, float rate, float frame) { return PlayImpl(nAnim, rate, frame); }

            CCameraAnimMenu *m_pCamera;  // 0x00
            long m_nView;                // 0x04
            const AnimInfo *m_pAnimTable; // 0x08
            long m_nAnimNum;             // 0x0C
            nw4r::ut::List m_List;       // 0x10, of CCameraMenuAnimInfoLink
            long m_nCurrent;             // 0x1C, the animation playing, -1 for none
            long m_nVariant;             // 0x20, first index of AnimInfo::pName

        public:
            // Declared after the data: the vptr is at 0x24.
            virtual ~CCameraMenuBG();
    };
}

#endif /* !MSW_CCAMERAMENUBG_HPP_ */
