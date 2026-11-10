#ifndef NW4R_LYT_PANE_HPP_
#define NW4R_LYT_PANE_HPP_

#include "nw4r/math/types.h"
#include "nw4r/ut/RuntimeTypeInfo.h"

namespace nw4r {
    namespace lyt {

        // Width and height of a pane.
        struct Size {
            float width;  // 0x00
            float height; // 0x04
        };

        // One node of a layout. Lives in the DOL. Only the members used so
        // far; the rest is opaque, so the class must not be instantiated
        // yet. Derived from only by TextBox (nw4r/lyt/TextBox.hpp), for
        // ut::DynamicCast.
        //
        // Offsets are what the game's own code shows (msw::CRadarIcon2D in
        // the DOL and its classes in the modules): translation at 0x2C,
        // rotation at 0x38, scale at 0x44, size at 0x4C, global matrix at 0x84, alpha at 0xB8,
        // flags at 0xBB, name at 0xBC; TextBox's members start at 0xD8.
        // Member and function names follow the public ogws decompilation's
        // description of nw4r::lyt::Pane (from memory, no code copied).
        class Pane {
            public:
                // vptr at 0. The vtable has 26 entries up to 0x6C (TextBox's
                // own start at 0x70, see CMenuPaneAccessor::SetTextBoxString
                // in the DOL). Only GetRuntimeTypeInfo (0x0C) is called so
                // far; the other slots are placeholders named by offset.
                virtual ~Pane();
                virtual const ut::detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const;
                virtual void Unk10();
                virtual void Unk14();
                virtual void Unk18();
                virtual void Unk1C();
                virtual void Unk20();
                virtual void Unk24();
                virtual void Unk28();
                virtual void Unk2C();
                virtual void Unk30();
                virtual void Unk34();
                virtual void Unk38();
                virtual void Unk3C();
                virtual void Unk40();
                virtual void Unk44();
                virtual void Unk48();
                virtual void Unk4C();
                virtual void Unk50();
                virtual void Unk54();
                virtual void Unk58();
                virtual void Unk5C();
                virtual void Unk60();
                virtual void Unk64();
                virtual void Unk68();
                virtual void Unk6C();

                void SetSize(const Size &rSize) { mSize = rSize; }
                // Added for HockeyF game/hockey/CCockpitHockeyCoin.cpp and MenuF
                // game/menu/CGameSelect.cpp.
                const Size &GetSize() const { return mSize; }
                const math::MTX34 &GetGlobalMtx() const { return mGlbMtx; }
                const char *GetName() const { return mName; }
                // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp.
                void SetTranslate(const math::VEC3 &rTranslate) { mTranslate = rTranslate; }
                // Added for MenuF game/menu/CChrCaption.cpp.
                void SetAlpha(unsigned char nAlpha) { mAlpha = nAlpha; }
                // Bit 0 of the flags.
                void SetVisible(bool bVisible)
                {
                    mFlag = (unsigned char)((mFlag & 0xFE) | (bVisible ? 1 : 0));
                }
                // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp.
                void SetRotate(const math::VEC3 &rRotate) { mRotate = rRotate; }
                void SetScale(const math::VEC2 &rScale) { mScale = rScale; }
                // Bit 2 of the flags (name from the ogws description).
                void SetLocationAdjust(bool bAdjust)
                {
                    mFlag = (unsigned char)((mFlag & 0xFB) | (bAdjust ? 4 : 0));
                }
                // Added for game/msw/cockpit/CMissionCount.cpp (C3DCoin::AttachPane).
                bool IsVisible() const { return mFlag & 1; }
                // Bit 2 of the flags: the pane's position is adjusted to the
                // screen's aspect ratio (C3DCoin::AttachPane scales x then).
                bool IsLocationAdjust() const { return (mFlag & 4) != 0; }
                const math::VEC3 &GetTranslate() const { return mTranslate; }
                Pane *GetParent() const { return mpParent; }

            private:
                unsigned char m_Unk04[0x8];
                Pane *mpParent;              // 0x0C, null at the root
                unsigned char m_Unk10[0x1C];
                math::VEC3 mTranslate;       // 0x2C
                math::VEC3 mRotate;          // 0x38
                math::VEC2 mScale;           // 0x44
                Size mSize;                  // 0x4C
                unsigned char m_Unk54[0x30];
                math::MTX34 mGlbMtx;         // 0x84
                unsigned char m_UnkB4[0x4];
                unsigned char mAlpha;        // 0xB8
                unsigned char m_UnkB9[0x2];
                unsigned char mFlag;         // 0xBB, bit 0: visible
                char mName[17];              // 0xBC
                unsigned char m_UnkCD[0xB];  // size 0xD8
        };
    }
}

#endif /* !NW4R_LYT_PANE_HPP_ */
