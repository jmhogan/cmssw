//
//
/**
    @file PatMuonHitFitTranslator.cc

    @brief Specialization of template class MuonTranslatorBase in the
    package HitFit for pat::Muon.

 */


#include "../interface/MuonTranslatorBase.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

namespace hitfit {


//template<>
MuonTranslatorBase::MuonTranslatorBase()  //Changed
{

    std::string CMSSW_BASE(getenv("CMSSW_BASE"));
    std::string resolution_filename = CMSSW_BASE +
        std::string("/src/TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt");
    resolution_ = EtaDepResolution(resolution_filename);

} // LeptonTranslatorBase<pat::Muon>::LeptonTranslatorBase()


//template<>
MuonTranslatorBase::MuonTranslatorBase(const std::string& ifile)  //Changed
{

    std::string CMSSW_BASE(getenv("CMSSW_BASE"));
    std::string resolution_filename;

    if (ifile.empty()) {
        resolution_filename = CMSSW_BASE +
        std::string("/src/TopQuarkAnalysis/PatHitFit/data/exampleMuonResolution.txt");
    } else {
        resolution_filename = ifile ;
    }

    resolution_ = EtaDepResolution(resolution_filename);

} // LeptonTranslatorBase<pat::Muon>::LeptonTranslatorBase(const std::string& s)


//template<>
MuonTranslatorBase::~MuonTranslatorBase()  //Changed
{
} // LeptonTranslatorBase<pat::Muon>::~LeptonTranslatorBase()


//template<>
Lepjets_Event_Lep
MuonTranslatorBase::operator()(TLorentzVector lepton, //Changed-------
                                            int type /*= hitfit::lepton_label */,
                                            bool useObjEmbRes /* = false */)
{

    Fourvec p(lepton.Px(),lepton.Py(),lepton.Pz(),lepton.Energy());

    double            muon_eta        = lepton.Eta();
    Vector_Resolution muon_resolution = resolution_.GetResolution(muon_eta);

    Lepjets_Event_Lep muon(p,
                           muon_label,
                           muon_resolution);
    return muon;

} 


//template<>
const EtaDepResolution&
MuonTranslatorBase::resolution() const   //Changed------
{
    return resolution_;
}


//template<>
bool
MuonTranslatorBase::CheckEta(TLorentzVector lepton) const //Changed---
{
    return resolution_.CheckEta(lepton.Eta());
}


} // namespace hitfit
