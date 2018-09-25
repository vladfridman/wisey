//
//  PoolBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef PoolBuilder_h
#define PoolBuilder_h

#include "IBuildableObjectTypeSpecifier.hpp"
#include "IBuilder.hpp"
#include "BuilderArgument.hpp"

namespace wisey {
  
  /**
   * Represents a build used to allocate buildable objects on memory pools
   *
   * The object builder is used as follows in the wisey language:
   *
   * NodeType* nodeA = build(NodeType).withField(1).onPool(pool);
   */
  class PoolBuilder : public IBuilder {
    
    IBuildableObjectTypeSpecifier* mTypeSpecifier;
    BuilderArgumentList mBuilderArgumentList;
    IExpression* mPoolExpression;
    int mLine;
    
  public:
    
    PoolBuilder(IBuildableObjectTypeSpecifier* typeSpecifier,
                    BuilderArgumentList builderArgumentList,
                    IExpression* poolExpression,
                    int line);

    ~PoolBuilder();
    
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

#endif /* PoolBuilder_h */
