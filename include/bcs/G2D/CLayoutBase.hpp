#ifndef BCS_G2D_CLAYOUTBASE_HPP_
#define BCS_G2D_CLAYOUTBASE_HPP_

namespace nw4r {
    namespace ut {
        class Font;
    }
}

namespace nw4r {
    namespace lyt {
        class AnimResource;
        class AnimTransform;
        class ArcResourceAccessor;
        class ArcResourceLink;
        class FontRefLink;
        class ResourceAccessor;
    }
}

namespace bcs {
    namespace G2D {
        class CLayoutDL;

        // What CLytManager::AddObject takes. Lives in the DOL. The id that
        // CLytManager::GetObject looks up is the first word. Polymorphic,
        // with the vptr at 0x10 after its data (its constructor, DOL
        // fn_80040D68, stores vtable lbl_80466930 there; the destructor
        // fn_80040DA4 is its first slot). Slots 0x0C to 0x14 are weak
        // inlines in the DOL (two empty ones and one returning 0.0f);
        // they are declared without bodies here.
        class CBase {
            public:
                CBase();

                unsigned long m_nId; // 0x00
                unsigned char m_Unk04[0xC];

                // vptr at 0x10
                virtual ~CBase();
                virtual void Unk0C();
                virtual void Unk10();
                virtual float GetSortZ();
        };

        // Lives in the DOL. Only the members used so far. The vtable is read
        // from CPaneCtrl's (eight entries); none of the virtuals has a name
        // in the game, so every name below is a guess. Argument lists and
        // return types of 0x14 to 0x20 are from their code (DOL
        // game/bcs/G2D/CLayoutBase.cpp).
        class CLayoutBase : public CBase {
            public:
                // (DOL fn_800A2524, no symbol; the CPaneCtrl constructor
                // calls it) Clears every member through Init.
                CLayoutBase();

                // vptr at 0x10 (CBase's). vtable __vt__Q33bcs3G2D11CLayoutBase.
                virtual ~CLayoutBase();
                // 0x0C, 0x10: empty overrides (weak inlines at the end of the
                // DOL's CLayoutBase.cpp).
                virtual void Unk0C() {}
                virtual void Unk10() {}
                // 0x14 (DOL fn_800A3194, no symbol) The z translation of the
                // root pane's global matrix; 0 without a root pane. Named
                // after the non-virtual SetSortZ.
                virtual float GetSortZ();
                // Called with (0, "<name>.brlyt"). Negative on failure
                // (msw::CCockpitTournamentTelopLogo::Set tests it). nUnk is
                // the resource type passed to the accessor.
                virtual long BuildLayout(long nUnk, const char *pLayoutName);
                // Called with (0, table of "<name>.brlan", entries). 0, or
                // -1 to -3 on failure.
                virtual long BuildAnim(long nUnk, const char **ppAnimNames, long nNum);
                // 0x20 (DOL fn_800A2DA4) Unbinds every transform of
                // animation nAnim from the layout. -1 for a bad index.
                virtual long UnbindAnim(long nAnim);
                // 0x24 (DOL fn_8003B958, a weak inline kept from CPaneCtrl.cpp)
                // Unbinds every animation from the layout. Called by BuildAnim
                // when binding fails and first thing by the destructor. Its
                // body (m_pLayout's Layout::UnbindAllAnimation) cannot be
                // written here: see CLayoutDL.hpp on nw4r/lyt/layout.h.
                virtual void UnbindAllAnim();

                // -1 to -3 on failure.
                long AddFontRefLink(nw4r::ut::Font *pFont, const char *pName);

