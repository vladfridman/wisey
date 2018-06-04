//
//  ObjectBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectBuilder_h
#define ObjectBuilder_h

#include "wisey/IExpression.hpp"
#include "wisey/IBuildableObjectTypeSpecifier.hpp"
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
    
    IBuildableObjectTypeSpecifier* mTypeSpecifier;
    ObjectBuilderArgumentList mObjectBuilderArgumentList;
    int mLine;
    
  public:
    
    ObjectBuilder(IBuildableObjectTypeSpecifier* typeSpecifier,
                  ObjectBuilderArgumentList ObjectBuilderArgumentList,
                  int line);
    
    ~ObjectBuilder();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ObjectBuilder_h */

