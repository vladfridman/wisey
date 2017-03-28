//
//  Model.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <set>

#include <llvm/IR/Instructions.h>

#include "yazyk/IObjectWithFieldsType.hpp"
#include "yazyk/IObjectWithMethodsType.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/ModelField.hpp"

namespace yazyk {

class Interface;
  
/**
 * Contains information about a MODEL including the llvm::StructType and field information
 */
class Model : public IObjectWithFieldsType, public IObjectWithMethodsType {
  std::string mName;
  llvm::StructType* mStructType;
  std::map<std::string, ModelField*> mFields;
  std::vector<Method*> mMethods;
  std::map<std::string, Method*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  std::vector<Interface*> mFlattenedInterfaceHierarchy;
  
public:
  
  Model(std::string name,
        llvm::StructType* structType,
        std::map<std::string, ModelField*> fields,
        std::vector<Method*> methods,
        std::vector<Interface*> interfaces);
  
  ~Model();
  
  /**
   * Gets a set of field names and returns the ones that are missing
   */
  std::vector<std::string> getMissingFields(std::set<std::string> givenFields) const;
  
  /**
   * Returns the name of the vTable global varaible
   */
  std::string getVTableName() const;

  /**
   * Returns the name of the global varaible containing types that this model implements
   */
  std::string getTypeTableName() const;
 
  /**
   * Returns interfaces that this model implements
   */
  std::vector<Interface*> getInterfaces() const;
  
  /**
   * Returns a list of all interfaces this model implements including inherited interfaces
   */
  std::vector<Interface*> getFlattenedInterfaceHierarchy() const;
  
  /**
   * Tells whether this model implements a given interface
   */
  bool doesImplmentInterface(Interface* interface) const;
  
  ModelField* findField(std::string fieldName) const override;
  
  Method* findMethod(std::string methodName) const override;

  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canAutoCastTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
  
private:
  
  int getInterfaceIndex(Interface* interface) const;
  
  std::vector<Interface*> createFlattenedInterfaceHierarchy() const;
  
  void addInterfaceAndItsParents(std::vector<Interface*>& result, Interface* interface) const;
  
};

} /* namespace yazyk */

#endif /* Model_h */
