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
      // ----------member data ---------------------------
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

   for (auto& hit: *(hits.product())) {
      HcalDetId id(hit.id());
      const auto *local_geo = calo_geo->getSubdetectorGeometry(id)->getGeometry(id);

      cell.SetMagPhi(hit.energy(), local_geo->getPosition().phi());
      hit_met += cell;
   }

   for (auto& hit: *(fhits.product())) {
      HcalDetId id(hit.id());
      const auto *local_geo = calo_geo->getSubdetectorGeometry(id)->getGeometry(id);

      cell.SetMagPhi(hit.energy(), local_geo->getPosition().phi());
      hit_met += cell;
   }

   TVector2 tp_met(0., 0.);

   for (auto& tp: *(tps)) {
      HcalTrigTowerDetId id = tp.id();
      auto energy = decoder->hcaletValue(id.ieta(), id.iphi(), tp.SOI_compressedEt());

      auto ids = tt_geo->detIds(id);
      float phi = 0.;
      for (auto& i: ids) {
         const auto *local_geo = calo_geo->getSubdetectorGeometry(i)->getGeometry(i);
         phi += local_geo->getPosition().phi();
      }
      phi /= ids.size();

      cell.SetMagPhi(energy, phi);
      tp_met += cell;
   }

   std::cout << hit_met.Mod() << " vs TP " << tp_met.Mod() << std::endl;
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
