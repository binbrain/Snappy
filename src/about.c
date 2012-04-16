#include "about.h"

GladeXML *
init_about ()
{        
    GladeXML *xml;
    GtkWidget *w;

    xml = glade_xml_new ("glade/about.glade", NULL, NULL);

    return xml;
}
