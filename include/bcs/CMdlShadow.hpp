#ifndef BCS_CMDLSHADOW_HPP_
#define BCS_CMDLSHADOW_HPP_

#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "revolution/gx/GXTexture.h"

namespace bcs {

    // A model drawn as the shadow of another. A DOL class, size 0x204.
    // Only what the modules use so far is declared.
    class CMdlShadow : public CMdlObj {
        public:
            CMdlShadow();
            // Builds the shadow of pMdl at once. (meanings of the last three
            // arguments not known; msw::CItemBall passes 0x20, 0, 0)
            CMdlShadow(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, long, unsigned long, long);
            virtual ~CMdlShadow();

            // Builds the model and a square nTexSize x nTexSize I8 texture
            // for the shadow. The last two arguments go to CMdlObj::Create;
            // nHeap also selects the heap of the texture buffer. Returns 0
            // if the model could not be built.
            int Create(nw4r::g3d::ResMdl mdl, long nTexSize, unsigned long, long nHeap);
            // Follows the pose of pMdl.
            void SetLink(const CMdlObj *pMdl);
            // (0x80060C04, unnamed in the DOL; name and meaning guessed)
            // Sets the byte at 0x1CC; msw::CDropCoin clears it for a coin
            // that rises out of the ground.
            void SetEnable(bool bEnable);
            // (0x80060C0C and 0x80060C4C, unnamed in the DOL; names guessed)
            // The shadow's alpha, 0 to 1; msw::CEvDemoMdlChr fades it with
            // the model.
            void SetAlpha(float fAlpha);
            float GetAlpha() const;
            // (name guessed) Sets the float at 0x200, the height of the
            // floor the shadow falls on (msw::CEventModel sets it from its
            // floor object).
            void SetFloorY(float fY) { m_Unk200 = fY; }

            // (DOL, 0x80060A14; vtable 0x80467710 slot 0x1C) Not written yet.
            virtual void ObjProc(unsigned long, unsigned long, void *);
            // (weak in the DOL at 0x80065ADC; slot 0x0C)
            virtual long GetObjType() const { return 5; }

        private:
            // (DOL 0x80060344, name guessed) Rebuilds the per-node data of
            // the model followed (m_pNodeData). Not written yet.
            void BuildLink(const CMdlObj *pMdl);

            // (names guessed from use)
            unsigned long m_nLinkHandle;  // 0x1C8: handle of the model followed
            unsigned char m_nAlpha;       // 0x1CC: 0xFF after construction
            unsigned char m_Unk1CD;       // 0x1CD
            short m_nTexSize;             // 0x1CE
            unsigned long m_Unk1D0;       // 0x1D0
            void *m_pNodeData;            // 0x1D4: new[], rebuilt by BuildLink
            void *m_pTexBuf;              // 0x1D8: image of m_TexObj
            GXTexObj *m_pTexObj;          // 0x1DC: points at m_TexObj
            GXTexObj m_TexObj;            // 0x1E0
            float m_Unk200;               // 0x200
    };
}

#endif /* !BCS_CMDLSHADOW_HPP_ */
