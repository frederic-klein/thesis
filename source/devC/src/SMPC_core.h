/*! \file SMPC_core.h
    \brief TODO insert brief description.

    TODO insert detailed description.
*/

typedef enum{SMPC_CORE_SUM, SMPC_CORE_MIN, SMPC_CORE_MIN_DISQUALIFIED, SMPC_CORE_MAX, SMPC_CORE_MAX_DISQUALIFIED}smpc_share_type;

void smpc_init();
void smpc_generate_shares(int shares[], int n, int k, int secret, smpc_share_type type); //!< creates shares out of the secret and stores them in the shares array
int get_share(int party);
int smpc_lagrange_interpolation(int involved_parties[], int shares[], int k);
