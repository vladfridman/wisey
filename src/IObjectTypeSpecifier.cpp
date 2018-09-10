//
//  IObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectTypeSpecifier.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

std::string IObjectTypeSpecifier::getFullName(IRGenerationContext& context,
                                              string shortName,
                                              IExpression* packageExpression,
                                              int line) {
  if (packageExpression) {
    return packageExpression->getType(context)->getTypeName() + "." + shortName;
  }
  
  const IObjectType* objectType = context.getObjectType();
  
  if (objectType && !objectType->getShortName().compare(shortName)) {
    return objectType->getTypeName();
  }
  
  const IObjectType* parentObject = objectType
  ? IObjectType::getParentOrSelf(&context, objectType, line) : objectType;
  const IObjectType* innerObject = parentObject ? parentObject->getInnerObject(shortName) : NULL;
  if (innerObject) {
    return innerObject->getTypeName();
  }

  ImportProfile* importProfile = objectType
    ? objectType->getImportProfile()
    : context.getImportProfile();
  
  assert(importProfile && "Import profile is null");
  return importProfile->getFullName(shortName, line);
}

