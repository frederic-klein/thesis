/*! \file configurations.h
    \brief Configuaration file for SMPC lib.

    Details.
*/

#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H


#define CONFIGURATIONS_MAX_SCORE 10000 //!<
#define CONFIGURATIONS_MINIMUM_COMPUTATION_GROUP 20 //!< n; number of nodes in computation group; never set value lower than 3!
#define CONFIGURATIONS_MAXIMUM_COMPUTATION_GROUP 20 //!< set to same value as minimum for constant group -> better performance
#define CONFIGURATIONS_DISCOVERY_LIMIT 20 //!< upper bound for device macs returned from discovery
#define CONFIGURATIONS_SHARES_THRESHOLD 0 //!< 0: maximum security, all shares are needed for reconstruction; value will be ignored if >n/2
#define CONFIGURATIONS_BOUNDING_PRIME 2147483647  //!< upper bound for device macs returned from discovery
//#define CONFIGURATIONS_BOUNDING_PRIME 599251  //!< upper bound for device macs returned from discovery
//#define CONFIGURATIONS_BOUNDING_PRIME 104729  //!< upper bound for device macs returned from discovery
//#define CONFIGURATIONS_BOUNDING_PRIME 17 // TODO remove DEBUG

//#define CONFIGURATIONS_BOUNDING_PRIME 52501 // TODO remove DEBUG
// OPTIONAL
#define CONFIGURATIONS_MIN_SCORE 100 //!<


#endif //CONFIGURATIONS_H
