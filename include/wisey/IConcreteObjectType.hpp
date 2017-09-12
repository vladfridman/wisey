//
//  IConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectType_h
#define IConcreteObjectType_h

#include <llvm/IR/Instructions.h>

#include "wisey/IField.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
class Interface;
  
/**
 * Interface representing a object that has a vTable: controller, node or a model
 *
 * Only objects implementing at least one interface have a vTable. Objects that do not implement
 * any interfaces do not use a vTable, methods are called directly.
 *
 * vTable is an array of arrays of i8* pointers:
 * [
 *   [vTable portion for the concrete object and first implemented interface],
 *   [vTable portion for the second implemented interface],
 *   [vTable portion for the third implemented interface],
 *   ...
 * ]
 *
 * vTable portion for the concrete object  and first implemented interface consists of
 * [
 *   i8* null,
 *   i8* <pointer to type table array>,
 *   i8* <pointer to destructor>,
 *   i8* <pointer to first interface method that is a concrete object's method>,
 *   i8* <pointer to second interface method that is a concrete object's method>,
 *   ...
 * ]
 *
 * vTable portion for an implemented interface consists of
 * [
 *   i8* <number of bytes needed to add to the interface object pointer to get the concrete object>
 *   i8* null,
 *   i8* null,
 *   i8* <pointer to first interface method that maps to concrete object method>,
 *   i8* <pointer to second interface method that maps to concrete object method>,
 *   ...
 * ]
 *
 * Typetable for the concrete object consists of array of pointers to type names:
 * [
 *   i8* <concrete object type name>,
 *   i8* <first interface type name>,
 *   i8* <second interface type name>,
 *   ...,
 *   i8* null
 * ]
 *
 */
class IConcreteObjectType : public IObjectType {
    
public:
  
  virtual ~IConcreteObjectType() { }

  /**
   * Looks for a field with a given name in the object
   */
  virtual IField* findField(std::string fieldName) const = 0;

  /**
   * Returns a map of all fields keyed by their names
   */
  virtual std::map<std::string, IField*> getFields() const = 0;
  
  /**
   * Returns the name of the vTable global varaible
   */
  virtual std::string getVTableName() const = 0;

  /**
   * Returns the size of the VTable array for this object
   */
  virtual unsigned long getVTableSize() const = 0;
  
  /**
   * Returns interfaces that this object implements
   */
  virtual std::vector<Interface*> getInterfaces() const = 0;

  /**
   * Returns a list of all interfaces this model implements including inherited interfaces
   */
  virtual std::vector<Interface*> getFlattenedInterfaceHierarchy() const = 0;

  /**
   * Returns the name of the global varaible containing types that this model implements
   */
  virtual std::string getTypeTableName() const = 0;
  
  /**
   * Finds a method with a given name.
   */
  virtual IMethod* findMethod(std::string methodName) const = 0;
  
  /**
   * Get list of all methods
   */
  virtual std::vector<IMethod*> getMethods() const = 0;

  /**
   * Generate global variable with the name of the given object
   */
  static void generateNameGlobal(IRGenerationContext& context, IConcreteObjectType* object);
  
  /**
   * Casts this concrete object to a given type
   */
  static llvm::Value* castTo(IRGenerationContext& context,
                             IConcreteObjectType* object,
                             llvm::Value* fromValue,
                             const IType* toType);

  /**
   * Returns the interface index in the list of flattened interface hierarchy
   */
  static int getInterfaceIndex(IConcreteObjectType* object, Interface* interface);
  
  /**
   * Initialize vTable of a newly created concrete object
   */
  static void initializeVTable(IRGenerationContext& context,
                               IConcreteObjectType* object,
                               llvm::Instruction* malloc);
  
  /**
   * Generate vTable global variable for the given object
   */
  static void generateVTable(IRGenerationContext& context, IConcreteObjectType* object);
  
  /**
   * Declare all fields as variables in the current scope
   */
  static void declareFieldVariables(IRGenerationContext& context, IConcreteObjectType* object);
  
  /**
   * Composes destructor function code
   */
  static void composeDestructorBody(IRGenerationContext& context, IConcreteObjectType* object);
  
  /**
   * Compose a call to destroy a given concrete object
   */
  static void composeDestructorCall(IRGenerationContext& context,
                                    IConcreteObjectType* objectOwnerType,
                                    llvm::Value* object);

private:
  
  static std::map<std::string, llvm::Function*>
  generateMethodFunctions(IRGenerationContext& context, IConcreteObjectType* object);  

  static void addTypeListInfo(IRGenerationContext& context,
                              IConcreteObjectType* object,
                              std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void addUnthunkInfo(IRGenerationContext& context,
                             IConcreteObjectType* object,
                             std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void generateInterfaceMapFunctions(IRGenerationContext& context,
                                            IConcreteObjectType* object,
                                            std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void createVTableGlobal(IRGenerationContext& context,
                                 IConcreteObjectType* object,
                                 std::vector<std::vector<llvm::Constant*>> interfaceVTables);
  
  static llvm::GlobalVariable* createTypeListGlobal(IRGenerationContext& context,
                                                    IConcreteObjectType* object);
  
  static void addDestructorInfo(IRGenerationContext& context,
                                IConcreteObjectType* objet,
                                std::vector<std::vector<llvm::Constant*>>& vTables);

  static std::string getObjectDestructorFunctionName(IConcreteObjectType* object);
  
};
  
} /* namespace wisey */

#endif /* IConcreteObjectType_h */
