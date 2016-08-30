import FWCore.ParameterSet.Config as cms

### put L1 track trigger configs here

#from SLHCUpgradeSimulations.L1TrackTrigger.L1TrackTrigger_cff import *


#### from old SLHCUpgradeSimulations.L1TrackTrigger


from L1Trigger.TrackTrigger.TrackTrigger_cff import *
##from SimTracker.TrackTriggerAssociation.TrackTriggerAssociator_cff import *

#L1TrackTrigger=cms.Sequence(TrackTriggerClustersStubs*TrackTriggerAssociatorClustersStubs*TrackTriggerTTTracks*TrackTriggerAssociatorTracks)
L1TrackTrigger=cms.Sequence(TrackTriggerClustersStubs)



