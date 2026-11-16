#ifndef MSW_COCKPIT_COCKPITUTIL_HPP_
#define MSW_COCKPIT_COCKPITUTIL_HPP_

namespace bcs {
    struct FMTX34;
    struct FVEC3;

    namespace G2D {
        class CPaneCtrl;
    }
}

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    class CChrCore;
    class CMenuPaneAccessor;

    namespace cockpit {

        // Free helpers of the cockpits. Live in the DOL (0x801DFE40); the
        // file name is a guess.

        // Added for BossF game/boss/CCockpitBossCoder.cpp: the leader of
        // team nTeam. (It only branches to CChrMan::GetTeamLeader.)
        CChrCore *GetTeamLeaderChrCore(long nTeam);

        // (name, argument names and meaning guessed; the DOL's fn_801DFE44,
        // added for game/msw/CCockpitChrIcon2D.cpp) Turns pOfs, an offset
        // in the model space of pMtx, into a world offset as seen from
        // camera nCamera: the model's rotation is replaced by the camera's,
        // scale kept. -1 when pOfs or pMtx is null, else 0.
        long GetBillboardOffset(const bcs::FVEC3 *pOfs, const bcs::FMTX34 *pMtx, long nCamera, bcs::FVEC3 *pOut);

        // Added for VolleyF game/volley/CCockpitVolleyCoder.cpp (return type
        // and argument names guessed): gives text pane pPane of layout
        // pLayoutName in pPaneCtrl the font resource pFontName, keeping its
        // font size. pLayoutName is not read. 0, or a negative number that
        // tells which step failed.
        long SetTextFont(bcs::G2D::CPaneCtrl *pPaneCtrl, nw4r::lyt::Pane *pPane, const char *pLayoutName,
                         const char *pFontName);

        // Added for HockeyF game/hockey/CCockpitHockeyCoder.cpp (argument
        // names guessed): the special-shot telop of pane nPane; the four
        // panes after it are the telop's parts. The coder passes -1.0f to
        // hide it. Plays nInAnim, then (fRate above 1) nLoopAnim at speed
        // 1 / fRate and nOutAnim; nHideAnim for a negative fRate. -1
        // without the pane, else 0.
        // Added for VolleyF game/volley/CCockpitVolley.cpp (return types
        // and argument names guessed): the number of split views (the
        // cockpits make the second view's objects when it is above 1), and
        // pText0, pSep and pText1 joined into pBuf (nLen characters);
        // returns pBuf.
        long GetViewSplitNum();
        const wchar_t *GetMergeString(wchar_t *pBuf, long nLen, const wchar_t *pText0, const wchar_t *pText1,
                                      const wchar_t *pSep);

        // Added for VolleyF game/volley/CCockpitVolleyBoss.cpp (DOL
        // 0x801DF710; return type and argument names guessed): the clock
        // at fTime, with animations nAnim and nAnimAll on text panes nPane
        // and nShadowPane. The three last arguments are -1, -1 and 0 there.
        // As CTimeCount::SetTimeValue (minutes into text box nPane, seconds
        // into nShadowPane), then, in the last ten seconds, the seconds into
        // text box nCountTextPane with animation nCountAnim on pane
        // nCountPane; otherwise that box gets "x" and nCountPane is hidden.
        // -1 without pAccessor or with a negative count pane; else the
        // count shown, -1 when none.
        long SetTimeValue(CMenuPaneAccessor *pAccessor, float fTime, int nArg, int bForce, long nAnim,
                          long nAnimAll, long nPane, long nShadowPane, long nCountPane, long nCountTextPane,
                          long nCountAnim);

        long SetSPGaugeTelop(CMenuPaneAccessor *pAccessor, long nPane, long nInAnim, long nLoopAnim,
                             long nOutAnim, long nHideAnim, float fRate);
    }
}

#endif /* !MSW_COCKPIT_COCKPITUTIL_HPP_ */
