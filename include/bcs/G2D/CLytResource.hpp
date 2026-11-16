#ifndef BCS_G2D_CLYTRESOURCE_HPP_
#define BCS_G2D_CLYTRESOURCE_HPP_

#include "bcs/CLoadMan.hpp"
#include "nw4r/ut/List.hpp"
namespace nw4r {
    namespace lyt {
        class ArcResourceAccessor;
        class ArcResourceLink;
        class FontRefLink;
        class MultiArcResourceAccessor;
        class ResourceAccessor;
    }
}

namespace bcs {
    namespace G2D {

        // Lives in the DOL (game/bcs/G2D/CLytResource.cpp). Size 0x4C
        // (CreateInstance allocates it), vtable lbl_80466948 with the
        // destructor only. Keeps the layout archives read from the disc and
        // every NW4R resource object it hands out, in nw4r::ut::Lists of
        // small nodes, so that it can free them all.
        class CLytResource {
            public:
                // An archive read from the disc (0x94 bytes, list 0x00).
                struct DataNode {
                    nw4r::ut::Link m_Link;  // 0x00
                    void *m_pData;          // 0x08
                    char m_szName[0x80];    // 0x0C, the file path
                    unsigned long m_nSize;  // 0x8C
                    // 0x90: bit 0 set when the data belongs to CLoadMan
                    // (GetResourceFromDvdMan sets it); otherwise it is ours
                    // to free with delete[].
                    unsigned char m_nFlags;
                };

                // A resource object handed out (0xC bytes, lists 0x0C..0x3C).
                template <typename T>
                struct ObjNode {
                    nw4r::ut::Link m_Link;  // 0x00
                    T *m_pObj;              // 0x08
                };

                static CLytResource *GetInstance();
                // (the DOL's fn_80040104; name guessed from CLytManager)
                // Creates the instance when there is none and returns it.
                static CLytResource *CreateInstance();
                // (the DOL's fn_800401A8; name guessed from CLytManager)
                static void DestroyInstance();

                // (the DOL's fn_800401F0; name guessed) Deletes every
                // object and archive kept; the destructor calls it.
                void Clear();

                // Releases the archive loaded under pName.
                void Cleanup(const char *pName);
                // (the DOL's fn_80040A2C; name guessed) Releases every
                // archive.
                void CleanupAll();
                // (added for HockeyF game/hockey/CRadarIcon2DHockeyGoal.cpp)
                // An accessor for folder pRootFolderName of loaded archive
                // pFileName; null on failure.
                nw4r::lyt::ArcResourceAccessor *NewArcResourceAccessor(const char *pFileName, const char *pRootFolderName);
                // Added for MenuF game/menu/CMenu.cpp (the menu passes 0 as
                // the second argument). Takes file pPath (CLoadMan id nId)
                // into the archive list once CLoadMan has it: its index
                // there, or -1 when it is not loaded.
                long GetResourceFromDvdMan(const char *pPath, unsigned long nId);
                // (added for MenuF game/menu/CSelectMenu.cpp) Loading an
                // archive in the background (0 when it started, -1 without
                // a file, -2 when CLoadMan refused), and its state: negative
                // while it is not complete, else its archive index.
                long LoadDataASync(const CLoadMan::FILE_INPUT *pFile);
                long IsCompleteDataASync(const CLoadMan::FILE_INPUT *pFile);

                // Added for DOL game/bcs/G2D/CLayoutBase.cpp. A new font
                // link, kept in the list at 0x30; null when out of memory.
                nw4r::lyt::FontRefLink *NewFontRefLink();
                // Each deletes an object the instance made and drops it from
                // its list (0x0C, 0x18, 0x24, 0x30). Static: they read the
                // instance themselves and do nothing without one.
                static void DeleteArcResourceAccessor(nw4r::lyt::ArcResourceAccessor *pAccessor);
                static void DeleteArcResourceLink(nw4r::lyt::ArcResourceLink *pLink);
                static void DeleteMultiArcResourceAccessor(nw4r::lyt::MultiArcResourceAccessor *pAccessor);
                static void DeleteFontRefLink(nw4r::lyt::FontRefLink *pLink);

            private:
                inline void *FindData(const char *pName);
                inline DataNode *FindDataNode(const char *pName);
                inline void DeleteDataNode(DataNode *pNode);

                nw4r::ut::List m_DataList;           // 0x00, DataNode
                nw4r::ut::List m_ArcAccessorList;    // 0x0C, ObjNode<ArcResourceAccessor>
                nw4r::ut::List m_ArcLinkList;        // 0x18, ObjNode<ArcResourceLink>
                nw4r::ut::List m_MultiAccessorList;  // 0x24, ObjNode<MultiArcResourceAccessor>
                nw4r::ut::List m_FontRefLinkList;    // 0x30, ObjNode<FontRefLink>
                // 0x3C: objects with a virtual destructor that nothing in
                // the file adds (type guessed).
                nw4r::ut::List m_AccessorList;

                // The instance (lbl_805C0880; no symbol in the game, name
                // guessed).
                static CLytResource *s_pInstance;

            public:
                // (the DOL's fn_800400A4) Declared after the lists: the
                // vptr is at 0x48.
                virtual ~CLytResource();
        };
    }
}

#endif /* !BCS_G2D_CLYTRESOURCE_HPP_ */
