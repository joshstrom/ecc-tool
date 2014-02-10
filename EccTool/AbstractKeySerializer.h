//
//  AbstractKeySerializer.h
//  EccTool
//
//  Created by Josh Strom on 2/8/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef EccTool_AbstractKeySerializer_h
#define EccTool_AbstractKeySerializer_h

#include <string>
#include <tuple>
#include "EccAlg.h"

using namespace std;

// Base class interface for key serializers.
class AbstractKeySerializer
{
public:
    virtual string SerializePublicKeys(const EccAlg& alg) const = 0;
    virtual string SerializePrivateKeys(const EccAlg& alg) const = 0;
    virtual EccAlg ParseKeys(const string& keys) const = 0;
};


#endif
