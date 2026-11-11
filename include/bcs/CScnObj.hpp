#ifndef BCS_CSCNOBJ_HPP_
#define BCS_CSCNOBJ_HPP_

#include "bcs/CG3dObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/math/types.h"
#include "revolution/gx/GXEnum.h"

namespace bcs {

    struct BCS_Color;
    class CHandleTable;

    // An object in the 3D scene. Size 0x118 (the members of bcs::CMdlObj
    // start there). A DOL class. The virtual functions are declared in the
    // order of the vtable of bcs::CMdlObj. Return types are read from the DOL
    // bodies unless marked.
    class CScnObj : public CG3dObj {
        public:
            // (enumerator names guessed) ForEach returns whether the
            // callback returned 1.
            enum ForEachResult {
                FOREACHRESULT_0,
                FOREACHRESULT_1
            };

            typedef ForEachResult (*ForEachFunc)(CScnObj *, void *);

            // Named in the mangled name of SetAlphaMode; the values are not
            // known yet.
            enum AlphaMode {
                ALPHAMODE_0,
                ALPHAMODE_1 // msw::CTournamentChip sets it on its model
            };

            // How a model follows the one it is attached to (the type of the
            // last parameter of msw::CStgMdl::AttachToMdl). Values not known;
            // msw::CTournamentChip::Destroy passes 0 with no parent.
            enum AttachMode {
                ATTACHMODE_0,
                ATTACHMODE_1, // msw::CTournamentMan (TourF) attaches its markers with 1 and 3
                ATTACHMODE_2,
                ATTACHMODE_3
            };

            // Which matrix GetMtx copies. (enumerator names guessed) 0 is the
            // one at 0x08 that CalcRootMtx builds, 1 the one at 0x38, 2 the
            // one at 0x68 that CalcViewMtx builds.
            enum ScnObjMtxType {
                SCNOBJMTXTYPE_ROOT,
                SCNOBJMTXTYPE_1,
                SCNOBJMTXTYPE_VIEW
            };

            // Bits of m_nFlag. (names guessed) msw::CTableSetMdlObj::SetVisible
            // sets both to hide a model and clears both to show it.
            enum {
                FLAG_HIDE_40 = 0x40,
                FLAG_HIDE_80 = 0x80
            };

