#ifndef CEVENTMODEL_HPP_
#define CEVENTMODEL_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/IEvNodeBindable.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "revolution/gx/GXEnum.h"

namespace bcs {
    struct FMTX34;
    class CAnmMotion;
    class CMdlObj;
    class CMdlShadow;
    class CScnGroup;
    class CSndTask;
}

namespace msw {

    class CChrMdlObj;
    class CEffectCluster;
    class CEventMove;
    class CEventMoveAction;
    class CEventNodeBase;

    // A model placed by an event: the characters and objects of the demos
    // and of the tournament map. A DOL class, vptr at 0x00. The constructor
    // in the DOL writes members up to 0xAC; msw::CEvDemoMdlChr, 0xE0 bytes,
    // has its own from 0xD0. Only the members named below are mapped.
    //
    // The virtual functions are in the order of the slots of
    // __vt__Q23msw15CTournamentChip (TourF), which also gives their names
    // and parameters. A body given here is the one the modules emit as a
    // weak function, or the body the DOL's own vtable points at for the
    // functions no module emits (marked "DOL body"). Return types are not
    // part of the mangled names: they are read from those bodies ("void" for
    // an empty one), and the pointer types are guesses.
    class CEventModel : public IEvNodeBindable {
            // CEventMove::SetFrame tests the flags directly.
            friend class CEventMove;

        public:
            // One step of a motion sequence. A sequence ends with an entry
            // whose nMot is 0. (member names guessed)
            struct MOT_DATA {
                short nMot;   // 0x00: motion number; negative for the mirrored motion
                short nFlag;  // 0x02: second argument of the virtual AnimPlay
                float fBlend; // 0x04: third argument
                float fFrame; // 0x08: added to the frame the sequence starts at
            };
            // One step of a sequence of animations played by name (see the
            // second AnimPlay below); it ends with an entry whose pName is
            // null. (member names guessed)
            struct MOT_NAME_DATA {
                const char *pName;   // 0x00
                unsigned long nFlag; // 0x04
                float fBlend;        // 0x08
                float fFrame;        // 0x0C
            };

