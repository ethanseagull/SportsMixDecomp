#ifndef CEVENTDEMOBASE_HPP_
#define CEVENTDEMOBASE_HPP_

#include "bcs/CTask.hpp"

namespace msw {
    class CEventCameraBase;
    class CEventDemo;

    // Base of each sport's event-demo task (DOL, size 0x40, vptr at 0).
    // One instance at a time, kept in m_pInstance. The virtuals are declared
    // in the order of its vtable (lbl_8046EFC0): bcs::CTask's four, then
    // fourteen of its own (Disp, an override, is declared last; see there).
    // The DOL's default bodies have no exported name; the ones below repeat
    // what they do (return 0, 1, -1 or nothing). Parameter names are guessed.
    class CEventDemoBase : public bcs::CTask {
        public:
            CEventDemoBase(bcs::CTask *pParent);
            virtual ~CEventDemoBase();
            virtual void Main();

            // Starts demo nDemo (0 entrance, 1 victory, 4 ready-start,
            // 5 change-court in every sport seen so far).
            virtual void CreateDemo(long nDemo, bcs::CTask *pParent) {}
            virtual int IsDemoPlay(long nDemo) const { return 0; }
            virtual void TimerEvent(short, short, unsigned long) {}
            virtual void CameraEvent(const char *pName, long, long, float) {}
            virtual void SendMessage(long, long, long, long) {}
            // Telops: cockpit objects shown by a demo. nCloseAnim < 0 leaves
            // the object's close animation alone.
            virtual void CreateTelop(long nObjId, long nCloseAnim) {}
            virtual void CreateTelop(long nObjId, long nParam, long nCloseAnim) {}
            virtual void PlayEndTelop(long nObjId) {}
            virtual void RemoveTelop(long nObjId) {}
            virtual int IsEndTelop(long nObjId) { return 1; }
            virtual void PlayEndTelopAll() {}
            virtual void RemoveTelopAll() {}
            virtual long GetTelopId(long nTelop) { return -1; }
            virtual void SetCockpitState(long, long) {}
            // bcs::CTask's slot. Declared last: the module copies of the
            // inline virtuals come out in reverse declaration order, and
            // the original has Disp before SendMessage.
            virtual void Disp() {}

            static CEventDemoBase *GetInstance();
            static CEventDemoBase *GetPtr();

            // (names guessed; no exported name in the DOL, fn_8011162C and
            // fn_80111668, both just after Main in the same file)
            // Passes a camera event to the running demo, if any. Called by
            // the event camera.
            static void SendCameraEvent(const char *pName, long nArg1, long nArg2, float fArg);
            // Queues pFunc to be called once the master fade that is playing
            // ends (Main); CEventDemo::Cleanup queues its cleanup this way.
            static void AddFadeEndFunc(void (*pFunc)());

        protected:
            // Called by each sport's CreateInstance once the object is made:
            // creates msw::CEvDemoRes and copies two bytes of
            // CGame::g_GameWork into it.
            void _Init();
            // Frees the list at 0x38 (name guessed: used by the destructors).
            void _Destroy();

            static CEventDemoBase *m_pInstance;

        private:
            // The static demo camera calls read m_pCamera directly.
            friend class CEventDemo;

            // An entry of the list at 0x38 (8 bytes, AddFadeEndFunc).
            struct FadeEndFunc {
                FadeEndFunc *pNext;
                void (*pFunc)();
            };

            CEventCameraBase *m_pCamera; // 0x34: a CEventCamera made by _Init (name guessed)
            FadeEndFunc *m_pList;       // 0x38: newest first; freed by _Destroy and Main
            int m_bFade;                // 0x3C: last IsPlayMasterFade result (Main)
    };
}

#endif /* !CEVENTDEMOBASE_HPP_ */
