//
//  Method.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Method_h
#define Method_h

#include <llvm/IR/DerivedTypes.h>

#include "yazyk/ICallableObjectType.hpp"
#include "yazyk/IType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodArgument.hpp"

namespace yazyk {
  
class CompoundStatement;
class Model;
  
/**
 * Contains information about a method including its return type and all of its arguments
 */
class Method {
  std::string mName;
  IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  unsigned long mIndex;
  CompoundStatement* mCompoundStatement;
  
public:
  
  Method(std::string name,
         IType* returnType,
         std::vector<MethodArgument*> arguments,
         unsigned long index,
         CompoundStatement* compoundStatement) :
  mName(name),
  mReturnType(returnType),
  mArguments(arguments),
  mIndex(index),
  mCompoundStatement(compoundStatement) { }
  
  ~Method() { mArguments.clear(); }
  
  /**
   * Returns the method's name
   */
  std::string getName() const;
  
  /**
   * Returns method's return type
   */
  IType* getReturnType() const;
  
  /**
   * Returns an array of method arguments
   */
  std::vector<MethodArgument*> getArguments() const;
  
  /**
   * Tells whether the two methods are equal in terms of their name, return type and arguments
   */
  bool equals(Method* method) const;
  
  /**
   * Tells index of this method in the container model or interface
   */
  unsigned long getIndex() const;
  
  /**
   * Returns function type corresponding to this method
   */
  llvm::FunctionType* getLLVMFunctionType(IRGenerationContext& context,
                                          ICallableObjectType* callableObject) const;

  /**
   * Generate IR for this method in a given model
   */
  llvm::Function* generateIR(IRGenerationContext& context, Model* model) const;

private:
  
  llvm::Function* createFunction(IRGenerationContext& context, Model* model) const;
  
  void createArguments(IRGenerationContext& context,
                       llvm::Function* function,
                       Model* model) const;
  
  void maybeAddImpliedVoidReturn(IRGenerationContext& context) const;
  
  void storeArgumentValue(IRGenerationContext& context,
                          std::string variableName,
                          IType* variableType,
                          llvm::Value* variableValue) const;
};

} /* namespace yazyk */

#endif /* Method_h */
