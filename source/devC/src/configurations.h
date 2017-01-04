/*! \file configurations.h
    \brief Configuaration file for SMPC lib.

    Details.
*/

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#ifndef EVAL_VALUE
#define EVAL_VALUE 20
#endif // EVAL_VALUE

#define CONFIGURATIONS_DISCOVERY_LIMIT EVAL_VALUE //!< upper bound for number of device macs returned from discovery
#define CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP EVAL_VALUE //!< n; number of nodes in computation group; never set value lower than 3!
#define CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP EVAL_VALUE //!< set to same value as minimum for constant group -> better performance; limited by p/max_score!

#define CONFIGURATIONS_MAX_SCORE 10000 //!< upper bound for score value; important for number of rounds in smpc min and max

// wolfCrypt settings
#define CONFIGURATIONS_AES_KEY_SIZE 192 //!< AES key size int bit
#define CONFIGURATIONS_RSA_KEY_SIZE 3072 //!< AES key size int bit


#ifndef CONFIGURATIONS_SHARES_THRESHOLD
#define CONFIGURATIONS_SHARES_THRESHOLD 0 //!< 0: maximum security, all shares are needed for reconstruction; value will be ignored if >n/2
#endif // CONFIGURATIONS_SHARES_THRESHOLD

#define CONFIGURATIONS_BOUNDING_PRIME 2147483647  //!< upper bound for Z_p range [0,p-1]
//#define CONFIGURATIONS_BOUNDING_PRIME 599251  //!< upper bound
//#define CONFIGURATIONS_BOUNDING_PRIME 104729  //!< upper bound
//#define CONFIGURATIONS_BOUNDING_PRIME 17 // TODO remove DEBUG

// OPTIONAL
#define CONFIGURATIONS_MIN_SCORE 100 //!<


#endif //CONFIGURATIONS_H
