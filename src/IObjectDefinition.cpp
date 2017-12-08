//
//  IObjectDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace std;
using namespace wisey;

string IObjectDefinition::getFullName(IRGenerationContext& context,
                                      IObjectTypeSpecifier* typeSpecifier) {
  return context.getObjectType()
  ? context.getObjectType()->getName() + "." + typeSpecifier->getShortName()
  : typeSpecifier->getName(context);
}

void IObjectDefinition::prototypeInnerObjects(IRGenerationContext& context,
                                              vector<IObjectDefinition*> innerObjectDefinitions) {
  for (IObjectDefinition* innerObjectDefinition : innerObjectDefinitions) {
    innerObjectDefinition->prototypeObjects(context);
  }
}

void IObjectDefinition::prototypeInnerObjectMethods(IRGenerationContext& context,
                                                    vector<IObjectDefinition*>
                                                    innerObjectDefinitions) {
  for (IObjectDefinition* innerObjectDefinition : innerObjectDefinitions) {
    innerObjectDefinition->prototypeMethods(context);
  }
}

void IObjectDefinition::generateInnerObjectIR(IRGenerationContext& context,
                                              vector<IObjectDefinition*> innerObjectDefinitions) {
  for (IObjectDefinition* innerObjectDefinition : innerObjectDefinitions) {
    innerObjectDefinition->generateIR(context);
  }
}

