#include "hashtable.h"
#include <string.h>

static unsigned int _default_hash_key(const unsigned int table_sz, const size_t keysz, const void *key) {
  unsigned int i;
  unsigned int r = 0;
  unsigned char *v = (unsigned char *)key;
  for(i = 0; i < keysz; i++) {
    r += v[i];
  }
  return(r % table_sz);
}

static int _default_key_cmp(const size_t a_sz, const void *a, const size_t b_sz, const void *b) {
  if(a_sz > b_sz) {
    return(1);
  } else if(a_sz < b_sz) {
    return(-1);
  } else {
    return(memcmp(a, b, a_sz));
  }
}

hash_table *create_hash_table(const unsigned int sz, fn_hash_key hash_key_fn, fn_key_cmp key_cmp_fn) {
  hash_table *h = malloc(sizeof(hash_table));
  if(h) {
    hash_elem **s;
    h->sz = sz;
    s = calloc(sz, sizeof(hash_elem *));
    if(s) {
      h->elems = s;
      h->hash_key = (hash_key_fn) ? hash_key_fn : (fn_hash_key)&_default_hash_key;
      h->key_cmp = (key_cmp_fn) ? key_cmp_fn : (fn_key_cmp)&_default_key_cmp;
    } else {
      free(h);
      h = NULL;
    }
  }
  return(h);
}

void delete_hash_table(hash_table *table) {
  int i;
  for(i = 0; i < table->sz; i++) {
    if(table->elems[i]) {
      hash_elem *c = table->elems[i];
      do {
        hash_elem *n = c->next;
        free(c);
        c = n;
      } while(table->elems[i] != c);
    }
    table->elems[i] = NULL;
  }
  free(table->elems);
  table->elems = NULL;
  free(table);
}

int get_value_from_hash_table(const hash_table *table, const size_t keysz, const void *key, size_t *valuesz, void **value) {
  unsigned int idx;
  hash_elem *e;
  idx = (*(table->hash_key))(table->sz, keysz, key);
  e = table->elems[idx];
  if(e) {
    hash_elem *c = e;
    do {
      if((*(table->key_cmp))(c->keysz, c->key, keysz, key) == 0) {
        if(valuesz) {
          *valuesz = c->valuesz;
        }
        if(value) {
          *value = c->value;
        }
        return(1);
      }
      c = c->next;
    } while(c != e);
  }
  return(0);
}

int exists_in_hash_table(const hash_table *table, const size_t keysz, const void *key) {
  return(get_value_from_hash_table(table, keysz, key, NULL, NULL));
}

int get_keys_in_hash_table(const hash_table *table, unsigned int *listsz, hash_key_list **list) {
  int i;
  hash_elem *e, *c;
  unsigned int r = 0;
  if(listsz && list) {
    hash_key_list *keys = *list;
    r = 0;
    for(i = 0; i < table->sz; i++) {
      e = table->elems[i];
      if(e) {
        c = e;
        do {
          keys[r].keysz = c->keysz;
          keys[r].key = c->key;
          r++;
          c = c->next;
        } while(c != e);
      }
    }
    *listsz = r;
  } else if(listsz) {
    for(i = 0; i < table->sz; i++) {
      e = table->elems[i];
      if(e) {
        c = e;
        do {
          r++;
          c = c->next;
        } while(c != e);
      }
    }
    *listsz = r;
  } else if(list) {
    hash_key_list *keys = *list;
    r = 0;
    for(i = 0; i < table->sz; i++) {
      e = table->elems[i];
      if(e) {
        c = e;
        do {
          keys[r].keysz = c->keysz;
          keys[r].key = c->key;
          r++;
          c = c->next;
        } while(c != e);
      }
    }
  }
  return(1);
}

int remove_value_from_hash_table(const hash_table *table, size_t keysz, void *key, void **r_key, void **r_value) {
  unsigned int idx;
  hash_elem *e;
  if(r_key) {
    *r_key = NULL;
  }
  if(r_value) {
    *r_value = NULL;
  }
  idx = (*(table->hash_key))(table->sz, keysz, key);
  e = table->elems[idx];
  if(e) {
    hash_elem *p, *c, *n;
    c = e;
    do {
      n = c->next;
      if((*(table->key_cmp))(c->keysz, c->key, keysz, key) == 0) {
        p = c->prev;
        if((c == n) && (c == p)) {
          /* length == 1 */
          table->elems[idx] = NULL;
        } else {
          /* length > 2 */
          n->prev = p;
          p->next = n;
          if(c == e) {
            table->elems[idx] = n;
          }
        }
        if(r_key) {
          *r_key = c->key;
        }
        if(r_value) {
          *r_value = c->value;
        }
        free(c);
        return(1);
      }
      c = n;
    } while(c != e);
  }
  return(0);
}

int set_value_to_hash_table(const hash_table *table, size_t keysz, void *key, size_t valuesz, void *value, void **r_key, void **r_value) {
  unsigned int idx;
  hash_elem *e, *v;
  idx = (*(table->hash_key))(table->sz, keysz, key);
  e = table->elems[idx];
  v = malloc(sizeof(hash_elem));
  v->keysz = keysz;
  v->valuesz = valuesz;
  v->key = key;
  v->value = value;
  if(r_key) {
    *r_key = NULL;
  }
  if(r_value) {
    *r_value = NULL;
  }
  if(e) {
    hash_elem *p, *c, *n;
    c = e;
    do {
      n = c->next;
      if((*(table->key_cmp))(c->keysz, c->key, keysz, key) == 0) {
        if(r_key) {
          *r_key = c->key;
        }
        if(r_value) {
          *r_value = c->value;
        }
        c->keysz = keysz;
        c->key = key;
        c->valuesz = valuesz;
        c->value = value;
        free(v);
        return(2);
      }
      c = n;
    } while(c != e);
    if((e == e->next) && (e == e->prev)) {
      e->next = v;
      v->next = e;
      e->prev = v;
      v->prev = e;
    } else {
      p = e->prev;
      p->next = v;
      v->next = e;
      e->prev = v;
      v->prev = p;
    }
  } else {
    v->prev = v;
    v->next = v;
    table->elems[idx] = v;
  }
  return(1);
}
