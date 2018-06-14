//
//  ObjectAllocator.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectAllocator_h
#define ObjectAllocator_h

#include "wisey/IBuildableObjectTypeSpecifier.hpp"
#include "wisey/IObjectCreator.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {
  
  /**
   * Represents allocator used to allocate buildable objects on memory pools
   *
   * The object builder is used as follows in the wisey language:
   *
   * ModelType* modelA = allocator(ModelType).withField(1).onPool(pool);
   */
  class ObjectAllocator : public IObjectCreator {
    
    IBuildableObjectTypeSpecifier* mTypeSpecifier;
    ObjectBuilderArgumentList mObjectBuilderArgumentList;
    IExpression* mPoolExpression;
    int mLine;
    
  public:
    
    ObjectAllocator(IBuildableObjectTypeSpecifier* typeSpecifier,
                    ObjectBuilderArgumentList objectBuilderArgumentList,
                    IExpression* poolExpression,
                    int line);

    ~ObjectAllocator();
    
    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* allocate(IRGenerationContext& context,
                          const IBuildableObjectType* buildable) const;

  };
  
} /* namespace wisey */

#endif /* ObjectAllocator_h */
