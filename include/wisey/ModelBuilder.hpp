//
//  ModelBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelBuilder_h
#define ModelBuilder_h

#include "wisey/BuilderArgument.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {

/**
 * Represents builder used to initialize buildable objects
 *
 * The object builder is used as follows in the wisey language:
 *
 * ModelType* modelA = builder(ModelType).withField(1).build();
 */
class ModelBuilder : public IExpression {
  
  ModelTypeSpecifier* mModelTypeSpecifier;
  BuilderArgumentList mBuilderArgumentList;
  
public:
  
  ModelBuilder(ModelTypeSpecifier* modelTypeSpecifier,
               BuilderArgumentList builderArgumentList)
  : mModelTypeSpecifier(modelTypeSpecifier),
  mBuilderArgumentList(builderArgumentList) { }
  
  ~ModelBuilder();
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
  bool existsInOuterScope(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* ModelBuilder_h */
