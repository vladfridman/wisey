//
//  NodeTypeSpecifierFull.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeTypeSpecifierFull_h
#define NodeTypeSpecifierFull_h

#include "wisey/INodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a fully qualified node type specifier
 */
class NodeTypeSpecifierFull : public INodeTypeSpecifier {
  IExpression* mPackageExpression;
  const std::string mShortName;
  
public:
  
  NodeTypeSpecifierFull(IExpression* packageExpression, std::string shortName);
  
  ~NodeTypeSpecifierFull();
  
  std::string getShortName() const override;
  
  IExpression* takePackage() override;

  std::string getName(IRGenerationContext& context) const override;
  
  Node* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* NodeTypeSpecifierFull_h */
