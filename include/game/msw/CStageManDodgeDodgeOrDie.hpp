#ifndef MSW_CSTAGEMANDODGEDODGEORDIE_HPP_
#define MSW_CSTAGEMANDODGEDODGEORDIE_HPP_

#include "game/msw/CStageManDodge.hpp"

namespace msw {

    // INCOMPLETE: the stage of the "Dodge or Die" party game of the dodge
    // game (DodgeF), base of the DOD stages (CStageDodgeDOD55 to 57). Its
    // constructor adds no fields, so the size is CStageManDodge's (0x244).
    // It gives the court, ground and outer field their own measures and
    // places the goals itself.
    //
    // Only the overrides are declared, in the order of their vtable slots;
    // none of them is inline (the unit that defines them emits them).
    class CStageManDodgeDodgeOrDie : public CStageManDodge {
        public:
            CStageManDodgeDodgeOrDie(bcs::CTask *pParent);
            virtual ~CStageManDodgeDodgeOrDie();

            virtual void Disp3D(long);
            virtual float GetCourtLeft() const;
            virtual float GetCourtRight() const;
            virtual float GetCourtFront() const;
            virtual float GetCourtBack() const;
            virtual float GetCourtRadius() const;
            virtual float GetTeamCourtLeft(long team) const;
            virtual float GetTeamCourtRight(long team) const;
            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual float GetTeamCourtFrontZoneLeft(long team) const;
            virtual float GetTeamCourtFrontZoneRight(long team) const;
            virtual float GetTeamCourtFrontZoneFront(long team) const;
            virtual float GetTeamCourtFrontZoneBack(long team) const;
            virtual float GetGroundLeft() const;
            virtual float GetGroundRight() const;
            virtual float GetGroundFront() const;
            virtual float GetGroundBack() const;
            virtual float GetTeamGroundLeft(long team) const;
            virtual float GetTeamGroundRight(long team) const;
            virtual float GetTeamGroundFront(long team) const;
            virtual float GetTeamGroundBack(long team) const;
            virtual float GetOuterFieldLeft() const;
            virtual float GetOuterFieldRight() const;
            virtual float GetOuterFieldFront() const;
            virtual float GetOuterFieldBack() const;
            virtual float GetTeamOuterFieldLeft(long team, long n) const;
            virtual float GetTeamOuterFieldRight(long team, long n) const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
            virtual unsigned long GetAreaFlagByPos(const bcs::FVEC3 &rPos, long) const;
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const;
            virtual const bcs::FVEC3 &GetBoardPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseGroundWidth_() const;
            virtual float GetBaseGroundDepth_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
    };
}

#endif /* !MSW_CSTAGEMANDODGEDODGEORDIE_HPP_ */
