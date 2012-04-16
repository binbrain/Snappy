#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <sqlite3.h>

#include "catalog.h"
#include "cardwidget.h"
#include "util.h"

/*
#include "editor.h"
#include "categories.h"
#include "preferences.h"
#include "about.h"
*/

typedef struct _GUIData GUIData;

void    init_gui    (GladeXML *xml, SnappyCatalog *scatalog);

#endif /* _INTERFACE_H */
