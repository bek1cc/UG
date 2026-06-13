// a_samp.inc - Open.mp / SA-MP Compatible
// Without SAMP_03DL defined, uses open.mp includes (which have all SA-MP natives + extras)
// With SAMP_03DL defined, uses SA-MP 0.3.DL compatibility layer

#if defined _INC_a_samp
        #endinput
#endif

#define _INC_a_samp

// The _open_mp.inc will check for SAMP_03DL and include samp_03dl_compat.inc
// instead of the open.mp includes
#include <open.mp>
