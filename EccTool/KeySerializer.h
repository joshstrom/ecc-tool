//
//  KeySerializer.h
//  EccTool
//
//  Created by Josh Strom on 2/8/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__Version1KeySerializer__
#define __EccTool__Version1KeySerializer__

#include <iostream>
#include <string>
#include "EccAlg.h"

using namespace std;

class KeySerializer
{
public:
    // Serializes the public keys from the given alg.
    string SerializePublicKeys(const EccAlg& alg) const;
    
    // Serializes the public and private keys from the given alg.
    string SerializePrivateKeys(const EccAlg& alg) const;
    
    // Creates an ECC alg from the provided serialized keys.
    EccAlg ParseKeys(const string& keys) const;
};

#endif /* defined(__EccTool__Version1KeySerializer__) */
