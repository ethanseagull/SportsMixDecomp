#ifndef NW4R_LYT_RESOURCEACCESSOR_HPP_
#define NW4R_LYT_RESOURCEACCESSOR_HPP_

#include "nw4r/ut/LinkList.h"

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

        // (added for DOL game/bcs/G2D/CLayoutBase.cpp) A font registered
        // with a resource accessor under a name. Size 0x8C (what
        // bcs::G2D::CLytResource allocates).
        class FontRefLink {
            public:
                FontRefLink();
                void Set(const char *pName, ut::Font *pFont);
                const char *GetFontName() const { return mFontName; }

                ut::LinkListNode mLink; // 0x00

            private:
                char mFontName[128];    // 0x08
                ut::Font *mpFont;       // 0x88
        };

        // (added for DOL game/bcs/G2D/CLayoutBase.cpp) One archive of a
        // MultiArcResourceAccessor. Not mapped.
        class ArcResourceLink;

        // What msw::CLytCtrl::NewArcResourceAccessor returns.
        class ArcResourceAccessor : public ResourceAccessor {
            public:
                // (added for DOL game/bcs/G2D/CLayoutBase.cpp)
                void RegistFont(FontRefLink *pLink);
                void UnregistFont(FontRefLink *pLink);
        };

        // (added for DOL game/bcs/G2D/CLayoutBase.cpp) An accessor over
        // several archives.
        class MultiArcResourceAccessor : public ResourceAccessor {
            public:
                void Detach(ArcResourceLink *pLink);
                void RegistFont(FontRefLink *pLink);
                void UnregistFont(FontRefLink *pLink);
        };
    }
}

#endif /* !NW4R_LYT_RESOURCEACCESSOR_HPP_ */
