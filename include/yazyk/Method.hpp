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

#include "yazyk/IMethodDescriptor.hpp"

namespace yazyk {
  
class CompoundStatement;
class Model;
  
/**
 * Contains information about a method including its return type and all of its arguments
 */
class Method : public IMethodDescriptor {
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
   * Defines LLVM function for this method
   */
  llvm::Function* defineFunction(IRGenerationContext& context,
                                 IObjectWithMethodsType* objectType) const;
  
  /**
   * Generate IR for this method in a given model or a controller object
   */
  void generateIR(IRGenerationContext& context,
                  llvm::Function* function,
                  IObjectWithMethodsType* objectType) const;

  std::string getName() const override;
  
  IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  unsigned long getIndex() const override;
  
private:
  
  void createArguments(IRGenerationContext& context,
                       llvm::Function* function,
                       IObjectWithMethodsType* objectType) const;
  
  void maybeAddImpliedVoidReturn(IRGenerationContext& context) const;
  
  void storeArgumentValue(IRGenerationContext& context,
                          std::string variableName,
                          IType* variableType,
                          llvm::Value* variableValue) const;
};

} /* namespace yazyk */

#endif /* Method_h */
