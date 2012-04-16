#ifndef _CATALOG_H
#define _CATALOG_H

#include "cardstore.h"
#include "category.h"

typedef struct _SnappyCatalog SnappyCatalog;

struct _SnappyCatalog {
    /* list of category objects */
    GList *categories;
    /* id of currently loaded category */
    gint loaded;
    DBStore db;
};

void                snappy_catalog_init (SnappyCatalog *scatalog);
void                snappy_catalog_dump_categories (SnappyCatalog *scatalog);

void                snappy_catalog_append_category (SnappyCatalog *scatalog, const gchar *name);
void                snappy_catalog_remove_category (SnappyCatalog *scatalog, gint id);
void                snappy_catalog_rename_category (SnappyCatalog *scatalog, gint id, const gchar *name);
void				snappy_catalog_set_loaded (SnappyCatalog *scatalog, gint id);
SnappyCategory *    snappy_catalog_get_category (SnappyCatalog *scatalog, gint id);

#endif /* _CATALOG_H */
