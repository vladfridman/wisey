#include <map>

/**
 * Library of functions that work with STL data structures
 */

void adjust_wisey_object_reference_count(void* objectPointer, int adjustment);
void destroy_wisey_object(void* objectPointer);

/**
 * Creates a hash map
 */
extern "C" void* stl_reference_to_object_map_create() {
  return new std::map<void*, void*>();
}

extern "C" void* stl_long_to_int_map_create() {
  return new std::map<int64_t, int32_t>();
}

/**
 * Destroys a hash map
 */
extern "C" void stl_reference_to_object_map_destroy(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  delete mapCast;
}

extern "C" void stl_long_to_int_map_destroy(void* map) {
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  delete mapCast;
}

/**
 * Erases map entry with the given key and decrements reference count of the contained object
 */
extern "C" void stl_reference_to_reference_map_erase(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  void* value = mapCast->at(key);
  adjust_wisey_object_reference_count(value, -1);
  mapCast->erase(key);
}

/**
 * Erases map entry with the given key and destroys the contained object
 */
extern "C" void stl_reference_to_owner_map_erase(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  void* value = mapCast->at(key);
  destroy_wisey_object(value);
  mapCast->erase(key);
}

/**
 * Erases map entry with the given key
 */
extern "C" void stl_long_to_int_map_erase(void* map, int64_t key) {
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  if (!mapCast->count(key)) {
    return;
  }
  mapCast->erase(key);
}

/**
 * Puts a key value pair into a hash map where value is a wisey object reference
 */
extern "C" void stl_reference_to_reference_map_put(void* map, void* key, void* value) {
  stl_reference_to_reference_map_erase(map, key);
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  mapCast->insert(std::pair<void*, void*>(key, value));
  adjust_wisey_object_reference_count(value, 1);
}

/**
 * Puts a key value pair into a hash map where value is a wisey object owner
 */
extern "C" void stl_reference_to_owner_map_put(void* map, void* key, void* value) {
  stl_reference_to_owner_map_erase(map, key);
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  mapCast->insert(std::pair<void*, void*>(key, value));
}

/**
 * Puts a key value pair into a long to int hash map
 */
extern "C" void stl_long_to_int_map_put(void* map, int64_t key, int32_t value) {
  stl_long_to_int_map_erase(map, key);
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  mapCast->insert(std::pair<int64_t, int32_t>(key, value));
}

/**
 * Return a value for the given key from a hash map
 */
extern "C" bool stl_long_to_int_map_has(void* map, int64_t key) {
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  return mapCast->count(key);
}

/**
 * Return a value for the given key from a hash map
 */
extern "C" void* stl_reference_to_object_map_get(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (mapCast->count(key)) {
    return mapCast->at(key);
  }
  return NULL;
}

/**
 * Return a value for the given key from a long to int hash map
 */
extern "C" int32_t stl_long_to_int_map_get(void* map, int64_t key) {
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  return mapCast->at(key);
}

/**
 * Return a value for the given key from a hash map, result is cast to ::wisey::object*
 */
extern "C" void* stl_reference_to_owner_map_take(void* map, void* key) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  if (!mapCast->count(key)) {
    return NULL;
  }
  void* result = mapCast->at(key);
  mapCast->erase(key);
  return result;
}

/**
 * Decrement references of objects contained in the hash map
 */
extern "C" void stl_reference_to_reference_map_clear(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  for (std::map<void*, void*>::iterator iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    adjust_wisey_object_reference_count(iterator->second, -1);
  }
  mapCast->clear();
}

/**
 * Destoys objects referenced by owner references in the hash map
 */
extern "C" void stl_reference_to_owner_map_clear(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  for (std::map<void*, void*>::iterator iterator = mapCast->begin(); iterator != mapCast->end(); iterator++) {
    destroy_wisey_object(iterator->second);
  }
  mapCast->clear();
}

/**
 * Clears a long to int hash map
 */
extern "C" void stl_long_to_int_map_clear(void* map) {
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  mapCast->clear();
}

/**
 * Returns reference to reference or reference to owner map size
 */
extern "C" int64_t stl_reference_to_object_map_size(void* map) {
  std::map<void*, void*>* mapCast = (std::map<void*, void*>*) map;
  return mapCast->size();
}

/**
 * Returns long to int map size
 */
extern "C" int64_t stl_long_to_int_map_size(void* map) {
  std::map<int64_t, int32_t>* mapCast = (std::map<int64_t, int32_t>*) map;
  return mapCast->size();
}

void destroy_wisey_object(void* objectPointer) {
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

void adjust_wisey_object_reference_count(void* objectPointer, int adjustment) {
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
    __sync_fetch_and_add(referenceCounter, adjustment);
  } else {
    *referenceCounter = *referenceCounter + adjustment;
  }
}
