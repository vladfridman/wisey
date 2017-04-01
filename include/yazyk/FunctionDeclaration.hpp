//
//  FunctionDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FunctionDeclaration_h
#define FunctionDeclaration_h

#include "yazyk/CompoundStatement.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/ITypeSpecifier.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {

/**
 * Represents a global function declaration
 *
 * TODO: remove this class when global functions are removed
 */
class FunctionDeclaration : public IStatement {
  const ITypeSpecifier& mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  CompoundStatement& mCompoundStatement;
  
public:
  
  FunctionDeclaration(const ITypeSpecifier& returnTypeSpecifier,
                      std::string methodName,
                      const VariableList& arguments,
                      CompoundStatement& compoundStatement) :
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments),
  mCompoundStatement(compoundStatement) { }
    
  llvm::Value* generateIR(IRGenerationContext& context) const override;

private:
  
  llvm::Function* createFunction(IRGenerationContext& context) const;
  
  void createArguments(IRGenerationContext& context, llvm::Function* function) const;
  
  void addImpliedVoidReturn(IRGenerationContext& context) const;
  
  void storeArgumentValue(IRGenerationContext& context,
                          std::string variableName,
                          IType* variableType,
                          llvm::Value* variableValue) const;
};
  
} /* namespace yazyk */

#endif /* FunctionDeclaration_h */
