#ifndef NW4R_LYT_PANE_HPP_
#define NW4R_LYT_PANE_HPP_

#include "nw4r/lyt/common.h"
#include "nw4r/math/types.h"
#include "nw4r/ut/Color.hpp"
#include "nw4r/ut/LinkList.h"
#include "nw4r/ut/RuntimeTypeInfo.h"

namespace nw4r {
    namespace lyt {

        class Material;
        class DrawInfo;
        class AnimResource;
        class AnimationLink;

        // Width and height of a pane.
        struct Size {
            // Zero (added for MenuF game/menu/CBgDisplay.cpp, which emits
            // it out of line; no earlier user default-constructs a Size).
            Size() : width(0.0f), height(0.0f) {}
            // Added for MenuF game/menu/CBackground.cpp.
            Size(float w, float h) : width(w), height(h) {}
            // Copies member by member: the game copies a Size with two
            // lfs/stfs pairs (CWiFiFrindVS::Set), not with lwz/stw.
            Size(const Size &rOther) : width(rOther.width), height(rOther.height) {}

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
                // 0x24, 0x28 (added for BossF game/boss/CCockpitBossCoder.cpp):
                // the colour of corner nIdx (0 to 3).
                virtual ut::Color GetVtxColor(unsigned long nIdx) const;
                virtual void SetVtxColor(unsigned long nIdx, ut::Color color);
                virtual void Unk2C();
                virtual void Unk30();
                virtual void Unk34();
                virtual void Unk38();
                // 0x3C (added for DodgeF game/dodge/CCockpitDodgeTelopInOutChange.cpp):
                // the pane named pName below this one, or null.
                virtual Pane *FindPaneByName(const char *pName, bool bRecursive);
                virtual void Unk40();
                virtual void Unk44();
                virtual void Unk48();
                virtual void Unk4C();
                virtual void Unk50();
                virtual void Unk54();
                // 0x58 to 0x64 named for MenuF game/menu/CBgDisplay.cpp (ogws
                // names): the animation link of rAnimRes bound to this pane
                // itself, enabling a bound animation, and the materials.
                virtual AnimationLink *FindAnimationLinkSelf(const AnimResource &rAnimRes);
                virtual void Unk5C();
                virtual void SetAnimationEnable(const AnimResource &rAnimRes, bool bEnable, bool bRecursive);
                virtual unsigned char GetMaterialNum() const;
                // 0x68 (VolleyF game/volley/CCockpitChrIcon2DVolley.cpp: a
                // picture pane's material). Constness guessed.
                virtual Material *GetMaterial() const;
                // 0x6C (added for MenuF game/menu/CBgDisplay.cpp).
                virtual Material *GetMaterial(unsigned long idx) const;
                // 0x70, the last slot: every pane vtable in the DOL ends with
                // LoadMtx (added for DOL game/bcs/G2D/CLineStripDL.cpp, which
                // emits a vtable of a pane class; the vtable is 0x74 bytes).
                virtual void LoadMtx(const DrawInfo &rInfo);

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
                // Added for DOL game/bcs/G2D/CLineStripDL.cpp.
                unsigned char GetGlobalAlpha() const { return mGlbAlpha; }
                // Bit 0 of the flags. Clear, then set, as two statements:
                // the game masks the flags before inserting the bit
                // (HockeyF CCockpitHockeySmashVS::SetupGame).
                void SetVisible(bool bVisible)
                {
                    mFlag &= (unsigned char)~1;
                    mFlag |= (unsigned char)(bVisible ? 1 : 0);
                }
                // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp (SetRotate
                // also for VolleyF game/volley/CCockpitChrIcon2DVolley.cpp).
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
                // (added for HockeyF game/hockey/CCockpitHockeyMap.cpp, ogws name)
                ut::LinkList<Pane, 4> &GetChildList() { return mChildList; }

                // Added for MenuF game/menu/CBgDisplay.cpp (ogws names).
                unsigned char GetBasePositionH() const { return detail::GetHorizontalPosition(mBasePosition); }
                void SetBasePositionH(unsigned char val) { detail::SetHorizontalPosition(&mBasePosition, val); }
                unsigned char GetBasePositionV() const { return detail::GetVerticalPosition(mBasePosition); }
                void SetBasePositionV(unsigned char val) { detail::SetVerticalPosition(&mBasePosition, val); }

                // Added for MenuF game/menu/CBackground.cpp (out of line in
                // the DOL). Return type of AppendChild guessed.
                void SetName(const char *pName);
                void AppendChild(Pane *pChild);
                // Bytes 0xCD-0xD6 are the user data (ogws description).
                void SetUserAllocated(bool bUserAllocated) { mbUserAllocated = bUserAllocated; }

            private:
                unsigned char m_Unk04[0x8];
                Pane *mpParent;              // 0x0C, null at the root
                // (added for HockeyF game/hockey/CCockpitHockeyMap.cpp) The
                // children; each child's list node is at its offset 4.
                ut::LinkList<Pane, 4> mChildList; // 0x10
                unsigned char m_Unk1C[0x10];
                math::VEC3 mTranslate;       // 0x2C
                math::VEC3 mRotate;          // 0x38
                math::VEC2 mScale;           // 0x44
                Size mSize;                  // 0x4C
                unsigned char m_Unk54[0x30];
                math::MTX34 mGlbMtx;         // 0x84
                unsigned char m_UnkB4[0x4];
                unsigned char mAlpha;        // 0xB8
                unsigned char mGlbAlpha;     // 0xB9 (ogws name; the alpha times the parents')
                unsigned char mBasePosition; // 0xBA (added for MenuF game/menu/CBgDisplay.cpp)
                unsigned char mFlag;         // 0xBB, bit 0: visible
                char mName[17];              // 0xBC
                unsigned char m_UnkCD[0x9];  // 0xCD
                bool mbUserAllocated;        // 0xD6 (name from the ogws description)
                unsigned char m_UnkD7[0x1];  // size 0xD8
        };
    }
}

#endif /* !NW4R_LYT_PANE_HPP_ */
