//
//  ModelBuilder.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelBuilder_h
#define ModelBuilder_h

#include "yazyk/IExpression.hpp"
#include "yazyk/ModelBuilderArgument.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"

namespace yazyk {

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
  
private:
  
  void checkArguments(Model* model) const;
  
  void checkArgumentsAreWellFormed(Model* model) const;
  
  void checkAllFieldsAreSet(Model* model) const;
};
  
} /* namespace yazyk */

#endif /* ModelBuilder_h */
