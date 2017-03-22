//
//  Interface.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Interface_hpp
#define Interface_hpp

#include <set>

#include <llvm/IR/Instructions.h>

#include "yazyk/ICallableObjectType.hpp"
#include "yazyk/MethodSignature.hpp"

namespace yazyk {

class MethodSignature;
class Model;
  
/**
 * Contains information about an Interface including the llvm::StructType and method information
 */
class Interface : public ICallableObjectType {
  std::string mName;
  llvm::StructType* mStructType;
  std::vector<Interface*> mParentInterfaces;
  std::vector<MethodSignature*> mMethodSignatures;
  
  /** 
   * includes inherited method signatures 
   */
  std::vector<MethodSignature*> mAllMethodSignatures;
  std::map<std::string, MethodSignature*> mNameToMethodSignatureMap;
  
public:
  
  Interface(std::string name,
            llvm::StructType* structType,
            std::vector<Interface*> parentInterfaces,
            std::vector<MethodSignature*> methodSignatures);
  
  ~Interface();
  
  /**
   * Generate functions that map interface methods to model methods
   */
  std::vector<std::vector<llvm::Constant*>> generateMapFunctionsIR(IRGenerationContext& context,
                                                                   Model* model,
                                                                   std::map<std::string,
                                                                    llvm::Function*>&
                                                                   methodFunctionMap,
                                                                   llvm::GlobalVariable* typeTable,
                                                                   unsigned long interfaceIndex) const;
  
  /**
   * Returns parent interfaces
   */
  std::vector<Interface*> getParentInterfaces() const;
  
  /**
   * Tells whether this interface extends a given one
   */
  bool doesExtendInterface(Interface* interface) const;
  
  /**
   * Given a value of type interface get the pointer back to the original object that implements it
   */
  static llvm::Value* getOriginalObject(IRGenerationContext& context, llvm::Value* value);
  
  MethodSignature* findMethod(std::string methodName) const override;

  std::string getObjectNameGlobalVariableName() const override;
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canCastLosslessTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
  
private:
  
  void prepare();
  
  unsigned long includeInterfaceMethods(Interface* parentInterface,
                                        unsigned long methodIndex);
  
  /**
   * Get all method signatures including the ones inherited from parent interfaces
   */
  std::vector<MethodSignature*> getAllMethodSignatures() const;

  llvm::Function* generateMapFunctionForMethod(IRGenerationContext& context,
                                               Model* model,
                                               llvm::Function* modelFunction,
                                               unsigned long interfaceIndex,
                                               MethodSignature* methodSignature) const;

  void generateMapFunctionBody(IRGenerationContext& context,
                               Model* model,
                               llvm::Function* modelFunction,
                               llvm::Function* mapFunction,
                               unsigned long interfaceIndex,
                               MethodSignature* methodSignature) const;

  llvm::Value* storeArgumentValue(IRGenerationContext& context,
                                  llvm::BasicBlock* basicBlock,
                                  std::string variableName,
                                  IType* variableType,
                                  llvm::Value* variableValue) const;
};
  
} /* namespace yazyk */

#endif /* Interface_hpp */
