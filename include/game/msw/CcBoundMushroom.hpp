#ifndef MSW_CCBOUNDMUSHROOM_HPP_
#define MSW_CCBOUNDMUSHROOM_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CChrOptMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    class CCollBodyUnit;

    namespace CC_CMN {
        namespace KINOPIO {

            // (DOL class; meaning guessed from the names) A mushroom of
            // Toad's air trick that bounces about a point of a court and
            // gets in the characters' way. 0x128 bytes (the modules
            // allocate their CC09_<GAME>::CcKinopioBoundMushroom, which adds
            // no member, with that size).
            //
            // INCOMPLETE: the bases and virtual functions are read from
            // __vt__Q33msw11CC09_VOLLEY22CcKinopioBoundMushroom (VolleyF):
            // CChrOptMdlObj at 0x94 and the state machine at 0xB0 (its
            // vptr at 0xF4). The virtuals the class adds come after the
            // state machine's in that vtable. Members mapped only where the
            // modules read them; names guessed.
            class CcBoundMushroom : public CChrObstacle, public CChrOptMdlObj, public bcs::CFsm<CcBoundMushroom> {
                public:
                    // At pos in court nCourt (negative: pos is absolute),
                    // fScale the size; nPlayer the owner, -1 for none.
                    CcBoundMushroom(bcs::CTask *pParent, nw4r::g3d::ResFile mdl, nw4r::g3d::ResFile mot,
                                    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo, const bcs::FVEC3 &pos,
                                    float fScale, long nCourt, bcs::BCS_Color color, long nPlayer);
                    virtual ~CcBoundMushroom();
                    virtual void Main();
                    virtual long SendMessage(long, long, long, long);
                    virtual float GetAttackRadius() const;
                    virtual float GetAttackRange() const;
                    virtual int IsAttackEnd() const;

                    virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
                    // The size of the mushroom relative to the parameters.
                    virtual float GetParamScale_() const;
                    virtual void SetAppearEffect_();
                    virtual void SetDisappearEffect_();
                    // Advances the bounce by fFrame frames.
                    virtual void MovePos_(float fFrame);
                    // Places the mushroom for the bounce.
                    virtual void CalcPos_();
                    virtual void PlaySe_(long nSe);
                    virtual void PlayLoopSe_();
                    virtual void StopLoopSe_();

                protected:
                    unsigned char m_UnkF8[0x4];
                    CBodyCollision m_BodyColl;   // 0xFC
                    unsigned char m_Unk108[0x4];
                    bcs::FVEC3 m_BasePos;        // 0x10C, the point it bounces about
                    float m_fSwing;              // 0x118, the side to side swing (0 to 1)
                    float m_fTurn;               // 0x11C, the turn round the point (0 to 1)
                    long m_nCourt;               // 0x120, negative for none
                    unsigned char m_Unk124[0x4];
            };
        }
    }
}

#endif /* !MSW_CCBOUNDMUSHROOM_HPP_ */
