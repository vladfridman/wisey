//
//  Node.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Node_h
#define Node_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/Field.hpp"
#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {
  
class Interface;
class NodeOwner;
  
/**
 * Contains information about a node including the llvm::StructType and field information
 */
class Node : public IBuildableConcreteObjectType {
  AccessLevel mAccessLevel;
  std::string mName;
  llvm::StructType* mStructType;
  bool mIsExternal;
  NodeOwner* mNodeOwner;
  std::vector<Field*> mFixedFields;
  std::vector<Field*> mStateFields;
  std::map<std::string, Field*> mFields;
  std::vector<Field*> mFieldsOrdered;
  std::map<Field*, unsigned long> mFieldIndexes;
  std::vector<IMethod*> mMethods;
  std::map<std::string, IMethod*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  std::vector<Constant*> mConstants;
  std::map<std::string, Constant*> mNameToConstantMap;
  std::map<std::string, const IObjectType*> mInnerObjects;
  ImportProfile* mImportProfile;

public:
  
  ~Node();

  /**
   * static method for node instantiation
   */
  static Node* newNode(AccessLevel accessLevel, std::string name, llvm::StructType* structType);

  /**
   * static method for external node instantiation
   */
  static Node* newExternalNode(std::string name, llvm::StructType* structType);

  /**
   * Returns the difference beteween the set of fixed fields and the fields given as argument
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;

  AccessLevel getAccessLevel() const override;

  void setFields(std::vector<Field*> fields, unsigned long startIndex) override;
  
  void setInterfaces(std::vector<Interface*> interfaces) override;
  
  void setMethods(std::vector<IMethod*> methods) override;
  
  void setStructBodyTypes(std::vector<llvm::Type*> types) override;
  
  void setConstants(std::vector<Constant*> constants) override;
  
  std::vector<Constant*> getConstants() const override;

  llvm::Instruction* build(IRGenerationContext& context,
                           const ObjectBuilderArgumentList& objectBuilderArgumentList,
                           int line) const override;
  
  Field* findField(std::string fieldName) const override;
  
  unsigned long getFieldIndex(Field* field) const override;

  std::vector<Field*> getFields() const override;
  
  IMethod* findMethod(std::string methodName) const override;
  
  Constant* findConstant(std::string constantName) const override;

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
                      const IType* toType,
                      int line) const override;
  
  std::string getVTableName() const override;
  
  unsigned long getVTableSize() const override;
  
  std::vector<Interface*> getInterfaces() const override;
  
  std::vector<Interface*> getFlattenedInterfaceHierarchy() const override;
  
  std::string getTypeTableName() const override;
  
  const IObjectOwnerType* getOwner() const override;
  
  bool isExternal() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

  void incremenetReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
  
  void decremenetReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
  
  llvm::Value* getReferenceCount(IRGenerationContext& context, llvm::Value* object) const override;
  
  void setImportProfile(ImportProfile* importProfile) override;
  
  ImportProfile* getImportProfile() const override;
  
  void addInnerObject(const IObjectType* innerObject) override;
  
  const IObjectType* getInnerObject(std::string shortName) const override;

private:
  
  Node(AccessLevel accessLevel, std::string name, llvm::StructType* structType, bool isExternal);

  void checkArguments(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const;

  void checkArgumentsAreWellFormed(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const;

  void checkAllFieldsAreSet(const ObjectBuilderArgumentList& ObjectBuilderArgumentList) const;

  llvm::Instruction* createMalloc(IRGenerationContext& context) const;

  void initializePresetFields(IRGenerationContext& context,
                              const ObjectBuilderArgumentList& ObjectBuilderArgumentList,
                              llvm::Instruction* malloc,
                              int line) const;

  void setStateFieldsToNull(IRGenerationContext& context, llvm::Instruction* malloc) const;

};
  
} /* namespace wisey */

#endif /* Node_h */
