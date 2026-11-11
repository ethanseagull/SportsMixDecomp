#ifndef MSW_CSTAGEVOLLEY03_HPP_
#define MSW_CSTAGEVOLLEY03_HPP_

#include "game/msw/CStageManVolley.hpp"

namespace msw {

    class CStgGmk_WaterWorks_Volley;

    // Volley stage 03, the fountain stage, made by CStageFactoryVolley. Size
    // 0x244: CStageManVolley, then twelve water jets
    // (msw::CStgGmk_WaterWorks_Volley) lined up along the net. Every
    // VOLLEY_PRM_S03_WATER_INTERVAL seconds a pattern of jets gushes up
    // (VOLLEY_PRM_S03_WATER_PATTERN picks it); a gushing jet blocks the ball.
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw14CStageVolley03.
    class CStageVolley03 : public CStageManVolley {
        public:
            CStageVolley03(bcs::CTask *pParent);
            virtual ~CStageVolley03();

            virtual void Disp();
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseNetHeightBottom_() const;
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                        nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnPostInitialize();
            virtual void OnTurnInit();
            virtual void OnArtsShootStart();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // The mean position of the risen jets; false (and the origin)
            // when none is up.
            virtual bool Water_GetCenterPos(bcs::FVEC3 &rPos) const;
            virtual void UpdateWaterPattern_(float frame);
            // Spreads the first VOLLEY_PRM_S03_WATER_NUM jets evenly along
            // the net and takes the others out.
            virtual void UpdateWaterPos_(float frame);
            // Starts the jets of pattern `pattern` (0 to 5).
            virtual void Water_Start(long pattern);
            virtual void Water_StopAll();
            virtual void Water_ResetAll();
            virtual bool Water_IsSwooshAll() const;
            virtual void Water_NextPattern();
            // A jet blocks the span from x - r to x + r.
            virtual bool Water_CheckHit(float x, float r) const;

        private:
            enum { WATER_MAX = 12 };

            CStgGmk_WaterWorks_Volley *m_apWater[WATER_MAX]; // 0x200
            long m_nPattern;                                 // 0x230, the pattern Water_Start runs next
            long m_Unk234;                                   // 0x234 (not touched here)
            float m_fInterval;                               // 0x238, frames until the next pattern
            float m_fUnk23C;                                 // 0x23C, cleared when a pattern starts
            float m_fStayTime;                               // 0x240, frames a pattern's jets stay up
    };
}

#endif /* !MSW_CSTAGEVOLLEY03_HPP_ */
