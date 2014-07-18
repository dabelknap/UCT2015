'''

Creates L1ExtraNtuples (L1 Style) using a UCT->GT jump

Authors: L. Dodd, N. Woods, T. Perry, A. Levine,, S. Dasu, M. Cepeda, E. Friis (UW Madison)

'''

import FWCore.ParameterSet.Config as cms
import os

from FWCore.ParameterSet.VarParsing import VarParsing
process = cms.Process("ReRunningL1")

# Get command line options
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')

options.register(
    'isMC',
    1,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    'Set to 1 for simulated samples - updates GT, emulates HCAL TPGs.')

options.parseArguments()


process.source = cms.Source ("PoolSource",
                             fileNames = cms.untracked.vstring(
"/store/mc/Fall13dr/Neutrino_Pt-2to20_gun/GEN-SIM-RAW/tsg_PU40bx25_POSTLS162_V2-v1/00005/6AF2C1E2-DF7F-E311-B452-003048679162.root",
                             )   
                             )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(2)
)

# Tested on Monte Carlo, for a test with data edit ahead
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS161_V12::All'

# Load emulation and RECO sequences
#if not options.isMC:
#    process.load("L1Trigger.UCT2015.emulationTPG_cfi")
#    print "Running on data!"     
#else:
process.load("L1Trigger.UCT2015.emulation_cfi")

# Load sequences
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("L1Trigger.UCT2015.uctl1extraparticles_cfi")

process.p1 = cms.Path(
    process.emulationSequence 
#    process.uct2015L1Extra
       #  *process.YourFavoritePlottingRoutine  --> This ends at l1extra production, anything after is up to the analyst 
)

# Make the framework shut up.
#process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.MessageLogger.cerr.threshold = cms.untracked.string('DEBUG')

# Output definition
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('out.root'),
    outputCommands = cms.untracked.vstring('drop *',
          'keep *_*_*_ReRunningL1',
          'keep *_l1extraParticles*_*_*') 
)
