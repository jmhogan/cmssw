//
//

/**
    @file PatMETHitFitTranslator.cc

    @brief Specialization of template class METTranslatorBase in the
    package HitFit for pat::MET

    @author Haryo Sumowidagdo <Suharyo.Sumowidagdo@cern.ch>

    @par Created
    Sat Jun 27 17:49:32 2009 UTC

    @version $Id: PatMETHitFitTranslator.cc,v 1.7 2011/01/29 22:21:38 haryo Exp $
 */


#include "../interface/METTranslatorBase.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include <cmath>

namespace hitfit {


//template<>
METTranslatorBase::METTranslatorBase()  //Changed
{
    resolution_ = Resolution(std::string("0,0,12"));
} // METTranslatorBase<pat::MET>::METTranslatorBase()


//template<>
METTranslatorBase::METTranslatorBase(const std::string& ifile) //Changed
{
    const Defaults_Text defs(ifile);
    std::string resolution_string(defs.get_string("met_resolution"));
    resolution_ = Resolution(resolution_string);

} // METTranslatorBase<pat::MET>::METTranslatorBase(const std::string& ifile)


//template<>
METTranslatorBase::~METTranslatorBase() //changed
{
} // METTranslatorBase<pat::MET>::~METTranslatorBase()


//template<>
Fourvec
METTranslatorBase::operator()(double metPx, double metPy, //Changed----
                                        bool useObjEmbRes /* = false */)
{
    double px = metPx;
    double py = metPy;

    return Fourvec (px,py,0.0,sqrt(px*px + py*py));

} // Fourvec METTranslatorBase<pat::MET>::operator()(const pat::MET& m)

//template<>
Fourvec
METTranslatorBase::operator()(double metPx, double metPy, double metEnergy, //Changed----
                                        bool useObjEmbRes /* = false */)
{
    double px = metPx;
    double py = metPy;

    return Fourvec (px,py,0.0,sqrt(px*px + py*py));

} // Fourvec METTranslatorBase<pat::MET>::operator()(const pat::MET& m)


//template<>
Resolution
METTranslatorBase::KtResolution(double metPx, double metPy,//Changed-----
                                          bool useObjEmbRes /* = false */) const
{
    return resolution_;
} // Resolution METTranslatorBase<pat::MET>::KtResolution(const pat::MET& m)



//template<>
Resolution
METTranslatorBase::METResolution(double metPx, double metPy, //Changed-----
                                           bool useObjEmbRes /* = false */) const
{
    return KtResolution(metPx,metPy,useObjEmbRes);
} // Resolution METTranslatorBase<pat::MET>::METResolution(const pat::MET& m)


} // namespace hitfit
