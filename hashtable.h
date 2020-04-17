#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdlib.h>

typedef struct hash_elem {
  struct hash_elem *next;
  struct hash_elem *prev;
  size_t keysz;
  size_t valuesz;
  void *key;
  void *value;
} hash_elem;

typedef struct hash_key_list {
  size_t keysz;
  void *key;
} hash_key_list;

typedef unsigned int (*fn_hash_key)(const unsigned int, const size_t, const void*);
typedef int (*fn_key_cmp)(const size_t, const void *, const size_t, const void *);

typedef struct hash_table {
  unsigned int sz;
  hash_elem **elems;
  fn_hash_key hash_key;
  fn_key_cmp key_cmp;
} hash_table;

hash_table *create_hash_table(const unsigned int sz, fn_hash_key, fn_key_cmp);
void delete_hash_table(hash_table *table);
int exists_in_hash_table(const hash_table *table, const size_t keysz, const void *key);
int get_keys_in_hash_table(const hash_table *table, unsigned int *listsz, hash_key_list **list);
int get_value_from_hash_table(const hash_table *table, const size_t keysz, const void *key, size_t *valuesz, void **value);
int remove_value_from_hash_table(const hash_table *table, size_t keysz, void *key, void **r_key, void **r_value);
int set_value_to_hash_table(const hash_table *table, size_t keysz, void *key, size_t valuesz, void *value, void **r_key, void **r_value);

#endif /* __HASHTABLE_H__ */
