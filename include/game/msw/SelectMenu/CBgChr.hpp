#ifndef SELECTMENU_CBGCHR_HPP_
#define SELECTMENU_CBGCHR_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "game/msw/CTableSetMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CMdlObj;
}

namespace nw4r {
    namespace math {
        struct AABB;
    }
}

namespace msw {

    class CMenuMdlTableSet;
    class CTopMenuChr;

    namespace SelectMenu {

        // The characters of the sports standing on a turning table in the
        // menu background (MenuF, game/menu/CBgChr.cpp, "SelectMenuBgChr.cpp"
        // in the original); menu object 0x3A of CSelectMenu. Size 0x104
        // (layout.py). The table is a model of "chara_table.brres" played
        // through a CMenuMdlTableSet, the characters a msw::CTopMenuChr, and
        // a name tag model with a screen box (msw::CBBoxObj) stands in
        // front of each sport. Member and argument names are guesses unless
        // the mangled names give them.
        class CBgChr : public CSelectMenuLayoutBase {
            public:
                // Events of SendEvent (names guessed from what they do).
                enum {
                    EVENT_DISP = 0x33,      // (int)pParam: show or hide everything
                    EVENT_CHARA_SPEED = 0x34 // pParam points to the characters' motion speed
                };

                // The entries of m_hMdl: the table, the name tags and their
                // screen boxes.
                enum {
                    MDL_TABLE = 0,
                    MDL_TAG = 6,
                    MDL_BBOX = 11,
                    MDL_NUM = 16
                };

                // The places on the table: five with the mixed sports, else four.
                enum {
                    POS_NUM = 4,
                    POS_NUM_MIX = 5
                };

                CBgChr(long nId) : CSelectMenuLayoutBase(nId) { Initialize(); }
                virtual ~CBgChr();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

                void Initialize();
                // A model of pName in file, drawn in layer 2 with motion
                // slot 0 attached; null if there is none.
                bcs::CMdlObj *CreateMdlObj(const char *pName, nw4r::g3d::ResFile file);
                // Turns the table: one step (m_nRotDir), or straight to
                // m_nReqPos.
                void StepControl();

                // Puts the sport of place nPos in front.
                void SetPosChara(long nPos);
                // The place in slot nSlot (0 in front).
                long GetPosChara(long nSlot);
                // Turns the table one step: 1 forward, -1 back. Returns 0.
                long RotPosChara(long nDir);
                // The slot of place nPos, -1 if it is not on the table.
                long GetCharaPos(long nPos);
                // Turns the characters to the sport in front (or of place
                // nPos); 0, or -1 without the characters.
                long SetSelectGameChara();
                long SetSelectGameChara(long nPos);
                // Lights the name tag of place nPos only; returns 0.
                long SetSelectGameTag(long nPos);
                void PlayAnim(long nAnim, float fRate, float fFrame);
                int IsAnimEnd();
                long PrepareDecideAnimChara();
                long IsEndPrepareDecideAnimChara();
                long PlayDecideAnimChara();
                long IsEndDecideAnimChara();
                void ResetChara();
                // 1 while the table is not turning.
                int IsRotAnim();
                void SetDispTagName(int bDisp);
                // Plays colour animation nAnimId on pMdl at fRate (backwards
                // from the end when negative).
                void PlayClr(bcs::CMdlObj *pMdl, long nAnimId, float fRate);
                // Plays motion nAnimId on the name tag of sport nTag.
                void SetTagNameAnim(long nTag, long nAnimId);
                // Fades every name tag in, or out with bOut.
                void TagNameFadeInOut(int bOut);
                // The screen boxes of the character and the name tag of
                // sport nGame; 0, -1 without the characters, -2 for a bad
                // index.
                long GetBBoxInfo(long nGame, nw4r::math::AABB &rChara, nw4r::math::AABB &rTag);
                void SetTableAnimSpeed(float fSpeed);
                float GetTableAnimSpeed();
                void SetCharaSpeed(float fSpeed);

            protected:
                // Inline helpers (no out-of-line copies in the module).
                // GetCharaIdFromPosChara's static tables are still there,
                // as @LOCAL@ objects.
                // The number of places (POS_NUM_MIX or POS_NUM). Literals:
                // with the enum constants the compiler branches where the
                // original computes the value without a branch.
                long GetPosNum() const { return m_bMixMode ? 5 : 4; }
                long GetCharaIdFromPosChara(long nPos);
                void SetTagColor(long nSel);
                void PlayTagMotion(bcs::CMdlObj *pMdl, long nAnimId);

                nw4r::g3d::ResFile m_ResFile[2];  // 0x5C, s_ResName's files
                CMenuMdlTableSet *m_pTableSet;    // 0x64, the table
                CTableSetMdlObj *m_pObj[MDL_NUM]; // 0x68, [0] is the table's
                unsigned long m_hMdl[MDL_NUM];    // 0xA8, bcs::CScnObj handles
                unsigned char m_nPos[8];          // 0xE8, the place in each slot, [0] in front
                CTopMenuChr *m_pTopMenuChr;       // 0xF0
                long m_nRotState;                 // 0xF4, 0 idle, 1 turning one step, 2 turning to m_nReqPos
                long m_nRotDir;                   // 0xF8, set by RotPosChara
                long m_nReqPos;                   // 0xFC, -1 for none
                int m_bMixMode;                   // 0x100
        };

        // CBgChr on the top menu. Size 0x104.
        class CBgChrTop : public CBgChr {
            public:
                CBgChrTop(long nId);
                virtual ~CBgChrTop();
        };

        // The table's model object, with a vtable of its own.
        class CBgChrObj : public CTableSetMdlObj {
            public:
                CBgChrObj(bcs::CMdlObj *pMdl) : CTableSetMdlObj(pMdl) {}
                virtual ~CBgChrObj();
        };
    }
}

#endif /* !SELECTMENU_CBGCHR_HPP_ */
