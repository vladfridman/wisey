//
//  ExternalStaticMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalStaticMethod_h
#define ExternalStaticMethod_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethod.hpp"

namespace wisey {
  
class Model;
  
/**
 * Contains information about an external static method implemented in a library
 */
class ExternalStaticMethod : public IMethod {
  std::string mName;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;
  
public:
  
  ExternalStaticMethod(std::string name,
                 const IType* returnType,
                 std::vector<MethodArgument*> arguments,
                 std::vector<const Model*> thrownExceptions) :
  mName(name),
  mReturnType(returnType),
  mArguments(arguments),
  mThrownExceptions(thrownExceptions) { }
  
  ~ExternalStaticMethod();
  
  bool isStatic() const override;
  
  llvm::Function* defineFunction(IRGenerationContext& context,
                                 const IObjectType* objectType) override;
  
  void generateIR(IRGenerationContext& context, const IObjectType* objectType) const override;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* ExternalStaticMethod_h */
