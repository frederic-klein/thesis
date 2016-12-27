/*! \file SMPC_core.h
    \brief TODO insert brief description.

    TODO insert detailed description.
*/
void smpc_init();
void smpc_generate_shares(int shares[], int n, int k, int secret); //!< creates shares out of the secret and stores them in the shares array
int smpc_lagrange_interpolation(int involved_parties[], int shares[], int k);
