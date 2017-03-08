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

#include "yazyk/IType.hpp"
#include "yazyk/Method.hpp"
#include "yazyk/Model.hpp"

namespace yazyk {
  
/**
 * Contains information about an Interface including the llvm::StructType and method information
 */
class Interface : public IType {
  std::string mName;
  llvm::StructType* mStructType;
  std::vector<Method*> mMethods;
  std::map<std::string, Method*> mNameToMethodMap;
  
public:
  
  Interface(std::string name,
            llvm::StructType* structType,
            std::vector<Method*> methods);
  
  ~Interface();
  
  /**
   * Finds a method with a given name
   */
  Method* findMethod(std::string methodName) const;
  
  /**
   * Generate functions that map interface methods to model methods
   */
  std::vector<llvm::Constant*> generateMapFunctionsIR(IRGenerationContext& context,
                                                      Model* model,
                                                      std::map<std::string, llvm::Function*>&
                                                        methodFunctionMap,
                                                      int interfaceIndex) const;
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canCastLosslessTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
  
private:
  
  llvm::Function* generateMapFunctionForMethod(IRGenerationContext& context,
                                               Model* model,
                                               llvm::Function* modelFunction,
                                               int interfaceIndex,
                                               Method* method) const;

  void generateMapFunctionBody(IRGenerationContext& context,
                               Model* model,
                               llvm::Function* modelFunction,
                               llvm::Function* mapFunction,
                               int interfaceIndex,
                               Method* method) const;

  llvm::Value* storeArgumentValue(IRGenerationContext& context,
                                  llvm::BasicBlock* basicBlock,
                                  std::string variableName,
                                  IType* variableType,
                                  llvm::Value* variableValue) const;
};
  
} /* namespace yazyk */

#endif /* Interface_hpp */
