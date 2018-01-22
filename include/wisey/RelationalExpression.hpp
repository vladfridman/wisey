//
//  RelationalExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef RelationalExpression_h
#define RelationalExpression_h

#include "wisey/IExpression.hpp"
#include "wisey/IHasType.hpp"
#include "wisey/RelationalOperation.hpp"

namespace wisey {
  
/**
 * Represents a relational expression such as a >= b
 */
class RelationalExpression : public IExpression {
  IExpression* mLeftExpression;
  IExpression* mRightExpression;
  RelationalOperation mOperation;
  int mLine;
  
public:

  RelationalExpression(IExpression* leftExpression,
                       RelationalOperation operation,
                       IExpression* rightExpression,
                       int line);

  ~RelationalExpression();
  
  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;

  llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
   
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  llvm::Value* generateIRForObjects(IRGenerationContext& context, const IType* assignToType) const;
  
  llvm::Value* generateIRForFloats(IRGenerationContext& context, const IType* assignToType) const;
  
  llvm::Value* generateIRForInts(IRGenerationContext& context, const IType* assignToType) const;

  void reportIncompatableTypes(const IType* leftType, const IType* rightType) const;

};
  
} /* namespace wisey */

#endif /* RelationalExpression_h */
