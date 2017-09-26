//
//  IObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

std::string IObjectTypeSpecifier::getFullName(IRGenerationContext& context,
                                              string shortName,
                                              vector<string> package) {
  if (package.size() == 0 && context.getImport(shortName) != NULL) {
    return context.getImport(shortName)->getName();
  }
  if (package.size() == 0 && context.getPackage().size() != 0) {
    return context.getPackage() + "." + shortName;
  }
  
  string fullName = "";
  for (string part : package) {
    fullName.append(part + ".");
  }
  fullName.append(shortName);
  
  return fullName;
}
