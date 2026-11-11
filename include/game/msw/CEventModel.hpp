#ifndef CEVENTMODEL_HPP_
#define CEVENTMODEL_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "revolution/gx/GXEnum.h"

namespace bcs {
    struct FMTX34;
    class CScnGroup;
}

namespace msw {

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
    class CEventModel {
        public:
            // One step of a motion sequence. A sequence ends with an entry
            // whose nMot is 0. (member names guessed)
            struct MOT_DATA {
                short nMot;   // 0x00: motion number; negative for the mirrored motion
                short nFlag;  // 0x02: second argument of the virtual AnimPlay
                float fBlend; // 0x04: third argument
                float fFrame; // 0x08: added to the frame the sequence starts at
            };

            CEventModel(const short *pMotTable);
            virtual ~CEventModel();
            virtual void _SetPos(const bcs::FVEC3 &) {}                 // 0x0C (DOL body)
            virtual void _SetRot(const bcs::FVEC3 &) {}                 // 0x10 (DOL body)
            virtual void _SetScale(const bcs::FVEC3 &) {}               // 0x14 (DOL body)
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
            virtual void *GetSndTaskSe(long) { return 0; }              // 0x8C
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
            void SetNode(long nNode, float fFrame);

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

            // (names guessed) Read directly by the modules.
            const bcs::FVEC3 &GetPos() const { return m_vPos; }
            const bcs::FVEC3 &GetRot() const { return m_vRot; }
            // (name guessed) Read by msw::CSceneCalc::_CalcFloor.
            const bcs::FVEC3 &GetPosOffset() const { return m_vPosOffset; }

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

            // (name guessed) The model's alpha now, 0 to 1.
            float GetAlpha() const { return m_fAlpha; }
            long GetVoiceId(long n) const
            {
                if (m_pVoiceTable != 0) {
                    return m_pVoiceTable[n];
                }
                return -1;
            }

        private:
            bcs::FVEC3 m_vPos;            // 0x04 (name guessed)
            bcs::FVEC3 m_vRot;            // 0x10 (name guessed) radians
            bcs::FVEC3 m_vScale;          // 0x1C (name guessed) see SetScale
            bcs::FVEC3 m_vPosOffset;      // 0x28 (name guessed) added by SetPosition
            unsigned long m_nFlag;        // 0x34 (name guessed)
            short m_nIndex;               // 0x38
            short m_nType;                // 0x3A
            const long *m_pVoiceTable;    // 0x3C (name guessed) sound numbers
            const long *m_pFootSeTable;   // 0x40 (name guessed) sound numbers
            const short *m_pMotTable;     // 0x44 (name guessed) motion number to animation number
            long m_nAnim;                 // 0x48 (name guessed)
            const MOT_DATA *m_pMotData;   // 0x4C (name guessed) the sequence playing
            unsigned char m_Unk50[4];     // 0x50
            short m_nMotIndex;            // 0x54 (name guessed) entry of the sequence
            short m_nUnk56;               // 0x56
            unsigned char m_Unk58[4];     // 0x58
            float m_fAlpha;               // 0x5C (name guessed) see _UpdateAlpha
            float m_fAlphaTarget;         // 0x60 (name guessed)
            float m_fAlphaSpeed;          // 0x64 (name guessed)
            // 0x68 to 0xB0 are not mapped. The class is 0xB0 bytes:
            // msw::CTournamentChip and msw::CTournamentTrophy (TourF) have
            // their own members from 0xB0.
            unsigned char m_Unk68[0x3C];  // 0x68
            nw4r::g3d::ResFile m_resFile; // 0xA4 (name guessed) see GetResFile
            unsigned char m_UnkA8[0x8];   // 0xA8
    };
}

#endif /* !CEVENTMODEL_HPP_ */
