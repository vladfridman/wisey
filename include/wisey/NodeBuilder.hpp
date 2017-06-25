//
//  NodeBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeBuilder_h
#define NodeBuilder_h

#include "wisey/BuilderArgument.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents builder used to initialize nodes
 *
 * The nodes builder is of the form:
 *
 * NodeType* modelA = builder(NodeType).withField(1).builder();
 */
class NodeBuilder : public IExpression {
    
  NodeTypeSpecifier* mNodeTypeSpecifier;
  BuilderArgumentList mBuilderArgumentList;
  
public:
  
  NodeBuilder(NodeTypeSpecifier* nodeTypeSpecifier,
              BuilderArgumentList builderArgumentList) :
  mNodeTypeSpecifier(nodeTypeSpecifier),
  mBuilderArgumentList(builderArgumentList) { }
  
  ~NodeBuilder();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* NodeBuilder_h */
