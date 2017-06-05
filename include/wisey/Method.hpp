//
//  Method.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Method_h
#define Method_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethodDescriptor.hpp"

namespace wisey {
  
class CompoundStatement;
class Model;
  
/**
 * Contains information about a method including its return type and all of its arguments
 */
class Method : public IMethodDescriptor {
  std::string mName;
  AccessLevel mAccessLevel;
  IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<IType*> mThrownExceptions;
  CompoundStatement* mCompoundStatement;
  unsigned long mIndex;
  llvm::Function* mFunction;
  
public:
  
  Method(std::string name,
         AccessLevel accessLevel,
         IType* returnType,
         std::vector<MethodArgument*> arguments,
         std::vector<IType*> thrownExceptions,
         CompoundStatement* compoundStatement,
         unsigned long index) :
  mName(name),
  mAccessLevel(accessLevel),
  mReturnType(returnType),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions),
  mCompoundStatement(compoundStatement),
  mIndex(index),
  mFunction(NULL) { }
  
  ~Method();
  
  /**
   * Defines LLVM function for this method
   */
  llvm::Function* defineFunction(IRGenerationContext& context, IObjectType* objectType);
  
  /**
   * Generate IR for this method in a given model or a controller object
   */
  void generateIR(IRGenerationContext& context, IObjectType* objectType) const;

  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  unsigned long getIndex() const override;
  
  std::vector<IType*> getThrownExceptions() const override;
  
private:
  
  void createArguments(IRGenerationContext& context,
                       llvm::Function* function,
                       IObjectType* objectType) const;
  
  void maybeAddImpliedVoidReturn(IRGenerationContext& context) const;
  
  void storeArgumentValue(IRGenerationContext& context,
                          std::string variableName,
                          IType* variableType,
                          llvm::Value* variableValue) const;
  
  void checkForUnhandledExceptions(IRGenerationContext& context) const;
};

} /* namespace wisey */

#endif /* Method_h */
