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
    snappy_catalog_dump_categories (&scatalog);

    // create a new category
    //snappy_catalog_append_category (&scatalog, "Russian");
        
    //scategory = snappy_catalog_get_category (&scatalog, 0);

    // add new card to any category
    //snappy_category_append_card (scategory, "brace", "yourself");

    // change card 1 front
    // the cards must be loaded to edit it, be careful with this function 
    //snappy_category_edit_card (scategory, "reyourself", 1, 2);

    // list cards

    gtk_main ();

    return 0;
}
