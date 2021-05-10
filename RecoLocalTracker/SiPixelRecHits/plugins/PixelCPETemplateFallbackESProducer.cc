#include "RecoLocalTracker/SiPixelRecHits/interface/PixelCPETemplateFallback.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include <string>
#include <memory>

class PixelCPETemplateFallbackESProducer : public edm::ESProducer {
public:
  PixelCPETemplateFallbackESProducer(const edm::ParameterSet& p);
  std::unique_ptr<PixelClusterParameterEstimator> produce(const TkPixelCPERecord&);
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magfieldToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> pDDToken_;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> hTTToken_;
  edm::ESGetToken<SiPixelLorentzAngle, SiPixelLorentzAngleRcd> lorentzAngleToken_;
  edm::ESGetToken<SiPixelTemplateDBObject, SiPixelTemplateDBObjectESProducerRcd> templateDBobjectToken_;
  edm::ESGetToken<SiPixelLorentzAngle, SiPixelLorentzAngleRcd> lorentzAngleWidthToken_;
  edm::ESGetToken<SiPixelGenErrorDBObject, SiPixelGenErrorDBObjectRcd> genErrorDBObjectToken_;

  edm::ParameterSet pset_;
  bool doLorentzFromAlignment_;
  bool useLAFromDB_;
  bool useLAWidthFromDB_;
  bool UseErrorsFromTemplates_;
};

using namespace edm;

PixelCPETemplateFallbackESProducer::PixelCPETemplateFallbackESProducer(const edm::ParameterSet& p) {
  std::string myname = p.getParameter<std::string>("ComponentName");

  useLAWidthFromDB_ = p.getParameter<bool>("useLAWidthFromDB");
  doLorentzFromAlignment_ = p.getParameter<bool>("doLorentzFromAlignment");
  useLAFromDB_ = p.getParameter<bool>("useLAFromDB");

  auto magname = p.getParameter<edm::ESInputTag>("MagneticFieldRecord");
  UseErrorsFromTemplates_ = p.getParameter<bool>("UseErrorsFromTemplates");

  pset_ = p;
  auto c = setWhatProduced(this, myname);
  magfieldToken_ = c.consumes(magname);
  pDDToken_ = c.consumes();
  hTTToken_ = c.consumes();
  templateDBobjectToken_ = c.consumes();
  if (useLAWidthFromDB_) {
    lorentzAngleWidthToken_ = c.consumes(edm::ESInputTag("", "forWidth"));
  }
  if (useLAFromDB_ || doLorentzFromAlignment_) {
    char const* laLabel = doLorentzFromAlignment_ ? "fromAlignment" : "";
    lorentzAngleToken_ = c.consumes(edm::ESInputTag("", laLabel));
  }
  if (UseErrorsFromTemplates_) {
    genErrorDBObjectToken_ = c.consumes();
  }
}

std::unique_ptr<PixelClusterParameterEstimator> PixelCPETemplateFallbackESProducer::produce(
    const TkPixelCPERecord& iRecord) {
  // Normal, default LA is used in case of template failure, load it unless
  // turned off
  // if turned off, null is ok, becomes zero
  const SiPixelLorentzAngle* lorentzAngleProduct = nullptr;
  if (useLAFromDB_ || doLorentzFromAlignment_) {
    lorentzAngleProduct = &iRecord.get(lorentzAngleToken_);
  }

  const SiPixelLorentzAngle* lorentzAngleWidthProduct = nullptr;
  if (useLAWidthFromDB_) {  // use the width LA
    lorentzAngleWidthProduct = &iRecord.get(lorentzAngleWidthToken_);
  }
  //std::cout<<" la width "<<lorentzAngleWidthProduct<<std::endl; //dk

  const SiPixelGenErrorDBObject* genErrorDBObjectProduct = nullptr;

  // Errors take only from new GenError
  if (UseErrorsFromTemplates_) {  // do only when generrors are needed
    genErrorDBObjectProduct = &iRecord.get(genErrorDBObjectToken_);
  }
  return std::make_unique<PixelCPETemplateFallback>(pset_,
                                                    &iRecord.get(magfieldToken_),
                                                    iRecord.get(pDDToken_),
                                                    iRecord.get(hTTToken_),
                                                    lorentzAngleProduct,
                                                    genErrorDBObjectProduct,
                                                    &iRecord.get(templateDBobjectToken_),
                                                    lorentzAngleWidthProduct);
}

void PixelCPETemplateFallbackESProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  // from PixelCPEBase
  PixelCPEBase::fillPSetDescription(desc);

  // from PixelCPETemplateFallback
  PixelCPETemplateFallback::fillPSetDescription(desc);

  // specific to PixelCPETemplateFallbackESProducer
  desc.add<std::string>("ComponentName", "PixelCPETemplateFallback");
  descriptions.add("_fallback_default", desc);
}

DEFINE_FWK_EVENTSETUP_MODULE(PixelCPETemplateFallbackESProducer);
