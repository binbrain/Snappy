#include "preferences.h"

GladeXML *
init_preferences (DBStore *database)
{        
    DBStore *db = (DBStore *)database;    
    GladeXML *xml;
    GtkWidget *w;

    xml = glade_xml_new ("glade/preferences.glade", NULL, NULL);

    /* make signal_connects */

    return xml;
    
}
