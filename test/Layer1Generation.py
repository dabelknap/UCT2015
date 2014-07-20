'''

Creates L1ExtraNtuples (L1 Style) using a UCT->GT jump

Authors: L. Dodd, N. Woods, T. Perry, A. Levine, S. Dasu, M. Cepeda, E. Friis (UW Madison)
         I. Ojalvo, D. Belknap
'''

import FWCore.ParameterSet.Config as cms
import os

from FWCore.ParameterSet.VarParsing import VarParsing
process = cms.Process("ReRunningL1")


process.source = cms.Source ("PoolSource",
                             fileNames = cms.untracked.vstring(
        "/store/user/ldodd/TT_Tune4C_13TeV-pythia8-tauola/TT_Tune4C_13TeV-pythia8-tauola-tsg_PU40bx25_POSTLS162_V2-v1/fb508503c16d6e4b02bc25104d11f7c2/skim_103_1_yUh.root"
#"file:/scratch/ojalvo/neutrinoGun.root"
                             )   
                             )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(2)
)

# Tested on Monte Carlo, for a test with data edit ahead
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS161_V12::All'

# Load emulation and RECO sequences
process.load("L1Trigger.UCT2015.emulationLayer1MC_cfi")

# Load sequences
process.load("Configuration.Geometry.GeometryIdeal_cff")

process.p1 = cms.Path(
    process.emulationSequence 
)

# Make the framework shut up.
#process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.MessageLogger.cerr.threshold = cms.untracked.string('DEBUG')

# Output definition (This isn't used yet!!! Just a place holder)
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('out.root'),
    outputCommands = cms.untracked.vstring('drop *',
          'keep *_*_*_ReRunningL1',
          'keep *_l1extraParticles*_*_*') 
)
