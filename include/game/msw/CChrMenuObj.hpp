#ifndef MSW_CCHRMENUOBJ_HPP_
#define MSW_CCHRMENUOBJ_HPP_

#include "nw4r/ut/List.hpp"
#include "revolution/rfl.h"

namespace msw {

    class CChrMdlObj;

    // A character model shown in the menus, kept in msw::CChrMenuCtrl's
    // list. Size 0x48, vptr at 0x44 (MenuF, game/menu/CChrMenuCtrl.cpp).
    // Member names guessed.
    class CChrMenuObj {
            friend class CChrMenuCtrl;

        public:
            CChrMenuObj()
            {
                m_pMdl = 0;
                m_nType = 0;
                m_nChr = 0;
                m_nSub = 0;
                m_Unk24 = 0;
                m_hEffect = 0;
                m_Unk2C = 0;
                m_nMot = -1;
                m_nMotFlag = 0;
                m_fMotRate = 0.0f;
                m_fMotBlend = 0.0f;
                m_fMotFrame = 0.0f;
            }

            // Selects one of the colour pairs of the silhouette (0 to 3).
            void SetSilhouette(long nColor);
            // Sets the model's alpha, clamped to [0, 1].
            void SetAlpha(float alpha);
            // Plays motion nMot of the model and remembers the arguments.
            void AnimPlayMot(long nMot, long nFlag, float rate, float blend, float frame);
            // Attaches effect nEff to the model's "trans" node.
            void SetAttachEffect(long nEff);
            void StopAttachEffect();
            void SetMiiNoface(int bNoFace);

            // (names guessed) Read and set by msw::SelectMenu::CCupChr.
            CChrMdlObj *GetMdl() const { return m_pMdl; }
            long GetChr() const { return m_nChr; }
            long GetSub() const { return m_nSub; }
            void SetId(long nId) { m_nId = nId; }

        private:
            nw4r::ut::Link m_Link;  // 0x00, in msw::CChrMenuCtrl's list
            CChrMdlObj *m_pMdl;     // 0x08
            long m_nType;           // 0x0C, 1 when built from a character, 3 from a given model
            long m_nChr;            // 0x10
            long m_nSub;            // 0x14
            RFLDataSource m_Source; // 0x18
            long m_Unk1C;           // 0x1C
            long m_nId;             // 0x20, a serial number from msw::CChrMenuCtrl
            long m_Unk24;           // 0x24
            unsigned long m_hEffect;// 0x28, from msw::CEffectCtrl::CreateAttachEffect
            unsigned char m_Unk2C;  // 0x2C
            long m_nMot;            // 0x30, the last AnimPlayMot's arguments
            long m_nMotFlag;        // 0x34
            float m_fMotRate;       // 0x38
            float m_fMotBlend;      // 0x3C
            float m_fMotFrame;      // 0x40

        public:
            // Declared after the data: the vptr is at 0x44.
            virtual ~CChrMenuObj();
    };
}

#endif /* !MSW_CCHRMENUOBJ_HPP_ */
