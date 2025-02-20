import FWCore.ParameterSet.Config as cms

SiPixelAliMilleFileExtractorHLTHGMinBias = cms.EDAnalyzer("MillePedeFileExtractor",
    fileBlobInputTag = cms.InputTag("SiPixelAliMillePedeFileConverterHLTHG",''),
    fileDir = cms.string('HLTHGCombinedAlignment/'),
    # File names the Extractor will use to write the fileblobs in the root
    # file as real binary files to disk, so that the pede step can read them.
    # This includes the formatting directive "%04d" which will be expanded to
    # 0000, 0001, 0002,...
    outputBinaryFile = cms.string('pedeBinaryHLTHGMinBias%04d.dat'))

SiPixelAliMilleFileExtractorHLTHGZMuMu = cms.EDAnalyzer("MillePedeFileExtractor",
    fileBlobInputTag = cms.InputTag("SiPixelAliMillePedeFileConverterHLTHGDimuon",''),
    fileDir = cms.string('HLTHGCombinedAlignment/'),
    # File names the Extractor will use to write the fileblobs in the root
    # file as real binary files to disk, so that the pede step can read them.
    # This includes the formatting directive "%04d" which will be expanded to
    # 0000, 0001, 0002,...
    outputBinaryFile = cms.string('pedeBinaryHLTHGDiMuon%04d.dat'))

from Alignment.MillePedeAlignmentAlgorithm.MillePedeAlignmentAlgorithm_cfi import *
from Alignment.CommonAlignmentProducer.AlignmentProducerAsAnalyzer_cff import AlignmentProducer
from Alignment.MillePedeAlignmentAlgorithm.MillePedeDQMModule_cff import *

SiPixelAliPedeAlignmentProducerHLTHGCombined = AlignmentProducer.clone(
    ParameterBuilder = dict(
        Selector = cms.PSet(
            alignParams = cms.vstring(
                "TrackerP1PXBLadder,111111",
                "TrackerP1PXECPanel,111111",
            )
        )
    ),
    doMisalignmentScenario = False,
    checkDbAlignmentValidity = False,
    applyDbAlignment = True,
    tjTkAssociationMapTag = 'TrackRefitter2',
    saveToDB = True,
    trackerAlignmentRcdName = "TrackerAlignmentHLTHGCombinedRcd"
)

SiPixelAliPedeAlignmentProducerHLTHGCombined.algoConfig = MillePedeAlignmentAlgorithm.clone(
    mode = 'pede',
    runAtPCL = True,
    #mergeBinaryFiles = [SiPixelAliMilleFileExtractorHGMinBias.outputBinaryFile.value()],
    #mergeBinaryFiles = [SiPixelAliMilleFileExtractorHGZMuMu.outputBinaryFile.value()],
    mergeBinaryFiles = ['pedeBinaryHLTHGMinBias%04d.dat','pedeBinaryHLTHGDiMuon%04d.dat -- 10.0'],
    binaryFile = '',
    TrajectoryFactory = cms.PSet(BrokenLinesTrajectoryFactory),
    minNumHits = 10,
    fileDir = 'HLTHGCombinedAlignment/',
    pedeSteerer = dict(
        pedeCommand = 'pede',
        method = 'inversion  5  0.8',
        options = [
            #'regularisation 1.0 0.05', # non-stated pre-sigma 50 mrad or 500 mum
            'entries 500',
            'chisqcut  30.0  4.5',
            'threads 1 1',
            'closeandreopen',
            'skipemptycons' 
            #'outlierdownweighting 3','dwfractioncut 0.1'
            #'outlierdownweighting 5','dwfractioncut 0.2'
        ],
        fileDir = 'HLTHGCombinedAlignment/',
        runDir = 'HLTHGCombinedAlignment/',
        steerFile = 'pedeSteerHGCombined',
        pedeDump = 'pedeHGCombined.dump'
    ),
    pedeReader = dict(
        fileDir = 'HLTHGCombinedAlignment/'
    ),
    MillePedeFileReader = dict(
        fileDir = "HLTHGCombinedAlignment/",
        isHG = True
    )
)

SiPixelAliDQMModuleHLTHGCombined = SiPixelAliDQMModule.clone()
SiPixelAliDQMModuleHLTHGCombined.outputFolder = "AlCaReco/SiPixelAliHLTHGCombined"
SiPixelAliDQMModuleHLTHGCombined.MillePedeFileReader.fileDir = "HLTHGCombinedAlignment/"
SiPixelAliDQMModuleHLTHGCombined.MillePedeFileReader.isHG = True

from DQMServices.Core.DQMEDHarvester import DQMEDHarvester
dqmEnvSiPixelAliHLTHGCombined = DQMEDHarvester('DQMHarvestingMetadata',
                                    subSystemFolder = cms.untracked.string('AlCaReco'))

ALCAHARVESTSiPixelAliHLTHGCombined = cms.Sequence(
    SiPixelAliMilleFileExtractorHLTHGMinBias*
    SiPixelAliMilleFileExtractorHLTHGZMuMu*
    SiPixelAliPedeAlignmentProducerHLTHGCombined*
    SiPixelAliDQMModuleHLTHGCombined*
    dqmEnvSiPixelAliHLTHGCombined)
