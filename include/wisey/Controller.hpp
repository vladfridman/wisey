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

#include "wisey/IExpression.hpp"
#include "wisey/IObjectWithVTable.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IObjectWithFieldsType.hpp"
#include "wisey/Method.hpp"

namespace wisey {

/**
 * Contains information about a Controller including its fields and methods
 */
class Controller : public IObjectWithFieldsType, public IObjectWithVTable {
  std::string mName;
  llvm::StructType* mStructType;
  std::vector<Field*> mReceivedFields;
  std::vector<Field*> mInjectedFields;
  std::vector<Field*> mStateFields;
  std::vector<Method*> mMethods;
  std::map<std::string, Field*> mFields;
  std::map<std::string, Method*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  
public:
  
  Controller(std::string name, llvm::StructType* structType)
  : mName(name), mStructType(structType) { }
  
  ~Controller();
  
  /**
   * Set received, injected and state fields to the given lists of fields
   */
  void setFields(std::vector<Field*> receivedFields,
                 std::vector<Field*> injectedFields,
                 std::vector<Field*> stateFields);

  /**
   * Set interfaces for this controller
   */
  void setInterfaces(std::vector<Interface*> interfaces);
  
  /**
   * Set methods for this controller
   */
  void setMethods(std::vector<Method*> methods);
  
  /**
   * Set the struct field types for the struct that represents this controller
   */
  void setStructBodyTypes(std::vector<llvm::Type*> types);

  /**
   * Inject an instance of this controller into LLVM code
   */
  llvm::Instruction* inject(IRGenerationContext& context, ExpressionList expressionList) const;
  
  Field* findField(std::string fieldName) const override;
  
  Method* findMethod(std::string methodName) const override;
  
  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  std::string getShortName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canAutoCastTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;

  std::string getVTableName() const override;
  
  unsigned long getVTableSize() const override;
  
  std::vector<Interface*> getInterfaces() const override;
  
  std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
  
  std::string getTypeTableName() const override;
 
private:

  int getInterfaceIndex(Interface* interface) const;

  void checkArguments(ExpressionList received) const;
  
  void addInterfaceAndItsParents(std::vector<Interface*>& result, Interface* interface) const;

  llvm::Instruction* createMalloc(IRGenerationContext& context) const;
  
  void initializeVTable(IRGenerationContext& context, llvm::Instruction* malloc) const;
  
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
