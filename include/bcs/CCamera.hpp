#ifndef BCS_CCAMERA_HPP_
#define BCS_CCAMERA_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {

    // Size 0xFC. The class lives in the DOL. Layout from its constructor
    // (__ct__Q23bcs7CCameraFlsPQ23bcs5CTask), destructor and Main; the order
    // of the virtual functions from the vtables the modules emit for the
    // classes derived from it.
    class CCamera : public CTask {
        public:
            // (name guessed) Size 0x50. What MainCamera fills in and Main
            // turns into a view.
            struct VIEW_DATA {
                FVEC3 pos;     // 0x00 (name guessed)
                FVEC3 at;      // 0x0C (name guessed)
                float roll;    // 0x18, radians (name guessed)
                FVEC3 m_Unk1C; // 0x1C: pos moved along the three axes below
                FVEC3 axisX;   // 0x28 (name guessed)
                FVEC3 axisY;   // 0x34 (name guessed)
                FVEC3 axisZ;   // 0x40 (name guessed)
                float m_Unk4C; // 0x4C: an angle from axisZ.x and axisZ.z
            };

            CCamera(long view, short priority, CTask *pParent);
            virtual ~CCamera();
            virtual void Main();
            virtual void Disp();
            virtual long GetCamKind() const { return 0; } // return type guessed
            // Called by Main while this is the view's active camera.
            virtual void MainCamera() {}
            virtual int OnActive(); // returns 0; type guessed
            virtual int OnLost();   // returns 0; type guessed

            // Makes this the camera of `view`; -1 is the camera's own view
            // (meaning of -1 guessed). Return type guessed.
            void SetActive(long view);

            // (name guessed) The view the camera belongs to.
            long GetView() const { return m_nView; }
            // (name guessed) Read by msw::CTournamentMan (TourF).
            const VIEW_DATA &GetViewData() const { return m_Data; }

            // Static: the body in the DOL reads the data block through r3.
            static void LoadQuakeData(const void *pData);

        protected:
            VIEW_DATA m_Data;            // 0x34 (name guessed)
            VIEW_DATA m_DataSave;        // 0x84: a copy of m_Data made by the constructor;
                                         // msw::CTournamentCamera::OnActive copies it back (name guessed)

        private:
            unsigned char m_UnkD4[0x18]; // 0xD4: the quake in progress
            float m_UnkEC;               // 0xEC
            short m_nCamPriority;        // 0xF0: the constructor's `priority` (name guessed)

        protected:
            long m_nView;                // 0xF4: the constructor's `view` (name guessed)

        private:
            unsigned long m_UnkF8;       // 0xF8: 1 from the constructor
    };
}

#endif /* !BCS_CCAMERA_HPP_ */
