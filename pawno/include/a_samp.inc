// SA-MP 0.3.DL Compatible a_samp.inc
// When SAMP_03DL is defined, includes standard SA-MP natives via _open_mp
// which will redirect to samp_03dl_compat.inc

#if defined _INC_a_samp
        #endinput
#endif

#define _INC_a_samp

// The _open_mp.inc will check for SAMP_03DL and include samp_03dl_compat.inc
// instead of the open.mp includes
#include <open.mp>
