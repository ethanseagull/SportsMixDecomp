#ifndef CCOCKPIT_HPP_
#define CCOCKPIT_HPP_

#include "game/msw/CMenuTask.hpp"

namespace msw {

    // The task that owns the in-game displays. Lives in the DOL; vptr at 0.
    // The virtual functions are declared in the order of its vtable
    // (lbl_804710C0): bcs::CTask's four, then thirteen of its own. Those
    // with no name in the DOL are placeholders.
    class CCockpit : public CMenuTask {
        public:
            CCockpit(bcs::CTask *pParent);
            virtual ~CCockpit();
            virtual void Main();
            virtual void Disp();

            virtual void Unk18() = 0;
            virtual void SetTeamScoreCoder(long, long);
            virtual void AddTeamScoreCoder(long, long);
            virtual void SetTeamScoreColorCoder(long, int);
            virtual void SetTeamCoinCoder(long, long);
            virtual void AddTeamCoinCoder(long, long, long);
            virtual void Unk30();
            virtual void Unk34();
            virtual void Unk38();
            virtual void Unk3C();
            virtual void SetItemRouletteStateImpl(long, long, long);
            // The archive that holds layout file nFile (0 to 7).
            virtual const char *GetArchiveFileName(long nFile);
            virtual const char *GetArchiveRootFolderName(long nFile);

            // Looks the task up in the task manager by s_TaskHandle; null
            // when there is none.
            static CCockpit *GetTask();

            // All static: each loads the task from s_TaskHandle itself.
            static long CreateObj(long nObjId, long nParam); // -1 without a task
            static long RemoveObj(long nObjId);               // -1 without a task
            static long IsEndObj(long nObjId);                // 1 without a task
            static void SetAnimEndClose(long nObjId, int bClose); // return type guessed
            // The word at 0x4C + 4 * nFile of the task (nFile 0 to 7); null
            // without a task or for another nFile.
            static const char *GetResourceFileName(long nFile);
    };
}

#endif /* !CCOCKPIT_HPP_ */
