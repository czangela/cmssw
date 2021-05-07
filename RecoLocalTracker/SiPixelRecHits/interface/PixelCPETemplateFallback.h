#ifndef RecoLocalTracker_SiPixelRecHits_interface_PixelCPETemplateFallback_h
#define RecoLocalTracker_SiPixelRecHits_interface_PixelCPETemplateFallback_h

// local include(s)
#include "PixelCPEGeneric.h"
#include "PixelCPETemplateReco.h"

class PixelCPETemplateFallback : public PixelCPEGeneric, public PixelCPETemplateReco {
public:
  struct ClusterParamTemplateFallback : public ClusterParamGeneric, public ClusterParamTemplate {
    ClusterParamTemplateFallback(const SiPixelCluster& cl) : ClusterParam(cl){};
  };
  PixelCPETemplateFallback(edm::ParameterSet const& conf,
                           const MagneticField* mag,
                           const TrackerGeometry& geom,
                           const TrackerTopology& ttopo,
                           const SiPixelLorentzAngle* lorentzAngle,
                           const SiPixelGenErrorDBObject* genErrorDBObject,
                           const SiPixelTemplateDBObject* templateDBobject,
                           const SiPixelLorentzAngle* lorentzAngleWidth);

  ~PixelCPETemplateFallback() override = default;

  static void fillPSetDescription(edm::ParameterSetDescription& desc);

protected:
  std::unique_ptr<PixelCPEBase::ClusterParam> createClusterParam(const SiPixelCluster&) const override;
  LocalPoint localPosition(DetParam const&, ClusterParam&) const override;
  LocalError localError(DetParam const&, ClusterParam&) const override;
};

#endif  // RecoLocalTracker_SiPixelRecHits_interface_PixelCPETemplateFallback_h
