#flake8: noqa
'''

Emulate the L1 and UCT upgrade primitives, and put them in the event.

Authors: Isobel Ojalvo, Sridhara Dasu, and others from UW

'''

import FWCore.ParameterSet.Config as cms


from Configuration.StandardSequences.RawToDigi_Data_cff import *
from L1Trigger.UCT2015.Lut import *
from L1Trigger.UCT2015.regionSF_cfi import *
from L1Trigger.UCT2015.jetSF_cfi import *



# Modify the HCAL TPGs according to the proposed HTR modification.  If the HCAL
# is above a given energy threshold, set the MIP bit.
hackHCALMIPs = cms.EDProducer(
    "HcalTpgMipEmbedder",
    src = cms.InputTag("hcalDigis"),
    threshold = cms.double(3), # In GeV
    rawThreshold = cms.uint32(3), # In TPG rank
    cutOnRawBits = cms.bool(False), # What to cut on
)

Layer1Digis = cms.EDProducer(
    "Layer1Emulator",
    hcalDigis = cms.InputTag("valHcalTriggerPrimitiveDigis"),
    ecalDigis = cms.InputTag("ecalDigis:EcalTriggerPrimitives"),
    debug = cms.bool(True),
)

CorrectedDigis = cms.EDProducer(
    "RegionCorrection",
    puMultCorrect = cms.bool(True), # PU corrections
    applyCalibration = cms.bool(False), # region calibration (Work in Progress))
    regionLSB = RCTConfigProducers.jetMETLSB,
    egammaLSB = cms.double(1.0), # This has to correspond with the value from L1CaloEmThresholds
    regionSF = regionSF_8TeV_data,
    regionSubtraction = regionSubtraction_8TeV_data
)

uctDigiStep = cms.Sequence(
    # Only do the digitization of objects that we care about
    #RawToDigi
    gctDigis
    * gtDigis
    * ecalDigis
    * hcalDigis
)


emulationSequence = cms.Sequence(uctDigiStep + Layer1Digis)
