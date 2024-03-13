#ifndef _LEVELINGOFFSET_H_
#define _LEVELINGOFFSET_H_

#include "stdint.h"

#define LODEVA 0.6f   //LevelingOffset default values 调平补偿的默认值

extern float LevelingOffset;
extern float oldLevelingOffset; 

void menuLevelingOffset(void);
float getLevelingOffset(void);
void setLevelingOffset(float offset);
void resetLevelingOffset(void);
bool saveOffset();
void menuSetLevelingOffset();
void menuSetLevelingValue();
void setLO_flag(bool _flag);

#endif
