// vim:set ts=4 sw=4 fdm=marker et:
//Update:
// 2013Nov13   twang   clear up different channel into single function
//                     clear up irrelevant things
// 2013Nov23   twang   GenInfo
#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GtFdlWord.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

#include "RecoVertex/KinematicFitPrimitives/interface/MultiTrackKinematicConstraint.h"
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/TwoTrackMassKinematicConstraint.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"

#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Math/interface/Error.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"

        //message logger
#include "FWCore/MessageLogger/interface/MessageLogger.h"
        //magnetic field
#include "MagneticField/Engine/interface/MagneticField.h"
        //vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Common/interface/Ref.h"
        //For Kalman vertex fitters
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
        //KalmanTrimmedVertexFinder
#include "RecoVertex/TrimmedKalmanVertexFinder/interface/KalmanTrimmedVertexFinder.h"
        //ROOT
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1F.h"
        //others
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <iostream>
        //Bfinder
#include "Bfinder/Bfinder/interface/format.h"
#include "Bfinder/Bfinder/interface/TriggerBooking.h"

#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TBranch.h"
#include <TROOT.h>
#include <TSystem.h>
#include <TObject.h>
#include <TFile.h>
#include <math.h>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <list>
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#define MUON_MASS   0.10565837
#define PION_MASS   0.13957018
#define KAON_MASS   0.493677
#define KSHORT_MASS 0.497614
#define KSTAR_MASS  0.89594
#define PHI_MASS    1.019455
#define JPSI_MASS   3.096916
#define PSI2S_MASS  3.686109

//
// class declaration
//

class Bfinder : public edm::EDAnalyzer
{//{{{
    public:
        explicit Bfinder(const edm::ParameterSet&);
        ~Bfinder();
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
 
    private:
        virtual void beginJob() ;
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob() ;
 
        virtual void beginRun(edm::Run const&, edm::EventSetup const&);
        virtual void endRun(edm::Run const&, edm::EventSetup const&);
        virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
        virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

        virtual void BranchOut2MuTk(
            BInfoBranches &BInfo,
            std::vector<pat::GenericParticle> input_tracks,
            bool isNeededTrack[MAX_TRACK],
            TLorentzVector v4_mu1,
            TLorentzVector v4_mu2,
            reco::TransientTrack muonPTT,
            reco::TransientTrack muonMTT,
            int *B_counter,
            float *mass_window,
            float MuMu_MASS,
            float Tk_MASS,
            int channel_number
        );

        virtual void BranchOut2MuX_XtoTkTk(
            BInfoBranches &BInfo,
            std::vector<pat::GenericParticle> input_tracks,
            bool isNeededTrack[MAX_TRACK],
            TLorentzVector v4_mu1,
            TLorentzVector v4_mu2,
            reco::TransientTrack muonPTT,
            reco::TransientTrack muonMTT,
            int *B_counter,
            float *mass_window,
            float MuMu_MASS,
            float TkTk_MASS,
            float TkTk_window,
            float Tk1_MASS,
            float Tk2_MASS,     
            int channel_number,
            int fit_option
        );
 
        // ----------member data ---------------------------
        edm::ESHandle<MagneticField> bField;
        edm::ParameterSet theConfig;
//      std::vector<std::string> TriggersForMatching_;
        edm::InputTag hltLabel_;
        edm::InputTag genLabel_;
        edm::InputTag muonLabel_;
        edm::InputTag trackLabel_;
        edm::InputTag puInfoLabel_;
        std::string   ntupleType_;//'upsilon','jpsi','all','no'

        edm::Service<TFileService> fs;
        TTree *root;
        EvtInfoBranches     EvtInfo;
        MuonInfoBranches    MuonInfo;
        TrackInfoBranches   TrackInfo;
        BInfoBranches       BInfo;
        GenInfoBranches     GenInfo;

        //histograms
        TH1F *MuonCutLevel;
        TH1F *TrackCutLevel;
        TH1F *XbujCutLevel;
        //How many channel
        static int const Nchannel = 20;
        TH1F *XbMassCutLevel[Nchannel];
        
};//}}}

void Bfinder::beginJob()
{//{{{
    root = fs->make<TTree>("root","root");
    EvtInfo.regTree(root);
    MuonInfo.regTree(root);
    TrackInfo.regTree(root);
    BInfo.regTree(root);
    GenInfo.regTree(root);
}//}}}

Bfinder::Bfinder(const edm::ParameterSet& iConfig):theConfig(iConfig)
{//{{{
    //now do what ever initialization is needed
//  TriggersForMatching_= iConfig.getUntrackedParameter<std::vector<std::string> >("TriggersForMatching");
    genLabel_           = iConfig.getParameter<edm::InputTag>("GenLabel");
    trackLabel_         = iConfig.getParameter<edm::InputTag>("TrackLabel");
    muonLabel_          = iConfig.getParameter<edm::InputTag>("MuonLabel");
    hltLabel_           = iConfig.getParameter<edm::InputTag>("HLTLabel");
    puInfoLabel_        = iConfig.getParameter<edm::InputTag>("PUInfoLabel");
    ntupleType_         = iConfig.getUntrackedParameter<std::string>("NtupleType","jpsi");

    MuonCutLevel        = fs->make<TH1F>("MuonCutLevel"     , "MuonCutLevel"    , 10, 0, 10);
    TrackCutLevel       = fs->make<TH1F>("TrackCutLevel"    , "TrackCutLevel"   , 10, 0, 10);
    XbujCutLevel        = fs->make<TH1F>("XbujCutLevel"     , "XbujCutLevel"    , 10, 0, 10);
    for(int i = 0; i < Nchannel; i++){
        XbMassCutLevel[i]      = fs->make<TH1F>(TString::Format("XbMassCutLevel_i")   ,TString::Format("XbMassCutLevel_i")  , 10, 0, 10);
    }
}//}}}

