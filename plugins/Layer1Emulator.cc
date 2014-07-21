/*
 * =====================================================================================
        Filename:  Layer1Emulator.cc

       Description: Emulate the Layer 1 TPG processing for the Stage 2
       Layer 1 to Layer 2 July Integration Test

         Author:  Isobel Ojalvo, Austin Belknap
         Company:  UW Madison
 * =====================================================================================
 */
#include <iostream>
#include <fstream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "CondFormats/L1TObjects/interface/L1CaloHcalScale.h"
#include "CondFormats/DataRecord/interface/L1CaloHcalScaleRcd.h"

#include "L1Trigger/UCT2015/interface/Layer1Links.h"

class Layer1Emulator : public edm::EDProducer {
  public:
    Layer1Emulator(const edm::ParameterSet& pset);
    virtual ~Layer1Emulator(){}
    virtual void beginJob() override;
    virtual void endJob() override;
    void produce(edm::Event& evt, const edm::EventSetup& es);


  private:
    //double threshold_;
    // int rawthreshold_;
    //bool cutOnRawBits_;
    edm::InputTag hcalDigis_;
    edm::InputTag ecalDigis_;
    bool debug_;

    std::ofstream outfile;
};


Layer1Emulator::Layer1Emulator(const edm::ParameterSet& pset) {
  hcalDigis_ = pset.getParameter<edm::InputTag>("hcalDigis");
  ecalDigis_ = pset.getParameter<edm::InputTag>("ecalDigis");
  debug_ = pset.exists("debug") ? pset.getParameter<bool>("debug") : false;
  produces<HcalTrigPrimDigiCollection>();
}


void
Layer1Emulator::produce(edm::Event& evt, const edm::EventSetup& es) {

  edm::ESHandle<L1CaloHcalScale> hcalScale;
  es.get<L1CaloHcalScaleRcd>().get(hcalScale);

  edm::Handle<HcalTrigPrimDigiCollection> hcalTpgs;
  std::auto_ptr<HcalTrigPrimDigiCollection> output(new HcalTrigPrimDigiCollection);

  Layer1Links layer1links(evt.id().event(), evt.id().luminosityBlock(), evt.id().run());

  if(evt.getByLabel(hcalDigis_, hcalTpgs)){
    output->reserve(hcalTpgs->size());
    std::cout<<"hcal tpg size "<< hcalTpgs->size()<<std::endl;
    for (size_t i = 0; i < hcalTpgs->size(); ++i) {
      HcalTriggerPrimitiveDigi tpg = (*hcalTpgs)[i];
      short ieta = tpg.id().ieta();
      short iphi = tpg.id().iphi();
      short zside = tpg.id().zside();
      
      // Get the real energy
      //double energy = hcalScale->et(tpg.SOI_compressedEt(), ieta, zside);
      
      ///if (debug_) {
      //std::cout << ieta << " " << energy << std::endl;
      //}
      int energy = tpg.SOI_compressedEt();

      layer1links.add_hcal_tower(ieta, iphi, energy, 0);

      if(debug_)
	if(energy>20)
	  std::cout << "Layer 1 HCAL et: " << energy << " eta: " << ieta << " iphi: " << iphi  << std::endl;
      
      HcalTriggerPrimitiveSample new_t0(tpg.t0().raw() | 0x100);
      tpg.setSample(tpg.presamples(), new_t0);
      output->push_back(tpg);
    }
  }

  edm::Handle<EcalTrigPrimDigiCollection> ecalTpgs;

  if(evt.getByLabel(ecalDigis_, ecalTpgs)){
    std::cout<<"ecal tpg size "<< ecalTpgs->size()<<std::endl;
    for (size_t i = 0; i < ecalTpgs->size(); ++i) {
      EcalTriggerPrimitiveDigi tpg = (*ecalTpgs)[i];
      short ieta = tpg.id().ieta();
      short iphi = tpg.id().iphi();
      bool fineGrain = tpg.fineGrain();
      short zside = tpg.id().zside();

      layer1links.add_ecal_tower(ieta, iphi, tpg.compressedEt(), fineGrain);

      // Get the real energy
      //double energy = hcalScale->et(tpg.SOI_compressedEt(), ieta, zside);

      //if (debug_) {
      //std::cout << ieta << " " << energy << std::endl;
      //}

      if(debug_)
        if(tpg.compressedEt()>5)
          std::cout << "Layer 1 ECAL et: " << tpg.compressedEt() << " eta: " << ieta << " iphi: " << iphi 
            << "Fine Grain " << fineGrain << std::endl;
    }
  }
  evt.put(output);

  layer1links.write_to_file(outfile);
}


void
Layer1Emulator::beginJob() {
  outfile.open("output.txt", std::ofstream::out);
}


void
Layer1Emulator::endJob() {
  outfile.close();
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(Layer1Emulator);
