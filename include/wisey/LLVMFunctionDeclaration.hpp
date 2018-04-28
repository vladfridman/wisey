//
//  LLVMFunctionDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionDeclaration_h
#define LLVMFunctionDeclaration_h

#include "wisey/IGlobalStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMStructType.hpp"

namespace wisey {
  
  /**
   * Declaration of an llvm function
   */
  class LLVMFunctionDeclaration : public IGlobalStatement {
    
    std::string mName;
    bool mIsExternal;
    bool mIsVarArg;
    const ITypeSpecifier* mReturnSpecifier;
    std::vector<const ITypeSpecifier*> mArgumentSpecifiers;
    
    LLVMFunctionDeclaration(std::string name,
                            bool isExternal,
                            bool isVarArg,
                            const ITypeSpecifier* returnSpecifier,
                            std::vector<const ITypeSpecifier*> argumentSpecifiers);
    
  public:
    
    /**
     * Creates an instance of LLVMFunctionDeclaration that represents an internal llvm function
     */
    static LLVMFunctionDeclaration* createInternal(std::string name,
                                                   const ITypeSpecifier* returnSpecifier,
                                                   std::vector<const ITypeSpecifier*>
                                                   argumentSpecifiers);
    
    /**
     * Creates an internal llvm function with variable arguments
     */
    static LLVMFunctionDeclaration* createInternalWithVarArg(std::string name,
                                                             const ITypeSpecifier* returnSpecifier,
                                                             std::vector<const ITypeSpecifier*>
                                                             argumentSpecifiers);

    /**
     * Creates an instance of LLVMFunctionDeclaration that represents an external llvm function
     */
    static LLVMFunctionDeclaration* createExternal(std::string name,
                                                   const ITypeSpecifier* returnSpecifier,
                                                   std::vector<const ITypeSpecifier*>
                                                   argumentSpecifiers);
    
    /**
     * Creates an external llvm function with variable arguments
     */
    static LLVMFunctionDeclaration* createExternalWithVarArg(std::string name,
                                                             const ITypeSpecifier* returnSpecifier,
                                                             std::vector<const ITypeSpecifier*>
                                                             argumentSpecifiers);

    ~LLVMFunctionDeclaration();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionDeclaration_h */