Bfinder::~Bfinder()
{//{{{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}//}}}

//
// member functions
//

// ------------ method called for each event  ------------
void Bfinder::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    //std::cout << "*************************\nReconstructing event number: " << iEvent.id() << "\n";
    using namespace edm;
    using namespace reco;
    //ESHandle<MagneticField> bField;
    iSetup.get<IdealMagneticFieldRecord>().get(bField);

    // Change used muon and track collections
    edm::Handle< std::vector<pat::Muon> > muons;
    iEvent.getByLabel(muonLabel_,muons);
    edm::Handle< std::vector<pat::GenericParticle> > tks;
    iEvent.getByLabel(trackLabel_, tks);

    //CLEAN all memory
    memset(&EvtInfo     ,0x00,sizeof(EvtInfo)   );
    memset(&MuonInfo    ,0x00,sizeof(MuonInfo)  );
    memset(&TrackInfo   ,0x00,sizeof(TrackInfo) );
    memset(&BInfo       ,0x00,sizeof(BInfo)    );
    memset(&GenInfo     ,0x00,sizeof(GenInfo)   );

    // EvtInfo section{{{
    //EvtInfo.hltnames->clear();
    EvtInfo.RunNo   = iEvent.id().run();
    EvtInfo.EvtNo   = iEvent.id().event();
    EvtInfo.BxNo    = iEvent.bunchCrossing();
    EvtInfo.LumiNo  = iEvent.luminosityBlock();
    EvtInfo.Orbit   = iEvent.orbitNumber();
    EvtInfo.McFlag  = !iEvent.isRealData();
    EvtInfo.nTrgBook= N_TRIGGER_BOOKINGS;
    
    //HLT
    edm::Handle<TriggerResults> TrgResultsHandle; //catch triggerresults
    bool with_TriggerResults = iEvent.getByLabel(hltLabel_,TrgResultsHandle);
    if(!with_TriggerResults){//{{{
        std::cout << "Sorry there is no TriggerResult in the file" << std::endl;
    }else{
        //get the names of the triggers
        const edm::TriggerNames &TrgNames = iEvent.triggerNames(*TrgResultsHandle);
        EvtInfo.trgCount = 0;
        for(int i=0; i< N_TRIGGER_BOOKINGS; i++){
            unsigned int TrgIndex = TrgNames.triggerIndex(TriggerBooking[i]);
            if (TrgIndex == TrgNames.size()) {
                EvtInfo.trgBook[i] = -4; // The trigger path is not known in this event.
            }else if ( !TrgResultsHandle->wasrun( TrgIndex ) ) {
                EvtInfo.trgBook[i] = -3; // The trigger path was not included in this event.
            }else if ( !TrgResultsHandle->accept( TrgIndex ) ) {
                EvtInfo.trgBook[i] = -2; // The trigger path was not accepted in this event.
            }else if (  TrgResultsHandle->error ( TrgIndex ) ) {
                EvtInfo.trgBook[i] = -1; // The trigger path has an error in this event.
            }else {
                EvtInfo.trgBook[i] = +1; // It's triggered.
                EvtInfo.trgCount++; 
            }
        }
        EvtInfo.nHLT = TrgNames.size();
        for(unsigned int i=0; i<TrgNames.size(); i++){
            EvtInfo.hltBits[i] = (TrgResultsHandle->accept(i) == true) ? 1:0;
        }
    }//end(!with_TriggerResults)}}}

    // Handle primary vertex properties
    Vertex thePrimaryV;
    math::XYZPoint RefVtx;
        //get beamspot information
    Vertex theBeamSpotV;
    reco::BeamSpot beamSpot;
    edm::Handle<reco::BeamSpot> beamSpotHandle;
    iEvent.getByLabel("offlineBeamSpot", beamSpotHandle);
    if (beamSpotHandle.isValid()){
        beamSpot = *beamSpotHandle;
        theBeamSpotV = Vertex(beamSpot.position(), beamSpot.covariance3D());
    }else{
        std::cout<< "No beam spot available from EventSetup \n";
    }

        //get vertex informationa
    edm::Handle<reco::VertexCollection> VertexHandle;
    iEvent.getByLabel("offlinePrimaryVertexHandle", VertexHandle);
    if (!VertexHandle.failedToGet() && VertexHandle->size()>0){
        //int nVtxTrks = 0;//outdated PV definition
        double max_tkSt = 0;
        for(std::vector<reco::Vertex>::const_iterator it_vtx = VertexHandle->begin();
            it_vtx != VertexHandle->end(); it_vtx++){
            if (!it_vtx->isValid()) continue;
            ////find primary vertex with most number of associated tracks, outdated PV defination
            //int tksize = it_vtx->tracksSize();
            //if(nVtxTrks < tksize){
            //    nVtxTrks = tksize;
            //    thePrimaryV = Vertex(*it_vtx);
            //}

            //find primary primary vertex with largest St
            double tkSt = 0;
            for(std::vector<reco::TrackBaseRef>::const_iterator it_tk = it_vtx->tracks_begin();
                it_tk != it_vtx->tracks_end(); it_tk++){
                tkSt += it_tk->get()->pt();
            }
            if (tkSt > max_tkSt){
                max_tkSt = tkSt;
                thePrimaryV = Vertex(*it_vtx);
            }
        }
    }else{ 
        thePrimaryV = Vertex(beamSpot.position(), beamSpot.covariance3D());
    }
    RefVtx = thePrimaryV.position();

    EvtInfo.PVx     = thePrimaryV.position().x();
    EvtInfo.PVy     = thePrimaryV.position().y();
    EvtInfo.PVz     = thePrimaryV.position().z();
    EvtInfo.PVxE    = thePrimaryV.xError();
    EvtInfo.PVyE    = thePrimaryV.yError();
    EvtInfo.PVzE    = thePrimaryV.zError();
    EvtInfo.PVnchi2 = thePrimaryV.normalizedChi2();
    EvtInfo.PVchi2  = thePrimaryV.chi2();

    // get pile-up information
    std::cout << "puInfoLabel_=" << puInfoLabel_ << std::endl;
    if (!iEvent.isRealData()){
        edm::Handle<std::vector< PileupSummaryInfo > >  PUHandle;
        iEvent.getByLabel(puInfoLabel_, PUHandle);
        std::vector<PileupSummaryInfo>::const_iterator PVI;
        for(PVI = PUHandle->begin(); PVI != PUHandle->end(); ++PVI) {
            EvtInfo.nPU[EvtInfo.nBX]   = PVI->getPU_NumInteractions();
            EvtInfo.BXPU[EvtInfo.nBX]  = PVI->getBunchCrossing();
            EvtInfo.trueIT[EvtInfo.nBX]= PVI->getTrueNumInteractions();
            EvtInfo.nBX += 1;
        }
    }else{
        EvtInfo.nBX = 0;
    }

    //}}}
    //printf("-----*****DEBUG:End of EvtInfo.\n");

    // Double check size=0.
    MuonInfo.size   = 0;
    TrackInfo.size  = 0;
    BInfo.uj_size  = 0;
    BInfo.size     = 0;
    GenInfo.size    = 0;
    
    int B_counter[Nchannel] = {0};
    int B_tag[Nchannel] = {0};
    B_tag[0] = 1;// RECONSTRUCTION: J/psi + K
    B_tag[1] = 0;// RECONSTRUCTION: J/psi + Pi
    B_tag[2] = 0;// RECONSTRUCTION: J/psi + Ks 
    B_tag[3] = 0;// RECONSTRUCTION: J/psi + K* (K+, Pi-)
    B_tag[4] = 0;// RECONSTRUCTION: J/psi + K* (K-, Pi+)
    B_tag[5] = 0;// RECONSTRUCTION: J/psi + phi
    B_tag[6] = 0;// RECONSTRUCTION: J/psi + pi pi <= psi', X(3872), Bs->J/psi f0

    std::vector<pat::Muon>              input_muons;
    std::vector<pat::GenericParticle>   input_tracks;
    input_muons = *muons;
    input_tracks = *tks;
    try{
        //standard check for validity of input data
        if (input_muons.size() == 0){
            std::cout << "There's no muon : " << iEvent.id() << std::endl;
        }else{
            std::cout << "Got " << input_muons.size() << " muons" << std::endl;
            if (input_tracks.size() == 0){
                std::cout << "There's no track: " << iEvent.id() << std::endl;
            }else{
                std::cout << "Got " << input_tracks.size() << " tracks" << std::endl;
                if (input_tracks.size() > 1 && input_muons.size() > 1){

                    //MuonInfo section{{{
                    int mu_hindex = -1;
                    const reco::GenParticle* genMuonPtr[MAX_MUON];
                    memset(genMuonPtr,0x00,MAX_MUON);
                    for(std::vector<pat::Muon>::const_iterator mu_it=input_muons.begin();
                        mu_it != input_muons.end() ; mu_it++){
                        if(mu_hindex >= MAX_MUON){
                            fprintf(stderr,"ERROR: number of muons exceeds the size of array.\n");
                            break;//exit(0);
                        }
                        mu_hindex = int(mu_it - input_muons.begin());
                        
                        MuonCutLevel->Fill(0);
                        //if(!(mu_it->innerTrack().isNonnull()*mu_it->globalTrack().isNonnull())) {continue;}
                        //MuonCutLevel->Fill(1);
                        if (!(mu_it->isTrackerMuon() || mu_it->isGlobalMuon())) continue;
                        MuonCutLevel->Fill(2);
                        //if (!(mu_it->isGlobalMuon()*mu_it->track().isAvailable()*mu_it->globalTrack().isAvailable())) continue;
                        //MuonCutLevel->Fill(3);
                        //if (mu_it->p()>200 || mu_it->pt()>200)                  continue;
                        //MuonCutLevel->Fill(4);
                        if (!muon::isGoodMuon(*mu_it,muon::TMOneStationTight))  continue;
                        MuonCutLevel->Fill(5);
                        if (fabs(mu_it->innerTrack()->dxy(thePrimaryV.position())) >= 3.        || 
                            fabs(mu_it->innerTrack()->dz(thePrimaryV.position()))  >= 30.       
                           ) continue;
                        if (mu_it->innerTrack()->hitPattern().pixelLayersWithMeasurement()<1    ||
                            mu_it->innerTrack()->normalizedChi2()>1.8                           //||
                           ) continue;
                        //if (mu_it->innerTrack()->hitPattern().trackerLayersWithMeasurement()<6  &&
                        //    mu_it->innerTrack()->hitPattern().numberOfValidStripHits()<11
                        //   ) continue;
                        if (mu_it->innerTrack()->hitPattern().trackerLayersWithMeasurement()<6) continue;
                        MuonCutLevel->Fill(6);
                        
                        MuonInfo.index          [MuonInfo.size] = MuonInfo.size;
                        MuonInfo.handle_index   [MuonInfo.size] = mu_hindex;
                        MuonInfo.charge         [MuonInfo.size] = mu_it->charge();
                        MuonInfo.pt             [MuonInfo.size] = mu_it->pt();
                        MuonInfo.eta            [MuonInfo.size] = mu_it->eta();
                        MuonInfo.phi            [MuonInfo.size] = mu_it->phi();
                        MuonInfo.i_striphit     [MuonInfo.size] = mu_it->innerTrack()->hitPattern().numberOfValidStripHits();
                        MuonInfo.i_pixelhit     [MuonInfo.size] = mu_it->innerTrack()->hitPattern().numberOfValidPixelHits();
                        MuonInfo.i_nStripLayer  [MuonInfo.size] = mu_it->innerTrack()->hitPattern().stripLayersWithMeasurement();
                        MuonInfo.i_nPixelLayer  [MuonInfo.size] = mu_it->innerTrack()->hitPattern().pixelLayersWithMeasurement();
                        MuonInfo.i_chi2         [MuonInfo.size] = mu_it->innerTrack()->chi2();
                        MuonInfo.i_ndf          [MuonInfo.size] = mu_it->innerTrack()->ndof();
                        MuonInfo.fpbarrelhit    [MuonInfo.size] = mu_it->innerTrack()->hitPattern().hasValidHitInFirstPixelBarrel();
                        MuonInfo.fpendcaphit    [MuonInfo.size] = mu_it->innerTrack()->hitPattern().hasValidHitInFirstPixelEndcap();
                        MuonInfo.d0             [MuonInfo.size] = mu_it->track()->d0();
                        MuonInfo.dz             [MuonInfo.size] = mu_it->track()->dz();
                        MuonInfo.dzPV           [MuonInfo.size] = mu_it->track()->dz(RefVtx);
                        MuonInfo.dxyPV          [MuonInfo.size] = mu_it->track()->dxy(RefVtx);
                        MuonInfo.iso_trk        [MuonInfo.size] = mu_it->trackIso();//R<0.3
                        //MuonInfo.iso_calo       [MuonInfo.size] = mu_it->caloIso();//sum of two iso
                        MuonInfo.iso_ecal       [MuonInfo.size] = mu_it->ecalIso();
                        MuonInfo.iso_hcal       [MuonInfo.size] = mu_it->hcalIso();
                        MuonInfo.n_matches      [MuonInfo.size] = mu_it->numberOfMatches();//only in chamber

                        if (!iEvent.isRealData())
                            genMuonPtr [MuonInfo.size] = mu_it->genParticle();
//if(mu_it->genParticle()->pt())std::cout<<"warnning!!   "<< mu_it->genParticle()->pt()<<std::endl;

                        if(mu_it->isGlobalMuon()){
                            MuonInfo.g_striphit [MuonInfo.size] = mu_it->globalTrack()->hitPattern().numberOfValidStripHits();
                            MuonInfo.g_pixelhit [MuonInfo.size] = mu_it->globalTrack()->hitPattern().numberOfValidPixelHits();
                            MuonInfo.g_chi2     [MuonInfo.size] = mu_it->globalTrack()->chi2();
                            MuonInfo.g_ndf      [MuonInfo.size] = mu_it->globalTrack()->ndof();
                            MuonInfo.nmuhit     [MuonInfo.size] = mu_it->globalTrack()->hitPattern().numberOfValidMuonHits();
                        }else{
                            MuonInfo.g_striphit [MuonInfo.size] = -1;
                            MuonInfo.g_pixelhit [MuonInfo.size] = -1;
                            MuonInfo.g_chi2     [MuonInfo.size] = -1;
                            MuonInfo.g_ndf      [MuonInfo.size] = -1;
                            MuonInfo.nmuhit     [MuonInfo.size] = -1;
                        }
                        int qm = 0;
                        for(int qi=1; qi!= 24; ++qi){
                            if (muon::isGoodMuon(*mu_it, muon::SelectionType(qi))){
                                qm += 1 << qi;
                            }
                        }
                        MuonInfo.muqual         [MuonInfo.size] = qm;   

                        //2.tracker hits requirement for MC/2011/2012 soft pion
                        //1.make sure non-zero for binary test
                        //0.pass all cuts
                        if (!iEvent.isRealData() || 
                            (iEvent.id().run() < 180297 && mu_it->innerTrack()->hitPattern().numberOfValidStripHits() > 10) || 
                            (iEvent.id().run() > 180296 && mu_it->innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5 )
                           ) MuonInfo.isGoodCand[MuonInfo.size] += 1 << 2;
                        MuonInfo.isGoodCand[MuonInfo.size] += 1 << 1;
                        if (((MuonInfo.isGoodCand[MuonInfo.size]>>1)&((MuonInfo.isGoodCand[MuonInfo.size]>>1)+1)) == 0)
                            MuonInfo.isGoodCand[MuonInfo.size] += 1;

                        MuonInfo.size++;
                    }//end of MuonInfo}}}
                    //printf("-----*****DEBUG:End of MuonInfo.\n");

                    //Preselect tracks{{{
                    bool isNeededTrack[MAX_TRACK];// Are the tracks redundant?
                    memset(isNeededTrack,false,MAX_TRACK);
                    for(std::vector<pat::GenericParticle>::const_iterator tk_it=input_tracks.begin();
                        tk_it != input_tracks.end(); tk_it++){
                        TrackCutLevel->Fill(0);//number of all tracks
                        bool isMuonTrack = false; //remove muon track
                        for(std::vector<pat::Muon>::iterator it=input_muons.begin() ; it != input_muons.end() ; it++){
                            if (!it->track().isNonnull())                   continue;
                            if (fabs(tk_it->pt() -it->track()->pt() )<0.00001 &&
                                fabs(tk_it->eta()-it->track()->eta())<0.00001 &&
                                fabs(tk_it->phi()-it->track()->phi())<0.00001 ){
                                    isMuonTrack = true;
                                    break;
                            }
                        }
                        if (isMuonTrack)                                    continue;
                        TrackCutLevel->Fill(1);//number of non muon tracks
                        if (tk_it->track()->normalizedChi2()>5)             continue;
                        TrackCutLevel->Fill(2);
                        if (tk_it->pt()<0.4)                                continue;
                        TrackCutLevel->Fill(3);
                        //if (tk_it->p()>200 || tk_it->pt()>200)              continue;
                        //TrackCutLevel->Fill(4);
                        if (fabs(tk_it->eta()) > 2.5)                       continue;
                        TrackCutLevel->Fill(5);
                        //if (tk_it->track()->hitPattern().numberOfValidStripHits()<10)continue;
                        //TrackCutLevel->Fill(6);
                        //if (tk_it->track()->hitPattern().numberOfValidPixelHits()<2) continue;
                        //TrackCutLevel->Fill(7);
                        isNeededTrack[tk_it-input_tracks.begin()] = true;
                    }//end of track preselection}}}
                    //printf("-----*****DEBUG:End of track preselection.\n");

                    // BInfo section{{{
                    int mu1_index = -1;
                    int mu1_hindex = -1;
                    bool gogogo = false;
                    for(std::vector<pat::Muon>::const_iterator mu_it1=input_muons.begin();
                        mu_it1 != input_muons.end() ; mu_it1++){
                        //Check if it in MuonInfo
                        mu1_hindex = int(mu_it1 - input_muons.begin());
                        gogogo = false;
                        for(int i=0; i < MuonInfo.size; i++){
                            if (mu1_hindex == MuonInfo.handle_index[i]){
                                gogogo = true;
                                break;
                            }
                        }
                        if (!gogogo) continue;
                        //Get the corrisponding index in MuonInfo
                        mu1_index ++;
                        if (mu_it1->charge()<0) continue;
                        
                        int mu2_index = -1;
                        int mu2_hindex = -1; 
                        for(std::vector<pat::Muon>::const_iterator mu_it2=input_muons.begin();
                            mu_it2 != input_muons.end() ; mu_it2++){
                            mu2_hindex = int(mu_it2 - input_muons.begin()); 
                            gogogo = false;
                            for(int j=0; j < MuonInfo.size; j++){
                                if(mu2_hindex == MuonInfo.handle_index[j]){
                                    gogogo = true;
                                    break;
                                }
                            }
                            if (!gogogo) continue;
                            mu2_index ++;   
                            if (mu_it2->charge()>0) continue;
                            XbujCutLevel->Fill(0);
                            
                            TLorentzVector v4_mu1,v4_mu2;
                            v4_mu1.SetPtEtaPhiM(mu_it1->pt(),mu_it1->eta(),mu_it1->phi(),MUON_MASS);
                            v4_mu2.SetPtEtaPhiM(mu_it2->pt(),mu_it2->eta(),mu_it2->phi(),MUON_MASS);
                            if (fabs((v4_mu1+v4_mu2).Mag()-JPSI_MASS)>0.4) continue;
    
                            //Fit 2 muon
                            reco::TransientTrack muonPTT(mu_it1->track(), &(*bField) );
                            reco::TransientTrack muonMTT(mu_it2->track(), &(*bField) );
                            if(!muonPTT.isValid()) continue;
                            if(!muonMTT.isValid()) continue;
                            XbujCutLevel->Fill(1);
    
                            const reco::Muon* rmu1 = dynamic_cast<const reco::Muon * >(mu_it1->originalObject());
                            const reco::Muon* rmu2 = dynamic_cast<const reco::Muon * >(mu_it2->originalObject());
                            if(muon::overlap(*rmu1, *rmu2)) continue;
                            XbujCutLevel->Fill(2);
    
                            KinematicParticleFactoryFromTransientTrack pFactory;
                            ParticleMass muon_mass = MUON_MASS; //pdg mass
                            float muon_sigma = muon_mass*1.e-6;
                            float chi = 0.;
                            float ndf = 0.;
                            std::vector<RefCountedKinematicParticle> muonParticles;
                            muonParticles.push_back(pFactory.particle(muonPTT,muon_mass,chi,ndf,muon_sigma));
                            muonParticles.push_back(pFactory.particle(muonMTT,muon_mass,chi,ndf,muon_sigma));
    
                            KinematicParticleVertexFitter   fitter;   
                            RefCountedKinematicTree         ujVFT;
                            ujVFT = fitter.fit(muonParticles); 
                            if (!ujVFT->isValid()) continue;
                            XbujCutLevel->Fill(3); 

                            ujVFT->movePointerToTheTop();
    
                            RefCountedKinematicParticle ujVFP       = ujVFT->currentParticle();
                            RefCountedKinematicVertex   ujVFPvtx    = ujVFT->currentDecayVertex();
                            ujVFT->movePointerToTheFirstChild();
                            KinematicParameters         ujmu1KP     = ujVFT->currentParticle()->currentState().kinematicParameters();
                            ujVFT->movePointerToTheNextChild();
                            KinematicParameters         ujmu2KP     = ujVFT->currentParticle()->currentState().kinematicParameters();
                            double chi2_prob_uj = TMath::Prob(ujVFPvtx->chiSquared(), ujVFPvtx->degreesOfFreedom());
                            if(chi2_prob_uj < 0.01) continue;
                            XbujCutLevel->Fill(4);

                            if (fabs(ujVFP->currentState().mass()-JPSI_MASS)>0.3) continue;

                            TLorentzVector uj_4vec,uj_mu1_4vec,uj_mu2_4vec;
                            uj_4vec.SetPxPyPzE(ujVFP->currentState().kinematicParameters().momentum().x(),
                                               ujVFP->currentState().kinematicParameters().momentum().y(),
                                               ujVFP->currentState().kinematicParameters().momentum().z(),
                                               ujVFP->currentState().kinematicParameters().energy());
                            uj_mu1_4vec.SetPxPyPzE( ujmu1KP.momentum().x(),
                                                    ujmu1KP.momentum().y(),
                                                    ujmu1KP.momentum().z(),
                                                    ujmu1KP.energy());
                            uj_mu2_4vec.SetPxPyPzE( ujmu2KP.momentum().x(),
                                                    ujmu2KP.momentum().y(),
                                                    ujmu2KP.momentum().z(),
                                                    ujmu2KP.energy());
                            //uj_4vec.Print();
                            //uj_mu1_4vec.Print();
                            //uj_mu2_4vec.Print();

                            BInfo.uj_index         [BInfo.uj_size]= BInfo.uj_size;
                            BInfo.uj_mass          [BInfo.uj_size]= uj_4vec.Mag();
                            BInfo.uj_pt            [BInfo.uj_size]= uj_4vec.Pt();
                            BInfo.uj_eta            [BInfo.uj_size]= uj_4vec.Eta();
                            BInfo.uj_phi            [BInfo.uj_size]= uj_4vec.Phi();
                            BInfo.uj_px            [BInfo.uj_size]= uj_4vec.Px();
                            BInfo.uj_py            [BInfo.uj_size]= uj_4vec.Py();
                            BInfo.uj_pz            [BInfo.uj_size]= uj_4vec.Pz();
                            BInfo.uj_vtxX          [BInfo.uj_size]= ujVFPvtx->position().x();
                            BInfo.uj_vtxY          [BInfo.uj_size]= ujVFPvtx->position().y();
                            BInfo.uj_vtxZ          [BInfo.uj_size]= ujVFPvtx->position().z();
                            BInfo.uj_vtxXE         [BInfo.uj_size]= sqrt(ujVFPvtx->error().cxx());
                            BInfo.uj_vtxYE         [BInfo.uj_size]= sqrt(ujVFPvtx->error().cyy());
                            BInfo.uj_vtxZE         [BInfo.uj_size]= sqrt(ujVFPvtx->error().czz());
                            BInfo.uj_vtxdof        [BInfo.uj_size]= ujVFPvtx->degreesOfFreedom();
                            BInfo.uj_vtxchi2       [BInfo.uj_size]= ujVFPvtx->chiSquared();
                            BInfo.uj_rfmu1_index   [BInfo.uj_size]= mu1_index;
                            BInfo.uj_rfmu2_index   [BInfo.uj_size]= mu2_index;
                            BInfo.uj_rfmu1_px      [BInfo.uj_size]= uj_mu1_4vec.Px();
                            BInfo.uj_rfmu1_py      [BInfo.uj_size]= uj_mu1_4vec.Py();
                            BInfo.uj_rfmu1_pz      [BInfo.uj_size]= uj_mu1_4vec.Pz();
                            BInfo.uj_rfmu2_px      [BInfo.uj_size]= uj_mu2_4vec.Px();
                            BInfo.uj_rfmu2_py      [BInfo.uj_size]= uj_mu2_4vec.Py();
                            BInfo.uj_rfmu2_pz      [BInfo.uj_size]= uj_mu2_4vec.Pz();

                            //4.|uj_Eta| < 1.2?
                            //3.good muon candidates?
                            //2.make sure non-zero for binary test
                            //1.Xb or X3872
                            //0.passed all selections
                            if (fabs(uj_4vec.Eta()) < 1.2)//Eta < 1.2
                                BInfo.uj_isGoodCand[BInfo.uj_size] += 1 << 4;
                            if (MuonInfo.isGoodCand[mu1_index]%2 == 1 && MuonInfo.isGoodCand[mu2_index]%2 == 1)
                                BInfo.uj_isGoodCand[BInfo.uj_size] += 1 << 3;
                            BInfo.uj_isGoodCand[BInfo.uj_size] += 1 << 2;
                            if (uj_4vec.Mag() > 7)//Xb or X3872
                                BInfo.uj_isGoodCand[BInfo.uj_size] += 1 << 1;
                            if (((BInfo.uj_isGoodCand[BInfo.uj_size]>>2)&((BInfo.uj_isGoodCand[BInfo.uj_size]>>2)+1))==0)
                                BInfo.uj_isGoodCand[BInfo.uj_size] += 1;

                            BInfo.uj_size++;
                            muonParticles.clear();

                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + K
                            //////////////////////////////////////////////////////////////////////////
                            float mass_window[2] = {4.3, 6.4};
                            if(B_tag[0] == 1){
                                BranchOut2MuTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    KAON_MASS,
                                    1
                                );
                            }                            
                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + Pi
                            //////////////////////////////////////////////////////////////////////////
                            if(B_tag[1] == 1){
                                BranchOut2MuTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    PION_MASS,
                                    2
                                );
                            }                            

                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + Ks
                            //////////////////////////////////////////////////////////////////////////

                            if(B_tag[2] == 1){
                                BranchOut2MuX_XtoTkTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    KSHORT_MASS,
                                    0.3,
                                    PION_MASS,        
                                    PION_MASS,
                                    3,
                                    1
                                );
                            }                            
                            
                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + K* (K+, Pi-)
                            //////////////////////////////////////////////////////////////////////////

                            if(B_tag[3] == 1){
                                BranchOut2MuX_XtoTkTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    KSTAR_MASS,
                                    0.4,
                                    KAON_MASS,        
                                    PION_MASS,
                                    4,
                                    0
                                );
                            }
                            
                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + K* (K-, Pi+)
                            //////////////////////////////////////////////////////////////////////////

                            if(B_tag[4] == 1){
                                BranchOut2MuX_XtoTkTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    KSTAR_MASS,
                                    0.4,
                                    PION_MASS,        
                                    KAON_MASS,
                                    5,
                                    0
                                );
                            }
                            
                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + phi
                            //////////////////////////////////////////////////////////////////////////
                            
                            if(B_tag[5] == 1){
                                BranchOut2MuX_XtoTkTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    PHI_MASS,
                                    0.1,
                                    KAON_MASS,        
                                    KAON_MASS,
                                    6,
                                    0
                                );
                            }

                            //////////////////////////////////////////////////////////////////////////
                            // RECONSTRUCTION: J/psi + pi pi <= psi', X(3872), Bs->J/psi f0
                            //////////////////////////////////////////////////////////////////////////
                            mass_window[0] = 3;
                            mass_window[1] = 6.4;
                            if(B_tag[6] == 1){
                                BranchOut2MuX_XtoTkTk(
                                    BInfo,
                                    input_tracks,
                                    isNeededTrack,
                                    v4_mu1,
                                    v4_mu2,
                                    muonPTT,
                                    muonMTT,
                                    B_counter,
                                    mass_window,
                                    JPSI_MASS,
                                    -1,
                                    1.6,
                                    PION_MASS,        
                                    PION_MASS,
                                    7,
                                    0
                                );
                            }
                            
                        }//Mu2
                    }//Mu1}}}
                    //printf("-----*****DEBUG:End of BInfo.\n");
                    printf("B_counter: %d/%d/%d/%d/%d/%d/%d/%d\n",B_counter[0],B_counter[1],B_counter[2],B_counter[3],B_counter[4],B_counter[5],B_counter[6], B_counter[7]);

                    // TrackInfo section {{{
                    const reco::GenParticle* genTrackPtr[MAX_GEN];
                    memset(genTrackPtr,0x00,MAX_GEN);
                    for(std::vector<pat::GenericParticle>::const_iterator tk_it=input_tracks.begin();
                        tk_it != input_tracks.end() ; tk_it++){
                        if(TrackInfo.size >= MAX_TRACK){
                            fprintf(stderr,"ERROR: number of tracks exceeds the size of array.\n");
                            break;;
                        }
                        int tk_hindex = int(tk_it - input_tracks.begin());
                        if (isNeededTrack[tk_hindex]==false) continue;

                        //Create list of relative xb candidates for later filling
                        std::vector<int> listOfRelativeXbCands;//1~nXb
                        for(int iXb=0; iXb < BInfo.size; iXb++){
                            if(BInfo.rftk1_index[iXb] == -tk_hindex-1){
                                listOfRelativeXbCands.push_back(iXb+1);
                            }else if(BInfo.rftk2_index[iXb] == -tk_hindex-1){
                                listOfRelativeXbCands.push_back(-iXb-1);
                            }
                        }
                        if (listOfRelativeXbCands.size() == 0) continue;//drop unused tracks

                        TrackInfo.index          [TrackInfo.size] = TrackInfo.size;
                        TrackInfo.handle_index   [TrackInfo.size] = tk_hindex;
                        TrackInfo.charge         [TrackInfo.size] = tk_it->charge();
                        TrackInfo.pt             [TrackInfo.size] = tk_it->pt();
                        TrackInfo.eta            [TrackInfo.size] = tk_it->eta();
                        TrackInfo.phi            [TrackInfo.size] = tk_it->phi();
                        //TrackInfo.p              [TrackInfo.size] = tk_it->p();
                        TrackInfo.striphit       [TrackInfo.size] = tk_it->track()->hitPattern().numberOfValidStripHits();
                        TrackInfo.pixelhit       [TrackInfo.size] = tk_it->track()->hitPattern().numberOfValidPixelHits();
                        TrackInfo.fpbarrelhit    [TrackInfo.size] = tk_it->track()->hitPattern().hasValidHitInFirstPixelBarrel();
                        TrackInfo.fpendcaphit    [TrackInfo.size] = tk_it->track()->hitPattern().hasValidHitInFirstPixelEndcap();
                        TrackInfo.chi2           [TrackInfo.size] = tk_it->track()->chi2();
                        TrackInfo.ndf            [TrackInfo.size] = tk_it->track()->ndof();
                        TrackInfo.d0             [TrackInfo.size] = tk_it->track()->d0();
                        TrackInfo.d0error        [TrackInfo.size] = tk_it->track()->d0Error();
                        TrackInfo.dzPV           [TrackInfo.size] = tk_it->track()->dz(RefVtx);
                        TrackInfo.dxyPV          [TrackInfo.size] = tk_it->track()->dxy(RefVtx);

                        if (!iEvent.isRealData())
                            genTrackPtr [TrackInfo.size] = tk_it->genParticle();

                        //1. make sure non-zero for binary test
                        //0. isGoodCand
                        TrackInfo.isGoodCand     [TrackInfo.size]+= 1 << 1;
                        if (((TrackInfo.isGoodCand[TrackInfo.size]>>1)&((TrackInfo.isGoodCand[TrackInfo.size]>>1)+1))==0)
                            TrackInfo.isGoodCand     [TrackInfo.size]+= 1;

                        //Fill correct track index and track quality to correspond Xb candidate
                        for(unsigned int iCands=0; iCands < listOfRelativeXbCands.size(); iCands++){
                            if (listOfRelativeXbCands[iCands]>0){
                                BInfo.rftk1_index[listOfRelativeXbCands[iCands]-1] = TrackInfo.size;
                                if ((TrackInfo.isGoodCand[TrackInfo.size]&1) == 1){
                                    if (BInfo.rftk2_index[listOfRelativeXbCands[iCands]-1]>=0){
                                        BInfo.isGoodCand[listOfRelativeXbCands[iCands]-1] += ((BInfo.isGoodCand[listOfRelativeXbCands[iCands]-1]>>2)&1)<<2;
                                    }else{
                                        BInfo.isGoodCand[listOfRelativeXbCands[iCands]-1] += 1 << 2;
                                    }
                                }
                            }else{
                                BInfo.rftk2_index[-listOfRelativeXbCands[iCands]-1] = TrackInfo.size;
                                if ((TrackInfo.isGoodCand[TrackInfo.size]&1) == 1){
                                    if (BInfo.rftk1_index[-listOfRelativeXbCands[iCands]-1]>=0){
                                        BInfo.isGoodCand[-listOfRelativeXbCands[iCands]-1] += ((BInfo.isGoodCand[-listOfRelativeXbCands[iCands]-1]>>2)&1)<<2;
                                    }else{
                                        BInfo.isGoodCand[-listOfRelativeXbCands[iCands]-1] += 1 << 2;
                                    }
                                }
                            }
                        }
                        TrackInfo.size++;
                    }//end of TrackInfo}}}
                    //printf("-----*****DEBUG:End of TrackInfo.\n");

                    // GenInfo section{{{
                    if (!iEvent.isRealData()){
//                    if (0){
                        edm::Handle< std::vector<reco::GenParticle> > gens;
                        iEvent.getByLabel(genLabel_, gens);
    
                        std::vector<const reco::Candidate *> cands;
                        for(std::vector<reco::GenParticle>::const_iterator it_gen = gens->begin();
                            it_gen != gens->end(); it_gen++ ){
                            cands.push_back(&*it_gen);
                        }
    
                        for(std::vector<reco::GenParticle>::const_iterator it_gen=gens->begin();
                            it_gen != gens->end(); it_gen++){
                            if (it_gen->status() > 2)                           continue;

                            //if is pion/kaon must be final state
                            if (
                                (abs(it_gen->pdgId()) == 111 && it_gen->status() == 2) ||//pi 0
                                (abs(it_gen->pdgId()) == 211 && it_gen->status() == 2) ||//pi +-
                                (abs(it_gen->pdgId()) == 311 && it_gen->status() == 2) ||//K0
                                (abs(it_gen->pdgId()) == 321 && it_gen->status() == 2) //K+-
                            ) continue;

                            bool isGenSignal = false;                                                                                                                                                       
                            //save target intermediat state particle
                            if (
                                abs(int(it_gen->pdgId()/100) % 100) == 3  ||//s menson
                                abs(int(it_gen->pdgId()/100) % 100) == 4  ||//c menson
                                abs(int(it_gen->pdgId()/100) % 100) == 5  ||//b menson
                                abs(it_gen->pdgId()) == 511 ||//B_0
                                abs(it_gen->pdgId()) == 521 ||//B_+-
                                abs(it_gen->pdgId()) == 130 ||//KL
                                abs(it_gen->pdgId()) == 310 ||//KS
                                abs(it_gen->pdgId()) == 313 ||//K*0(892)
                                abs(it_gen->pdgId()) == 323 ||//K*+-(892)
                                abs(it_gen->pdgId()) == 333 ||//phi(1020)
                                it_gen->pdgId() == 443      ||//Jpsi
                                it_gen->pdgId() == 100443   ||//Psi(2S)
                                it_gen->pdgId() == 553      ||//Upsilon
                                it_gen->pdgId() == 100553     //Upsilon(2S)
                               ) isGenSignal = true;

                            if (abs(it_gen->pdgId()) == 13                              &&
                                it_gen->numberOfMothers() == 1                          &&
                                (it_gen->mother()->pdgId() == 443 ||
                                 it_gen->mother()->pdgId() == 553 )                     &&
                                it_gen->mother()->numberOfDaughters() >= 2              &&
                                it_gen->mother()->numberOfMothers() == 1                &&
                                (it_gen->mother()->mother()->pdgId() == 511 ||
                                 it_gen->mother()->mother()->pdgId() == 521 )
                                //(it_gen->mother()->mother()->pdgId() == 100553 ||
                                // it_gen->mother()->mother()->pdgId() == 100443 )        &&
                                //it_gen->mother()->mother()->numberOfDaughters() == 3    &&
                                //abs(it_gen->mother()->mother()->daughter(1)->pdgId()) == 211 
                               ) isGenSignal = true;//signal mu

                            if ((abs(it_gen->pdgId()) == 111 || 
                                abs(it_gen->pdgId()) == 211 || 
                                abs(it_gen->pdgId()) == 311 || 
                                abs(it_gen->pdgId()) == 321) &&
                                it_gen->numberOfMothers() == 1                          &&
                                (it_gen->mother()->pdgId() == 511 || 
                                it_gen->mother()->pdgId() == 521 ||       
                                it_gen->mother()->mother()->pdgId() == 511 || 
                                it_gen->mother()->mother()->pdgId() == 521 )  &&
                                (it_gen->mother()->daughter(0)->pdgId() == 553 ||  
                                it_gen->mother()->daughter(0)->pdgId() == 443 ||
                                it_gen->mother()->mother()->daughter(0)->pdgId() == 553 ||  
                                it_gen->mother()->mother()->daughter(0)->pdgId() == 443 ) &&
                                //(it_gen->mother()->mother()->pdgId() == 100553 ||
                                //it_gen->mother()->mother()->pdgId() == 100443 )        &&
                                //it_gen->mother()->numberOfDaughters()== 3               &&
                                (it_gen->mother()->daughter(0)->numberOfDaughters()>=2 || 
                                it_gen->mother()->mother()->daughter(0)->numberOfDaughters()>=2) &&
                                (abs(it_gen->mother()->daughter(0)->daughter(0)->pdgId()) == 13 || 
                                abs(it_gen->mother()->mother()->daughter(0)->daughter(0)->pdgId()) == 13)
                               ) isGenSignal = true;//signal pion/kaon                           
/*
                            if ((it_gen->pdgId() == 443 || it_gen->pdgId() == 553)      &&
                                cand(it_gen->mother()->pdgId() == 100443 ||
                                 it_gen->mother()->pdgId() == 100553 )                  &&
                                it_gen->mother()->numberOfDaughters() == 3              &&
                                abs(it_gen->mother()->daughter(1)->pdgId()) == 211      &&
                                it_gen->numberOfDaughters() >= 2                        &&
                                abs(it_gen->daughter(0)->pdgId()) == 13    
                               ) isGenSignal = true;//signal uj

                            if ((it_gen->pdgId()==100443 || it_gen->pdgId()==100553)    &&
                                it_gen->numberOfDaughters() == 3                        &&
                                it_gen->daughter(0)->numberOfDaughters() >= 2           &&
                                abs(it_gen->daughter(0)->daughter(0)->pdgId()) == 13    &&
                                abs(it_gen->daughter(1)->pdgId()) == 211 
                               ) isGenSignal = true;//signal xb
*/
                            if (!isGenSignal) continue;

                            int iMo1 = -1,  iMo2 = -1,  iDa1 = -1,  iDa2 = -1;
                            for(std::vector<const reco::Candidate *>::iterator iCands = cands.begin();
                                iCands != cands.end(); iCands++){
                                if (it_gen->numberOfMothers() >= 2){
                                    if (it_gen->mother(0) == *iCands)
                                        iMo1 = iCands - cands.begin();
                                    if (it_gen->mother(1) == *iCands)
                                        iMo2 = iCands - cands.begin();
                                }else if(it_gen->numberOfMothers() == 1){
                                    if (it_gen->mother(0) == *iCands)
                                        iMo1 = iCands - cands.begin();
                                }
                                if (it_gen->numberOfDaughters() >= 2){
                                    if (it_gen->daughter(0) == *iCands)
                                        iDa1 = iCands - cands.begin();
                                    else if (it_gen->daughter(1) == *iCands)
                                        iDa2 = iCands - cands.begin();
                                }else if(it_gen->numberOfDaughters() == 1){
                                    if (it_gen->daughter(0) == *iCands)
                                        iDa1 = iCands - cands.begin();
                                }
                            }
    
                            //depend on TrackInfo.handle_index and MuonInfo.handle_index
                            //printf("-----*****DEBUG:Start of matching.\n");
                            if (abs(it_gen->pdgId()) == 13){
                                //printf("-----*****DEBUG:Entered muon matching block.\n");
                                for(int muonIdx = 0; muonIdx < MuonInfo.size; muonIdx++){
                                    MuonInfo.geninfo_index[muonIdx] = -1;
                                    // match by pat::Muon
                                    if (genMuonPtr[muonIdx] == 0) continue;
                                    if (it_gen->p4() == genMuonPtr[muonIdx]->p4()){
                                        MuonInfo.geninfo_index[muonIdx] = GenInfo.size;
                                        //printf("-----*****DEBUG:[Mu]Tar.Pt /Ref.Pt = %9f/%9f\n",
                                        //    MuonInfo.pt [muonIdx],it_gen->pt ());
                                        break;
                                    }
                                }
                            }
                            //Find all other particle in TrackInfo
                            else{
                                //printf("-----*****DEBUG:Entered pion matching block.\n");
                                for(int trackIdx = 0; trackIdx < TrackInfo.size; trackIdx++){
                                    TrackInfo.geninfo_index[trackIdx] = -1;
                                    //match by pat::GenericParticle
                                    if (genTrackPtr[trackIdx] == 0 ) continue;
                                    if (it_gen->p4() == genTrackPtr[trackIdx]->p4()){
                                        TrackInfo.geninfo_index[trackIdx] = GenInfo.size;
                                        break;
                                    }
                                }
                            }
    
                            GenInfo.index[GenInfo.size]         = GenInfo.size;
                            GenInfo.handle_index[GenInfo.size]  = it_gen-gens->begin();
                            GenInfo.pt[GenInfo.size]            = it_gen->pt();
                            GenInfo.eta[GenInfo.size]           = it_gen->eta();
                            GenInfo.phi[GenInfo.size]           = it_gen->phi();
                            GenInfo.mass[GenInfo.size]          = it_gen->mass();
                            GenInfo.pdgId[GenInfo.size]         = it_gen->pdgId();
                            GenInfo.status[GenInfo.size]        = it_gen->status();
                            GenInfo.nMo[GenInfo.size]           = it_gen->numberOfMothers();
                            GenInfo.nDa[GenInfo.size]           = it_gen->numberOfDaughters();
                            GenInfo.mo1[GenInfo.size]           = iMo1;//To be matched later.
                            GenInfo.mo2[GenInfo.size]           = iMo2;
                            GenInfo.da1[GenInfo.size]           = iDa1;
                            GenInfo.da2[GenInfo.size]           = iDa2;
                            GenInfo.size++;
                        }
                        //printf("-----*****DEBUG:End of gens loop.\n");
                        
                        //Pass handle_index to igen
                        for(int igen = 0; igen < GenInfo.size; igen++){
                            int iMo1 = GenInfo.mo1[igen];
                            int iMo2 = GenInfo.mo2[igen];
                            int iDa1 = GenInfo.da1[igen];
                            int iDa2 = GenInfo.da2[igen];
                            for(int k = 0; k < GenInfo.size; k++){
                                if (iMo1 == GenInfo.handle_index[k])
                                    GenInfo.mo1[igen] = k;
                                else if (iMo2 == GenInfo.handle_index[k])
                                    GenInfo.mo2[igen] = k;
                                else if (iDa1 == GenInfo.handle_index[k])
                                    GenInfo.da1[igen] = k;
                                else if (iDa2 == GenInfo.handle_index[k])
                                    GenInfo.da2[igen] = k;
                            }
                            //In case that GEN particles are omitted from GenInfo
                            //handle_index couldn't be the same as igen
                            //since the very first proton pair has status 3.
                            if (iMo1 == GenInfo.mo1[igen])
                                GenInfo.mo1[igen] = -1;
                            if (iMo2 == GenInfo.mo2[igen])
                                GenInfo.mo2[igen] = -1;
                            if (iDa1 == GenInfo.da1[igen])
                                GenInfo.da1[igen] = -1;
                            if (iDa2 == GenInfo.da2[igen])
                               GenInfo.da2[igen] = -1;
                        }
                        //printf("-----*****DEBUG:End of IndexToIgen\n");
                    }//isRealData}}}
                    //printf("-----*****DEBUG:End of GenInfo.\n");
                    //std::cout<<"Start to fill!\n";
                }//has nMuons>1 nTracks>1
            }//Tracks
        }//Muonss
    }//try
    catch (std::exception & err){
            std::cout  << "Exception during event number: " << iEvent.id()
                << "\n" << err.what() << "\n";
    }//catch 
    std::cout << "BInfo.size=" << BInfo.size << std::endl;
    root->Fill();
    //std::cout<<"filled!\n";
}


