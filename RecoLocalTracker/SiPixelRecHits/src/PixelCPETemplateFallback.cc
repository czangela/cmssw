#include "RecoLocalTracker/SiPixelRecHits/interface/PixelCPETemplateFallback.h"

PixelCPETemplateFallback::PixelCPETemplateFallback(edm::ParameterSet const& conf,
                                                   const MagneticField* mag,
                                                   const TrackerGeometry& geom,
                                                   const TrackerTopology& ttopo,
                                                   const SiPixelLorentzAngle* lorentzAngle,
                                                   const SiPixelGenErrorDBObject* genErrorDBObject,
                                                   const SiPixelTemplateDBObject* templateDBobject,
                                                   const SiPixelLorentzAngle* lorentzAngleWidth = nullptr)
    : PixelCPEBase(conf, mag, geom, ttopo, lorentzAngle, genErrorDBObject, templateDBobject, lorentzAngleWidth, 1),
      PixelCPEGeneric(conf, mag, geom, ttopo, lorentzAngle, genErrorDBObject, lorentzAngleWidth),
      PixelCPETemplateReco(conf, mag, geom, ttopo, lorentzAngle, templateDBobject){};

std::unique_ptr<PixelCPEBase::ClusterParam> PixelCPETemplateFallback::createClusterParam(
    const SiPixelCluster& cl) const {
  return std::make_unique<ClusterParamTemplateFallback>(cl);
};

void PixelCPETemplateFallback::fillPSetDescription(edm::ParameterSetDescription& desc) {
  PixelCPETemplateReco::fillPSetDescription(desc);
  PixelCPEGeneric::fillPSetDescription(desc);
};

LocalPoint PixelCPETemplateFallback::localPosition(DetParam const& theDetParam,
                                                   ClusterParam& theClusterParamBase) const {
  auto theClusterParam = dynamic_cast<ClusterParamTemplateFallback&>(theClusterParamBase);
  LocalPoint p = PixelCPETemplateReco::localPosition(theDetParam, theClusterParam);
  if (theClusterParam.ierr != 0 || (UseClusterSplitter_ && theClusterParam.templQbin_ == 0)) {
    p = PixelCPEGeneric::localPosition(theDetParam, theClusterParam);
  }
  return p;
}

LocalError PixelCPETemplateFallback::localError(DetParam const& theDetParam, ClusterParam& theClusterParam) const {
  return PixelCPETemplateReco::localError(theDetParam, theClusterParam);
};