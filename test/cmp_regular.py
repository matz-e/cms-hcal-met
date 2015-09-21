#!/usr/bin/env python
# vim: foldmethod=marker foldlevel=0

import FWCore.ParameterSet.Config as cms

process = cms.Process('test')
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(500))

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
process.analyzer = cms.EDAnalyzer("HcalMetStudy",
        TriggerPrimitives = cms.VInputTag('hcalDigis'),
        RecHits = cms.InputTag('hbhereco'),
)

# process.p = cms.Path(process.RawToDigi * process.dump * process.analyzer) # for plots
process.p = cms.Path(process.RawToDigi * process.analyzer) # for plots

process.schedule = cms.Schedule(process.p)

from SLHCUpgradeSimulations.Configuration.postLS1Customs import customisePostLS1
process = customisePostLS1(process)

process.TFileService = cms.Service("TFileService",
        closeFileFast = cms.untracked.bool(True),
        fileName = cms.string('legacy.root'))

process.source = cms.Source('PoolSource',
        fileNames = cms.untracked.vstring([
            '/store/data/Run2015A/MET/RECO/PromptReco-v1/000/248/036/00000/2C85BD5C-5714-E511-9F7D-02163E011BBD.root',
            '/store/data/Run2015A/MET/RECO/PromptReco-v1/000/248/038/00000/546E74FC-5D14-E511-9DCE-02163E0145BA.root'
        ]),
        secondaryFileNames = cms.untracked.vstring([
            '/store/data/Run2015A/MET/RAW/v1/000/248/036/00000/4E4AF8A7-B412-E511-A0B3-02163E014406.root',
            '/store/data/Run2015A/MET/RAW/v1/000/248/038/00000/DA2F1ED8-0513-E511-BBE5-02163E01451E.root'
        ]))
