//
//  MethodDeclaration.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef MethodDeclaration_h
#define MethodDeclaration_h

#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/CompoundStatement.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IStatement.hpp"
#include "yazyk/ITypeSpecifier.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/VariableDeclaration.hpp"

namespace yazyk {

/**
 * Represents a model's method declaration.
 *
 * A method contains a coumpound statement that is the body of the method.
 */
class MethodDeclaration {
  const AccessSpecifier mAccessSpecifier;
  const ITypeSpecifier& mReturnTypeSpecifier;
  std::string mMethodName;
  VariableList mArguments;
  CompoundStatement& mCompoundStatement;
  
public:
  
  MethodDeclaration(const AccessSpecifier& accessSpecifier,
                    const ITypeSpecifier& returnTypeSpecifier,
                    std::string methodName,
                    const VariableList& arguments,
                    CompoundStatement& compoundStatement) :
  mAccessSpecifier(accessSpecifier),
  mReturnTypeSpecifier(returnTypeSpecifier),
  mMethodName(methodName),
  mArguments(arguments),
  mCompoundStatement(compoundStatement) { }
  
  llvm::Value* generateIR(IRGenerationContext& context, Model* model = NULL) const;

  /**
   * Returns object representing the method for storage in a model object
   */
  Method* getMethod(IRGenerationContext& context) const;
  
private:
  
  llvm::Function* createFunction(IRGenerationContext& context, Model* model) const;
  
  void createArguments(IRGenerationContext& context,
                       llvm::Function* function,
                       Model* model) const;
  
  void addImpliedVoidReturn(IRGenerationContext& context) const;
  
  void storeArgumentValue(IRGenerationContext& context,
                          std::string variableName,
                          IType* variableType,
                          llvm::Value* variableValue) const;

};

} /* namespace yazyk */

#endif /* MethodDeclaration_h */
