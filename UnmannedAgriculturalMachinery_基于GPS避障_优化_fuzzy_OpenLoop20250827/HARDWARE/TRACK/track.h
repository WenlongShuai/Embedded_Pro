#ifndef __TRACK_H__
#define __TRACK_H__

#include <math.h>
#include "sys.h"
#include "hi600.h"
#include "w25qxx.h"

void Track_Init(void);
void Track(void);
void POSI_COPY(POSI_ST* posi_dest,POSI_ST posi_sour);


#endif /* __TRACK_H__ */

