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

#include "wisey/INodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents node type specifier
 */
class NodeTypeSpecifier : public INodeTypeSpecifier {
  const std::string mPackage;
  const std::string mShortName;
  
public:
  
  NodeTypeSpecifier(std::string package, std::string shortName);
  
  ~NodeTypeSpecifier();
  
  std::string getShortName() const override;
  
  std::string getPackage() const override;

  std::string getName(IRGenerationContext& context) const override;

  const Node* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* NodeTypeSpecifier_h */
