#include "editor.h"

static void
hide_editor_dialog (gpointer data, gpointer user_data)
{
    CBData *cbdata = (CBData *)user_data;
    GladeXML *xml = (GladeXML *)cbdata->xml;
    GtkWidget *w;
    //GtkTreeModel *model;

    w = glade_xml_get_widget (xml, "editor-dialog");
    gtk_widget_hide (w);

    //w = glade_xml_get_widget (xml, "editor-card-view");
    //model = GTK_TREE_MODEL (gtk_tree_view_get_model (GTK_TREE_VIEW (w)));
    //g_object_unref (w);

    g_free (cbdata);
}

enum 
{
    COL_NUM = 0
    ,COL_FRONT
    ,COL_BACK
    ,NUM_COLS
};

static void
cards_add_rows (gpointer data, gpointer user_data)
{
    GtkListStore *store = (GtkListStore *)user_data;
    GtkTreeIter iter;
    SnappyCard *scard = (SnappyCard *)data;
    gint id;
    gchar *back, *front;

    g_object_get (scard, "id", &id, "front", &front, "back", &back, NULL);
    g_print ("cards_add_rows\n");
        
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter
                       ,COL_NUM, id
                       ,COL_FRONT, g_strdup (front)
                       ,COL_BACK, g_strdup (back)
                       ,-1);
}

static GtkTreeModel *
editor_create_model (GList *cards)
{
    GtkListStore *store;
    GtkTreeIter iter;
    
    store = gtk_list_store_new (NUM_COLS, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING);
    g_list_foreach (cards, cards_add_rows, store);

    return GTK_TREE_MODEL (store);
}

static void
cell_edited (GtkCellRendererText *cell, gchar *arg1, gchar *arg2, gpointer user_data)
{
    SnappyCard *scard;
    CBData *cbdata = (CBData *)user_data;
    SnappyCatalog *scatalog = cbdata->scatalog;
    SnappyCategory *scategory;
    GladeXML *xml = cbdata->xml;
    GtkTreeModel *model;
    GtkWidget *w;
    GList *cards;
    
    xml = cbdata->xml; 
    scategory = snappy_catalog_get_category (scatalog, scatalog->loaded);
  
    scard = snappy_category_get_card (scategory, atoi (arg1));
    snappy_card_set_text (scard, g_strdup (arg2), 0);
     
    cards = scategory->cards;
    w = glade_xml_get_widget (xml, "editor-cards-treeview");
    model = editor_create_model (cards);
    gtk_tree_view_set_model (GTK_TREE_VIEW (w), model);
    
    g_object_unref (model);
}

/*
 * Creates the view, and makes the call
 * to create and pack the model
 */
static void
editor_create_table_view (CBData *user_data)
{
    SnappyCatalog *scatalog;
    SnappyCategory *scategory;
    
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;
    GladeXML *xml = (GladeXML *)user_data->xml;
    GList *cards;
    
    view = glade_xml_get_widget (xml, "editor-cards-treeview");

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view)
                                                ,-1
                                                ,"Order"
                                                ,renderer
                                                ,"text", COL_NUM
                                                ,NULL); 
    renderer = gtk_cell_renderer_text_new ();
    g_object_set (renderer
                 ,"editable", 1
                 ,NULL);
    g_signal_connect (GTK_WIDGET (renderer), "edited", G_CALLBACK (cell_edited), user_data);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view)
                                                ,-1
                                                ,"Front"
                                                ,renderer
                                                ,"text", COL_FRONT
                                                ,NULL);
    renderer = gtk_cell_renderer_text_new ();
    g_object_set (renderer
                 ,"editable", 1
                 ,NULL);
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view)
                                                ,-1
                                                ,"Back"
                                                ,renderer
                                                ,"text", COL_BACK
                                                ,NULL);
    scatalog = user_data->scatalog;
    scategory = snappy_catalog_get_category (scatalog, scatalog->loaded);
    cards = (GList *)scategory->cards;
    
    model = editor_create_model (cards);
    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    col = gtk_tree_view_get_column (GTK_TREE_VIEW (view), 1);
    gtk_tree_view_column_set_min_width (col, 225);

    col = gtk_tree_view_get_column (GTK_TREE_VIEW (view), 2);
    gtk_tree_view_column_set_min_width (col, 225);
    
    g_object_unref (model);
}

static void
unwrap_categories (gpointer data, gpointer user_data)
{
    SnappyCategory *scategory = (SnappyCategory *)data;
    GtkWidget *w = (GtkWidget *)user_data;
    const gchar *name;
    
    g_object_get (scategory, "name", &name, NULL);
    gtk_combo_box_append_text (GTK_COMBO_BOX (w), name);
}

static void
category_changed (GtkWidget *combobox, gpointer user_data)
{
    CBData *cbdata = (CBData *) user_data;
    GladeXML *xml = (GladeXML *) cbdata->xml;
    GtkWidget *w = glade_xml_get_widget (xml, "editor-combobox");
    gint selection = 0;

    selection = gtk_combo_box_get_active (GTK_COMBO_BOX (w));
}

void
new_card () {}

GladeXML *
init_editor (SnappyCatalog *scatalog)
{        
    CBData *data = (CBData *) g_malloc (sizeof (CBData));
    SnappyCategory *scategory;
    GladeXML *xml;
    GtkWidget *w, *root;
    
    xml = glade_xml_new ("glade/editor.glade", NULL, NULL);

    data->xml = xml;
    data->scatalog = scatalog;
   
    /* populate combox with categories */ 
    w = glade_xml_get_widget (xml, "editor-combobox");
    g_list_foreach (scatalog->categories, unwrap_categories, w);
   
    /* prepare table */
    editor_create_table_view (data);
    
    /* make signal_connects */
    w = glade_xml_get_widget (xml, "editor-finish-button");
    g_signal_connect (w, "clicked", G_CALLBACK (hide_editor_dialog), data);

    w = glade_xml_get_widget (xml, "editor-new-button");
    g_signal_connect (w, "clicked", G_CALLBACK (new_card), data);

    w = glade_xml_get_widget (xml, "editor-combobox");
    g_signal_connect (w, "changed", G_CALLBACK (category_changed), data);
    
    return xml;
}
