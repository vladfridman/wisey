//
//  Model.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "wisey/Field.hpp"
#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {

class Interface;
class ModelOwner;
  
/**
 * Contains information about a model including the llvm::StructType and field information
 */
class Model : public IBuildableConcreteObjectType {
  std::string mName;
  llvm::StructType* mStructType;
  bool mIsExternal;
  ModelOwner* mModelOwner;
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
  
  ~Model();

  /**
   * static method for model instantiation
   */
  static Model* newModel(std::string name, llvm::StructType* structType);

  /**
   * static method for external model instantiation
   */
  static Model* newExternalModel(std::string name, llvm::StructType* structType);

  /**
   * Gets a set of field names and returns the ones that are missing
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;
  
  /**
   * Returns the name of the global variable containing RTTI type representing this model
   */
  std::string getRTTIVariableName() const;
  
  /**
   * Tells whether this model implements a given interface
   */
  bool doesImplmentInterface(Interface* interface) const;
  
  /**
   * Creates a global variable with type description for this model in RTTI format
   */
  void createRTTI(IRGenerationContext& context) const;
  
  /**
   * Returns the size of this object in bytes
   */
  llvm::Value* getSize(IRGenerationContext& context) const;
  
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
  
  Model(std::string name, llvm::StructType* structType, bool isExternal);
  
  void checkArguments(const ObjectBuilderArgumentList& objectBuilderArgumentList) const;
  
  void checkArgumentsAreWellFormed(const ObjectBuilderArgumentList& objectBuilderArgumentList) const;
  
  void checkAllFieldsAreSet(const ObjectBuilderArgumentList& objectBuilderArgumentList) const;
  
  llvm::Instruction* createMalloc(IRGenerationContext& context) const;

  void initializeFields(IRGenerationContext& context,
                        const ObjectBuilderArgumentList& ObjectBuilderArgumentList,
                        llvm::Instruction* malloc,
                        int line) const;
};

} /* namespace wisey */

#endif /* Model_h */