            CScnObj();
            virtual ~CScnObj();
            // 0x0C: 1 for a plain scene object (inline; the original's weak
            // copy is at 0x8001E508).
            virtual long GetObjType() const { return 1; }
            virtual void ObjProc(unsigned long, unsigned long, void *);    // 0x1C
            virtual int ForEach(ForEachFunc pFunc, void *pArg, int);       // 0x20
            virtual float GetValueForSortOpa() const;                      // 0x24
            virtual float GetValueForSortXlu() const;                      // 0x28
            virtual void CalcBoundingVolume();                             // 0x2C
            virtual void CalcWorldMtx(const FMTX34 *);                     // 0x30 (return type guessed)
            virtual void GetNodeLocalMtx(unsigned long, FMTX34 &) const;   // 0x34 (return type guessed)
            virtual const FMTX34 *GetNodeLocalMtxPtr(unsigned long) const; // 0x38 (const result guessed)
            virtual void GetNodeWorldMtx(unsigned long, FMTX34 &) const;   // 0x3C (return type guessed)
            // 0x10 and 0x40 to 0x10C: names from the vtables of bcs::CScnObj
            // in the DOL and of msw::SelectMenu::CCourtBallMdlObj in MenuF
            // (which names bcs::CMdlObj's overrides). The ones from 0x54 on
            // without a DOL name are one-line bodies there; the material
            // setters are empty in bcs::CScnObj. Return types guessed.
            virtual int IsScnObj() const;                                                   // 0x10
            virtual const FMTX34 *GetNodeWorldMtxPtr(unsigned long) const;                  // 0x40
            virtual void SetNodeLocalMtx(unsigned long, const FMTX34 &);                    // 0x44
            virtual void SetNodeWorldMtx(unsigned long, const FMTX34 &);                    // 0x48
            virtual void SetAlphaRate(unsigned char);                                       // 0x4C
            virtual void SetAlphaRateF(float);                                              // 0x50
            virtual void SetAlphaMode(AlphaMode);                                           // 0x54
            virtual unsigned char GetAlphaRate() const;                                     // 0x58
            virtual float GetAlphaRateF() const;                                            // 0x5C
            virtual AlphaMode GetAlphaMode() const;                                         // 0x60
            virtual void SetExBlendMode(int);                                               // 0x64
            virtual long GetEffectRootNodeIdx() const;                                      // 0x68
            virtual void ExEffectDraw(int, unsigned long, void *);                          // 0x6C
            virtual void DrawJointObjOpa(void *);                                           // 0x70
            virtual void DrawJointObjXluExOpa(void *);                                      // 0x74
            virtual void DrawJointObjXluExXlu(void *);                                      // 0x78
            virtual void SetMatTevColor(_GXTevRegID, BCS_Color);                            // 0x7C
            virtual void SetMatTevColor(const char *, _GXTevRegID, BCS_Color);              // 0x80
            virtual void SetMatTevColorRGB(_GXTevRegID, BCS_Color);                         // 0x84
            virtual void SetMatTevColorRGB(const char *, _GXTevRegID, BCS_Color);           // 0x88
            virtual void SetMatTevColorA(_GXTevRegID, unsigned char);                       // 0x8C
            virtual void SetMatTevColorA(const char *, _GXTevRegID, unsigned char);         // 0x90
            // 0x94 to 0x108: names and parameters from the slots of the
            // vtable of msw::CFgMdl in the DOL (bcs::CMdlObj's functions).
            // Return types guessed. The vtable ends at 0x108: bcs::CScnGroup's
            // has 67 words and a class derived from it adds its own
            // virtuals from 0x10C (lbl_804666F8); the zero word after
            // bcs::CScnObj's vtable in the DOL is alignment padding.
            virtual void RestoreMatTevColor(_GXTevRegID);                                   // 0x94
            virtual void RestoreMatTevColor(const char *, _GXTevRegID);                     // 0x98
            virtual void SetMatTevKColor(_GXTevKColorID, BCS_Color);                        // 0x9C
            virtual void SetMatTevKColor(const char *, _GXTevKColorID, BCS_Color);          // 0xA0
            virtual void SetMatTevKColorRGB(_GXTevKColorID, BCS_Color);                     // 0xA4
            virtual void SetMatTevKColorRGB(const char *, _GXTevKColorID, BCS_Color);       // 0xA8
            virtual void SetMatTevKColorA(_GXTevKColorID, unsigned char);                   // 0xAC
            virtual void SetMatTevKColorA(const char *, _GXTevKColorID, unsigned char);     // 0xB0
            virtual void RestoreMatTevKColor(_GXTevKColorID);                               // 0xB4
            virtual void RestoreMatTevKColor(const char *, _GXTevKColorID);                 // 0xB8
            virtual void SetMatPixDstAlpha(unsigned char, unsigned char);                   // 0xBC
            virtual void SetMatPixDstAlpha(const char *, unsigned char, unsigned char);     // 0xC0
            virtual void SetMatFogIdx(long);                                                // 0xC4
            virtual void SetMatFogIdx(const char *, long);                                  // 0xC8
            virtual void SetMatChanMatColor(_GXChannelID, BCS_Color);                       // 0xCC
            virtual void SetMatChanMatColor(const char *, _GXChannelID, BCS_Color);         // 0xD0
            virtual void SetMatChanMatColorRGB(_GXChannelID, BCS_Color);                    // 0xD4
            virtual void SetMatChanMatColorRGB(const char *, _GXChannelID, BCS_Color);      // 0xD8
            virtual void SetMatChanMatColorA(_GXChannelID, unsigned char);                  // 0xDC
            virtual void SetMatChanMatColorA(const char *, _GXChannelID, unsigned char);    // 0xE0
            virtual void RestoreMatChanMatColor(_GXChannelID);                              // 0xE4
            virtual void RestoreMatChanMatColor(const char *, _GXChannelID);                // 0xE8
            virtual void SetMatChanAmbColor(_GXChannelID, BCS_Color);                       // 0xEC
            virtual void SetMatChanAmbColor(const char *, _GXChannelID, BCS_Color);         // 0xF0
            virtual void SetMatChanAmbColorRGB(_GXChannelID, BCS_Color);                    // 0xF4
            virtual void SetMatChanAmbColorRGB(const char *, _GXChannelID, BCS_Color);      // 0xF8
            virtual void SetMatChanAmbColorA(_GXChannelID, unsigned char);                  // 0xFC
            virtual void SetMatChanAmbColorA(const char *, _GXChannelID, unsigned char);    // 0x100
            virtual void RestoreMatChanAmbColor(_GXChannelID);                              // 0x104
            virtual void RestoreMatChanAmbColor(const char *, _GXChannelID);                // 0x108

