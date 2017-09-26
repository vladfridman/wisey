//
//  NodeTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeTypeSpecifier_h
#define NodeTypeSpecifier_h

#include <string>

#include "wisey/IBuildableConcreteObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents node type specifier
 */
class NodeTypeSpecifier : public IBuildableConcreteObjectTypeSpecifier {
  const std::vector<std::string> mPackage;
  const std::string mName;
  
public:
  
  NodeTypeSpecifier(std::vector<std::string> package, std::string name)
  : mPackage(package), mName(name) { }
  
  ~NodeTypeSpecifier() { }
  
  Node* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* NodeTypeSpecifier_h */
