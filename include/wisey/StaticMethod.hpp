//
//  StaticMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StaticMethod_h
#define StaticMethod_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethod.hpp"

namespace wisey {

class CompoundStatement;
class Model;

/**
 * Represents concrete object's static method
 */
class StaticMethod : public IMethod {
  std::string mName;
  AccessLevel mAccessLevel;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;
  CompoundStatement* mCompoundStatement;
  llvm::Function* mFunction;
  
public:
  
  StaticMethod(std::string name,
               AccessLevel accessLevel,
               const IType* returnType,
               std::vector<MethodArgument*> arguments,
               std::vector<const Model*> thrownExceptions,
               CompoundStatement* compoundStatement) :
  mName(name),
  mAccessLevel(accessLevel),
  mReturnType(returnType),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions),
  mCompoundStatement(compoundStatement),
  mFunction(NULL) { }
  
  ~StaticMethod();
  
  bool isStatic() const override;

  llvm::Function* defineFunction(IRGenerationContext& context, IObjectType* objectType) override;
  
  void generateIR(IRGenerationContext& context, IObjectType* objectType) const override;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  void printToStream(std::iostream& stream) const override;

private:
  
  void createArguments(IRGenerationContext& context,
                       llvm::Function* function,
                       IObjectType* objectType) const;
  
};
  
} /* namespace wisey */

#endif /* StaticMethod_h */
