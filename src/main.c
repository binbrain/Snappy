#include <sqlite3.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "gui.h"
#include "card.h"
#include "category.h"
#include "catalog.h"

gint main (int argc, char *argv[])
{
    SnappyCatalog scatalog;
    SnappyCategory *scategory;
    SnappyCard *scard;
    GladeXML *xml;
    gint result;
    
    gtk_init (&argc, &argv);
    glade_init ();
    xml = glade_xml_new ("glade/snappy.glade", NULL, NULL);
    
    snappy_catalog_init (&scatalog);
    init_gui (xml, &scatalog);
    
    g_type_init ();
    // list categories
    gtk_main ();

    return 0;
}