            // fn_8004196C (name guessed). Creates the handle table and the
            // delayed-delete list; called once at start-up.
            static void Initialize();

            // The object registered under a handle, or null.
            static CScnObj *GetScnObj(unsigned long handle);

            // Queues the object to be deleted later.
            static void AppendDelayDeleteObject(CScnObj *pObj);
            // fn_80041A64 (name guessed). Deletes the queued objects. Unless
            // bAll is set, an object with flag 0x100000 or 0x200000 stays
            // queued.
            static void ExecDelayDeleteObject(int bAll);

            // fn_80041CBC (name guessed). Sets (bOn) or clears flag.
            void SetFlag(unsigned long flag, int bOn);

            // fn_80041DC4 (name guessed). Copies *pMtx to the matrix `type`,
            // or sets it to identity when pMtx is null.
            void SetMtx(ScnObjMtxType type, const FMTX34 *pMtx);
            void SetMtx(ScnObjMtxType type, const FMTX34 &rMtx);
            // fn_80041E0C (name guessed). Copies the matrix `type` to *pMtx.
            void GetMtx(ScnObjMtxType type, FMTX34 *pMtx) const;
            // In the DOL. Copies the matrix `type` to rMtx.
            void GetMtx(ScnObjMtxType type, FMTX34 &rMtx) const;

            // Makes the object follow node `nNode` of pParent; a null parent
            // detaches it. (parameter names and return type guessed)
            void AttachToObject(const CScnObj *pParent, long nNode, AttachMode mode);
            // (meaning of the two values guessed: the opaque and the
            // translucent pass) A negative value leaves that one unchanged.
            void SetDrawPriority(long nOpa, long nXlu);
            // (meaning guessed) The opaque pass priority alone.
            void SetDrawPriorityOpa(long nOpa);
            void SetDrawPriorityXlu(long nXlu);

            // fn_80041F24 and fn_80041FB0 (names guessed). The bounding box,
            // brought up to date first when flag 0x20000 says it is stale.
            // The first argument of the first one is not read (callers pass
            // 0 or 1).
            void GetBoundingBox(long, nw4r::math::AABB *pBox);
            void GetBoundingBox(nw4r::math::AABB *pBox);

            // fn_80042040 (name guessed after msw::CChrCore::RotateYTo_).
            // Turns the Y angle towards fTarget by fStep the short way round;
            // returns 1 once it is reached.
            int RotateYTo(float fTarget, float fStep);
            // fn_80042128 (name guessed after msw::CChrCore::RotateYRate_).
            // Turns the Y angle by fRate of the way to fTarget.
            void RotateYRate(float fTarget, float fRate);

            // Rebuilds the matrix at 0x08 from the rotation and the
            // translation members.
            void CalcRootMtx();
            // fn_80042260 (name guessed). The world matrix of the node this
            // object is attached to, or null; forgets an attachment whose
            // object is gone.
            const FMTX34 *GetAttachMtx();
            // fn_800424A0, unnamed in the binary (name guessed). Concatenates
            // pViewMtx with the matrix at 0x38 into the one at 0x68, then
            // calls CalcSortValue. ObjProc runs it for task 4.
            void CalcViewMtx(const FMTX34 *pViewMtx);
            // fn_800424F0 (name guessed). The depth used to sort the object:
            // from the bounding box seen through pViewMtx when flags 0x40000
            // and 0x10000 are set, else the view translation's z.
            void CalcSortValue(const FMTX34 *pViewMtx);

