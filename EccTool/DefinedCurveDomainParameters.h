//
//  DefinedCurveDomainParameters.h
//  EccTool
//
//  Created by Josh Strom on 1/26/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_DefinedCurveDomainParameters_h
#define EccTool_DefinedCurveDomainParameters_h

#include "EccDefs.h"
#include <exception>

namespace ecc
{
    const vector<string> GetSupportedCurves();
    const DomainParameters GetCurveByName(const string& name);
    DomainParameters GetSecp256k1Curve();
    DomainParameters GetSecp112r1Curve();

}

#endif
