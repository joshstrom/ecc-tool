//
//  Version1KeySerializer.h
//  EccTool
//
//  Created by Josh Strom on 2/8/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#ifndef __EccTool__Version1KeySerializer__
#define __EccTool__Version1KeySerializer__

#include <iostream>
#include "AbstractKeySerializer.h"

class Version1KeySerializer final : public AbstractKeySerializer
{
public:
    string SerializePublicKeys(const EccAlg& alg) const override;
    string SerializePrivateKeys(const EccAlg& alg) const override;
    EccAlg ParseKeys(const string& keys) const override;
};

#endif /* defined(__EccTool__Version1KeySerializer__) */
