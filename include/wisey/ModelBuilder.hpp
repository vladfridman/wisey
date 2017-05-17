//
//  ModelBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelBuilder_h
#define ModelBuilder_h

#include "wisey/IExpression.hpp"
#include "wisey/ModelBuilderArgument.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {

/**
 * Represents builder used to initialize models
 *
 * The model builder is of the form:
 *
 * ModelType modelA = builder(ModelType).withField(1).builder();
 */
class ModelBuilder : public IExpression {
  
  ModelTypeSpecifier& mModelTypeSpecifier;
  ModelBuilderArgumentList* mModelBuilderArgumentList;
  
public:
  
  ModelBuilder(ModelTypeSpecifier& modelTypeSpecifier,
               ModelBuilderArgumentList* modelBuilderArgumentList)
  : mModelTypeSpecifier(modelTypeSpecifier),
  mModelBuilderArgumentList(modelBuilderArgumentList) { }
  
  ~ModelBuilder() { }
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
  IType* getType(IRGenerationContext& context) const override;
  
  void releaseOwnership(IRGenerationContext& context) const override;
  
private:
  
  std::string getVariableName() const;
  
};
  
} /* namespace wisey */

#endif /* ModelBuilder_h */
