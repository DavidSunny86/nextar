//@ Abhishek Dey
//@ Jan 9 2007
//@ Sep 17 2007

#ifndef XCOL_ROUTINES_H
#define XCOL_ROUTINES_H

#ifdef __cplusplus
extern "C" {
#endif

void xFlipMirror16B(s16* outdata,const s16* indata,s32 width,s32 height,s32 pitch);
void xFlipMirror24Bto32BShuffle(s32* outdata,const char* indata,s32 width,s32 height,s32 pitch);
void xFlipMirror24Bto16BShuffle(s32* outdata,const char* indata,s32 width,s32 height,s32 pitch);
void xFlipMirror32Bto32B(s32* outdata,const s32* indata,s32 width,s32 height,s32 pitch);

#ifdef __cplusplus
}
#endif

#endif //XCOL_ROUTINES_H