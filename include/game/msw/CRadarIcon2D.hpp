#ifndef MSW_CRADARICON2D_HPP_
#define MSW_CRADARICON2D_HPP_

// PENDING: the next two stand in for bcs/G2D/CPaneCtrl.hpp and
// game/msw/CMenuLinkBase.hpp until what they add is merged (see the files).
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CMenuLinkBase.hpp"

#include "game/msw/CMenuLytObj.hpp"
#include "nw4r/lyt/Pane.hpp"

// The SDK's texture object (GXTexObj). Only named here.
struct _GXTexObj;

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // The icons that show where the characters and balls are when they are
    // off screen, one set per view of a split screen. Size 0x7C. Lives in the
    // DOL; each game module derives its own (CRadarIcon2DBoss, ...) and
    // supplies CreateCharaIcon and CreateBallIcon. CMenuLinkBase is at 0
    // (vptr at 0x18), CMenuLytObj at 0x1C (vptr at 0x28), own members from
    // 0x2C.
    //
    // The layout is read from the constructor and from the unnamed Initialize
    // and Finalize in the DOL. The virtual functions are in the order of
    // __vt__Q23msw16CRadarIcon2DBoss (BossF); the DOL has no name for
    // Initialize and Finalize, which are named after the overrides the
    // modules export. Return types are guesses, and so are the names of
    // members and parameters.
    class CRadarIcon2D : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CPaneCtrl, 1> {
        public:
            // One icon: three panes (an empty one that carries the other
            // two, a frame copied from the layout, a picture made from a
            // texture) and the texture shown. 0x1C bytes.
            struct ICON_PARAMETER {
                enum {
                    PANE_NULL,
                    PANE_BASE,
                    PANE_PICTURE,
                    PANE_NUM
                };

                // (name guessed; no symbol) Records pPaneObj as pane nPane
                // of the icon. Inline in every Create*Icon.
                void SetPaneObj(int nPane, bcs::G2D::CPaneObj *pPaneObj)
                {
                    if (pPaneObj != 0) {
                        m_pPaneName[nPane] = pPaneObj->GetPane()->GetName();
                        m_pPaneObj[nPane] = pPaneObj;
                    }
                }

                const char *m_pPaneName[PANE_NUM];        // 0x00
                bcs::G2D::CPaneObj *m_pPaneObj[PANE_NUM]; // 0x0C
                const _GXTexObj *m_pTexObj;               // 0x18
            };

            enum {
                VIEW_MAX = 4,      // views of a split screen
                ICON_MAX = 8       // icons of one kind in a view
            };

            CRadarIcon2D(long nId);
            virtual ~CRadarIcon2D();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();

            // Builds the layout and allocates m_pIconParam. Called by the
            // constructor.
            virtual void Initialize();
            virtual void InitCharaIcons(ICON_PARAMETER *pParam);
            // Makes the icon of character nChr. The three names are those of
            // the panes to create.
            virtual void CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                         const char *pBaseName, const char *pPictureName) = 0;
            virtual void InitBallIcons(ICON_PARAMETER *pParam);
            virtual void CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                        const char *pBaseName, const char *pPictureName) = 0;
            // Frees m_pIconParam. Called by the destructor.
            virtual void Finalize();
            virtual void DispCharaIcon(long, const bcs::FVEC3 &, float, float);
            virtual void DispBallIcon(long, const bcs::FVEC3 &, float, float);
            virtual void CreateCharaIconPicture(ICON_PARAMETER *pParam, long nChr, const char *pPictureName);
            virtual void CreateBallIconPicture(ICON_PARAMETER *pParam, long nBall, const char *pPictureName);

            // The name of the layout file the icons come from. Not static:
            // callers pass `this`, although the body only loads a global.
            const char *GetLayoutBinaryFileName();

            // Static: callers pass the pane only.
            static void InitNullPane(nw4r::lyt::Pane *pPane);
            static void InitPicPane(nw4r::lyt::Pane *pPane);

        protected:
            // VIEW_MAX * ICON_MAX character icons, then as many ball icons.
            ICON_PARAMETER *m_pIconParam;          // 0x2C
            ICON_PARAMETER *m_pCharaIcon[VIEW_MAX]; // 0x30, each view's part of m_pIconParam
            ICON_PARAMETER *m_pBallIcon[VIEW_MAX];  // 0x40
            // Picture sizes, chosen by the number of views in Initialize.
            nw4r::lyt::Size m_Unk50;               // 0x50
            nw4r::lyt::Size m_CharaIconSize;       // 0x58, same values as m_Unk50
            nw4r::lyt::Size m_MiiIconSize;         // 0x60, m_Unk50 scaled; for character types from 0x13
            nw4r::lyt::Size m_Unk68;               // 0x68
            long m_Unk70;                          // 0x70, 0 from Initialize
            long m_Unk74;                          // 0x74, 0 from Initialize
            long m_Unk78;                          // 0x78, 1 from Initialize
    };
}

#endif /* !MSW_CRADARICON2D_HPP_ */
