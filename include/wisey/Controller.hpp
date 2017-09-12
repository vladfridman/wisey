//
//  Controller.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include <llvm/IR/Instructions.h>

#include "wisey/FieldInjected.hpp"
#include "wisey/FieldReceived.hpp"
#include "wisey/FieldState.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/Interface.hpp"

namespace wisey {

class ControllerOwner;
  
/**
 * Contains information about a Controller including its fields and methods
 */
class Controller : public IConcreteObjectType {
  std::string mName;
  ControllerOwner* mControllerOwner;
  llvm::StructType* mStructType;
  std::vector<FieldReceived*> mReceivedFields;
  std::vector<FieldInjected*> mInjectedFields;
  std::vector<FieldState*> mStateFields;
  std::vector<IMethod*> mMethods;
  std::map<std::string, IField*> mFields;
  std::map<std::string, IMethod*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  
public:
  
  Controller(std::string name, llvm::StructType* structType);
  
  ~Controller();
  
  /**
   * Set received, injected and state fields to the given lists of fields
   */
  void setFields(std::vector<FieldReceived*> receivedFields,
                 std::vector<FieldInjected*> injectedFields,
                 std::vector<FieldState*> stateFields);

  /**
   * Set interfaces for this controller
   */
  void setInterfaces(std::vector<Interface*> interfaces);
  
  /**
   * Set methods for this controller
   */
  void setMethods(std::vector<IMethod*> methods);
  
  /**
   * Set the struct field types for the struct that represents this controller
   */
  void setStructBodyTypes(std::vector<llvm::Type*> types);

  /**
   * Inject an instance of this controller into LLVM code
   */
  llvm::Instruction* inject(IRGenerationContext& context, ExpressionList expressionList) const;
  
  IField* findField(std::string fieldName) const override;
  
  std::map<std::string, IField*> getFields() const override;
  
  IMethod* findMethod(std::string methodName) const override;
  
  std::vector<IMethod*> getMethods() const override;
  
  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  std::string getShortName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;

  std::string getVTableName() const override;

  unsigned long getVTableSize() const override;
  
  std::vector<Interface*> getInterfaces() const override;
  
  std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
  
  std::string getTypeTableName() const override;
  
  const IObjectOwnerType* getOwner() const override;
 
private:

  void checkArguments(ExpressionList received) const;
  
  void addInterfaceAndItsParents(std::vector<Interface*>& result, Interface* interface) const;

  llvm::Instruction* createMalloc(IRGenerationContext& context) const;
  
  void initializeReceivedFields(IRGenerationContext& context,
                                ExpressionList& controllerInjectorArguments,
                                llvm::Instruction* malloc) const;
  
  void initializeInjectedFields(IRGenerationContext& context,
                                llvm::Instruction* malloc) const;
  
  void initializeStateFields(IRGenerationContext& context,
                             llvm::Instruction* malloc) const;
  
};
  
} /* namespace wisey */

#endif /* Controller_h */
