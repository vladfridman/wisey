//
//  ExternalMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalMethod_h
#define ExternalMethod_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethod.hpp"

namespace wisey {
  
class Model;
  
/**
 * Contains information about an external method implemented in a library
 */
class ExternalMethod : public IMethod {
  std::string mName;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;
  
public:
  
  ExternalMethod(std::string name,
                 const IType* returnType,
                 std::vector<MethodArgument*> arguments,
                 std::vector<const Model*> thrownExceptions) :
  mName(name),
  mReturnType(returnType),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions) { }
  
  ~ExternalMethod();
  
  bool isStatic() const override;
  
  llvm::Function* defineFunction(IRGenerationContext& context, IObjectType* objectType) override;
  
  void generateIR(IRGenerationContext& context, IObjectType* objectType) const override;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* ExternalMethod_h */