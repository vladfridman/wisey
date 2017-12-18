//
//  Identifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Identifier_h
#define Identifier_h

#include "wisey/IExpression.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {

/**
 * Represents an identifier in a wisey program.
 *
 * Identifier could either refer to a variable, a method or a part of the package name
 * Identifier type is determined in the following order: if the method with this name is found
 * in the current object then it is a method, if a variable with this name exists then it is
 * a variable, otherwise it is an undefined identifier that could be part of a package name
 * or an undefined variable.
 *
 * See {@link IdentifierChain} for explanation on how identifier chains are interpreted as packages
 */
class Identifier : public IExpression {
  
  const std::string mName;
  
public:
  
  Identifier(const std::string& name);
  
  ~Identifier();

  const std::string& getIdentifierName() const;

  IVariable* getVariable(IRGenerationContext& context,
                         std::vector<const IExpression*>& arrayIndices) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context, IRGenerationFlag flag) const override;
  
  const IType* getType(IRGenerationContext& context) const override;
  
  bool isConstant() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  IMethodDescriptor* getMethod(IRGenerationContext& context) const;
  
};
  
} /* namespace wisey */

#endif /* Identifier_h */
