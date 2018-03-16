//
//  ThreadDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadDefinition_h
#define ThreadDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/StateFieldDeclaration.hpp"
#include "wisey/ThreadTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents model definition which is analogous to an immutable class in C++
   */
  class ThreadDefinition : public IConcreteObjectDefinition {
    AccessLevel mAccessLevel;
    ThreadTypeSpecifierFull* mThreadTypeSpecifierFull;
    const ITypeSpecifier* mSendsTypeSpecifier;
    std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    
  public:
    
    ThreadDefinition(AccessLevel accessLevel,
                     ThreadTypeSpecifierFull* threadTypeSpecifierFull,
                     const ITypeSpecifier* sendsTypeSpecifier,
                     std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                     std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                     std::vector<IObjectDefinition*> innerObjectDefinitions);
    
    ~ThreadDefinition();
    
    /**
     * Adds a field that contains the native thread handle and automatically generated methods
     */
    static std::vector<IObjectElementDeclaration*>
    addThreadObjectElements(IRGenerationContext& context,
                            std::vector<IObjectElementDeclaration*> objectElements,
                            const Thread* thread);
    
    Thread* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  private:
    
    static MethodDeclaration* createStartMethodDeclaration(IRGenerationContext& context,
                                                           const Thread* thread);
    
    static MethodDeclaration* createSendMethodDeclaration(IRGenerationContext& context,
                                                          const Thread* thread);
    
    static StateFieldDeclaration* createNativeThreadHandleField(IRGenerationContext& context);
    
    static StateFieldDeclaration* createNativeThreadResultField(IRGenerationContext& context);
    
    static llvm::Function* defineRunBridgeFunction(IRGenerationContext& context,
                                                   const Thread* thread);
    
    static void composeRunBridgeFunction(IRGenerationContext& context,
                                         llvm::Function* function,
                                         const IObjectType* thread);
    
  };
  
} /* namespace wisey */

#endif /* ThreadDefinition_h */

