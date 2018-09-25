//
//  IBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "IBuilder.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void IBuilder::checkArguments(IRGenerationContext& context,
                                    const IBuildableObjectType* buildable,
                                    const BuilderArgumentList& builderArgumentList,
                                    int line) {
  checkArgumentsAreWellFormed(context, buildable, builderArgumentList, line);
  checkAllFieldsAreSet(context, buildable, builderArgumentList, line);
}

void IBuilder::checkArgumentsAreWellFormed(IRGenerationContext& context,
                                                 const IBuildableObjectType* buildable,
                                                 const BuilderArgumentList& builderArgumentList,
                                                 int line) {
  bool areArgumentsWellFormed = true;
  
  for (BuilderArgument* argument : builderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(context, buildable, line);
  }
  
  if (!areArgumentsWellFormed) {
    throw 1;
  }
}

void IBuilder::checkAllFieldsAreSet(IRGenerationContext& context,
                                          const IBuildableObjectType* buildable,
                                          const BuilderArgumentList& builderArgumentList,
                                          int line) {
  set<string> allFieldsThatAreSet;
  for (BuilderArgument* argument : builderArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingFields(buildable, allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    context.reportError(line, "Field " + missingField + " of object " + buildable->getTypeName() +
                        " is not initialized.");
  }
  throw 1;
}

vector<string> IBuilder::getMissingFields(const IBuildableObjectType* buildable,
                                                set<string> givenFields) {
  vector<string> missingFields;
  
  for (IField* receivedField : buildable->getReceivedFields()) {
    if (givenFields.find(receivedField->getName()) == givenFields.end()) {
      missingFields.push_back(receivedField->getName());
    }
  }
  
  return missingFields;
}