            CEventModel(const short *pMotTable);
            virtual ~CEventModel();
            // 0x0C to 0x14: _SetPos, _SetRot, _SetScale of IEvNodeBindable.
            virtual void SetDisp(int) {}                                // 0x18 (DOL body)
            virtual void SetShadowDisp(int) {}                          // 0x1C
            virtual void UpdateFrame(float) {}                          // 0x20 (DOL body)
            virtual void SetDrawLayer(long) {}                          // 0x24 (DOL body)
            virtual void SetDrawPriority(long) {}                       // 0x28 (DOL body)
            virtual void AppendChild(bcs::CScnGroup *) {}               // 0x2C (DOL body)
            virtual void SetLightSelector(long) {}                      // 0x30 (DOL body)
            virtual void ResetLightSelector() {}                        // 0x34 (DOL body)
            virtual bcs::CScnObj *GetScnObj() { return 0; }             // 0x38 (DOL body)
            virtual const bcs::CScnObj *GetScnObj() const { return 0; } // 0x3C (DOL body)
            virtual const bcs::FMTX34 *GetNodeWorldMtxPtr(unsigned long) const { return 0; } // 0x40 (DOL body)
            virtual long GetNodeId(const char *) const { return -1; }   // 0x44 (DOL body)
            // The next three are empty weak functions in the modules too.
            virtual void SetMatTevColor(const char *, bcs::BCS_Color, _GXTevRegID) {} // 0x48
            virtual void SetBaseColor(bcs::BCS_Color) {}                // 0x4C
            virtual void SetColorScale(bcs::BCS_Color) {}               // 0x50
            virtual void SetCameraView(long) {}                         // 0x54
            // Takes the animations from another resource file.
            virtual void ChangeAnimFile(nw4r::g3d::ResFile) {}          // 0x58
            virtual void SetAnimFrame(float, int) {}                    // 0x5C
            virtual void SetAnimSpeed(float) {}                         // 0x60
            virtual int IsAnimResLoopType(long) const { return 0; }     // 0x64
            // The length in frames of motion `nMot`.
            virtual float AnimResFrame(long nMot) const { return 0.0f; } // 0x68
            // The frame at which motion `nMot` fires a trigger; negative
            // when it has none. (meaning of the last two arguments guessed
            // from the callers in msw::TournamentData)
            virtual float AnimResTrgFrame(long nMot, unsigned long nTrg, float) const { return 0.0f; } // 0x6C
            // Plays motion `nMot`, a number looked up in the motion table.
            // msw::CEvDemoMdlChr's override passes fBlend and fFrame on to
            // bcs::CAnmMotion::Play.
            virtual float AnimPlay(long nMot, unsigned long nFlag, float fBlend, float fFrame, int) { return 0.0f; } // 0x70
            virtual float AnimResTrgFrame(const char *, unsigned long, unsigned long, float) const { return 0.0f; } // 0x74
            virtual float AnimPlay(const char *, unsigned long, float, float, int) { return 0.0f; } // 0x78
            virtual float AnimPlay(nw4r::g3d::ResFile, const char *, unsigned long, float, float, int) { return 0.0f; } // 0x7C
            virtual float GetNowAlpha() const { return 0.0f; }          // 0x80
            virtual void _UpdateFlags() {}                              // 0x84
            virtual long GetSeMax() const { return -1; }                // 0x88
            virtual bcs::CSndTask *GetSndTaskSe(long) { return 0; }    // 0x8C
            virtual void SetSndTaskSe(long, unsigned long) {}           // 0x90
            virtual long GetEffMax() const { return -1; }               // 0x94
            virtual unsigned long GetEffcetHandle(long) const { return 0; } // 0x98
            virtual void SetEffectHandle(unsigned long, long) {}        // 0x9C
            virtual long GetEffcetId(long) const { return -1; }         // 0xA0
            virtual void SetEffectId(long, long) {}                     // 0xA4
            virtual void Create() {}                                    // 0xA8
            virtual void Destroy() {}                                   // 0xAC (DOL body)
            virtual int IsObj() const { return 0; }                     // 0xB0

            // Plays the sequence `pData` from entry `nIndex` on; fFrame is
            // the frame to start at. (meaning of the last argument, kept at
            // 0x56, not known; callers pass -1)
            void AnimPlay(const MOT_DATA *pData, long nIndex, float fFrame, long);

            // Sets or clears bit 0x2000 of the flags: the sequences then
            // play the mirrored motions.
            void SetMotionReverse(int bReverse);
            void SetEnableFootSe(int bEnable);
            // (added for VolleyF CPerfectDemoVolley; meaning guessed) Whether
            // the model follows the floor of the scene.
            void SetFloorMove(int bMove);

            // (DOL functions; names from the symbols, parameter names
            // guessed) Whether the model is shown; attach a node path to the
            // model (pNode is the model's from then on); start node path
            // nNode at frame fFrame.
            int IsDisp() const;
            void SetLinkNode(CEventNodeBase *pNode, int b);
            // Returns whether the node path can be followed.
            int SetNode(long nNode, float fFrame);

            // (DOL functions; parameter names guessed) Used by TourF's
            // Stage33: show or hide the model's group; stop, erase or force
            // off the effects in the slots nFirst..nLast; the offset,
            // rotation and mirroring applied to the node path; whether the
            // node path moves the model.
            void SetGroupDisp(int bDisp);
            void StopEffect(long nFirst, long nLast);
            void StopEffectForce(long nFirst, long nLast);
            void EraseEffect(long nFirst, long nLast);
            void SetMoveOffset(const bcs::FVEC3 &vOffset);
            void SetMoveOffsetRot(const bcs::FVEC3 &vRot);
            void SetMoveFlip(long nFlip);
            void SetUpdateMove(int bUpdate);

