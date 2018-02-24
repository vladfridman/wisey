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
  
  StaticMethod(const IObjectType* objectType,
               std::string name,
               AccessLevel accessLevel,
               const IType* returnType,
               std::vector<MethodArgument*> arguments,
               std::vector<const Model*> thrownExceptions,
               CompoundStatement* compoundStatement,
               int line);
  
  ~StaticMethod();
  
  bool isStatic() const override;

  llvm::Function* defineFunction(IRGenerationContext& context) override;
  
  void generateIR(IRGenerationContext& context) const override;
  
  std::string getName() const override;
  
  AccessLevel getAccessLevel() const override;
  
  const IType* getReturnType() const override;
  
  std::vector<MethodArgument*> getArguments() const override;
  
  std::vector<const Model*> getThrownExceptions() const override;
  
  ObjectElementType getObjectElementType() const override;

  std::string getTypeName() const override;
  
  llvm::FunctionType* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;

  bool isOwner() const override;
  
  bool isReference() const override;

  const IObjectType* getObjectType() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void createLocalVariable(IRGenerationContext& context, std::string name) const override;
  
  void createFieldVariable(IRGenerationContext& context,
                           std::string name,
                           const IConcreteObjectType* object) const override;
  
  void createParameterVariable(IRGenerationContext& context,
                               std::string name,
                               llvm::Value* value) const override;

  const ArrayType* getArrayType(IRGenerationContext& context) const override;

private:
  
  void createArguments(IRGenerationContext& context, llvm::Function* function) const;
  
};
  
} /* namespace wisey */

#endif /* StaticMethod_h */
