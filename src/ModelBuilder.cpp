//
//  ModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Log.hpp"
#include "yazyk/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ModelBuilder::generateIR(IRGenerationContext& context) const {
  bool areArgumentsWellFormed = true;
  
  for (vector<ModelBuilderArgument*>::iterator iterator = mModelBuilderArgumentList->begin();
       iterator != mModelBuilderArgumentList->end();
       iterator++) {
    ModelBuilderArgument* argument = *iterator;
    areArgumentsWellFormed &= argument->checkArgument();
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some arguments for the model builder are not well formed");
    exit(1);
  }
  
  return NULL;
}