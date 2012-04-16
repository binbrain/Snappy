#ifndef _EDITOR_H
#define _EDITOR_H

#include <glade/glade.h>

#include "catalog.h"
#include "category.h"
#include "card.h"
#include "util.h"

typedef struct _CBData CBData;
struct _CBData {
    GladeXML *xml;
    SnappyCatalog *scatalog;
};

GladeXML * init_editor (SnappyCatalog *scatalog);

#endif /* _EDITOR_H */
