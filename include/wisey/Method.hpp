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
#include "wisey/IObjectType.hpp"

namespace wisey {
  
class CompoundStatement;
class Model;
  
/**
 * Contains information about a method including its return type and all of its arguments
 */
class Method : public IMethod {
  const IObjectType* mObjectType;
  std::string mName;
  AccessLevel mAccessLevel;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;
  CompoundStatement* mCompoundStatement;
  llvm::Function* mFunction;
  int mLine;
  
public:
  
  Method(const IObjectType* objectType,
         std::string name,
         AccessLevel accessLevel,
         const IType* returnType,
         std::vector<MethodArgument*> arguments,
         std::vector<const Model*> thrownExceptions,
         CompoundStatement* compoundStatement,
         int line);
  
  ~Method();
  
  bool isStatic() const override;

  llvm::Function* defineFunction(IRGenerationContext& context) override;
  
  void generateIR(IRGenerationContext& context) const override;

  std::string getName() const override;
  
  llvm::FunctionType* getLLVMType(IRGenerationContext& context) const override;

  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  ObjectElementType getObjectElementType() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

private:
  
  void createArguments(IRGenerationContext& context, llvm::Function* function) const;
  
};

} /* namespace wisey */

#endif /* Method_h */
