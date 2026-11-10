#ifndef CTOURNAMENTCHIP_HPP_
#define CTOURNAMENTCHIP_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CEventModel.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {
    class CScnGroup;
}

namespace msw {

    class CTrophyObj;

    // A chip (stage marker) of the tournament map: a bcs::CMdlObj made
    // from one model resource, shown as a msw::CEventModel. Module TourF,
    // game/tour/CTournamentChip.cpp. Size 0xD0 (CreateModel allocates it).
    // msw::CTournamentRouteMan creates one per map point and fills the stage
    // values (0xBC to 0xC8) directly. Member names are guessed.
    class CTournamentChip : public CEventModel {
        public:
            // The constructor is inline in CreateModel.
            CTournamentChip()
                : CEventModel(0), m_Mdl(), m_pMdlObj(0), m_nUnkB8(0), m_nStagePoint(-1), m_nStageChip(-1),
                  m_fFrame(0.0f)
            {
                SetType(-1);
            }

            virtual ~CTournamentChip();
            virtual void _SetPos(const bcs::FVEC3 &rPos);
            virtual void _SetRot(const bcs::FVEC3 &rRot);
            virtual void _SetScale(const bcs::FVEC3 &rScale);
            virtual void SetDisp(int bDisp);
            virtual void UpdateFrame(float fFrame);
            virtual void SetDrawLayer(long nLayer);
            virtual void SetDrawPriority(long nPriority);
            virtual void AppendChild(bcs::CScnGroup *pGroup);
            virtual void SetLightSelector(long nSelector);
            virtual void ResetLightSelector();
            virtual bcs::CScnObj *GetScnObj() { return m_pMdlObj; }
            virtual const bcs::CScnObj *GetScnObj() const { return m_pMdlObj; }
            virtual const bcs::FMTX34 *GetNodeWorldMtxPtr(unsigned long nNode) const;
            virtual long GetNodeId(const char *pName) const;
            virtual void Create();
            virtual void Destroy();

            // Makes a chip from model resource `mdl`, shown or hidden, in
            // draw layer nLayer, with light selector nSelector (-1 for none).
            static CTournamentChip *CreateModel(nw4r::g3d::ResMdl mdl, int bDisp, long nLayer, long nSelector);

        private:
            // The part of Create that CreateModel shares: makes the model
            // object from m_Mdl. (name guessed; inline in both)
            void _CreateMdlObj();

            nw4r::g3d::ResMdl m_Mdl;  // 0xB0
            bcs::CMdlObj *m_pMdlObj;  // 0xB4
            unsigned long m_nUnkB8;   // 0xB8: cleared by the constructor and Destroy, not read here

        public:
            // Set by msw::CTournamentRouteMan; UpdateFrame passes them on
            // to it two frames after the chip was made, then sets them to
            // -1. A point is set up in preference to a chip.
            long m_nStagePoint;       // 0xBC: argument of SetupStagePoint, -1 for none
            long m_nStageChip;        // 0xC0: arguments of SetupStageChip, -1 for none
            long m_nStageChipArg1;    // 0xC4
            long m_nStageChipArg2;    // 0xC8

        private:
            float m_fFrame;           // 0xCC: frames since the model was made
    };

    // The tournament cup on the map: a msw::CTrophyObj shown as a
    // msw::CEventModel. Module TourF, game/tour/CTournamentChip.cpp. Size
    // 0xB8 (CreateModel allocates it). Member names are guessed.
    class CTournamentTrophy : public CEventModel {
        public:
            // The constructor is inline in CreateModel.
            CTournamentTrophy() : CEventModel(0), m_pTrophy(0), m_nUnkB4(0) {}

            virtual ~CTournamentTrophy();
            virtual void _SetPos(const bcs::FVEC3 &rPos);
            virtual void _SetRot(const bcs::FVEC3 &rRot);
            virtual void _SetScale(const bcs::FVEC3 &rScale);
            virtual void SetDisp(int bDisp);
            virtual void UpdateFrame(float fFrame);
            virtual void SetDrawLayer(long nLayer);
            virtual void SetDrawPriority(long nPriority);
            virtual void AppendChild(bcs::CScnGroup *pGroup);
            virtual void SetLightSelector(long nSelector);
            virtual void ResetLightSelector();
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual const bcs::FMTX34 *GetNodeWorldMtxPtr(unsigned long nNode) const;
            virtual long GetNodeId(const char *pName) const;
            virtual void SetBaseColor(bcs::BCS_Color color);
            virtual void SetColorScale(bcs::BCS_Color color);
            virtual void Destroy();

            // Makes the cup from archive nArc (a msw::CLoadData entry) for
            // the cup and game mode being played, in draw layer nLayer, with
            // light selector nSelector (-1 for none). (the two ints: the
            // first is passed on to CTrophyObj::CreateTrophy, the second is
            // not read)
            static CTournamentTrophy *CreateModel(long nArc, int, int, long nLayer, long nSelector);

        private:
            CTrophyObj *m_pTrophy;    // 0xB0
            unsigned long m_nUnkB4;   // 0xB4: cleared by the constructor and Destroy, not read here
    };
}

#endif /* !CTOURNAMENTCHIP_HPP_ */
