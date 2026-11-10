#ifndef MSW_CFGMDL_HPP_
#define MSW_CFGMDL_HPP_

#include "bcs/CMultViewAlphaMdlObj.hpp"
#include "game/msw/CStgMdlObjImpl.hpp"

namespace msw {

    // A model of the stage that fades out by view (crowds, figures). A DOL
    // class, size 0x224. Its own virtual functions follow the vtable part
    // of CStgMdlObjImpl (slot 0x128 on). They get and set the members of
    // CStgMdlObjImpl; their names are guessed. The two alpha test ranges are
    // named after the BasketF parameters (BASKET_PRM_S06_ALPHA_TEST_NEAR_RANG
    // and ..._FAR_RANG) that CNpcGhostBasketBase passes to them.
    class CFgMdl : public bcs::CMultViewAlphaMdlObj, public CStgMdlObjImpl {
        public:
            CFgMdl();
            virtual ~CFgMdl();

            virtual float GetAlphaTestNearRange() const;       // 0x128, the float at 0x200
            virtual void SetAlphaTestNearRange(float range);   // 0x12C
            virtual float GetAlphaTestFarRange() const;        // 0x130, 0x204
            virtual void SetAlphaTestFarRange(float range);    // 0x134
            virtual float GetFadeDir() const;                  // 0x138, 0x208: an angle about Y
            virtual void SetFadeDir(float rad);                // 0x13C
            virtual float GetFadeAngle0() const;               // 0x140, 0x20C: an angle
            virtual void SetFadeAngle0(float rad);             // 0x144
            virtual float GetFadeAngle1() const;               // 0x148, 0x210: an angle
            virtual void SetFadeAngle1(float rad);             // 0x14C
            virtual float GetUnk214() const;                   // 0x150
            virtual void SetUnk214(float);                     // 0x154
            virtual float GetUnk218() const;                   // 0x158
            virtual void SetUnk218(float);                     // 0x15C
            virtual float GetUnk214And218() const;             // 0x160, returns GetUnk218()
            virtual void SetUnk214And218(float);               // 0x164, sets both
            virtual unsigned long GetFadeEnable() const;       // 0x168, the word at 0x21C
            virtual void SetFadeEnable(unsigned long bEnable); // 0x16C
            // More virtual functions follow (to 0x184).

            // (guessed) Inline in the modules: VolleyF's stage and BasketF's
            // CNpcGhostBasketBase::NewMdlObj_ both call the two functions in
            // a row.
            void Create(nw4r::g3d::ResMdl mdl, unsigned long flag, long n)
            {
                bcs::CMultViewAlphaMdlObj::Create(mdl, flag, n);
                PatchMaterial_(mdl, m_BaseColor);
            }
    };
}

#endif /* !MSW_CFGMDL_HPP_ */