            // How CalcSortValue reads the box: 1 its nearest corner, 2 its
            // farthest, anything else its centre. (meaning guessed)
            void SetBBoxZSortCalcType(long nType);

            // Chains pObj to the end of the joint list; null clears the list.
            void SetJointObj(CScnObj *pObj);
            // Queues every object of the joint list for deletion.
            void DeleteJointObj();
            // fn_80042BDC and fn_80042CBC (names guessed). Whether this
            // object or one in its joint list is drawn in the opaque /
            // translucent pass.
            int IsDrawOpaJoint() const;
            int IsDrawXluJoint() const;
            // fn_80042CA8 and fn_80042D88 (names guessed). Whether the
            // object itself is drawn in that pass.
            int IsDrawOpa() const;
            int IsDrawXlu() const;

            // (name guessed) Whether task nTask of ObjProc is switched off:
            // bit 0 of the flags switches off tasks 0 to 8, bit n task n.
            bool IsProcDisabled(unsigned long nTask) const
            {
                if (nTask < 9 && (((1 << nTask) | 1) & m_nFlag)) {
                    return true;
                }
                return false;
            }
            // (name guessed)
            unsigned long GetFlag() const { return m_nFlag; }
            // (name guessed) Whether any bit of flag is set. Inline in
            // CalcSortValue (the double cntlzw of a bool result).
            bool TestFlag(unsigned long flag) const { return (m_nFlag & flag) != 0; }
            // (names guessed) The draw priorities SetDrawPriority stores.
            // bcs::CScnObjGather compares them as signed longs.
            long GetDrawPriorityOpa() const { return m_nDrawPriorityOpa; }
            long GetDrawPriorityXlu() const { return m_nDrawPriorityXlu; }
            // (name guessed) The handle GetScnObj takes.
            unsigned long GetHandle() const { return m_nHandle; }

            // (names guessed) Inline in the modules.
            void OnFlag(unsigned long flag) { m_nFlag |= flag; }
            void OffFlag(unsigned long flag) { m_nFlag &= ~flag; }
            // (name and meaning guessed) Sets or clears both of the bits
            // 0x10000000 and 0x20000000; HockeyF's CStageHockey01 does it to
            // switch its audience models with the camera.
            void SetHide(int bHide)
            {
                if (bHide) {
                    OnFlag(0x10000000);
                } else {
                    OffFlag(0x10000000);
                }
                if (bHide) {
                    OnFlag(0x20000000);
                } else {
                    OffFlag(0x20000000);
                }
            }
            // (name guessed) Inline in the modules: msw::CChrMarkerDodgeArrow
            // stores a yaw in radians to 0xB4, brought into [0, 2 pi] after
            // the model pointer is loaded, so the wrapping is taken to be
            // part of the setter.
            void SetRotY(float fRotY)
            {
                while (fRotY > 6.2831855f) {
                    fRotY -= 6.2831855f;
                }
                while (fRotY < 0.0f) {
                    fRotY += 6.2831855f;
                }
                m_fRotY = fRotY;
            }
            // (name guessed) The same for the X angle (0xB0), as
            // msw::CItemBallShellWreck::Main stores it.
            void SetRotX(float fRotX)
            {
                while (fRotX > 6.2831855f) {
                    fRotX -= 6.2831855f;
                }
                while (fRotX < 0.0f) {
                    fRotX += 6.2831855f;
                }
                m_fRotX = fRotX;
            }
            // (name guessed) The same for the Z angle (0xB8), as
            // MenuF's msw::SelectMenu::CCupChr::StepControl turns it.
            void SetRotZ(float fRotZ)
            {
                while (fRotZ > 6.2831855f) {
                    fRotZ -= 6.2831855f;
                }
                while (fRotZ < 0.0f) {
                    fRotZ += 6.2831855f;
                }
                m_fRotZ = fRotZ;
            }

            // (name guessed) Sets the three angles at once; msw::CDropCoin
            // stores them through one load of the model pointer.
            void SetTransOffset(const FVEC3 &rOffset) { m_vTransOffset = rOffset; }
            // (name guessed) SetRotX, SetRotY and SetRotZ in one.
            void SetRotXYZ(float fRotX, float fRotY, float fRotZ)
            {
                SetRotX(fRotX);
                SetRotY(fRotY);
                SetRotZ(fRotZ);
            }

