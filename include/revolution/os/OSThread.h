#ifndef REVOLUTION_OS_OSTHREAD_H_
#define REVOLUTION_OS_OSTHREAD_H_

// Only what the game's code uses so far (added for DOL game/bcs/CMovie.cpp).
// The structures are opaque here: only their sizes matter to the game.

#ifdef __cplusplus
extern "C" {
#endif

// A thread: saved context, state, queues and stack bounds (0x318 bytes).
typedef struct OSThread {
    unsigned char m_Unk000[0x318];
} OSThread;

// A recursive lock (0x18 bytes: waiting queue, owner, count, link).
typedef struct OSMutex {
    unsigned char m_Unk00[0x18];
} OSMutex;

void OSInitMutex(OSMutex *mutex);
void OSLockMutex(OSMutex *mutex);
void OSUnlockMutex(OSMutex *mutex);

// (added for the DOL's game/bcs/CMainApp.cpp) A counting semaphore (count
// and waiting queue, 0x10 bytes). OSSignalSemaphore has no name in the DOL
// yet (fn_8027CB30).
typedef struct OSSemaphore {
    unsigned char m_Unk00[0x10];
} OSSemaphore;

long OSWaitSemaphore(OSSemaphore *sem);
OSThread *OSGetCurrentThread(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_OS_OSTHREAD_H_ */
