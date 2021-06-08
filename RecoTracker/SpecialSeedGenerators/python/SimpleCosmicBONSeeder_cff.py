import FWCore.ParameterSet.Config as cms

from RecoTracker.SpecialSeedGenerators.SimpleCosmicBONSeeder_cfi import *
import RecoTracker.TkSeedingLayers.seedingLayersEDProducer_cfi as _mod

simpleCosmicBONSeedingLayers= _mod.seedingLayersEDProducer.clone(
    layerList = cms.vstring(*layerList),
    **layerInfo
)
