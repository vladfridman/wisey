//
//  Controller.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include <llvm/IR/Instructions.h>

#include "yazyk/Interface.hpp"
#include "yazyk/IObjectWithFieldsType.hpp"
#include "yazyk/IObjectWithMethodsType.hpp"
#include "yazyk/Method.hpp"

namespace yazyk {

/**
 * Contains information about a Controller including its fields and methods
 */
class Controller : public IObjectWithFieldsType, public IObjectWithMethodsType {
  std::string mName;
  llvm::StructType* mStructType;
  std::map<std::string, Field*> mFields;
  std::vector<Method*> mMethods;
  std::map<std::string, Method*> mNameToMethodMap;
  std::vector<Interface*> mInterfaces;
  
public:
  
  Controller(std::string name,
             llvm::StructType* structType,
             std::map<std::string, Field*> fields,
             std::vector<Method*> methods,
             std::vector<Interface*> interfaces);
  
  ~Controller();

  /**
   * Returns interfaces that this model implements
   */
  std::vector<Interface*> getInterfaces() const;

  
  Field* findField(std::string fieldName) const override;
  
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
};
  
} /* namespace yazyk */

#endif /* Controller_h */
