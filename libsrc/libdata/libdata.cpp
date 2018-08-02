#include <map>

/**
 * Library of functions that work with STL data structures
 */

void adjust_reference_count(void* objectPointer, int adjustment);
void destroy_object(void* objectPointer);

/**
 * Creates a hash map
 */
extern "C" void* stl_map_create() {
  return new std::map<void*, void*>();
}

/**
 * Destroys a hash map
 */
extern "C" void stl_map_destroy(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  delete mapCast;
}

/**
 * Erases map entry with the given key and decrements reference count of the contained object
 */
extern "C" void stl_map_erase_reference(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  void* value = mapCast->at(key);
  adjust_reference_count(value, -1);
  mapCast->erase(key);
}

/**
 * Erases map entry with the given key and destroys the contained object
 */
extern "C" void stl_map_erase_owner(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  void* value = mapCast->at(key);
  destroy_object(value);
  mapCast->erase(key);
}

/**
 * Puts a key value pair into a hash map where value is a wisey object reference
 */
extern "C" void stl_map_put_reference(void* map, void* key, void* value) {
  stl_map_erase_reference(map, key);
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  mapCast->insert(std::pair<void*, void*>(key, value));
  adjust_reference_count(value, 1);
}

/**
 * Puts a key value pair into a hash map where value is a wisey object owner
 */
extern "C" void stl_map_put_owner(void* map, void* key, void* value) {
  stl_map_erase_owner(map, key);
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  mapCast->insert(std::pair<void*, void*>(key, value));
}

/**
 * Return a value for the given key from a hash map
 */
extern "C" void* stl_map_get(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (mapCast->count(key)) {
    return mapCast->at(key);
  }
  return NULL;
}

/**
 * Decrement references of objects contained in the hash map
 */
extern "C" void stl_map_of_references_clear(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  for (std::map<void*, void*>::iterator iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    adjust_reference_count(iterator->second, -1);
  }
  mapCast->clear();
}

/**
 * Destoys objects referenced by owner references in the hash map
 */
extern "C" void stl_map_of_owners_clear(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  for (std::map<void*, void*>::iterator iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    destroy_object(iterator->second);
  }
  mapCast->clear();
}

void destroy_object(void* objectPointer) {
  int8_t** object = (int8_t**) objectPointer;
  int8_t* vTablePortion = *object;
  int64_t* offsetPointer = (int64_t*) vTablePortion;
  int64_t offset = *offsetPointer;
  int8_t* objectStart = ((int8_t*) object) - offset;
  int8_t*** vTable = (int8_t***) objectStart;
  int8_t** firstTable = *vTable;
  int8_t* destructorPointer = firstTable[2];
  void (*destructor) (int8_t*, int8_t*, int8_t*, int8_t*) =
     (void (*) (int8_t*, int8_t*, int8_t*, int8_t*)) destructorPointer;

  destructor(objectStart, NULL, NULL, NULL);
}

void adjust_reference_count(void* objectPointer, int adjustment) {
  int8_t** object = (int8_t**) objectPointer;
  int8_t* vTablePortion = *object;
  int64_t* offsetPointer = (int64_t*) vTablePortion;
  int64_t offset = *offsetPointer;
  int8_t* objectStart = ((int8_t*) object) - offset;
  int8_t* objectShell = objectStart - sizeof(int64_t);
  int64_t* referenceCounter = (int64_t*) objectShell;

  int8_t*** vTable = (int8_t***) objectStart;
  int8_t** firstTable = *vTable;
  int8_t** typeTable = (int8_t**) firstTable[1];
  char* typeShortName = (char*) typeTable[0];
  char firstLetter = typeShortName[0];
  if (firstLetter == 'm' || firstLetter == 't') {
    // TODO: change to safe reference counter adjustment here
    *referenceCounter = *referenceCounter + adjustment;
  } else {
    *referenceCounter = *referenceCounter + adjustment;
  }
}