            // (DOL functions used by msw::CTournamentMan in TourF; return
            // types and parameter names guessed) Advances the model's move by
            // fFrame; links a move action or a move to the model; the frame
            // of the move; the star effect of a character; attaches the
            // model to node pNode of pParent.
            void UpdateMove(float fFrame);
            void SetLinkAction(CEventMoveAction *pAction, int b);
            void SetLinkMove(CEventMove *pMove, int b);
            float GetMoveFrame() const;
            void SetStarEffect(int bStar);
            void AttachToObject(const CEventModel *pParent, const char *pNode, bcs::CScnObj::AttachMode mode);

            // (DOL functions; the names of the ones unnamed in the DOL and
            // all parameter names guessed)
            // Set or clear one bit of the flags: 0x20 the node path can be
            // followed; 0x800 motion numbers are animation numbers (no
            // motion table); 0x4000 the floor height comes from the object
            // set by SetFloorObj; 0x40000 and 0x80000 are handed on by
            // _UpdateFlags (msw::CEventChrCore).
            void SetValidMove(int bValid);
            void SetDirectAnim(int bDirect);
            void SetFloorObjEnable(int bEnable);
            void SetEnableFlag18(int bEnable);
            void SetEnableFlag19(int bEnable);
            int IsValidMove() const;
            // Plays the sequence pData from its first entry once, after
            // marking its last entry to loop when that animation loops.
            void SetMotionSeq(MOT_DATA *pData);
            // AnimPlay for a sequence of animations played by name.
            void AnimPlay(const MOT_NAME_DATA *pData, long nIndex, float fFrame, long nLoop);
            // Attaches the model to node nNode of pParent.
            void AttachToObject(const CEventModel *pParent, long nNode, bcs::CScnObj::AttachMode mode);
            // Makes the model follow pParent (see _UpdateAttach).
            void AttachToModel(CEventModel *pParent);
            // The model whose scene object gives the floor height.
            void SetFloorObj(CEventModel *pObj);
            // The length in frames of the longest of the animations pName
            // the bits of nFlag select (1 skeletal, 2 colour, 4 texture
            // pattern, 8 texture matrix, 0x10 shape) in the model's file.
            float AnimResFrame(const char *pName, unsigned long nFlag) const;
            // Sets the rotation of the move's node path (only y is used;
            // SetMoveOffset and SetMoveOffsetRot set the other two).
            void SetMovePathRot(const bcs::FVEC3 &vRot);
            // Moves the model in a straight line by vMove over fFrame frames;
            // returns the node path it builds for that.
            CEventNodeBase *SetMoveLinear(const bcs::FVEC3 &vMove, float fFrame, int bUpdate);
            // Plays sound nSe in sound slot nSlot (stopping the one there),
            // at the model's position or without one; the three floats go
            // to msw::CGameSnd::Play.
            bcs::CSndTask *PlaySe(long nSe, long nSlot, float, float, float);
            bcs::CSndTask *PlaySeNoPos(long nSe, long nSlot, float, float, float);
            void StopSe(float fFade, long nFirst, long nLast);

        protected:
            // Follows the floor (flag 0x8) or the attached model.
            void _UpdateFloor();
            void _UpdateAttach();
            // Forget the sound and effect slots whose sound or effect ended.
            void _UpdateSe();
            void _UpdateEffect();
            // The effect in slot n, if it is still alive.
            CEffectCluster *_GetEffect(long n);
            // Plays entry m_nMotIndex of the sequence (to m_nMotLoop at its
            // end), fFrame added to its start frame.
            void _PlayMotion(float fFrame);
            void _PlayMotionName(float fFrame);
            // _NextMotion for the sequence by name.
            void _NextMotionName(short nStep, float fFrame);
            // A model object of file m_mdlFile (the first model when pName
            // is null) with the animations nFlag selects.
            bcs::CMdlObj *_CreateMdlObj(const char *pName, unsigned long nFlag);
            // Gives pMdl a skeletal animation of m_resFile when nFlag asks
            // for one.
            bcs::CAnmMotion *_CreateAnmMotion(bcs::CMdlObj *pMdl, unsigned long nFlag);
            // Plays the animations pName of file on pMdl, those nFlag
            // selects; returns the length of the longest.
            float _AnimPlayRes(bcs::CMdlObj *pMdl, const nw4r::g3d::ResFile &file, const char *pName,
                               unsigned long nFlag, float fBlend, float fFrame);
            // Whether every animation of pMdl that nFlag selects has ended.
            int _IsAnimEnd(const bcs::CMdlObj *pMdl, unsigned long nFlag) const;
            // Sets the floor height of the shadow from the floor object.
            void _UpdateFloorHeight(bcs::CMdlShadow *pShadow);