            void SetRot(float fRotX, float fRotY, float fRotZ)
            {
                m_fRotX = fRotX;
                m_fRotY = fRotY;
                m_fRotZ = fRotZ;
            }

        private:
            // fn_800429B4 (name guessed). Unchains the joint list.
            void ClearJointObj()
            {
                if (m_pJointObj != 0) {
                    m_pJointObj->ClearJointObj();
                }
                m_pJointObj = 0;
            }

        protected:
            // (names guessed) Indexed by ScnObjMtxType: the root matrix
            // CalcRootMtx builds, the world matrix CalcWorldMtx builds and
            // the view matrix CalcViewMtx builds.
            FMTX34 m_Mtx[3];              // 0x08

        public:
            // (name guessed) The translation CalcRootMtx reads.
            FVEC3 m_vTrans;               // 0x98

            // 0xA4 (name guessed): an offset added to m_vTrans; BasketF's
            // ghost bobs its model up and down with it.
            FVEC3 m_vTransOffset;

        public:
            // (names guessed) The rotation about the three axes in radians,
            // as CalcRootMtx reads it. msw::CChrMarkerVolleyJump::MainFunc
            // turns the Y one directly.
            float m_fRotX;                // 0xB0
            float m_fRotY;                // 0xB4
            float m_fRotZ;                // 0xB8

            // (name guessed) Set with the translation and rotation by the
            // volley stage's figures before CalcRootMtx.
            FVEC3 m_vScale;               // 0xBC

        private:
            // (names guessed) What AttachToObject stores: the handle of the
            // object followed, its node and the AttachMode.
            unsigned long m_nAttachHandle;      // 0xC8
            unsigned short m_nAttachNode;       // 0xCC
            unsigned char m_nAttachMode;        // 0xCE
            unsigned char m_nBBoxZSortCalcType; // 0xCF
            nw4r::math::AABB m_BBox;            // 0xD0 (name guessed)
            unsigned long m_UnkE8;              // 0xE8
            unsigned short m_UnkEC;             // 0xEC
            // 0xEE: tested against a mask by bcs::CScnObjGather::Add.
            unsigned short m_UnkEE;
            CScnObj *m_pJointObj;               // 0xF0 (name guessed)
            unsigned short m_UnkF4;             // 0xF4
            unsigned short m_UnkF6;             // 0xF6
            // 0xF8 (name guessed): the handle GetScnObj looks the object up
            // by; the constructor sets it and AppendDelayDeleteObject reads it.
            unsigned long m_nHandle;
            unsigned long m_nFlag;              // 0xFC (name guessed)
            unsigned char m_nDrawPriorityOpa;   // 0x100 (name guessed)
            unsigned char m_nDrawPriorityXlu;   // 0x101 (name guessed)
            unsigned char m_nAlphaRate;         // 0x102 (name guessed)
            unsigned char m_nAlphaMode;         // 0x103 (name guessed)
            // 0x104 (name guessed): what GetValueForSortXlu returns.
            float m_fSortValue;
            // (names guessed) The links of the child list of the
            // bcs::CScnGroup holding this object.
            CScnObj *m_pGroupPrev;              // 0x108
            CScnObj *m_pGroupNext;              // 0x10C
            // (names guessed) The links of the opaque and the translucent
            // draw list bcs::CScnObjGather builds after sorting.
            CScnObj *m_pGatherOpaNext;          // 0x110
            CScnObj *m_pGatherXluNext;          // 0x114

            // (names guessed) The table of handles (lbl_805C0890) and the
            // handles queued by AppendDelayDeleteObject (lbl_805C0894,
            // lbl_805C0898).
            static CHandleTable *s_pHandleTable;
            static unsigned long *s_pDelayDeleteHandle;
            static unsigned long s_nDelayDeleteNum;

            friend class CScnGroup;
            friend class CScnObjGather;
    };
}

#endif /* !BCS_CSCNOBJ_HPP_ */
