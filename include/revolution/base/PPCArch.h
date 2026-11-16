#ifndef REVOLUTION_BASE_PPCARCH_H_
#define REVOLUTION_BASE_PPCARCH_H_

// Only what the game's code uses so far (added for DOL game/bcs/CMovie.cpp).

#ifdef __cplusplus
extern "C" {
#endif

// Bit 0x10000000 of HID2 is set while the locked cache is on.
#define HID2_LCE 0x10000000

// Reads the HID2 register.
unsigned long PPCMfhid2(void);
// (added for the DOL's game/bcs/CMainApp.cpp) Stops the processor for good.
void PPCHalt(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_BASE_PPCARCH_H_ */
