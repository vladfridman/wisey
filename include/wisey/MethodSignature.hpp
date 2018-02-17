//
//  MethodSignature.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectElement.hpp"

namespace wisey {
  
class Interface;
  
class MethodSignature : public IMethodDescriptor, public IObjectElement {
  
  const IObjectType* mObjectType;
  std::string mName;
  const IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  std::vector<const Model*> mThrownExceptions;

public:
  
  MethodSignature(const IObjectType* objectType,
                  std::string name,
                  const IType* returnType,
                  std::vector<MethodArgument*> arguments,
                  std::vector<const Model*> thrownExceptions);
  
  ~MethodSignature();
  
  /**
   * Creates a copy of the object with a different index
   */
  MethodSignature* createCopy(const Interface* interface) const;

  bool isStatic() const override;
  
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
  
  const IObjectType* getObjectType() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void allocateVariable(IRGenerationContext& context, std::string name) const override;

};
  
} /* namespace wisey */

#endif /* MethodSignature_h */