        public:

            // (names guessed) Inline in TourF: the position (moved by the
            // offset at 0x28) and the rotation, kept and handed to the
            // virtual setters.
            void SetPosition(const bcs::FVEC3 &vPos)
            {
                m_vPos = vPos;
                bcs::FVEC3 vWork;
                bcs::FVEC3Sum(vWork, vPos, m_vPosOffset);
                _SetPos(vWork);
            }
            void SetRotation(const bcs::FVEC3 &vRot)
            {
                m_vRot = vRot;
                _SetRot(vRot);
            }

            // (name guessed) Inline in TourF: fades the alpha to fAlpha at
            // fSpeed per frame (at once for a speed of 0 or less); nMode
            // goes to bits 8 to 10 of the flags.
            void SetFade(unsigned long nMode, float fAlpha, float fSpeed)
            {
                m_nFlag = (m_nFlag & ~0x700) | (nMode << 8);
                if (fSpeed <= 0.0f) {
                    m_fAlpha = fAlpha;
                    m_fAlphaTarget = fAlpha;
                    m_fAlphaSpeed = 0.0f;
                } else {
                    m_fAlphaTarget = fAlpha;
                    m_fAlphaSpeed = fSpeed;
                }
            }

            // (names guessed) Inline in TourF: the scale is kept and handed
            // to the virtual _SetScale; the model's resource file is copied
            // out.
            void SetScale(const bcs::FVEC3 &vScale)
            {
                m_vScale = vScale;
                _SetScale(vScale);
            }
            nw4r::g3d::ResFile GetResFile() const { return m_resFile; }
            // (name guessed; added for TourF msw::CTournamentRouteMan) The
            // file with the model.
            nw4r::g3d::ResFile GetMdlFile() const { return m_mdlFile; }

            // (names guessed) Read directly by the modules. GetPos is
            // IEvNodeBindable's (TourF has a weak GetPos__Q23msw15IEvNodeBindableCFv).
            const bcs::FVEC3 &GetRot() const { return m_vRot; }
            // (name guessed) Read by msw::CSceneCalc::_CalcFloor.
            const bcs::FVEC3 &GetPosOffset() const { return m_vPosOffset; }
            // (names guessed) Read by msw::CEventMove.
            const bcs::FVEC3 &GetScale() const { return m_vScale; }
            CEventMoveAction *GetMoveAction() const { return m_pAction; }
            // (names guessed; added for TourF msw::CTournamentRouteMan) The
            // matrix AttachToModel keeps, written by the route manager; the
            // alpha fade has ended.
            void SetAttachMtx(const bcs::FMTX34 &mtx) { m_AttachMtx = mtx; }
            int IsFadeEnd() const { return m_fAlphaSpeed == 0.0f; }

            // (names guessed) Inline in the modules, which write these
            // members directly.
            void SetVoiceTable(const long *pTable) { m_pVoiceTable = pTable; }
            void SetFootSeTable(const long *pTable) { m_pFootSeTable = pTable; }
            void SetMotTable(const short *pTable) { m_pMotTable = pTable; }
            // (name guessed) The animation playing now, -1 for none: the
            // motion table's entry for the motion last played.
            long GetAnimNo() const { return m_nAnim; }