// ------------ method called once each job just after ending the event loop  ------------
void Bfinder::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
void Bfinder::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void Bfinder::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void Bfinder::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void Bfinder::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Bfinder::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

void Bfinder::BranchOut2MuTk(
    BInfoBranches &BInfo, 
    std::vector<pat::GenericParticle> input_tracks, 
    bool isNeededTrack[MAX_TRACK],
    TLorentzVector v4_mu1, 
    TLorentzVector v4_mu2,
    reco::TransientTrack muonPTT,
    reco::TransientTrack muonMTT,
    int *B_counter,
    float *mass_window,
    float MuMu_MASS,
    float Tk_MASS,
    int channel_number

){
  if(channel_number > Nchannel){ printf("Exceeding Maximal allowed channel, exit"); return;}
  float chi = 0.;
  float ndf = 0.;
  int tk1_hindex = -1;
  KinematicParticleFactoryFromTransientTrack pFactory;
  ParticleMass muon_mass = MUON_MASS; //pdg mass
  float muon_sigma = muon_mass*1.e-6;

  for(std::vector<pat::GenericParticle>::const_iterator tk_it1=input_tracks.begin();
      tk_it1 != input_tracks.end() ; tk_it1++){
      tk1_hindex = int(tk_it1 - input_tracks.begin());
      if (!isNeededTrack[tk1_hindex]) continue;
      if (abs(tk_it1->charge()) != 1) continue;
      
      TLorentzVector v4_tk1;
      v4_tk1.SetPtEtaPhiM(tk_it1->pt(),tk_it1->eta(),tk_it1->phi(),KAON_MASS);
  
      if ((v4_mu1+v4_mu2+v4_tk1).Mag()<mass_window[0]-0.2 || (v4_mu1+v4_mu2+v4_tk1).Mag()>mass_window[1]+0.2) continue;
      
      reco::TransientTrack kaonTT(tk_it1->track(), &(*bField) );
      if (!kaonTT.isValid()) continue;
      
      ParticleMass kaon_mass = Tk_MASS;
      float kaon_sigma = kaon_mass*1.e-6;
      
      if (BInfo.size >= MAX_XB) continue;
      
      std::vector<RefCountedKinematicParticle> Xb_candidate;
      Xb_candidate.push_back(pFactory.particle(muonPTT,muon_mass,chi,ndf,muon_sigma));
      Xb_candidate.push_back(pFactory.particle(muonMTT,muon_mass,chi,ndf,muon_sigma));
      Xb_candidate.push_back(pFactory.particle(kaonTT,kaon_mass,chi,ndf,kaon_sigma));
      RefCountedKinematicTree xbVFT;
      
      ParticleMass uj_mass = MuMu_MASS;
      MultiTrackKinematicConstraint *uj_c = new TwoTrackMassKinematicConstraint(uj_mass);
      KinematicConstrainedVertexFitter kcvFitter;
      xbVFT = kcvFitter.fit(Xb_candidate, uj_c);
      
      if (!xbVFT->isValid()) continue;
      xbVFT->movePointerToTheTop();
      RefCountedKinematicParticle     xbVFP       = xbVFT->currentParticle();
      RefCountedKinematicVertex       xbVFPvtx    = xbVFT->currentDecayVertex();
      if (!xbVFPvtx->vertexIsValid()) continue;
      std::vector<RefCountedKinematicParticle> xCands  = xbVFT->finalStateParticles();
      
      double chi2_prob = TMath::Prob(xbVFPvtx->chiSquared(),xbVFPvtx->degreesOfFreedom());
      if (chi2_prob < 0.01) continue;
      XbMassCutLevel[0]->Fill(3);
      
      if (xbVFP->currentState().mass()<mass_window[0] || xbVFP->currentState().mass()>mass_window[1]) continue;
      
      XbMassCutLevel[0]->Fill(4);
      
      TLorentzVector xb_4vec,xb_mu1_4vec,xb_mu2_4vec,xb_tk1_4vec,xb_tk2_4vec;
      xb_4vec.SetPxPyPzE(xbVFP->currentState().kinematicParameters().momentum().x(),
                         xbVFP->currentState().kinematicParameters().momentum().y(),
                         xbVFP->currentState().kinematicParameters().momentum().z(),
                         xbVFP->currentState().kinematicParameters().energy());
      xb_mu1_4vec.SetPxPyPzE(xCands[0]->currentState().kinematicParameters().momentum().x(),
                             xCands[0]->currentState().kinematicParameters().momentum().y(),
                             xCands[0]->currentState().kinematicParameters().momentum().z(),
                             xCands[0]->currentState().kinematicParameters().energy());
      xb_mu2_4vec.SetPxPyPzE(xCands[1]->currentState().kinematicParameters().momentum().x(),
                             xCands[1]->currentState().kinematicParameters().momentum().y(),
                             xCands[1]->currentState().kinematicParameters().momentum().z(),
                             xCands[1]->currentState().kinematicParameters().energy());
      xb_tk1_4vec.SetPxPyPzE(xCands[2]->currentState().kinematicParameters().momentum().x(),
                             xCands[2]->currentState().kinematicParameters().momentum().y(),
                             xCands[2]->currentState().kinematicParameters().momentum().z(),
                             xCands[2]->currentState().kinematicParameters().energy());
      
      BInfo.index[BInfo.size]   = BInfo.size;
      BInfo.mass[BInfo.size]    = xb_4vec.Mag();
      BInfo.pt[BInfo.size]    = xb_4vec.Pt();
      BInfo.eta[BInfo.size]    = xb_4vec.Eta();
      BInfo.phi[BInfo.size]    = xb_4vec.Phi();
      BInfo.px[BInfo.size]      = xb_4vec.Px();
      BInfo.py[BInfo.size]      = xb_4vec.Py();
      BInfo.pz[BInfo.size]      = xb_4vec.Pz();
      BInfo.pxE[BInfo.size]     = sqrt(xbVFP->currentState().kinematicParametersError().matrix()(3,3));
      BInfo.pyE[BInfo.size]     = sqrt(xbVFP->currentState().kinematicParametersError().matrix()(4,4));
      BInfo.pzE[BInfo.size]     = sqrt(xbVFP->currentState().kinematicParametersError().matrix()(5,5));
      BInfo.vtxX[BInfo.size]    = xbVFPvtx->position().x();
      BInfo.vtxY[BInfo.size]    = xbVFPvtx->position().y();
      BInfo.vtxZ[BInfo.size]    = xbVFPvtx->position().z();
      BInfo.vtxXE[BInfo.size]   = sqrt(xbVFPvtx->error().cxx());
      BInfo.vtxYE[BInfo.size]   = sqrt(xbVFPvtx->error().cyy());
      BInfo.vtxZE[BInfo.size]   = sqrt(xbVFPvtx->error().czz());
      BInfo.vtxdof[BInfo.size]  = xbVFPvtx->degreesOfFreedom();
      BInfo.vtxchi2[BInfo.size] = xbVFPvtx->chiSquared();
      
      BInfo.rfuj_index[BInfo.size]  = BInfo.uj_size-1;
      BInfo.rftk1_index[BInfo.size] = -tk1_hindex-1;
      BInfo.rftk2_index[BInfo.size] = -tk1_hindex-1;
      
      BInfo.rfmu1_px[BInfo.size]=xb_mu1_4vec.Px();
      BInfo.rfmu1_py[BInfo.size]=xb_mu1_4vec.Py();
      BInfo.rfmu1_pz[BInfo.size]=xb_mu1_4vec.Pz();
      BInfo.rfmu2_px[BInfo.size]=xb_mu2_4vec.Px();
      BInfo.rfmu2_py[BInfo.size]=xb_mu2_4vec.Py();
      BInfo.rfmu2_pz[BInfo.size]=xb_mu2_4vec.Pz();
      BInfo.rftk1_px[BInfo.size]=xb_tk1_4vec.Px();
      BInfo.rftk1_py[BInfo.size]=xb_tk1_4vec.Py();
      BInfo.rftk1_pz[BInfo.size]=xb_tk1_4vec.Pz();
      BInfo.rftk2_px[BInfo.size]=-999.;
      BInfo.rftk2_py[BInfo.size]=-999.;
      BInfo.rftk2_pz[BInfo.size]=-999.;
      
      BInfo.type[BInfo.size] = channel_number;
      B_counter[channel_number-1]++;
      
      Xb_candidate.clear();
      xCands.clear();
      BInfo.size++;
  }//Tk1
}

