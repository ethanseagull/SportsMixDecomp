#ifndef BCS_G2D_CLYTMANAGER_HPP_
#define BCS_G2D_CLYTMANAGER_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/ut/List.hpp"
#include "revolution/gx/GXEnum.h"
#include "revolution/mem.h"
#include "revolution/mtx.h"

namespace nw4r {
    namespace lyt {
        class DrawInfo;
    }
    namespace ut {
        struct Rect;
    }
}

namespace bcs {
    namespace G2D {
        class CBase;

        // Lives in the DOL (game/bcs/G2D/CLytManager.cpp). A task of size
        // 0x50 (CreateInstance allocates it); vtable lbl_80466958 holds the
        // destructor, Main, Disp and CTask's GetFrameRateF. Function names
        // without a symbol in the game are guessed.
        class CLytManager : public CTask {
            public:
                // A registered object (0x14 bytes, kept in the list at 0x34).
                struct ObjNode {
                    nw4r::ut::Link m_Link;  // 0x00
                    unsigned long m_nId;    // 0x08, the object's id
                    CBase *m_pObj;          // 0x0C
                    unsigned long m_nMask;  // 0x10, 1 << the bit given to AddObject
                };

                // (name guessed) A list link handed out by NewLink; the
                // callers append it to their own lists. 0xC bytes, from a
                // pool of 0x3000 bytes that Disp empties every frame.
                struct LinkNode {
                    nw4r::ut::Link m_Link;  // 0x00
                    void *m_pData;          // 0x08
                };

                static CLytManager *GetInstance();
                // (name guessed; the DOL's fn_8003F418) Creates the instance
                // kept in the DOL's sbss when there is none and returns it.
                static CLytManager *CreateInstance();
                // (name guessed) Asks the task manager to delete the
                // instance and forgets it.
                static void DestroyInstance();
                // (name guessed; the DOL's fn_8003F580, added for
                // game/msw/CNowLoading.cpp) The allocator for nw4r::lyt,
                // set up from a heap on the first call.
                static MEMAllocator *GetAllocator();

                virtual ~CLytManager();
                // (name from the CTask slot) Updates every registered
                // object that is not hidden or waiting.
                virtual void Main();
                // (name from the CTask slot) Hands the link pool back.
                virtual void Disp();

                // (name guessed) Deletes every registered object.
                void DeleteObjectAll();
                // Added for TourF game/tour/CCockpitTournamentMenuWinTrophy.cpp.
                // Returns 1 when the object was added; nBit is a bit number.
                int AddObject(CBase *pObj, long nBit);
                // (name guessed) Unregisters and deletes pObj; 1 when it was
                // registered.
                int DeleteObject(CBase *pObj);
                // Null when no object has that id. The return type is a
                // guess (the function reads a pointer out of a list node).
                CBase *GetObject(unsigned long nId);

                // (name guessed) Fills pDrawInfo and the projection for
                // drawing a layout of rectangle *pRect in view nView (the
                // whole screen when negative), then sets the projection
                // through GX ('GX') or into the current display list ('GD').
                long SetupDraw(nw4r::lyt::DrawInfo *pDrawInfo, const nw4r::ut::Rect *pRect, long nView, unsigned long nGxType);
                // (name guessed) The DrawInfo and projection part of
                // SetupDraw. Returns 0.
                long CalcDrawInfo(nw4r::lyt::DrawInfo *pDrawInfo, const nw4r::ut::Rect *pRect, long nView, Mtx44 projMtx);
                // (name guessed) Sets the projection and the 2D pixel state
                // through GX ('GX') or GD ('GD').
                void SetProjection(GXProjectionType type, Mtx44 projMtx, unsigned long nGxType);

                // Added for game/msw/cockpit/CMissionCount.cpp (C3DCoin): the
                // scale that puts a location-adjusted pane in place, for the
                // aspect mode bAdjust. The vector (the DOL's fn_8003FE7C,
                // name guessed; written to pScale, which the function takes
                // after `this`) has the horizontal scale in x and 1 in y and
                // z; GetLocationAdjustScaleH returns that scale alone.
                void GetLocationAdjustScale(FVEC3 *pScale, int bAdjust);
                float GetLocationAdjustScaleH(int bAdjust);
                // (added for game/msw/CNowLoading.cpp) The vertical
                // counterpart of GetLocationAdjustScaleH.
                float GetLocationAdjustScaleV(int bAdjust);

                // (name guessed) The next link of the pool, holding pData.
                LinkNode *NewLink(void *pData);
                // (names guessed) For every object: hide it once more
                // (bHide 1) or once less; and add nAdd to its wait count
                // (not below 0). nLayer is not used.
                void SetHideAll(long nLayer, int bHide);
                void AddWaitAll(long nLayer, long nAdd);

                // (name guessed) Bit 0 of the word at 0x44, what C3DCoin
                // passes to both.
                int GetAdjustMode() const { return m_nFlag & 1; }

            private:
                inline CLytManager();
                inline ObjNode *FindNode(unsigned long nId);
                inline void DeleteNodeAll();

                nw4r::ut::List m_ObjList; // 0x34, of ObjNode
                long m_Unk40;             // 0x40, set to 6
                unsigned long m_nFlag;    // 0x44 (name guessed): bit 0 the 16:9 location adjust
                LinkNode *m_pLinkPool;    // 0x48
                long m_nLinkNum;          // 0x4C
        };
    }
}

#endif /* !BCS_G2D_CLYTMANAGER_HPP_ */