            // Weak functions in TourF.
            long GetIndex() const { return m_nIndex; }
            long GetType() const { return m_nType; }
            // (name guessed) Inline in msw::CTournamentChip's constructor.
            void SetType(long nType) { m_nType = nType; }

        protected:
            // Moves the alpha at 0x5C towards its target by the speed at
            // 0x64 times fFrame. (description read from the DOL body)
            void _UpdateAlpha(float fFrame);

            // (name guessed) Sets the bits nMask of the flags when b is
            // non-zero, else clears them.
            void _SetFlag(unsigned long nMask, int b)
            {
                if (b) {
                    m_nFlag |= nMask;
                } else {
                    m_nFlag &= ~nMask;
                }
            }

            // (name guessed) The model's alpha now, 0 to 1.
            float GetAlpha() const { return m_fAlpha; }
        public:
            // (public for TourF msw::CTournamentRouteMan)
            long GetVoiceId(long n) const
            {
                if (m_pVoiceTable != 0) {
                    return m_pVoiceTable[n];
                }
                return -1;
            }

        protected:

            // (fn_80112994, unnamed in the DOL; name guessed) The animation
            // number of motion nMot: |nMot| itself while bit 0x800 of the
            // flags is set, else the motion table's entry, made positive.
            long _GetResAnimNo(long nMot) const;
            // (fn_80112B24, unnamed in the DOL; name guessed) Moves the
            // sequence at 0x4C on by nStep entries (back to the entry at
            // 0x56 at its end) and plays that entry, fFrame added to its
            // start frame.
            void _NextMotion(short nStep, float fFrame);

        protected:
            // 0x04 to 0x34: position, rotation, scale and position offset,
            // in IEvNodeBindable (protected for msw::CEventChrCore, which
            // reads them directly).
            unsigned long m_nFlag;        // 0x34 (name guessed)
            short m_nIndex;               // 0x38
            short m_nType;                // 0x3A
            const long *m_pVoiceTable;    // 0x3C (name guessed) sound numbers
            const long *m_pFootSeTable;   // 0x40 (name guessed) sound numbers
            const short *m_pMotTable;     // 0x44 (name guessed) motion number to animation number
            long m_nAnim;                 // 0x48 (name guessed)
            const MOT_DATA *m_pMotData;   // 0x4C (name guessed) the sequence playing
            const MOT_NAME_DATA *m_pMotNameData; // 0x50 (name guessed) the sequence by name playing
            short m_nMotIndex;            // 0x54 (name guessed) entry of the sequence
            short m_nMotLoop;             // 0x56 (name guessed) entry played after the last one; -1 to stop
            float m_fAnimSpeed;           // 0x58 (name guessed) see CEventChrCore::SetAnimSpeed
            float m_fAlpha;               // 0x5C (name guessed) see _UpdateAlpha
            float m_fAlphaTarget;         // 0x60 (name guessed)
            float m_fAlphaSpeed;          // 0x64 (name guessed)
            // The class is 0xB0 bytes: msw::CTournamentChip and
            // msw::CTournamentTrophy (TourF) have their own members from
            // 0xB0. (names guessed)
            // The model's place relative to the model it is attached to
            // (see AttachToModel, _UpdateAttach).
            bcs::FMTX34 m_AttachMtx;      // 0x68
            unsigned long m_nAttachHandle; // 0x98: handle of that model's scene object
            unsigned long m_nFloorHandle; // 0x9C: handle of the scene object giving the floor height
            nw4r::g3d::ResFile m_mdlFile; // 0xA0: file with the model
            nw4r::g3d::ResFile m_resFile; // 0xA4 (name guessed) see GetResFile
            // The move and the move action linked to the model (see
            // SetLinkMove, SetLinkAction); bits 0x1 and 0x2 of the flags say
            // the model owns them, bit 0x4 that it owns the move's node path.
            CEventMove *m_pMove;          // 0xA8
            CEventMoveAction *m_pAction;  // 0xAC
    };
}

#endif /* !CEVENTMODEL_HPP_ */
