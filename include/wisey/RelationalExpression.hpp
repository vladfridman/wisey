//
//  RelationalExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef RelationalExpression_h
#define RelationalExpression_h

#include "wisey/IBinaryExpression.hpp"
#include "wisey/IHasType.hpp"
#include "wisey/RelationalOperation.hpp"

namespace wisey {
  
  /**
   * Represents a relational expression such as a >= b
   */
  class RelationalExpression : public IBinaryExpression {
    const IExpression* mLeft;
    const IExpression* mRight;
    RelationalOperation mOperation;
    int mLine;
    
  public:
    
    RelationalExpression(const IExpression* left,
                         RelationalOperation operation,
                         const IExpression* right,
                         int line);
    
    ~RelationalExpression();
    
    const IExpression* getLeft() const override;
    
    const IExpression* getRight() const override;
    
    std::string getOperation() const override;

    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* generateIRForObjects(IRGenerationContext& context,
                                      const IType* assignToType) const;
    
    llvm::Value* generateIRForFloats(IRGenerationContext& context, const IType* assignToType) const;
    
    llvm::Value* generateIRForInts(IRGenerationContext& context, const IType* assignToType) const;
    
    void reportIncompatableTypes(IRGenerationContext& context,
                                 const IType* leftType,
                                 const IType* rightType) const;
    
  };
  
} /* namespace wisey */

#endif /* RelationalExpression_h */

