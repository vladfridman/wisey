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
#include "wisey/IMethod.hpp"

namespace wisey {
  
class CompoundStatement;
class Model;
  
/**
 * Contains information about a method including its return type and all of its arguments
 */
class Method : public IMethod {
  std::string mName;
  AccessLevel mAccessLevel;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;
  CompoundStatement* mCompoundStatement;
  llvm::Function* mFunction;
  
public:
  
  Method(std::string name,
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
  
  ~Method();
  
  bool isStatic() const override;
  
  llvm::Function* defineFunction(IRGenerationContext& context,
                                 const IObjectType* objectType) override;
  
  void generateIR(IRGenerationContext& context, const IObjectType* objectType) const override;

  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  ObjectElementType getObjectElementType() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  void createArguments(IRGenerationContext& context,
                       llvm::Function* function,
                       const IObjectType* objectType) const;
  
};

} /* namespace wisey */

#endif /* Method_h */
