// -*- C++ -*-
//
// Package:    Debug/TauRoast
// Class:      SaturatedFilter
// 
/**\class TauRoast SaturatedFilter.cc Debug/TauRoast/plugins/SaturatedFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Matthias Wolf
//         Created:  Mon Sep 28 14:42:31 CEST 2015
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloCollections.h"
#include "DataFormats/HcalDigi/interface/HcalTriggerPrimitiveDigi.h"

//
// class declaration
//

class SaturatedFilter : public edm::EDFilter {
   public:
      explicit SaturatedFilter(const edm::ParameterSet&);
      ~SaturatedFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<HcalTrigPrimDigiCollection> token_;
      unsigned int max_;
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
SaturatedFilter::SaturatedFilter(const edm::ParameterSet& config) :
   max_(config.getParameter<unsigned int>("maxValue"))
{
   token_ = consumes<HcalTrigPrimDigiCollection>(config.getParameter<edm::InputTag>("triggerPrimitives"));
}


SaturatedFilter::~SaturatedFilter()
{
}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
SaturatedFilter::filter(edm::Event& event, const edm::EventSetup& setup)
{
   edm::Handle<HcalTrigPrimDigiCollection> tps;
   event.getByToken(token_, tps);

   for (const auto& tp: *tps) {
      if ((unsigned int) tp.SOI_compressedEt() == max_)
         return false;
   }

   return true;
}

// ------------ method called once each job just before starting event loop  ------------
void 
SaturatedFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SaturatedFilter::endJob() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SaturatedFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(SaturatedFilter);
