#ifndef __WWDG_H
#define __WWDG_H

#include "stm32f10x_wwdg.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

void WWDG_Config(void);
void WWDG_FeedDog(void);

#endif
