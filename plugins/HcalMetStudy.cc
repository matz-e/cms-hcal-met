// -*- C++ -*-
//
// Package:    Debug/HcalMetStudy
// Class:      HcalMetStudy
// 
/**\class HcalMetStudy HcalMetStudy.cc Debug/HcalMetStudy/plugins/HcalMetStudy.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Matthias Wolf
//         Created:  Thu, 17 Sep 2015 11:46:53 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "CalibFormats/CaloTPG/interface/CaloTPGTranscoder.h"
#include "CalibFormats/CaloTPG/interface/CaloTPGRecord.h"

#include "DataFormats/HcalDetId/interface/HcalTrigTowerDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloCollections.h"

#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HFRecHit.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalTrigTowerGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "TTree.h"
#include "TVector2.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class HcalMetStudy : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit HcalMetStudy(const edm::ParameterSet&);
      ~HcalMetStudy();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      edm::InputTag trigprims_;
      std::vector<edm::InputTag> rechits_;

      TTree* t_;
      double tp_met_;
      double rh_met_;
      std::vector<double> tp_mets_;
      std::vector<double> rh_mets_;
      std::vector<double> tp_mets_eta_;
      std::vector<double> rh_mets_eta_;

      std::vector<double> et_cuts_ = {-1e6, 1., 5.};
      std::vector<double> eta_cuts_ = {1.5, 3., 5.};
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HcalMetStudy::HcalMetStudy(const edm::ParameterSet& config) :
   trigprims_(config.getParameter<edm::InputTag>("TriggerPrimitives")),
   rechits_(config.getParameter<std::vector<edm::InputTag>>("RecHits"))
{
   //now do what ever initialization is needed
   usesResource("TFileService");

   tp_mets_ = std::vector<double>(et_cuts_.size(), 0.);
   rh_mets_ = std::vector<double>(et_cuts_.size(), 0.);
   tp_mets_eta_ = std::vector<double>(eta_cuts_.size(), 0.);
   rh_mets_eta_ = std::vector<double>(eta_cuts_.size(), 0.);

   edm::Service<TFileService> fs;
   t_ = fs->make<TTree>("met", "MET");
   t_->Branch("tp", &tp_met_);
   t_->Branch("rh", &rh_met_);

   for (unsigned int i = 0; i < et_cuts_.size(); ++i) {
      t_->Branch(("tp_et_cut_" + std::to_string(i)).c_str(), &tp_mets_[i]);
      t_->Branch(("rh_et_cut_" + std::to_string(i)).c_str(), &rh_mets_[i]);
   }

   for (unsigned int i = 0; i < eta_cuts_.size(); ++i) {
      t_->Branch(("tp_eta_cut_" + std::to_string(i)).c_str(), &tp_mets_eta_[i]);
      t_->Branch(("rh_eta_cut_" + std::to_string(i)).c_str(), &rh_mets_eta_[i]);
   }
}


HcalMetStudy::~HcalMetStudy()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
HcalMetStudy::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
   using namespace edm;

   ESHandle<CaloTPGTranscoder> decoder;
   setup.get<CaloTPGRecord>().get(decoder);

   edm::ESHandle<HcalTrigTowerGeometry> tt_geo;
   setup.get<CaloGeometryRecord>().get(tt_geo);

   edm::ESHandle<CaloGeometry> calo_geo;
   setup.get<CaloGeometryRecord>().get(calo_geo);

   Handle<HcalTrigPrimDigiCollection> tps;
   if (!event.getByLabel(trigprims_, tps)) {
      LogError("HcalTrigPrimDigiCleaner") <<
         "Can't find TP collection with tag '" << trigprims_ << "'" << std::endl;
      return;
   }

   edm::Handle< edm::SortedCollection<HBHERecHit> > hits;
   if (!event.getByLabel(rechits_[0], hits)) {
      edm::LogError("HcalCompareLegacyChains") <<
         "Can't find RH collection with tag '" << rechits_[0] << "'" << std::endl;
      return;
   }

   edm::Handle< edm::SortedCollection<HFRecHit> > fhits;
   if (!event.getByLabel(rechits_[1], fhits)) {
      edm::LogError("HcalCompareLegacyChains") <<
         "Can't find RH collection with tag '" << rechits_[1] << "'" << std::endl;
      return;
   }

   TVector2 cell;
   TVector2 hit_met(0., 0.);
   TVector2 hit_mets[et_cuts_.size()];
   TVector2 hit_mets_eta[eta_cuts_.size()];

   for (auto& hit: *(hits.product())) {
      HcalDetId id(hit.id());
      const auto *local_geo = calo_geo->getSubdetectorGeometry(id)->getGeometry(id);

      auto et = hit.energy() / cosh(local_geo->getPosition().eta());
      cell.SetMagPhi(et, local_geo->getPosition().phi());
      hit_met += cell;

      for (unsigned int i = 0; i < et_cuts_.size(); ++i) {
         if (et >= et_cuts_[i])
            hit_mets[i] += cell;
      }

      for (unsigned int i = 0; i < eta_cuts_.size(); ++i) {
         if (local_geo->getPosition().eta() < eta_cuts_[i]) {
            hit_mets_eta[i] += cell;
            break;
         }
      }
   }

   for (auto& hit: *(fhits.product())) {
      HcalDetId id(hit.id());
      const auto *local_geo = calo_geo->getSubdetectorGeometry(id)->getGeometry(id);

      auto et = hit.energy() / cosh(local_geo->getPosition().eta());
      cell.SetMagPhi(et, local_geo->getPosition().phi());
      hit_met += cell;

      for (unsigned int i = 0; i < et_cuts_.size(); ++i) {
         if (et >= et_cuts_[i])
            hit_mets[i] += cell;
      }

      for (unsigned int i = 0; i < eta_cuts_.size(); ++i) {
         if (local_geo->getPosition().eta() < eta_cuts_[i]) {
            hit_mets_eta[i] += cell;
            break;
         }
      }
   }

   TVector2 tp_met(0., 0.);
   TVector2 tp_mets[et_cuts_.size()];
   TVector2 tp_mets_eta[eta_cuts_.size()];

   for (auto& tp: *(tps)) {
      HcalTrigTowerDetId id = tp.id();
      auto energy = decoder->hcaletValue(id.ieta(), id.iphi(), tp.SOI_compressedEt());

      auto ids = tt_geo->detIds(id);
      float phi = 0.;
      float eta = 0.;
      for (auto& i: ids) {
         const auto *local_geo = calo_geo->getSubdetectorGeometry(i)->getGeometry(i);
         phi += local_geo->getPosition().phi();
         eta += local_geo->getPosition().eta();
      }
      phi /= ids.size();
      eta /= ids.size();

      cell.SetMagPhi(energy, phi);
      tp_met += cell;

      for (unsigned int i = 0; i < et_cuts_.size(); ++i) {
         if (energy >= et_cuts_[i])
            tp_mets[i] += cell;
      }

      for (unsigned int i = 0; i < eta_cuts_.size(); ++i) {
         if (eta < eta_cuts_[i]) {
            tp_mets_eta[i] += cell;
            break;
         }
      }
   }

   tp_met_ = tp_met.Mod();
   rh_met_ = hit_met.Mod();

   for (unsigned int i = 0; i < et_cuts_.size(); ++i) {
      tp_mets_[i] = tp_mets[i].Mod();
      rh_mets_[i] = hit_mets[i].Mod();
   }

   for (unsigned int i = 0; i < eta_cuts_.size(); ++i) {
      tp_mets_eta_[i] = tp_mets_eta[i].Mod();
      rh_mets_eta_[i] = hit_mets_eta[i].Mod();
   }

   t_->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void 
HcalMetStudy::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HcalMetStudy::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HcalMetStudy::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HcalMetStudy);
