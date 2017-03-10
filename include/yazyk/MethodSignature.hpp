//
//  MethodSignature.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "yazyk/IMethodDescriptor.hpp"

namespace yazyk {
  
class MethodSignature : public IMethodDescriptor {
  
  std::string mName;
  IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  unsigned long mIndex;
  
public:
  
  MethodSignature(std::string name,
                  IType* returnType,
                  std::vector<MethodArgument*> arguments,
                  unsigned long index) :
  mName(name),
  mReturnType(returnType),
  mArguments(arguments),
  mIndex(index) { }
  
  ~MethodSignature() { mArguments.clear(); }
  
  std::string getName() const override;
  
  IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  unsigned long getIndex() const override;
  
  llvm::FunctionType* getLLVMFunctionType(IRGenerationContext& context,
                                          ICallableObjectType* callableObject) const override;

};
  
} /* namespace yazyk */

#endif /* MethodSignature_h */
