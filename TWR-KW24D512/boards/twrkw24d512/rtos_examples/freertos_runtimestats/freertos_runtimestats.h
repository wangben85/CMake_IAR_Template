#ifndef _FREERTOS_RUNTIMESTATS_H_
#define _FREERTOS_RUNTIMESTATS_H_

extern volatile uint32_t lptmrTicks;
void ConfigureTimerForRunTimeStats(void);
#endif
