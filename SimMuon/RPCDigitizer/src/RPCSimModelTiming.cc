#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCRollSpecs.h"
#include "SimMuon/RPCDigitizer/src/RPCSimModelTiming.h"
#include "SimMuon/RPCDigitizer/src/RPCSimSetUp.h"

#include "SimMuon/RPCDigitizer/src/RPCSynchronizer.h"
#include "Geometry/CommonTopologies/interface/RectangularStripTopology.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"

#include <cmath>

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EventSetup.h>
#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>
#include <FWCore/Framework/interface/ESHandle.h>

#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include <Geometry/Records/interface/MuonGeometryRecord.h>
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "SimMuon/RPCDigitizer/src/RPCSimSetUp.h"

#include<cstring>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stdlib.h>
#include <utility>
#include <map>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandPoissonQ.h"

//fast trick to coupe with the condidions
//RPCSimModelTiming::RPCSimModelTiming(const edm::ParameterSet& config) : RPCSimAsymmetricCls(config)
RPCSimModelTiming::RPCSimModelTiming(const edm::ParameterSet& config) : RPCSimAverageNoiseEffCls(config) //, IRPCPreciseTiming(config)
{

}

RPCSimModelTiming::~RPCSimModelTiming()
{

}

void RPCSimModelTiming::simulateIRPC(const RPCRoll* roll,
                const edm::PSimHitContainer& rpcHits,
                 CLHEP::HepRandomEngine* engine) 
{

  _rpcSync->setRPCSimSetUp(getRPCSimSetUp());
  theRpcDigiSimLinks.clear();
  theDetectorHitMap.clear();
  theRpcDigiSimLinks = RPCDigiSimLinks(roll->id().rawId());

  RPCDetId rpcId = roll->id();
  RPCGeomServ RPCname(rpcId);
  //std::string nameRoll = RPCname.name();

  const Topology& topology=roll->specs()->topology();

  for (edm::PSimHitContainer::const_iterator _hit = rpcHits.begin();
       _hit != rpcHits.end(); ++_hit){

    if(_hit-> particleType() == 11) continue;

    // Here I hould check if the RPC are up side down;
    const LocalPoint& entr=_hit->entryPoint();

    int time_hit = _rpcSync->getSimHitBxAndTimingForIRPC(&(*_hit), engine);
    double precise_time = _rpcSync->getSmearedTime();

    float posX = roll->strip(_hit->localPosition()) - static_cast<int>(roll->strip(_hit->localPosition()));

    std::vector<float> veff = (getRPCSimSetUp())->getEff(rpcId.rawId());

    // Effinciecy
    int centralStrip = topology.channel(entr)+1;;
    float fire = CLHEP::RandFlat::shoot(engine);

    if (fire < veff[centralStrip-1]) {

      int fstrip=centralStrip;
      int lstrip=centralStrip;

      // Compute the cluster size
      // double w = CLHEP::RandFlat::shoot(engine);
      //if (w < 1.e-10) w=1.e-10;
//       int clsize = this->getClSize(posX, engine); // This is for one and the same cls for all the chambers
      int clsize = this->getClSize(rpcId.rawId(),posX, engine); // This is for cluster size chamber by chamber
      std::vector<int> cls;
      cls.push_back(centralStrip);
      if (clsize > 1){
	for (int cl = 0; cl < (clsize-1)/2; cl++){
	  if (centralStrip - cl -1 >= 1  ){
	    fstrip = centralStrip-cl-1;
	    cls.push_back(fstrip);
	  }
	  if (centralStrip + cl + 1 <= roll->nstrips() ){
	    lstrip = centralStrip+cl+1;
	    cls.push_back(lstrip);
	  }
	}
	if (clsize%2 == 0 ){
	  // insert the last strip according to the 
	  // simhit position in the central strip 
	  double deltaw=roll->centreOfStrip(centralStrip).x()-entr.x();
	  if (deltaw<0.) {
	    if (lstrip < roll->nstrips() ){
	      lstrip++;
	      cls.push_back(lstrip);
	    }
	  }else{
	    if (fstrip > 1 ){
	      fstrip--;
	      cls.push_back(fstrip);
	    }
	  }
	}
      }

      for (std::vector<int>::iterator i=cls.begin(); i!=cls.end();i++){
	// Check the timing of the adjacent strip
	if(*i != centralStrip){
	  if(CLHEP::RandFlat::shoot(engine) < veff[*i-1]){
	    std::pair<int, int> digi(*i,time_hit);
            RPCDigi adigi(*i,time_hit);
            adigi.hasTime(true);
            adigi.setTime(precise_time);
            irpc_digis.insert(adigi);

	    theDetectorHitMap.insert(DetectorHitMap::value_type(digi,&(*_hit)));
	  }
	} 
	else {
	  std::pair<int, int> digi(*i,time_hit);
	  RPCDigi adigi(*i,time_hit);
          adigi.hasTime(true);
          adigi.setTime(precise_time);
          irpc_digis.insert(adigi);
 theDetectorHitMap.insert(DetectorHitMap::value_type(digi,&(*_hit)));
	}
      }
    }
  }
}

void RPCSimModelTiming::simulateIRPCNoise(const RPCRoll* roll,
                     CLHEP::HepRandomEngine* engine) 
{

RPCDetId rpcId = roll->id();

  RPCGeomServ RPCname(rpcId);

  std::vector<float> vnoise = (getRPCSimSetUp())->getNoise(rpcId.rawId());
  std::vector<float> veff = (getRPCSimSetUp())->getEff(rpcId.rawId());

  unsigned int nstrips = roll->nstrips();
  double area = 0.0;

  if ( rpcId.region() == 0 )
    {
      const RectangularStripTopology* top_ = dynamic_cast<const
        RectangularStripTopology*>(&(roll->topology()));
      float xmin = (top_->localPosition(0.)).x();
      float xmax = (top_->localPosition((float)roll->nstrips())).x();
      float striplength = (top_->stripLength());
      area = striplength*(xmax-xmin);
    }
  else
    {
      const TrapezoidalStripTopology* top_=dynamic_cast<const TrapezoidalStripTopology*>(&(roll->topology()));
      float xmin = (top_->localPosition(0.)).x();
      float xmax = (top_->localPosition((float)roll->nstrips())).x();
      float striplength = (top_->stripLength());
      area = striplength*(xmax-xmin);
    }

  for(unsigned int j = 0; j < vnoise.size(); ++j){

    if(j >= nstrips) break;

    double ave =
      vnoise[j]*nbxing*gate*area*1.0e-9*frate/((float)roll->nstrips());

    CLHEP::RandPoissonQ randPoissonQ(*engine, ave);
    N_hits = randPoissonQ.fire();

    for (int i = 0; i < N_hits; i++ ){
      
      double precise_time = CLHEP::RandFlat::shoot(engine, (nbxing*gate)/gate);
      int time_hit = (static_cast<int>(precise_time)) - nbxing/2;
//      std::pair<int, int> digi(j+1,time_hit);
//      strips.insert(digi);
            RPCDigi adigi(j+1,time_hit);
            irpc_digis.insert(adigi);

    }
  }



}

