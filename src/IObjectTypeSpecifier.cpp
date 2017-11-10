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
                                              vector<string> package) {
  if (package.size() > 0) {
    string fullName = "";
    for (string part : package) {
      fullName.append(part + ".");
    }
    fullName.append(shortName);
    
    return fullName;
  }
  
  if (context.getImport(shortName) != NULL) {
    return context.getImport(shortName)->getName();
  }
  if (context.getPackage().size() != 0) {
    return context.getPackage() + "." + shortName;
  }
  
  Log::e("Could not identify packge for object " + shortName);
  exit(1);
}

