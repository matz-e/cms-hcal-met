#!/usr/bin/env python
# vim: foldmethod=marker foldlevel=0

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

options = VarParsing.VarParsing('analysis')
options.parseArguments()

process = cms.Process('test')
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.suppressError = cms.untracked.vstring("caloStage1Digis")

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.maxEvents))

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'GR_P_V56'
process.prefer("GlobalTag")

# process.load('Configuration.Geometry.GeometryExtended2017Reco_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.Services_cff')

process.dump = cms.EDAnalyzer("EventContentAnalyzer")
process.study = cms.EDAnalyzer("HcalMetStudy",
        TriggerPrimitives = cms.InputTag('hcalDigis'),
        RecHits = cms.VInputTag('hbhereco', 'hfreco'),
)

process.comp = cms.EDAnalyzer("HcalCompareLegacyChains",
        TriggerPrimitives = cms.InputTag('hcalDigis'),
        RecHits = cms.InputTag('hbhereco'),
        DataFrames = cms.VInputTag()
)

process.filterSaturated = cms.EDFilter("SaturatedFilter",
        triggerPrimitives = cms.InputTag('hcalDigis'),
        maxValue = cms.uint32(0xff)
)

process.CaloTPGTranscoder.uncompress = cms.bool(False)

process.p = cms.Path(
        process.RawToDigi
        # * process.dump
        * process.filterSaturated
        * process.comp
        * process.study
)

process.schedule = cms.Schedule(process.p)

from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1
process = customisePostLS1(process)

process.TFileService = cms.Service("TFileService",
        closeFileFast = cms.untracked.bool(True),
        fileName = cms.string('legacy.root'))

from Debug.HcalMetStudy import files_raw, files_reco

process.source = cms.Source('PoolSource',
        fileNames = files_reco.readFiles,
        secondaryFileNames = files_raw.readFiles
)
