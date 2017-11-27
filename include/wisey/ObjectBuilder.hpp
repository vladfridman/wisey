//
//  ObjectBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectBuilder_h
#define ObjectBuilder_h

#include "wisey/IBuildableConcreteObjectTypeSpecifier.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

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
  ObjectBuilderArgumentList mObjectBuilderArgumentList;
  
public:
  
  ObjectBuilder(IBuildableConcreteObjectTypeSpecifier* typeSpecifier,
                ObjectBuilderArgumentList ObjectBuilderArgumentList);
  
  ~ObjectBuilder();
  
  IVariable* getVariable(IRGenerationContext& context) const override;

  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* ObjectBuilder_h */
