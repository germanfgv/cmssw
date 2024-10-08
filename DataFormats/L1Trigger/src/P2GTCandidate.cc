#include "DataFormats/L1Trigger/interface/P2GTCandidate.h"

namespace l1t {

  bool P2GTCandidate::operator==(const P2GTCandidate& rhs) const {
    return objectType_ == rhs.objectType_ && hwPT_ == rhs.hwPT_ && hwPhi_ == rhs.hwPhi_ && hwEta_ == rhs.hwEta_ &&
           hwZ0_ == rhs.hwZ0_ && hwIsolationPT_ == rhs.hwIsolationPT_ && hwQualityScore_ == rhs.hwQualityScore_ &&
           hwQualityFlags_ == rhs.hwQualityFlags_ && hwCharge_ == rhs.hwCharge_ && hwD0_ == rhs.hwD0_ &&
           hwBeta_ == rhs.hwBeta_ && hwMass_ == rhs.hwMass_ && hwIndex_ == rhs.hwIndex_ &&
           hwSeed_pT_ == rhs.hwSeed_pT_ && hwSeed_z0_ == rhs.hwSeed_z0_ && hwScalarSumPT_ == rhs.hwScalarSumPT_ &&
           hwNumber_of_tracks_ == rhs.hwNumber_of_tracks_ &&
           hwNumber_of_displaced_tracks_ == rhs.hwNumber_of_displaced_tracks_ && hwSum_pT_pv_ == rhs.hwSum_pT_pv_ &&
           hwType_ == rhs.hwType_ && hwNumber_of_tracks_in_pv_ == rhs.hwNumber_of_tracks_in_pv_ &&
           hwNumber_of_tracks_not_in_pv_ == rhs.hwNumber_of_tracks_not_in_pv_;
  }

  bool P2GTCandidate::operator!=(const P2GTCandidate& rhs) const { return !operator==(rhs); };
}  // namespace l1t
