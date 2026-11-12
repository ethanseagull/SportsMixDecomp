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

        // A font registered with a resource accessor under a name. Size 0x8C
        // (what bcs::G2D::CLytResource allocates and game/msw/CNowLoading.cpp
        // allocates). Members added for DOL game/bcs/G2D/CLayoutBase.cpp.
        class FontRefLink {
            public:
                FontRefLink();
                // Empty and inline: game/msw/CNowLoading.cpp's delete
                // tests the pointer before freeing it, which a class
                // without a destructor would not.
                ~FontRefLink() {}
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

        // What msw::CLytCtrl::NewArcResourceAccessor returns. 0xB0 bytes
        // (game/msw/CNowLoading.cpp allocates it).
        class ArcResourceAccessor : public ResourceAccessor {
            public:
                // (the DOL's fn_8042A2F0)
                ArcResourceAccessor();
                virtual ~ArcResourceAccessor();
                virtual void *GetResource(unsigned long nType, const char *pName, unsigned long *pSize);
                virtual ut::Font *GetFont(const char *pName);
                // (the DOL's fn_8042A350) Opens the archive at pArchive with
                // pRootDir as its root folder; false when it is not one.
                bool Attach(void *pArchive, const char *pRootDir);
                // (the DOL's fn_8042A3D0) Forgets the archive and returns it.
                void *Detach();
                void RegistFont(FontRefLink *pLink);
                // (the DOL's fn_8042A450)
                void UnregistFont(FontRefLink *pLink);

            private:
                unsigned char m_Unk04[0xAC]; // 0x04
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
