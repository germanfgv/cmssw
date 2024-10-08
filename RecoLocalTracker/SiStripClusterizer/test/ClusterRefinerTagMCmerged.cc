//
// Use MC truth to identify merged clusters, i.e., those associated with more than one
// (in-time) SimTrack.
//
// Author:  Bill Ford (wtford)  6 March 2015
//

// system includes
#include <memory>

// user includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "FWCore/Framework/interface/Event.h"

class ClusterRefinerTagMCmerged : public edm::stream::EDProducer<> {
public:
  explicit ClusterRefinerTagMCmerged(const edm::ParameterSet& conf);
  virtual void produce(edm::Event&, const edm::EventSetup&);

private:
  template <class T>
  bool findInput(const edm::EDGetTokenT<T>&, edm::Handle<T>&, const edm::Event&);
  virtual void refineCluster(const edm::Handle<edmNew::DetSetVector<SiStripCluster>>& input,
                             edmNew::DetSetVector<SiStripCluster>& output);

  const edm::InputTag inputTag;
  typedef edm::EDGetTokenT<edmNew::DetSetVector<SiStripCluster>> token_t;
  token_t inputToken;
  edm::ParameterSet confClusterRefiner_;
  bool useAssociateHit_;

  TrackerHitAssociator::Config trackerHitAssociatorConfig_;
  std::unique_ptr<TrackerHitAssociator> associator_;
};

ClusterRefinerTagMCmerged::ClusterRefinerTagMCmerged(const edm::ParameterSet& conf)
    : inputTag(conf.getParameter<edm::InputTag>("UntaggedClusterProducer")),
      confClusterRefiner_(conf.getParameter<edm::ParameterSet>("ClusterRefiner")),
      useAssociateHit_(!confClusterRefiner_.getParameter<bool>("associateRecoTracks")),
      trackerHitAssociatorConfig_(confClusterRefiner_, consumesCollector()) {
  produces<edmNew::DetSetVector<SiStripCluster>>();
  inputToken = consumes<edmNew::DetSetVector<SiStripCluster>>(inputTag);
}

void ClusterRefinerTagMCmerged::produce(edm::Event& event, const edm::EventSetup& es) {
  auto output = std::make_unique<edmNew::DetSetVector<SiStripCluster>>();
  output->reserve(10000, 4 * 10000);

  associator_.reset(new TrackerHitAssociator(event, trackerHitAssociatorConfig_));
  edm::Handle<edmNew::DetSetVector<SiStripCluster>> input;

  if (findInput(inputToken, input, event))
    refineCluster(input, *output);
  else
    edm::LogError("Input Not Found") << "[ClusterRefinerTagMCmerged::produce] ";  // << inputTag;

  LogDebug("Output") << output->dataSize() << " clusters from " << output->size() << " modules";
  output->shrink_to_fit();
  event.put(std::move(output));
}

void ClusterRefinerTagMCmerged::refineCluster(const edm::Handle<edmNew::DetSetVector<SiStripCluster>>& input,
                                              edmNew::DetSetVector<SiStripCluster>& output) {
  for (edmNew::DetSetVector<SiStripCluster>::const_iterator det = input->begin(); det != input->end(); det++) {
    // DetSetVector filler to receive the clusters we produce
    edmNew::DetSetVector<SiStripCluster>::TSFastFiller outFill(output, det->id());
    uint32_t detId = det->id();
    int ntk = 0;
    int NtkAll = 0;
    for (edmNew::DetSet<SiStripCluster>::iterator clust = det->begin(); clust != det->end(); clust++) {
      auto const& amp = clust->amplitudes();
      SiStripCluster* newCluster = new SiStripCluster(clust->firstStrip(), amp.begin(), amp.end());
      if (associator_ != 0) {
        std::vector<SimHitIdpr> simtrackid;
        if (useAssociateHit_) {
          std::vector<PSimHit> simhit;
          associator_->associateCluster(clust, DetId(detId), simtrackid, simhit);
          NtkAll = simtrackid.size();
          ntk = 0;
          if (simtrackid.size() > 1) {
            for (auto const& it : simtrackid) {
              int NintimeHit = 0;
              for (auto const& ih : simhit) {
                // std::cout << "  hit(tk, evt) trk(tk, evt) bunch ("
                // 	  << ih.trackId() << ", " << ih.eventId().rawId() << ") ("
                // 	  << it.first << ", " << it.second.rawId() << ") "
                // 	  << ih.eventId().bunchCrossing()
                // 	  << std::endl;
                if (ih.trackId() == it.first && ih.eventId() == it.second && ih.eventId().bunchCrossing() == 0)
                  ++NintimeHit;
              }
              if (NintimeHit > 0)
                ++ntk;
            }
          }
        } else {
          associator_->associateSimpleRecHitCluster(clust, DetId(detId), simtrackid);
          ntk = NtkAll = simtrackid.size();
        }
        if (ntk > 1) {
          newCluster->setMerged(true);
        } else {
          newCluster->setMerged(false);
        }
      }
      outFill.push_back(*newCluster);
      // std::cout << "t_m_w " << " " << NtkAll << " " << newCluster->isMerged()
      // 		<< " " << clust->amplitudes().size()
      // 		<< std::endl;
    }  // traverse clusters
  }  // traverse sensors
}

template <class T>
inline bool ClusterRefinerTagMCmerged::findInput(const edm::EDGetTokenT<T>& tag,
                                                 edm::Handle<T>& handle,
                                                 const edm::Event& e) {
  e.getByToken(tag, handle);
  return handle.isValid();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ClusterRefinerTagMCmerged);
