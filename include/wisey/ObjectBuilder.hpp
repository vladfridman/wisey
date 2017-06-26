//
//  ObjectBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectBuilder_h
#define ObjectBuilder_h

#include "wisey/BuilderArgument.hpp"
#include "wisey/IBuildableConcreteObjectTypeSpecifier.hpp"
#include "wisey/IExpression.hpp"

namespace wisey {

/**
 * Represents builder used to initialize buildable objects
 *
 * The object builder is used as follows in the wisey language:
 *
 * ModelType* modelA = builder(ModelType).withField(1).build();
 */
class ObjectBuilder : public IExpression {
  
  IBuildableConcreteObjectTypeSpecifier* mTypeSpecifier;
  BuilderArgumentList mBuilderArgumentList;
  
public:
  
  ObjectBuilder(IBuildableConcreteObjectTypeSpecifier* typeSpecifier,
               BuilderArgumentList builderArgumentList) :
  mTypeSpecifier(typeSpecifier),
  mBuilderArgumentList(builderArgumentList) { }
  
  ~ObjectBuilder();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* ObjectBuilder_h */