                // (DOL game/bcs/G2D/CLayoutBase.cpp; no symbols, names guessed)
                // Detaches the archive links from a multi-archive accessor
                // and frees them (DOL fn_800A2840).
                void DetachArcResourceLinkAll();
                // Puts pLink in a free font slot and registers it with the
                // accessor unless it has that font already. The slot, or -2
                // when all four are taken (DOL fn_800A2978).
                long RegistFontRefLink(nw4r::lyt::FontRefLink *pLink);
                // Unregisters and frees every font link (DOL fn_800A2A60).
                void UnregistFontRefLinkAll();
                // The transform of animation nAnim bound to the layout, the
                // first one after pAfter when pAfter is not null
                // (DOL fn_800A2E44).
                nw4r::lyt::AnimTransform *FindAnimTransform(long nAnim, nw4r::lyt::AnimTransform *pAfter);
                // Animation nAnim, or null (DOL fn_800A2FB4).
                nw4r::lyt::AnimResource *GetAnimResource(long nAnim);
                // The animation pAnimTrans plays, or null (DOL fn_800A2FE0).
                nw4r::lyt::AnimResource *FindAnimResource(nw4r::lyt::AnimTransform *pAnimTrans);

                // Added for TourF game/tour/CCockpitTournamentMenuWinTrophy.cpp.
                // 0, or negative on failure (DOL game/bcs/G2D/CLayoutBase.cpp).
                long CreateLayoutObject(long nUnk);
                long AttachArchiveResource(nw4r::lyt::ArcResourceAccessor *pAccessor);
                CLayoutDL *GetLayout(); // the word at 0x14
                // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp.
                // Nonzero when animation nAnim has ended (return type guessed).
                int IsAnimEnd(long nAnim);
                // Number of frames of animation nAnim less one (its last
                // frame); -1 when there is no such animation
                // (main.dol msw::cockpit::CSPGauge::SetParam).
                float GetAnimFrameMax(long nAnim);
                // (added for VolleyF game/volley/CCockpitVolleyTelopServe.cpp)
                // Length of animation nAnim in frames; -1 when there is no
                // such animation.
                float GetAnimFrameSize(long nAnim);
                // (DOL fn_800A31BC, no symbol; name guessed) Nonzero when
                // animation nAnim exists and its resource has the loop flag.
                int IsAnimLoop(long nAnim);
                // (name guessed) Sets the z translation of the layout's root
                // pane (see GetSortZ).
                void SetSortZ(float fZ);

                // (name guessed; inline, no symbol) How many animations
                // BuildAnim made. GetAnimFrameMax checks its index against it.
                unsigned long GetAnimNum() const { return m_nAnimNum; }

            private:
                // (DOL fn_800A26B4, no symbol; name guessed) Clears 0x14 to
                // 0x47.
                void Init();
                // (inline; name guessed) Frees the accessor, its fonts and its
                // archive links. Shared by the destructor and
                // AttachArchiveResource.
                inline void ReleaseArchiveResource();

            protected:
                // Size 0x48: CPaneCtrl's own data starts there.
                CLayoutDL *m_pLayout;        // 0x14
                // 0x18. (names guessed from use) A plain ArcResourceAccessor,
                // or a MultiArcResourceAccessor when m_bMultiArc is set.
                nw4r::lyt::ResourceAccessor *m_pAccessor;
                nw4r::lyt::ArcResourceLink *m_pArcLink[4];   // 0x1C
                nw4r::lyt::FontRefLink *m_pFontRefLink[4];   // 0x2C
                nw4r::lyt::AnimResource *m_pAnim; // 0x3C: one per animation (name guessed)
                unsigned long m_nAnimNum;    // 0x40: entries of m_pAnim (name guessed)
                // 0x44, flags (Init clears the whole word). Bit 3 is read by
                // DodgeF's COutSignDodge::Main
                // (game/dodge/CCockpitDodgeTelopInOutChange.cpp) through
                // IsAnimStop; its meaning is a guess.
                union {
                    unsigned long m_nFlags;
                    struct {
                        unsigned long m_nUnk44Hi : 28;
                        unsigned long m_bAnimStop : 1;
                        // Bit 2 (name guessed): CLayoutObj::PlayAnim copies
                        // IsAnimLoop of the animation it starts here.
                        unsigned long m_bAnimLoop : 1;
                        unsigned long m_nUnk44Lo : 1;
                        // Bit 0 (name guessed): m_pAccessor is a
                        // MultiArcResourceAccessor. AttachArchiveResource
                        // clears it.
                        unsigned long m_bMultiArc : 1;
                    };
                };

            public:
                // (no symbol; name guessed) Bit 3 of the flags at 0x44.
                int IsAnimStop() const { return m_bAnimStop; }
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTBASE_HPP_ */
