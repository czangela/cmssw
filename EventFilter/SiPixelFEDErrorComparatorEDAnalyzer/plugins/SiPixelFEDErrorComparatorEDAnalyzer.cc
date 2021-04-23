// system include files
#include <memory>
#include <assert.h>
#include <algorithm>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/SiPixelDetId/interface/PixelFEDChannel.h"

class SiPixelFEDErrorComparatorEDAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit SiPixelFEDErrorComparatorEDAnalyzer(const edm::ParameterSet&);
  ~SiPixelFEDErrorComparatorEDAnalyzer() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  using DetContainer = std::vector<uint32_t>;

private:
  void beginJob() override{};
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void retrieveCollectionsFromLabels(const edm::Event&);
  void sortCollections();
  void compareCollectionsAndAssertOnDifference();
  void endJob() override{};

  // ----------member data ---------------------------
  std::vector<edm::EDGetTokenT<DetIdCollection>> inactivePixelDetectorLabels_;
  std::vector<edm::EDGetTokenT<PixelFEDChannelCollection>> badPixelFEDChannelsLabels_;

  std::vector<edm::EDGetTokenT<DetIdCollection>> inactivePixelDetectorLabelsRefactored_;
  std::vector<edm::EDGetTokenT<PixelFEDChannelCollection>> badPixelFEDChannelsLabelsRefactored_;

  DetContainer badPixelDets_;
  DetContainer badPixelFEDs_;

  DetContainer badPixelDetsRefactored_;
  DetContainer badPixelFEDsRefactored_;
};

SiPixelFEDErrorComparatorEDAnalyzer::SiPixelFEDErrorComparatorEDAnalyzer(const edm::ParameterSet& iConfig) {
  auto inactivePixelDetectorTags = iConfig.getParameter<std::vector<edm::InputTag>>("inactivePixelDetectorLabels");
  for (auto& t : inactivePixelDetectorTags) {
    inactivePixelDetectorLabels_.emplace_back(consumes<DetIdCollection>(t));
  }

  inactivePixelDetectorTags = iConfig.getParameter<std::vector<edm::InputTag>>("inactivePixelDetectorLabelsRefactored");
  for (auto& t : inactivePixelDetectorTags) {
    inactivePixelDetectorLabelsRefactored_.emplace_back(consumes<DetIdCollection>(t));
  }

  auto badPixelFEDChannelCollectionTags =
      iConfig.getParameter<std::vector<edm::InputTag>>("badPixelFEDChannelCollectionLabels");
  for (auto& t : badPixelFEDChannelCollectionTags) {
    badPixelFEDChannelsLabels_.emplace_back(consumes<PixelFEDChannelCollection>(t));
  }

  badPixelFEDChannelCollectionTags =
      iConfig.getParameter<std::vector<edm::InputTag>>("badPixelFEDChannelCollectionLabelsRefactored");
  for (auto& t : badPixelFEDChannelCollectionTags) {
    badPixelFEDChannelsLabelsRefactored_.emplace_back(consumes<PixelFEDChannelCollection>(t));
  }
}

// ------------ method called for each event  ------------
void SiPixelFEDErrorComparatorEDAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  retrieveCollectionsFromLabels(iEvent);
  sortCollections();
  compareCollectionsAndAssertOnDifference();
}

void SiPixelFEDErrorComparatorEDAnalyzer::retrieveCollectionsFromLabels(const edm::Event& iEvent) {
  auto addDetId = [&](const auto id, auto container) {
    const auto detid = DetId(id);
    container.push_back(detid.rawId());
  };

  auto addFEDDetId = [&](const auto channel, auto container) {
    const auto detid = DetId(channel.detId());
    container.push_back(detid.rawId());
  };

  auto addCollectionToContainerByFunction = [&](const auto labels, auto container, auto addToContainer) {
    for (const auto& token : labels) {
      for (const auto& element : iEvent.get(token)) {
        addToContainer(element, container);
      }
    }
  };

  addCollectionToContainerByFunction(badPixelFEDChannelsLabels_, badPixelFEDs_, addFEDDetId);
  addCollectionToContainerByFunction(inactivePixelDetectorLabels_, badPixelDets_, addDetId);
  addCollectionToContainerByFunction(badPixelFEDChannelsLabelsRefactored_, badPixelFEDsRefactored_, addFEDDetId);
  addCollectionToContainerByFunction(inactivePixelDetectorLabelsRefactored_, badPixelDetsRefactored_, addDetId);
}

void SiPixelFEDErrorComparatorEDAnalyzer::sortCollections() {
  std::sort(badPixelDets_.begin(), badPixelDets_.end());
  std::sort(badPixelFEDs_.begin(), badPixelFEDs_.end());
  std::sort(badPixelDetsRefactored_.begin(), badPixelDetsRefactored_.end());
  std::sort(badPixelFEDsRefactored_.begin(), badPixelFEDsRefactored_.end());
}

void SiPixelFEDErrorComparatorEDAnalyzer::compareCollectionsAndAssertOnDifference() {
  if (!badPixelDets_.empty() || !badPixelFEDs_.empty() || !badPixelDetsRefactored_.empty() ||
      !badPixelFEDsRefactored_.empty()) {
    assert(badPixelDets_ == badPixelDetsRefactored_);
    assert(badPixelFEDs_ == badPixelFEDsRefactored_);
    std::cout << "Pixel errors: " << badPixelDets_.size() << " " << badPixelDetsRefactored_.size() << std::endl;
    std::cout << "FED errors: " << badPixelFEDs_.size() << " " << badPixelFEDsRefactored_.size() << std::endl;
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void SiPixelFEDErrorComparatorEDAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  auto vers01InpTag = edm::InputTag("siPixelDigis");
  auto vers02InpTag = edm::InputTag("siPixelDigisCPURefactored");
  using VI = std::vector<edm::InputTag>;
  desc.add<VI>("inactivePixelDetectorLabels", VI{{vers01InpTag}});
  desc.add<VI>("badPixelFEDChannelCollectionLabels", VI{{vers01InpTag}});
  desc.add<VI>("inactivePixelDetectorLabelsRefactored", VI{{vers02InpTag}});
  desc.add<VI>("badPixelFEDChannelCollectionLabelsRefactored", VI{{vers02InpTag}});
  descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiPixelFEDErrorComparatorEDAnalyzer);
