#ifndef MSW_CSTGGMK_PINBALL_MDL_HPP_
#define MSW_CSTGGMK_PINBALL_MDL_HPP_

#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgMdl.hpp"

namespace bcs {
    class CMdlObj;
    class CScnObj;
}

namespace msw {

    // The model of a ball. It rolls: SetMtx turns it by the distance it
    // moved over its radius, and GetMtx lifts it by its radius. While it
    // waits in the launcher it is carried on a node of the launcher's
    // model (SetAttach). (class name guessed; a DOL class, size 0x1A4, vtable
    // lbl_804854A8; the gimmick's NewMdl inlines its constructor; the names
    // of the new virtual functions are guesses)
    class CStgGmk_Pinball_Mdl : public CStgMdl {
        public:
            CStgGmk_Pinball_Mdl(CStgObj *pParent)
                : CStgMdl(pParent), m_pDebugObj(0), m_nUnk130(0), m_bMtxDirty(1), m_fUnk198(1.0f), m_pAttachObj(0)
            {
                PSMTXIdentity(m_RollMtx.m);
                PSMTXIdentity(m_DispMtx.m);
            }
            virtual ~CStgGmk_Pinball_Mdl();
            virtual void Create();
            virtual void Main(float rate);
            virtual void SetMtx(const bcs::FMTX34 &rMtx);
            virtual const bcs::FMTX34 &GetMtx() const;
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual void OnPostCreate();

            // A wire box of the ball's size round m_pDebugObj, in view nView.
            virtual void DrawDebug(long nView);                           // 0x124
            virtual void SetUnk130(long n) { m_nUnk130 = n; }             // 0x128
            virtual void Roll(const bcs::FVEC3 &rDir, float radius, float dist); // 0x12C
            virtual void SetUnk198(float f) { m_fUnk198 = f; }            // 0x130
            virtual float GetRadius() const;                              // 0x134
            virtual void SetAttach(bcs::CScnObj *pObj, long node);        // 0x138
            virtual void ClearAttach() { SetAttach(0, 0); }               // 0x13C
            virtual long GetUnk130() const { return m_nUnk130; }          // 0x140

        private:
            bcs::CScnObj *m_pDebugObj;      // 0x12C (never set in the DOL)
            long m_nUnk130;                 // 0x130
            bcs::FMTX34 m_RollMtx;          // 0x134, the rotation from rolling
            mutable bcs::FMTX34 m_DispMtx;  // 0x164, the matrix GetMtx returns
            mutable int m_bMtxDirty;        // 0x194
            float m_fUnk198;                // 0x198
            bcs::CScnObj *m_pAttachObj;     // 0x19C, the model carrying the ball
            long m_nAttachNode;             // 0x1A0
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_MDL_HPP_ */
