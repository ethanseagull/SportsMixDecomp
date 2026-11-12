#ifndef NW4R_LYT_RESOURCEACCESSOR_HPP_
#define NW4R_LYT_RESOURCEACCESSOR_HPP_

namespace nw4r {
    namespace ut {
        class Font;
    }

    namespace lyt {

        // Only the virtuals the game calls so far, in vtable order (the
        // destructor at 0x08, GetResource at 0x0C). Shape of the NW4R
        // layout library's resource accessors; no members are needed yet.
        class ResourceAccessor {
            public:
                virtual ~ResourceAccessor();
                // The resource called pName, or null; *pSize gets its size
                // when pSize is not null.
                virtual void *GetResource(unsigned long nType, const char *pName, unsigned long *pSize) = 0;
                virtual ut::Font *GetFont(const char *pName);
        };

        // A font registered with an ArcResourceAccessor under a name. Size
        // 0x8C, no destructor (added for MenuF game/menu/CBgDisplay.cpp;
        // ogws names, from memory, no code copied).
        class FontRefLink {
            public:
                FontRefLink();
                // Empty; the game deletes links with a null check of its own
                // (MenuF game/menu/CBgDisplay.cpp).
                ~FontRefLink() {}

                void Set(const char *pName, ut::Font *pFont);

            private:
                void *mLink[2];          // 0x00, list links
                char mFontName[0x80];    // 0x08
                ut::Font *mpFont;        // 0x88
        };

        // What msw::CLytCtrl::NewArcResourceAccessor returns.
        class ArcResourceAccessor : public ResourceAccessor {
            public:
                // Added for MenuF game/menu/CBgDisplay.cpp (out of line in the DOL).
                void RegistFont(FontRefLink *pLink);
        };
    }
}

#endif /* !NW4R_LYT_RESOURCEACCESSOR_HPP_ */
