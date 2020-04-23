 //
//

/**
    @file PatElectronHitFitTranslator.cc

    @brief Specialization of template class ElectronTranslatorBase in the
    package HitFit for TLorentzVector.


 */


#include "../interface/ElectronTranslatorBase.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

namespace hitfit {

//template<>
ElectronTranslatorBase::ElectronTranslatorBase() //Changed
{

    std::string CMSSW_BASE(getenv("CMSSW_BASE"));
    std::string resolution_filename = CMSSW_BASE +
        std::string("/src/TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt");
    resolution_ = EtaDepResolution(resolution_filename);

} 


//template<>
ElectronTranslatorBase::ElectronTranslatorBase(const std::string& ifile)  //Changed
{

    std::string CMSSW_BASE(getenv("CMSSW_BASE"));
    std::string resolution_filename;

    if (ifile.empty()) {
        resolution_filename = CMSSW_BASE +
        std::string("/src/TopQuarkAnalysis/PatHitFit/data/exampleElectronResolution.txt");
    } else {
        resolution_filename = ifile ;
    }

    resolution_ = EtaDepResolution(resolution_filename);

} // ElectronTranslatorBase<pat::Electron>::ElectronTranslatorBase(const std::string& ifile)


//template<>
ElectronTranslatorBase::~ElectronTranslatorBase()  //Changed
{
}


//template<>
Lepjets_Event_Lep
ElectronTranslatorBase::operator()(TLorentzVector lepton, //Changed------
                                                int type /*= hitfit::lepton_label*/,
                                                bool useObjEmbRes /* = false */ )
{
    Fourvec p(lepton.Px(),lepton.Py(),lepton.Pz(),lepton.Energy());

    double            electron_eta        = lepton.Eta();
    Vector_Resolution electron_resolution = resolution_.GetResolution(electron_eta);

    Lepjets_Event_Lep electron(p,
                               electron_label,
                               electron_resolution);
    return electron;

} 


//template<>
const EtaDepResolution&
ElectronTranslatorBase::resolution() const //Changed
{
    return resolution_;
}


//template<>
bool
ElectronTranslatorBase::CheckEta(TLorentzVector lepton) const  //Changed------
{
    double            electron_eta        = lepton.Eta();
    return resolution_.CheckEta(electron_eta);
}


} // namespace hitfit
