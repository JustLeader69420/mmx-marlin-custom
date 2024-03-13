#ifndef _BABYSTEP_H_
#define _BABYSTEP_H_

#define BABYSTEP_MAX_HIGH 5.0f
#define WHO_AXIS Z_AXIS

extern bool autoCloseBabysetp;
extern float old_baby_step_value;
TERN_(LEVELING_OFFSET, extern float babystep_value);
// extern float old_z_offset;

void menuBabyStep(void);
float getBabyStepZAxisTotalMM();
void setBabyStepZAxisIncMM(float mm);

#endif
