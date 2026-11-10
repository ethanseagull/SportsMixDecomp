#ifndef COINRES_HPP_
#define COINRES_HPP_

namespace msw {
    class CMenuPaneAccessor;

    // Resources of the coin counter that every sport's cockpit shows
    // (CCockpit<Mode>Coin). Lives in the DOL (game/msw/CoinRes.s); a
    // namespace, as the name has no C prefix (a guess). Signatures read from
    // the DOL bodies.
    namespace CoinRes {
        // Which coin layout: "coin.brlyt", "VO_cockpit_02_coin.brlyt",
        // "VO_cockpit_03_coin.brlyt" (the volley cockpit picks one by the
        // screen split; names guessed from the files).
        enum E_COINLAYOUTID {
            COINLAYOUT_COCKPIT = 0,
            COINLAYOUT_VOLLEY_02 = 1,
            COINLAYOUT_VOLLEY_03 = 2
        };

        // The layout file, its animation files and the number of them (11),
        // and the pane name table of layout nId.
        const char *GetLayoutResource(E_COINLAYOUTID nId);
        const char **GetLayoutAnimResource(E_COINLAYOUTID nId);
        long GetLayoutAnimResourceNum(E_COINLAYOUTID nId);
        const char **GetLayoutPaneNameTable(E_COINLAYOUTID nId);

        // Number of teams with a counter (2).
        long GetTeamNum();
        // Indexes into the pane name table, per team (bytes in the DOL).
        unsigned char GetCoinPane(long nTeam);
        unsigned char GetCoinDispPane(long nTeam);
        unsigned char GetCoinAnimPane(long nTeam);
        unsigned char GetCoin3DPane(long nTeam);
        unsigned char GetCoinTelopPane(long nTeam);
        // The pane the telops hang under in game mode nMode (14, or 15 for
        // mode 1).
        unsigned char GetTelopAttachPane(long nMode);
        // Places the counter panes (return type guessed).
        void SetCoinPosition(CMenuPaneAccessor *pAccessor, unsigned char nPane);
        // The coins team nTeam has in the running game.
        long GetGameCoinNum(long nTeam);
    }
}

#endif /* !COINRES_HPP_ */
