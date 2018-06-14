//
//  IObjectCreator.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectCreator.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void IObjectCreator::checkArguments(IRGenerationContext& context,
                                    const IBuildableObjectType* buildable,
                                    const ObjectBuilderArgumentList& objectBuilderArgumentList,
                                    int line) {
  checkArgumentsAreWellFormed(context, buildable, objectBuilderArgumentList, line);
  checkAllFieldsAreSet(context, buildable, objectBuilderArgumentList, line);
}

void IObjectCreator::checkArgumentsAreWellFormed(IRGenerationContext& context,
                                                 const IBuildableObjectType* buildable,
                                                 const ObjectBuilderArgumentList&
                                                 objectBuilderArgumentList,
                                                 int line) {
  bool areArgumentsWellFormed = true;
  
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(context, buildable, line);
  }
  
  if (!areArgumentsWellFormed) {
    throw 1;
  }
}

void IObjectCreator::checkAllFieldsAreSet(IRGenerationContext& context,
                                          const IBuildableObjectType* buildable,
                                          const ObjectBuilderArgumentList&
                                          objectBuilderArgumentList,
                                          int line) {
  set<string> allFieldsThatAreSet;
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
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

vector<string> IObjectCreator::getMissingFields(const IBuildableObjectType* buildable,
                                                set<string> givenFields) {
  vector<string> missingFields;
  
  for (IField* receivedField : buildable->getReceivedFields()) {
    if (givenFields.find(receivedField->getName()) == givenFields.end()) {
      missingFields.push_back(receivedField->getName());
    }
  }
  
  return missingFields;
}