void Bfinder::BranchOut2MuX_XtoTkTk(
    BInfoBranches &BInfo, 
    std::vector<pat::GenericParticle> input_tracks, 
    bool isNeededTrack[MAX_TRACK],
    TLorentzVector v4_mu1, 
    TLorentzVector v4_mu2,
    reco::TransientTrack muonPTT,
    reco::TransientTrack muonMTT,
    int *B_counter,
    float *mass_window,
    float MuMu_MASS,
    float TkTk_MASS,
    float TkTk_window,
    float Tk1_MASS,
    float Tk2_MASS,
    int channel_number,
    int fit_option
){
    if(channel_number > Nchannel){ printf("Exceeding Maximal allowed channel, exit"); return;}
    float chi = 0.;
    float ndf = 0.;
    KinematicParticleFactoryFromTransientTrack pFactory;
    ParticleMass muon_mass = MUON_MASS; //pdg mass
    float muon_sigma = muon_mass*1.e-6;
    int tk1_hindex = -1;
    int tk2_hindex = -1;

    for(std::vector<pat::GenericParticle>::const_iterator tk_it1=input_tracks.begin();
        tk_it1 != input_tracks.end() ; tk_it1++){
        tk1_hindex = int(tk_it1 - input_tracks.begin());
        if (!isNeededTrack[tk1_hindex]) continue;
        if (tk_it1->charge()<0) continue;
        
        for(std::vector<pat::GenericParticle>::const_iterator tk_it2=input_tracks.begin();
            tk_it2 != input_tracks.end() ; tk_it2++){
            tk2_hindex = int(tk_it2 - input_tracks.begin());
            if (!isNeededTrack[tk2_hindex]) continue;
            if (tk_it2->charge()>0) continue;
            
            TLorentzVector v4_tk1,v4_tk2;
            v4_tk1.SetPtEtaPhiM(tk_it1->pt(),tk_it1->eta(),tk_it1->phi(),Tk1_MASS);
            v4_tk2.SetPtEtaPhiM(tk_it2->pt(),tk_it2->eta(),tk_it2->phi(),Tk2_MASS);
            if(TkTk_MASS > 0) {if (fabs((v4_tk1+v4_tk2).Mag()-TkTk_MASS)>TkTk_window) continue;}
            else {if (fabs((v4_tk1+v4_tk2).Mag())>TkTk_window) continue;}
            
            if ((v4_mu1+v4_mu2+v4_tk1+v4_tk2).Mag()<mass_window[0]-0.2 || (v4_mu1+v4_mu2+v4_tk1+v4_tk2).Mag()>mass_window[1]+0.2) continue;
            
            reco::TransientTrack tk1PTT(tk_it1->track(), &(*bField) );
            reco::TransientTrack tk2MTT(tk_it2->track(), &(*bField) );
            if (!tk1PTT.isValid()) continue;
            if (!tk2MTT.isValid()) continue;
            
            ParticleMass tk1_mass = Tk1_MASS;
            float tk1_sigma = tk1_mass*1.e-6;
            ParticleMass tk2_mass = Tk2_MASS;
            float tk2_sigma = tk2_mass*1.e-6;
            
            if (BInfo.size >= MAX_XB) continue;
            //doing tktk fit
            std::vector<RefCountedKinematicParticle> tktk_candidate;
            tktk_candidate.push_back(pFactory.particle(tk1PTT,tk1_mass,chi,ndf,tk1_sigma));
            tktk_candidate.push_back(pFactory.particle(tk2MTT,tk2_mass,chi,ndf,tk2_sigma));
            
            XbMassCutLevel[channel_number-1]->Fill(0);
            KinematicParticleVertexFitter   tktk_fitter;
            RefCountedKinematicTree         tktk_VFT;
            tktk_VFT = tktk_fitter.fit(tktk_candidate);
            if(!tktk_VFT->isValid()) continue;
            
            tktk_VFT->movePointerToTheTop();
            
            XbMassCutLevel[channel_number-1]->Fill(1);
            RefCountedKinematicParticle tktk_VFP   = tktk_VFT->currentParticle();
            RefCountedKinematicVertex   tktk_VFPvtx = tktk_VFT->currentDecayVertex();
            double chi2_prob_tktk = TMath::Prob(tktk_VFPvtx->chiSquared(),
                                                tktk_VFPvtx->degreesOfFreedom());
            if(chi2_prob_tktk < 0.01) continue;
            XbMassCutLevel[channel_number-1]->Fill(2);


            std::vector<RefCountedKinematicParticle> Xb_candidate;
            Xb_candidate.push_back(pFactory.particle(muonPTT,muon_mass,chi,ndf,muon_sigma));
            Xb_candidate.push_back(pFactory.particle(muonMTT,muon_mass,chi,ndf,muon_sigma));
            if(fit_option == 0){
                Xb_candidate.push_back(pFactory.particle(tk1PTT,tk1_mass,chi,ndf,tk1_sigma));
                Xb_candidate.push_back(pFactory.particle(tk2MTT,tk2_mass,chi,ndf,tk2_sigma));
            }
            else if(fit_option == 1){
                VirtualKinematicParticleFactory vFactory;
                float tktkchi = tktk_VFPvtx->chiSquared();
                float tktkndf = tktk_VFPvtx->degreesOfFreedom();
                Xb_candidate.push_back(vFactory.particle(tktk_VFP->currentState(),tktkchi,tktkndf,tktk_VFP));
            }
            RefCountedKinematicTree xbVFT;
            
            ParticleMass uj_mass = MuMu_MASS;
            MultiTrackKinematicConstraint *uj_c = new  TwoTrackMassKinematicConstraint(uj_mass);
            KinematicConstrainedVertexFitter kcvFitter;
            xbVFT = kcvFitter.fit(Xb_candidate, uj_c);
            
            if (!xbVFT->isValid()) continue;
            xbVFT->movePointerToTheTop();
            RefCountedKinematicParticle     xbVFP       = xbVFT->currentParticle();
            RefCountedKinematicVertex       xbVFPvtx    = xbVFT->currentDecayVertex();
            if (!xbVFPvtx->vertexIsValid()) continue;
            
            std::vector<RefCountedKinematicParticle> tktkCands  = tktk_VFT->finalStateParticles();
            std::vector<RefCountedKinematicParticle> xCands  = xbVFT->finalStateParticles();
            
            double chi2_prob = TMath::Prob(xbVFPvtx->chiSquared(),xbVFPvtx->degreesOfFreedom());
            if (chi2_prob < 0.01) continue;
            XbMassCutLevel[channel_number-1]->Fill(3);
            
            //Cut out a mass window
            if (xbVFP->currentState().mass()<mass_window[0]|| xbVFP->currentState().mass()>mass_window[1]) continue;
            
            XbMassCutLevel[channel_number-1]->Fill(4);
            
            TLorentzVector xb_4vec,xb_mu1_4vec,xb_mu2_4vec,tktk_4vec,xb_tk1_4vec,xb_tk2_4vec,tktk_tk1_4vec, tktk_tk2_4vec;
            xb_4vec.SetPxPyPzE(xbVFP->currentState().kinematicParameters().momentum().x(),
                               xbVFP->currentState().kinematicParameters().momentum().y(),
                               xbVFP->currentState().kinematicParameters().momentum().z(),
                               xbVFP->currentState().kinematicParameters().energy());
            tktk_4vec.SetPxPyPzE(tktk_VFP->currentState().kinematicParameters().momentum().x(),
                                 tktk_VFP->currentState().kinematicParameters().momentum().y(),
                                 tktk_VFP->currentState().kinematicParameters().momentum().z(),
                                 tktk_VFP->currentState().kinematicParameters().energy());
            
            xb_mu1_4vec.SetPxPyPzE(xCands[0]->currentState().kinematicParameters().momentum().x(),
                                   xCands[0]->currentState().kinematicParameters().momentum().y(),
                                   xCands[0]->currentState().kinematicParameters().momentum().z(),
                                   xCands[0]->currentState().kinematicParameters().energy());
            xb_mu2_4vec.SetPxPyPzE(xCands[1]->currentState().kinematicParameters().momentum().x(),
                                   xCands[1]->currentState().kinematicParameters().momentum().y(),
                                   xCands[1]->currentState().kinematicParameters().momentum().z(),
                                   xCands[1]->currentState().kinematicParameters().energy());
            tktk_tk1_4vec.SetPxPyPzE(tktkCands[0]->currentState().kinematicParameters().momentum().x(),
                                   tktkCands[0]->currentState().kinematicParameters().momentum().y(),
                                   tktkCands[0]->currentState().kinematicParameters().momentum().z(),
                                   tktkCands[0]->currentState().kinematicParameters().energy());
            tktk_tk2_4vec.SetPxPyPzE(tktkCands[1]->currentState().kinematicParameters().momentum().x(),
                                   tktkCands[1]->currentState().kinematicParameters().momentum().y(),
                                   tktkCands[1]->currentState().kinematicParameters().momentum().z(),
                                   tktkCands[1]->currentState().kinematicParameters().energy());
            if(fit_option == 0){
                xb_tk1_4vec.SetPxPyPzE(xCands[2]->currentState().kinematicParameters().momentum().x(),
                                       xCands[2]->currentState().kinematicParameters().momentum().y(),
                                       xCands[2]->currentState().kinematicParameters().momentum().z(),
                                       xCands[2]->currentState().kinematicParameters().energy());
                xb_tk2_4vec.SetPxPyPzE(xCands[3]->currentState().kinematicParameters().momentum().x(),
                                       xCands[3]->currentState().kinematicParameters().momentum().y(),
                                       xCands[3]->currentState().kinematicParameters().momentum().z(),
                                       xCands[3]->currentState().kinematicParameters().energy());
            }
            
            if(fit_option == 1){
                xb_tk1_4vec.SetPxPyPzE(xCands[2]->currentState().kinematicParameters().momentum().x(),
                                       xCands[2]->currentState().kinematicParameters().momentum().y(),
                                       xCands[2]->currentState().kinematicParameters().momentum().z(),
                                       xCands[2]->currentState().kinematicParameters().energy());
            }
            
            BInfo.index[BInfo.size]   = BInfo.size;
            BInfo.mass[BInfo.size]    = xb_4vec.Mag();
            BInfo.pt[BInfo.size]    = xb_4vec.Pt();
            BInfo.eta[BInfo.size]    = xb_4vec.Eta();
            BInfo.phi[BInfo.size]    = xb_4vec.Phi();
            BInfo.px[BInfo.size]      = xb_4vec.Px();
            BInfo.py[BInfo.size]      = xb_4vec.Py();
            BInfo.pz[BInfo.size]      = xb_4vec.Pz();
            BInfo.pxE[BInfo.size]     = sqrt(xbVFP->currentState().kinematicParametersError().matrix()(3,3));
            BInfo.pyE[BInfo.size]     = sqrt(xbVFP->currentState().kinematicParametersError().matrix()(4,4));
            BInfo.pzE[BInfo.size]     = sqrt(xbVFP->currentState().kinematicParametersError().matrix()(5,5));
            BInfo.vtxX[BInfo.size]    = xbVFPvtx->position().x();
            BInfo.vtxY[BInfo.size]    = xbVFPvtx->position().y();
            BInfo.vtxZ[BInfo.size]    = xbVFPvtx->position().z();
            BInfo.vtxXE[BInfo.size]   = sqrt(xbVFPvtx->error().cxx());
            BInfo.vtxYE[BInfo.size]   = sqrt(xbVFPvtx->error().cyy());
            BInfo.vtxZE[BInfo.size]   = sqrt(xbVFPvtx->error().czz());
            BInfo.vtxdof[BInfo.size]  = xbVFPvtx->degreesOfFreedom();
            BInfo.vtxchi2[BInfo.size] = xbVFPvtx->chiSquared();
            
            BInfo.rfuj_index[BInfo.size]  = BInfo.uj_size-1;
            BInfo.rftk1_index[BInfo.size] = -tk1_hindex-1;
            BInfo.rftk2_index[BInfo.size] = -tk2_hindex-1;
            
            //tktk fit info
            BInfo.tktk_mass[BInfo.size]    = tktk_4vec.Mag();
            BInfo.tktk_pt[BInfo.size]      = tktk_4vec.Pt();
            BInfo.tktk_eta[BInfo.size]      = tktk_4vec.Eta();
            BInfo.tktk_phi[BInfo.size]      = tktk_4vec.Phi();
            BInfo.tktk_px[BInfo.size]      = tktk_4vec.Px();
            BInfo.tktk_py[BInfo.size]      = tktk_4vec.Py();
            BInfo.tktk_pz[BInfo.size]      = tktk_4vec.Pz();
            BInfo.tktk_vtxX[BInfo.size]    = tktk_VFPvtx->position().x();
            BInfo.tktk_vtxY[BInfo.size]    = tktk_VFPvtx->position().y();
            BInfo.tktk_vtxZ[BInfo.size]    = tktk_VFPvtx->position().z();
            BInfo.tktk_vtxXE[BInfo.size]   = sqrt(tktk_VFPvtx->error().cxx());
            BInfo.tktk_vtxYE[BInfo.size]   = sqrt(tktk_VFPvtx->error().cyy());
            BInfo.tktk_vtxZE[BInfo.size]   = sqrt(tktk_VFPvtx->error().czz());
            BInfo.tktk_vtxdof[BInfo.size]  = tktk_VFPvtx->degreesOfFreedom();
            BInfo.tktk_vtxchi2[BInfo.size] = tktk_VFPvtx->chiSquared();
            BInfo.tktk_rftk1_px[BInfo.size]=tktk_tk1_4vec.Px();
            BInfo.tktk_rftk1_py[BInfo.size]=tktk_tk1_4vec.Py();
            BInfo.tktk_rftk1_pz[BInfo.size]=tktk_tk1_4vec.Pz();
            BInfo.tktk_rftk2_px[BInfo.size]=tktk_tk2_4vec.Px();
            BInfo.tktk_rftk2_py[BInfo.size]=tktk_tk2_4vec.Py();
            BInfo.tktk_rftk2_pz[BInfo.size]=tktk_tk2_4vec.Pz();
            
            BInfo.rfmu1_px[BInfo.size]=xb_mu1_4vec.Px();
            BInfo.rfmu1_py[BInfo.size]=xb_mu1_4vec.Py();
            BInfo.rfmu1_pz[BInfo.size]=xb_mu1_4vec.Pz();
            BInfo.rfmu2_px[BInfo.size]=xb_mu2_4vec.Px();
            BInfo.rfmu2_py[BInfo.size]=xb_mu2_4vec.Py();
            BInfo.rfmu2_pz[BInfo.size]=xb_mu2_4vec.Pz();
            //If option == 1, this momentum is the tktk virtual particle p.
            BInfo.rftk1_px[BInfo.size]=xb_tk1_4vec.Px();
            BInfo.rftk1_py[BInfo.size]=xb_tk1_4vec.Py();
            BInfo.rftk1_pz[BInfo.size]=xb_tk1_4vec.Pz();
            if(fit_option == 0){
                BInfo.rftk2_px[BInfo.size]=xb_tk2_4vec.Px();
                BInfo.rftk2_py[BInfo.size]=xb_tk2_4vec.Py();
                BInfo.rftk2_pz[BInfo.size]=xb_tk2_4vec.Pz();
            }
            else if(fit_option == 1){
                BInfo.rftk2_px[BInfo.size]=-999;
                BInfo.rftk2_py[BInfo.size]=-999;
                BInfo.rftk2_pz[BInfo.size]=-999;
            }
            
            BInfo.type[BInfo.size] = channel_number;
            B_counter[channel_number-1]++;
            
            Xb_candidate.clear();
            xCands.clear();
            BInfo.size++;
        }//Tk2
    }//Tk1
}

//define this as a plug-in
DEFINE_FWK_MODULE(Bfinder);