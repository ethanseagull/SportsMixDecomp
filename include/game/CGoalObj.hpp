#ifndef CGOALOBJ_HPP_
#define CGOALOBJ_HPP_

#include "bcs/CMdlObj.hpp"
#include "game/msw/CStgGmk.hpp"

// A goal on the court: a stage gimmick with a ring and a board that can be
// crushed and dunked on. A DOL class in the global namespace (8CGoalObj),
// derived from msw::CStgGmk; BasketF's msw::CGoalObjBasket derives from it.
//
// The virtual functions follow __vt__Q23msw14CGoalObjBasket (BasketF):
// CStgGmk's overrides first, then the class's own from vtable offset 0xB0.
// Return types and parameter names are guessed. The accessors with bodies
// here are inline: BasketF's game/basket/CGoalObjBasket.cpp and HockeyF's
// game/hockey/CGoalObjHockey.cpp both have weak copies of them, in reverse
// declaration order.
//
// Size 0x6498: msw::CGoalObjHockey adds no members and is allocated at
// 0x6498, and msw::CGoalObjBasket's own members start there.
class CGoalObj : public msw::CStgGmk {
    public:
        // (enumerator names guessed)
        enum DUNK_POW {
            DUNK_POW_0
        };

        // The bits of m_nAutoFlags.
        enum {
            AUTO_POS = 1 << 0,
            AUTO_ROT = 1 << 1,
            AUTO_CALC_RING_POS = 1 << 2
        };

        CGoalObj(bcs::CTask *pParent, long idx);
        virtual ~CGoalObj();
        virtual void Main();

        virtual void SetPos(const bcs::FVEC3 &rPos);
        virtual void OnPostCreate();
        virtual void OnEntranceDemoStart();
        virtual void OnPeriodInit();
        virtual void OnTurnInit();

        virtual void SetSubType(long type) { m_nSubType = type; }                      // 0xB0
        virtual long GetSubType() const { return m_nSubType; }                         // 0xB4
        virtual void SetAutoPosFlag(int flag)                                          // 0xB8
        {
            if (flag) {
                m_nAutoFlags |= AUTO_POS;
            } else {
                m_nAutoFlags &= ~AUTO_POS;
            }
        }
        virtual int IsAutoPos() const { return m_nAutoFlags & AUTO_POS; }               // 0xBC
        virtual void SetAutoRotFlag(int flag)                                          // 0xC0
        {
            if (flag) {
                m_nAutoFlags |= AUTO_ROT;
            } else {
                m_nAutoFlags &= ~AUTO_ROT;
            }
        }
        virtual int IsAutoRot() const { return (m_nAutoFlags >> 1) & 1; }              // 0xC4
        virtual void SetAutoCalcRingPosFlag(int flag)                                  // 0xC8
        {
            if (flag) {
                m_nAutoFlags |= AUTO_CALC_RING_POS;
            } else {
                m_nAutoFlags &= ~AUTO_CALC_RING_POS;
            }
        }
        virtual int IsAutoCalcRingPos() const { return (m_nAutoFlags >> 2) & 1; }      // 0xCC
        virtual void SetPosAuto();                                                     // 0xD0
        virtual const bcs::FVEC3 &GetPos() const { return m_Pos; }                     // 0xD4
        virtual void SetRot(const bcs::FVEC3 &rRot);                                   // 0xD8
        virtual void SetRotAuto();                                                     // 0xDC
        virtual const bcs::FVEC3 &GetRot() const { return m_Rot; }                     // 0xE0
        virtual void SetDefaultOffset(const bcs::FVEC3 &rOffset) { m_DefaultOffset = rOffset; } // 0xE4
        virtual const bcs::FVEC3 &GetDefaultOffset() const { return m_DefaultOffset; } // 0xE8
        virtual unsigned long GetMdlHandle() const;                                    // 0xEC
        virtual long GetRingNodeIdx() const;                                           // 0xF0
        virtual const bcs::FVEC3 &GetRingPos() const { return m_RingPos; }             // 0xF4
        virtual const bcs::FVEC3 &GetBoardPos() const { return m_BoardPos; }           // 0xF8
        virtual const bcs::FVEC3 &GetBoardVec() const { return m_BoardVec; }           // 0xFC
        virtual const bcs::FVEC3 &GetVec() const { return m_Vec; }                     // 0x100
        virtual void CrushStart();                                                     // 0x104
        virtual void CrushEnd();                                                       // 0x108
        virtual void GoalIn() {}                                                       // 0x10C
        virtual void DunkStart(DUNK_POW pow, const bcs::FVEC3 &rPos);                  // 0x110
        virtual void DunkRelease();                                                    // 0x114
        virtual void DunkCancel();                                                     // 0x118
        virtual int IsNowDunkMot() const;                                              // 0x11C
        virtual void RingHit();                                                        // 0x120
        // The model object of model idx, if there is one.
        virtual bcs::CScnObj *GetMdlObj(int idx)                                       // 0x124
        {
            if (GetMdl(idx) != 0) {
                return GetMdl(idx)->GetMdlObj();
            }
            return 0;
        }
        virtual const bcs::CScnObj *GetMdlObj(int idx) const                          // 0x128
        {
            if (GetMdl(idx) != 0) {
                return GetMdl(idx)->GetMdlObj();
            }
            return 0;
        }

        // From the end of msw::CStgGmk (0x68); the constructor stores 0 at
        // 0x68 (meaning not known).
        unsigned long m_Unk68;
        // 0x6C, set to 1 by the constructor.
        long m_nSubType;
        // 0x70, the constructor's idx: the goal's side. HockeyF's
        // CGoalMdlHockey::NewMdl turns the fading model by it.
        long m_nGoalIdx;
        bcs::FVEC3 m_Pos;           // 0x74
        bcs::FVEC3 m_Rot;           // 0x80
        bcs::FVEC3 m_Vec;           // 0x8C
        bcs::FVEC3 m_RingPos;       // 0x98
        bcs::FVEC3 m_BoardPos;      // 0xA4
        bcs::FVEC3 m_BoardVec;      // 0xB0
        bcs::FVEC3 m_DefaultOffset; // 0xBC
        unsigned char m_UnkC8[0xD4 - 0xC8];
        unsigned long m_nAutoFlags; // 0xD4, AUTO_POS and the others
        unsigned char m_UnkD8[0x6498 - 0xD8];
};

#endif /* !CGOALOBJ_HPP_ */
