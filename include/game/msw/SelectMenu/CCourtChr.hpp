#ifndef SELECTMENU_CCOURTCHR_HPP_
#define SELECTMENU_CCOURTCHR_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "revolution/rfl.h"

namespace bcs {
    class CMdlObj;
    class CScnObj;
    struct FVEC3;
}

namespace msw {

    class CChrMdlObj;
    class CMenuMdlTableSet;

    namespace SelectMenu {

        class CCourtBallMdlObj;
        class CCourtCharaObj;

        // Size 0xFC (layout.py). The court of the menu background (MenuF,
        // game/menu/CCourtChr.cpp): a table model played through a
        // CMenuMdlTableSet, up to six characters standing on it (one
        // CCourtCharaObj each, fading the CChrMenuObj models in and out),
        // the balls of the sports and the player markers. Menu object 0x3B
        // of CSelectMenu. Member and argument names are guesses.
        class CCourtChr : public CSelectMenuLayoutBase {
            public:
                // Events of SendEvent (names guessed from what they do).
                enum {
                    EVENT_DISP = 0x35,        // (int)pParam: show or hide the table and the characters
                    EVENT_PLAY_ANIM = 0x36,   // play table animation (long)pParam unless it is playing
                    EVENT_PLAY_ANIM_FORCE = 0x37,
                    EVENT_PUSH_ANIM = 0x38,   // queue animation (long)pParam
                    EVENT_CHANGE_ANIM = 0x39, // drop the queue after the fade, then queue (long)pParam
                    EVENT_BALL_SELECT = 0x3A, // show ball (long)pParam only
                    EVENT_ANIM_SPEED = 0x3B,  // pParam points to the new speed of the table's animations
                    EVENT_GET_ANIM = 0x3C     // returns the table's animation
                };

                // The entries of m_hMdl: 0 is not used, then the characters,
                // the balls and the markers.
                enum {
                    MDL_CHARA = 1,
                    MDL_BALL = 7,
                    MDL_MARKER = 12,
                    MDL_NUM = 18
                };

                CCourtChr(long nId);
                virtual ~CCourtChr();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

                void Initialize();
                // A model of pName in file, drawn in layer 2 with motion
                // slot 0 attached; null if there is none.
                bcs::CMdlObj *CreateMdlObj(const char *pName, nw4r::g3d::ResFile file);
                // The same with a CCourtBallMdlObj.
                CCourtBallMdlObj *CreateBallMdlObj(const char *pName, nw4r::g3d::ResFile file);
                // Makes the model of character nChr (a Mii type from 19) in
                // colour nSub (0xFD to 0xFF: a silhouette) and registers it
                // with id nId in msw::CChrMenuCtrl; with bEffect 1 the
                // character's effect is attached. Null on failure.
                CChrMdlObj *CreateChrMenuObj(long nId, long nChr, long nSub, long bEffect, RFLDataSource source,
                                             long nMii, long nLayer);

                // Puts character nChr on place nIndex (0 to 5) and makes it
                // the one shown in slot nSlot (0 to 5; none if negative).
                // Returns 0 when that character is already there, else 1.
                // The last argument is not used.
                unsigned long SetCharaModel(long nIndex, long nChr, long nSub, long bEffect, long nSlot,
                                            RFLDataSource source, long nMii, int bUnused);
                // Plays motion nMotion on the character of place nIndex,
                // unless it is playing already. The last argument is not
                // used.
                void SetCharaMotion(long nIndex, long nMotion, long nFlag, float fRate, float fBlend,
                                    float fFrame, int bUnused);
                // The model handle of place nIndex, current or about to
                // replace it.
                unsigned long GetMdlHandleByCharaId(long nIndex);
                unsigned long GetNextMdlHandleByCharaId(long nIndex);
                // After fWait frames, drops the queue of the table and plays
                // nAnim at fSpeed.
                void WaitToPlayAnim(float fWait, long nAnim, float fSpeed);
                void SetDispEnable(int bDisp);
                void SetCharaDispEnable(int bDisp);
                // The texture pName of material nMat of the table, or an
                // invalid one.
                nw4r::g3d::ResTexObj GetResTexObj(const char *pName, unsigned long nMat);
                // Marker nMarker (0 to 5) in colour nColor (0 to 4).
                void SetMarkerColor(long nMarker, long nColor);
                void SetMarkerDisp(long nMarker, int bDisp);
                // The alpha of every marker, at most 1.
                void SetMarkerAlpha(float fAlpha);
                // The position of node "pos_xx" of the table; 0, or -1 to -3
                // when there is no such node, table set or model.
                long GetNodePositionByPosId(long nPos, bcs::FVEC3 &rPos);
                void SetTableAnimSpeed(float fSpeed);
                float GetTableAnimSpeed();

                // (name from layout.py, set from another unit) Shows the
                // fifth ball, the one of the mixed sports.
                void SetMixModeFlag(int bMixMode) { m_bMixMode = bMixMode; }

            protected:
                // These have no copy in the module: inlined everywhere and
                // the out-of-line bodies dead-stripped (their static tables
                // are still there, as @LOCAL@ objects).
                void StateControl();
                unsigned char GetMdlObjIdFromCharaId(unsigned char nChara);
                unsigned char GetMdlObjIdFromBallId(unsigned char nBall);
                unsigned char GetMdlObjIdFromMarkerId(unsigned char nMarker);

                // (names guessed) Inline helpers of SendEvent and the Set
                // functions.
                int IsAnimEnd();
                int IsAnimLooped();
                float GetAnimRate(long nAnim);
                void PlayAnim(long nAnim);
                void PushAnim(long nAnim);
                void SetBallDispEnable(int bDisp);
                void SelectBall(long nBall);
                void InitShown();

                nw4r::g3d::ResFile m_ResFile[7];  // 0x5C, s_ResName's files
                CMenuMdlTableSet *m_pTableSet;    // 0x78, the table
                unsigned long m_hMdl[MDL_NUM];    // 0x7C, bcs::CScnObj and CChrMenuCtrl handles
                long m_UnkC4;                     // 0xC4
                unsigned char m_nShown[6];        // 0xC8, the place shown in each slot
                int m_bClose;                     // 0xD0, Main closes the menu when set
                float m_UnkD4;                    // 0xD4
                CCourtCharaObj *m_pCharaObj[6];   // 0xD8
                long m_nAnim;                     // 0xF0, the table's animation, -1 for none
                float m_fAnimSpeed;               // 0xF4, multiplies the table's animation rates
                int m_bMixMode;                   // 0xF8
        };

        // The court with the balls of the sports on it (court selection).
        // Size 0xFC, CCourtChr's layout.
        class CCourtBall : public CCourtChr {
            public:
                // Event of SendEvent: (int)pParam shows or hides the balls.
                enum { EVENT_BALL_DISP = 0x47 };

                CCourtBall(long nId);
                virtual ~CCourtBall();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
        };

        // The court with the characters and one ball. Size 0xFC.
        class CCourtChrBall : public CCourtChr {
            public:
                CCourtChrBall(long nId);
                virtual ~CCourtChrBall();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
        };
    }
}

#endif /* !SELECTMENU_CCOURTCHR_HPP_ */
