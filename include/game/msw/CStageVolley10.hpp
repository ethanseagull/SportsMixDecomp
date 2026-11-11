#ifndef MSW_CSTAGEVOLLEY10_HPP_
#define MSW_CSTAGEVOLLEY10_HPP_

#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {

    class CStgMdl;

    // Volley stage 10, the panel stage (VolleyF), made by CStageFactoryVolley
    // (with 4 for stages 10, 37 and 38). Size 0x228: CStageManVolley
    // (0x200), then the panels. Unlike the other sports' panel stages it
    // does not use the DOL's CStage10Base: the court is cut into a grid of
    // 3 columns by 6 rows, three rows per team (index col + 3 * row, 18
    // panels; rows 0 to 2 belong to team 0). Each panel has a local type
    // (0 none, 1 and 2 of one team colour, 3 and 4 of the other; 2 and 4
    // the doubled kind) shown by its model's animation.
    //
    // Three rule sets (GetPanelSetType, the constructor's argument):
    // set 3, random panels that a hit takes; set 2 ("B"), panels a team
    // paints by hitting them and its mirror on the other side, with lines
    // of three doubled and a full side starting the fever; set 4 ("C"),
    // the same without the mirror, starting from two painted panels.
    // Names of the rule sets, the local types and the members are guesses.
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw14CStageVolley10.
    // The inline ones are emitted weak after the out-of-line functions.
    class CStageVolley10 : public CStageManVolley {
        public:
            enum {
                PANEL_NUM = 18,
                PANEL_COL_NUM = 3,
                PANEL_ROW_NUM = 6,
                TEAM_ROW_NUM = 3
            };

            // A matrix that starts as the identity. GetPanelMtx_ returns it by
            // value: the original builds the result in the caller's buffer and
            // runs this constructor first, which a plain FMTX34 return does not
            // reproduce (a guess at the original's type, behaviour unchanged).
            struct MTX : public bcs::FMTX34 {
                MTX() { PSMTXIdentity(m); }
            };

            CStageVolley10(bcs::CTask *pParent, long nSetType);
            virtual ~CStageVolley10();

            virtual void Disp();
            virtual void Disp3D(long);
            virtual long GetPanelType(const bcs::FVEC3 &rPos) const;
            virtual long HitPanel(const bcs::FVEC3 &rPos, int);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long GetQPanelEnabled_() const { return 0; }
            virtual long SE_GetSlotNum() const { return 4; }
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual bool OnPreCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, nw4r::g3d::ResFile file,
                                        CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPostInitialize();
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnEntranceEventStart();
            virtual void OnSoundEvent(long event);

            // The rule set: 2, 3 or 4 (see above).
            virtual long GetPanelSetType() const;                                   // 0x448
            // The type of panel idx: GetPanelTypeByLocalType_ of its local
            // type; 0 for no panel.
            virtual long GetPanelType(long idx) const;                              // 0x44C
            virtual void InitPanelAll(long nSetType);                               // 0x450
            virtual void DeletePanelAll();                                          // 0x454
            virtual void ResetPanelAll();                                           // 0x458
            virtual void ResetPanelAnimAll();                                       // 0x45C
            // Panel index of column col and row row (both clamped).
            virtual long GetPanelIdx_(long col, long row) const;                    // 0x460
            // The same with the row counted within team nTeam's half.
            virtual long GetPanelIdxColRow_(long col, long row, long nTeam) const;  // 0x464
            // The panel under rPos, -1 off the court.
            virtual long GetPanelIdx_(const bcs::FVEC3 &rPos) const;                // 0x468
            // The panel at the same place in the other half (mirrored).
            virtual long GetPanelNegIdx_(long idx) const;                           // 0x46C
            virtual bool GetPanelColRow_(const bcs::FVEC3 &rPos, long *pCol, long *pRow, long *pTeam) const // 0x470
            {
                return GetPanelColRow_(GetPanelIdx_(rPos), pCol, pRow, pTeam);
            }
            virtual bool GetPanelColRow_(long idx, long *pCol, long *pRow, long *pTeam) const; // 0x474
            virtual bcs::FVEC3 GetPanelPos_(long idx) const;                        // 0x478
            virtual MTX GetPanelMtx_(long idx) const;                       // 0x47C
            virtual float GetPanelWidth_() const;                                   // 0x480
            virtual float GetPanelDepth_() const;                                   // 0x484
            // The team attacking the half of team nTeam.
            virtual long GetOffenseTeamNo(long nTeam) const;                        // 0x488
            virtual long GetTeamNoByPanel_(long col, long row, long nTeam) const;   // 0x48C
            // The team whose colour local type `type` has; -1 for none.
            virtual long GetTeamNoByPanelType_(long type) const;                    // 0x490
            virtual long GetMulRateByPanel_(long col, long row, long nTeam) const;  // 0x494
            // The points rate of a panel type: 2 for type 2, else 1.
            virtual long GetMulRateByPanelType_(long type) const;                   // 0x498
            // The local type of team nTeam's colour, plain (kind 1) or
            // doubled (kind 2).
            virtual long GetPanelTypeByTeamNo_(long nTeam, long kind) const;        // 0x49C
            virtual long GetPanelLocalTypeColRow_(long col, long row, long nTeam) const; // 0x4A0
            virtual long GetPanelLocalType_(long idx) const;                        // 0x4A4
            virtual long GetPanelTypeByLocalType_(long local) const;                // 0x4A8
            // Sets the local type of panel idx and plays the model's change
            // animation.
            virtual void SetPanelLocalType_(long idx, long local) const;            // 0x4AC
            // The same without the animation.
            virtual void SetPanelLocalTypeImm_(long idx, long local) const;         // 0x4B0
            virtual void PanelSetB_SetPanelLocalType_(long nOffense, long col, long row, long nTeam, long local); // 0x4B4
            virtual void PanelSetB_SetPanelLocalTypeSub_(long nOffense, long col, long row, long nTeam, long local); // 0x4B8
            virtual void PanelSetB_UpDownCheck_(long nOffense, long nTeam);         // 0x4BC
            virtual void PanelSetB_UpDownCheckSub_(long nOffense, long col, long row, long nTeam); // 0x4C0
            virtual void PanelSetC_UpDownCheck_(long nOffense, long nTeam, long hitCol, long hitRow); // 0x4C4
            virtual void PanelSetC_UpDownCheckSub_(long nOffense, long col, long row, long nTeam, long hitCol,
                                                   long hitRow);                    // 0x4C8
            virtual float PanelSetB_GetAnmFrame() const;                            // 0x4CC
            // Every panel of team nTeam's half doubled.
            virtual bool PanelSetB_IsFever_(long nTeam) const;                      // 0x4D0
            virtual void PanelSetB_FeverStart_(long nTeam);                         // 0x4D4
            virtual void PanelSetB_FeverCancel_();                                  // 0x4D8
            virtual void UpdateEdge_();                                             // 0x4DC
            // Debug drawing of the panel edges.
            virtual void DrawEdge_(long);                                           // 0x4E0
            virtual void DrawEdgeOne_(long col, long row, long nTeam);              // 0x4E4

            // Of the running stage (CStageMan's instance, taken to be this
            // stage). nTeam other than 0 takes the mirrored panel.
            static bcs::FVEC3 GetPanelPos_CR(long nTeam, long col, long row);
            static float GetPanelWidth();
            static float GetPanelDepth();
            // (meaning guessed) The panel at col, row (of team nTeam's half,
            // as GetPanelPos_CR) seen by the team attacking that half: 0
            // none, 1 the other colour, 2 its own plain, 3 its own doubled.
            static long GetPanelType_CR(long nTeam, long col, long row);

        private:
            // Main_'s work on doubled panel idx (inline there).
            void SyncDoubleAnm_(long idx);

            long m_nSetType;               // 0x200
            CStgMdl **m_ppPanelMdl;        // 0x204, PANEL_NUM models
            CStgMdl **m_ppEdgeMdl[2];      // 0x208, PANEL_NUM edge models each (sets 2 and 4)
            long *m_pLocalType;            // 0x210, PANEL_NUM local types
            unsigned long *m_pEdgeFlag;    // 0x214, PANEL_NUM edge bits (UpdateEdge_)
            float m_fAnmFrame;             // 0x218, the frame of the doubled panels' loop
            CStgMdl *m_pFeverMdl;          // 0x21C, "s<n>_ground_03"
            CArn *m_pEdgeArn;              // 0x220, archive of "s<n>_panel_edge"
            long m_nEdgeIdx;               // 0x224
    };
}

#endif /* !MSW_CSTAGEVOLLEY10_HPP_ */
