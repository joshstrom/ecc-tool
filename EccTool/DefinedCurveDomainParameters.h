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
    // Returns a list of supported curve names.
    const vector<string> GetSupportedCurves();
    
    // Returns the specified curve parameters.
    const DomainParameters GetCurveByName(const string& name);
    
    // Gets the Secp256k1 curve parameters (this is the curve used by Bitcoin).
    DomainParameters GetSecp256k1Curve();
    
    // Gets the Secp112r1 curve parameters.
    DomainParameters GetSecp112r1Curve();
}

#endif
