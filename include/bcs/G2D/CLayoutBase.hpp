#ifndef BCS_G2D_CLAYOUTBASE_HPP_
#define BCS_G2D_CLAYOUTBASE_HPP_

namespace nw4r {
    namespace ut {
        class Font;
    }
}

namespace nw4r {
    namespace lyt {
        class ArcResourceAccessor;
    }
}

namespace bcs {
    namespace G2D {
        class CLayoutDL;

        // What CLytManager::AddObject takes. Lives in the DOL. The id that
        // CLytManager::GetObject looks up is the first word; the size of 0x10
        // is a guess that follows from CLayoutBase's vptr sitting at 0x10
        // (the vptr could as well be this class's own).
        class CBase {
            public:
                unsigned long m_nId; // 0x00
                unsigned char m_Unk04[0xC];
        };

        // Lives in the DOL. Only the members used so far. The vtable is read
        // from CPaneCtrl's (eight entries); none of the virtuals has a name
        // in the game, so every name, argument list and return type below is
        // a guess.
        class CLayoutBase : public CBase {
            public:
                // vptr at 0x10
                virtual ~CLayoutBase();
                virtual void Unk0C();
                virtual void Unk10();
                virtual void Unk14();
                // Called with (0, "<name>.brlyt"). Negative on failure
                // (msw::CCockpitTournamentTelopLogo::Set tests it).
                virtual long BuildLayout(long nUnk, const char *pLayoutName);
                // Called with (0, table of "<name>.brlan", entries).
                virtual void BuildAnim(long nUnk, const char **ppAnimNames, long nNum);
                virtual void Unk20();
                virtual void Unk24();

                // -1 to -3 on failure.
                long AddFontRefLink(nw4r::ut::Font *pFont, const char *pName);
                void SetSortZ(float fZ); // return type guessed (tail call)

                // Added for TourF game/tour/CCockpitTournamentMenuWinTrophy.cpp.
                // Return types of the first two are guesses.
                void CreateLayoutObject(long nUnk);
                void AttachArchiveResource(nw4r::lyt::ArcResourceAccessor *pAccessor);
                CLayoutDL *GetLayout(); // the word at 0x14
                // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp.
                // Nonzero when animation nAnim has ended (return type guessed).
                int IsAnimEnd(long nAnim);
                // Number of frames of animation nAnim; 0 when there is no
                // such animation (main.dol msw::cockpit::CSPGauge::SetParam).
                float GetAnimFrameMax(long nAnim);
                // (added for VolleyF game/volley/CCockpitVolleyTelopServe.cpp)
                // Length of animation nAnim in frames.
                float GetAnimFrameSize(long nAnim);

                // (name guessed; inline, no symbol) How many animations
                // BuildAnim made. GetAnimFrameMax checks its index against it.
                unsigned long GetAnimNum() const { return m_nAnimNum; }

            private:
                // Size 0x48: CPaneCtrl's own data starts there.
                CLayoutDL *m_pLayout;        // 0x14
                unsigned char m_Unk18[0x24]; // 0x18
                void *m_pAnim;               // 0x3C: 0x10 bytes per animation (name guessed)
                unsigned long m_nAnimNum;    // 0x40: entries of m_pAnim (name guessed)
                unsigned char m_Unk44[0x4];  // 0x44
        };
    }
}

#endif /* !BCS_G2D_CLAYOUTBASE_HPP_ */
