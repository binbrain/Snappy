#include "categories.h"

enum
{
    COL_NAME = 0
    ,NUM_COLS
};

static void
categories_add_rows (gpointer data, gpointer user_data)
{
    SnappyCategory *scategory = (SnappyCategory *)data;
    GtkListStore *store = (GtkListStore *)user_data;    
    GtkTreeIter iter;

    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter
                       ,COL_NAME, g_strdup (scategory->name)
                       ,-1);
}

static GtkTreeModel *
categories_create_model (SnappyCatalog *scatalog)
{
    GtkListStore *store;
    
    store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING);
    g_list_foreach (scatalog->categories, categories_add_rows, store);    
    
    return GTK_TREE_MODEL (store);
}    

static GtkWidget *
categories_create_table (GtkWidget *view, SnappyCatalog *scatalog)
{
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkTreeModel *model;

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view)
                                                ,-1
                                                ,"Name"
                                                ,renderer
                                                ,"text", COL_NAME
                                                ,NULL);
    model = categories_create_model (scatalog);
    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
    g_object_unref (model);

    return view;
}

void
categories_add_button_clicked (GtkWidget *w, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;
    GtkWidget *add_category;

    add_category = glade_xml_get_widget (xml, "category-add");
    gtk_widget_show (add_category);
}

void
categories_finish_button_clicked (GtkWidget *button, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;
    GtkWidget *w;

    w = glade_xml_get_widget (xml, "categories-dialog");
    gtk_widget_hide (w);
}
            
void
categories_edit_button_clicked (GtkWidget *button, gpointer user_data)
{
}

void
category_ok_button_clicked (GtkWidget *ok_button, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *w;
    DBStore db;
    const gchar *category;
    gint err;
    gint result;

    w = glade_xml_get_widget (xml, "category-name-entry");
    category = gtk_entry_get_text (GTK_ENTRY (w));

    err = cardstore_open (&db);
    if (err)
    {
        exit (1);
    }

    cardstore_new_category (&db, g_strdup (category));
    
    /* repopulate the categories table */
    w = glade_xml_get_widget (xml, "categories-treeview");
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (w));
    
    if (g_utf8_collate (category, "Not Catagorized"))
    {
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter
                           ,COL_NAME, g_strdup (category)
                           ,-1);
    }
    cardstore_close (&db);
    w = glade_xml_get_widget (xml, "category-add");
    gtk_widget_hide (w);
}

void
categories_delete_button_clicked (GtkWidget *button, gpointer user_data)
{
}

void
category_cancel_button_clicked (GtkWidget *button, gpointer user_data)
{
}

GladeXML *
init_categories (SnappyCatalog *scatalog)
{        
    GladeXML *xml;
    GtkWidget *w;
    
    xml = glade_xml_new ("glade/categories.glade", NULL, NULL);

    /* populate the categories table */
    w = glade_xml_get_widget (xml, "categories-treeview");
    categories_create_table (w, scatalog);

    /* make signal connects for categories */
    w = glade_xml_get_widget (xml, "categories-add-button");
    g_signal_connect (w, "clicked", G_CALLBACK (categories_add_button_clicked), xml);

    w = glade_xml_get_widget (xml, "categories-edit-button");
    g_signal_connect (w, "clicked", G_CALLBACK (categories_edit_button_clicked), xml);
    
    w = glade_xml_get_widget (xml, "categories-delete-button");
    g_signal_connect (w, "clicked", G_CALLBACK (categories_delete_button_clicked), xml);

    w = glade_xml_get_widget (xml, "categories-finish-button");
    g_signal_connect (w, "clicked", G_CALLBACK (categories_finish_button_clicked), xml);
    
    /* make signal connects for category */
    w = glade_xml_get_widget (xml, "category-ok-button");
    g_signal_connect (w, "clicked", G_CALLBACK (category_ok_button_clicked), xml);

    return xml;
}
