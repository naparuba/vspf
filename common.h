#ifndef COMMON_H
#define COMMON_H

#define TRUE 1
#define FALSE 0

#define DEBUG 1

//dont define les type
typedef unsigned long ul32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

//juste pour comptatiblite IHM
#define FIFO_CORE_TO_IHM "./core_to_ihm"
#define FIFO_IHM_TO_CORE "./ihm_to_core"


#define FIFO_GUARDIANS_TO_IHM "./core_to_ihm"
#define FIFO_IHM_TO_GUARDIANS "./ihm_to_core"

#define FIFO_SIN_TO_GUARDIANS "./sin_to_guardians"
#define FIFO_GUARDIANS_TO_SIN "./guardians_to_sin"

#define FIFO_DEBUG_SIN "./fifo_debug_sin"
#define FIFO_WARN_SIN "./fifo_warn_sin"
#define FIFO_CRIT_SIN "./fifo_crit_sin"

#define FIFO_DEBUG_GUARDIANS "./fifo_debug_guardians"
#define FIFO_WARN_GUARDIANS "./fifo_warn_guardians"
#define FIFO_CRIT_GUARDIANS "./fifo_crit_guardians"

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#endif
