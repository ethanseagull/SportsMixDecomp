#ifndef CNOWLOADING_HPP_
#define CNOWLOADING_HPP_

#include "bcs/CTask.hpp"

namespace nw4r {
    namespace lyt {
        class ArcResourceAccessor;
        class Layout;
        class FontRefLink;
        class AnimTransform;
    }
}

namespace msw {

    // The "now loading" layout drawn while files load. A task of 0x5C
    // bytes; the one instance is kept in the DOL's sbss (s_pInstance), and
    // the static setters do nothing without it.
    class CNowLoading : public bcs::CTask {
        public:
            // Inline: the DOL has no copy, CreateInstance builds it in place.
            CNowLoading() : bcs::CTask(0, 0x3D)
            {
                SetUnk08(0xFC00);
                m_bLoaded = 0;
                m_nStep = 0;
                m_nType = 0;
                m_Unk40 = 0;
                m_fTime = 0.0f;
                m_pResAccessor = 0;
                m_pLayout = 0;
                m_pFontRefLink = 0;
                m_pAnimTrans = 0;
                m_bDisp = 1;
            }

            // (name guessed; the DOL's fn_80142F04) Creates the instance.
            static void CreateInstance();
            // (name guessed; the DOL's fn_80142F98) Non-zero once the
            // layout's archive has loaded; 0 without an instance.
            static int IsLoaded();
            // Static: the body in the DOL stores into the global instance.
            static void SetType(long);
            // Added for MenuF game/menu/CMenu.cpp: shows or hides the
            // loading screen.
            static void SetDisp(int bDisp);

            virtual ~CNowLoading();
            virtual void Main();
            virtual void Disp();

        private:
            // (name guessed; the DOL's fn_8014337C) Draws the layout and
            // advances its animation.
            void DrawLayout();

            static CNowLoading *s_pInstance; // (name guessed) lbl_805C0C48

            int m_bLoaded;                                  // 0x34 (name guessed)
            long m_nStep;                                   // 0x38 (name guessed)
            long m_nType;                                   // 0x3C (name guessed) 1 and 2 show the layout
            long m_Unk40;                                   // 0x40
            float m_fTime;                                  // 0x44 (name guessed) SetType clears it
            nw4r::lyt::ArcResourceAccessor *m_pResAccessor; // 0x48
            nw4r::lyt::Layout *m_pLayout;                   // 0x4C
            nw4r::lyt::FontRefLink *m_pFontRefLink;         // 0x50
            nw4r::lyt::AnimTransform *m_pAnimTrans;         // 0x54
            int m_bDisp;                                    // 0x58 (name guessed) set by SetDisp
    };
}

#endif /* !CNOWLOADING_HPP_ */
