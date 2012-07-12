#include <gdk/gdkkeysyms.h>

#include "gui.h"

static gint delete_event_cb (GtkWidget *w, GdkEventAny *e, gpointer data)
{
 /* Callback for "delete_event" signal */
 /* TODO: Add code to confim if the user wants to quit */

 g_print("delete_event_cb\n");
 return 0;
}

static gint 
quit (GtkWidget *w, gpointer data)
{
    g_print ("goodbye");
    gtk_main_quit();
    return 0;
}

static void
finished_category (GtkWidget *button, SnappyCategory *scategory)
{
    SnappyCardWidget *swidget;
    SnappyCategoryScore *score;
    GTimeVal time;
    GtkWidget *w;

    score = &scategory->score;
    snappy_category_reset (scategory);
    
    g_get_current_time (&time);
    score->time = time.tv_sec - score->time;
    w = lookup_widget (GTK_WIDGET (button), "main-skipped-label");
    score->skipped = atoi (gtk_label_get_text (GTK_LABEL (w))) + 1;
    w = lookup_widget (GTK_WIDGET (button), "main-correct-label");
    score->correct = atoi (gtk_label_get_text (GTK_LABEL (w))) + 1;
    snappy_category_mark_score (scategory);
    
    g_print ("took about  %i\n", score->time);
    
    w = lookup_widget (GTK_WIDGET (button), "eventbox0");
    swidget = gtk_container_get_children (GTK_CONTAINER (w))->data;
    snappy_card_widget_set_text (GTK_WIDGET (swidget), g_strdup (""), TRUE);
    snappy_card_widget_set_text (GTK_WIDGET (swidget), g_strdup (""), FALSE);

    w = lookup_widget (GTK_WIDGET (button), "main-ok-button");
    gtk_widget_set_sensitive (w, FALSE);
    w = lookup_widget (GTK_WIDGET (button), "main-flip-button");
    gtk_widget_set_sensitive (w, FALSE);
    w = lookup_widget (GTK_WIDGET (button), "main-skip-button");
    gtk_widget_set_sensitive (w, FALSE);
    w = lookup_widget (GTK_WIDGET (button), "main-category-combobox");
    gtk_widget_set_sensitive (w, TRUE);
}

static gboolean
setup_next_card (GtkWidget *button, SnappyCategory *scategory)
{
    SnappyCardWidget *swidget;
    SnappyCard *scard = NULL;
    GtkWidget *w;
    const gchar *front, *back;
    
    scard = snappy_category_get_next_card (scategory);
    if (!scard) return FALSE;
    g_object_get (scard, "front", &front, "back", &back, NULL);

    w = lookup_widget (GTK_WIDGET (button), "eventbox0");
    swidget = gtk_container_get_children (GTK_CONTAINER (w))->data;
    snappy_card_widget_set_text (GTK_WIDGET (swidget), back, TRUE);
    snappy_card_widget_set_text (GTK_WIDGET (swidget), front, FALSE);
    return TRUE;
}    

/*  and then setup_next_card */
static void
ok_button_clicked (GtkWidget *button, gpointer user_data)
{
    SnappyCatalog *scatalog = (SnappyCatalog *)user_data;
    SnappyCategory *scategory;
    SnappyCard *scard;
    GtkWidget *w;
    gint category;
    gint correct;
    gchar *label = (gchar *) g_malloc (sizeof (gchar));
    
    w = lookup_widget (GTK_WIDGET (button), "main-category-combobox");
    category = gtk_combo_box_get_active (GTK_COMBO_BOX (w));
    
    w = lookup_widget (GTK_WIDGET (button), "main-correct-label");
    correct = atoi (gtk_label_get_text (GTK_LABEL (w))) + 1;
    g_sprintf (label, "%i", correct);
    gtk_label_set_text (GTK_LABEL (w), label);
    
    scategory = snappy_catalog_get_category (scatalog, category);
    g_print ("catergory %i\n", category); 
    scard = snappy_category_get_card (scategory, scategory->card);

    scard->status = CORRECT;
    if (!setup_next_card (button, scategory)) {
        finished_category (button, scategory);
        snappy_catalog_set_loaded (scatalog, 0);
    }
}

static void
flip_button_clicked (GtkWidget *button, gpointer user_data)
{
    SnappyCardWidget *swidget;
    GtkWidget *w;
    
    w = lookup_widget (GTK_WIDGET (button), "eventbox0");
    swidget = gtk_container_get_children (GTK_CONTAINER (w))->data;
    snappy_card_widget_flip (GTK_WIDGET (swidget));
}

static void
skip_button_clicked (GtkWidget *button, gpointer user_data)
{
    SnappyCatalog *scatalog = (SnappyCatalog *)user_data;
    SnappyCategory *scategory;
    SnappyCard *scard;
    GtkWidget *swidget;
    GtkWidget *w;
    gint category;
    gint skipped;
    gchar *label = (gchar *) g_malloc (sizeof (gchar));
    
    w = lookup_widget (GTK_WIDGET (button), "main-category-combobox");
    category = gtk_combo_box_get_active (GTK_COMBO_BOX (w));
    
    w = lookup_widget (GTK_WIDGET (button), "main-skipped-label");
    skipped = atoi (gtk_label_get_text (GTK_LABEL (w))) + 1;
    g_sprintf (label, "%i", skipped);
    gtk_label_set_text (GTK_LABEL (w), label);
    
    scategory = snappy_catalog_get_category (scatalog, category);
    scard = snappy_category_get_card (scategory, scategory->card);

    scard->status = SKIPPED;
    if (!setup_next_card (button, scategory)) finished_category (button, scategory);
}

/* 
 * activate Ok, skipped, and flip buttons. 
 */    
static void
start_button_clicked (GtkWidget *button, gpointer user_data)
{
    SnappyCardWidget *swidget;
    SnappyCatalog *scatalog = (SnappyCatalog *)user_data;
    SnappyCategory *scategory;
    SnappyCategoryScore *score;
    SnappyCard *scard;
    GTimeVal time;
    GtkWidget *w;
    GtkWidget *root;
    gint category;
    const gchar *front, *back;
    
    w = lookup_widget (GTK_WIDGET (button), "main-category-combobox");
    category = gtk_combo_box_get_active (GTK_COMBO_BOX(w));
    snappy_catalog_set_loaded (scatalog, category);
    gtk_widget_set_sensitive (w, FALSE);
    g_print ("selected %i\n", category);
    
    scategory = snappy_catalog_get_category (scatalog, category);
    scard = snappy_category_get_card (scategory, 0);
    score = &scategory->score;
    g_object_get (scard, "front", &front, "back", &back, NULL);
 
    w = lookup_widget (GTK_WIDGET (button), "eventbox0");
    swidget = gtk_container_get_children (GTK_CONTAINER (w))->data;
    snappy_card_widget_set_text (GTK_WIDGET (swidget), back, TRUE);
    snappy_card_widget_set_text (GTK_WIDGET (swidget), front, FALSE);

    w = lookup_widget (GTK_WIDGET (button), "main-ok-button");
    gtk_widget_set_sensitive (w, TRUE);
    w = lookup_widget (GTK_WIDGET (button), "main-flip-button");
    gtk_widget_set_sensitive (w, TRUE);
    w = lookup_widget (GTK_WIDGET (button), "main-skip-button");
    gtk_widget_set_sensitive (w, TRUE);

    w = lookup_widget (GTK_WIDGET (button), "main-correct-label");
    gtk_label_set_text (GTK_LABEL (w), "0");
    w = lookup_widget (GTK_WIDGET (button), "main-skipped-label");
    gtk_label_set_text (GTK_LABEL (w), "0");

    g_get_current_time (&time);
    score->time = time.tv_sec;

    w = lookup_widget (GTK_WIDGET (button), "fullscreen1");
    if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (w))) {

        w = lookup_widget (GTK_WIDGET (button), "menubar1");
        gtk_widget_hide (w);
        w = lookup_widget (GTK_WIDGET (button), "hbox5");
        gtk_widget_hide (w);
        w = lookup_widget (GTK_WIDGET (button), "vbox7");
        gtk_widget_hide (w);
        w = lookup_widget (GTK_WIDGET (button), "statusbar1");
        gtk_widget_hide (w);
        w = lookup_widget (GTK_WIDGET (button), "eventbox1");
        
        w = lookup_widget (GTK_WIDGET (button), "window1");
        gtk_window_fullscreen (GTK_WINDOW (w));
    }
}

/* 
 * Make all the buttons go gray, grab the score from the label
 */
static void
stop_button_clicked (GtkWidget *button, gpointer user_data)
{
    SnappyCardWidget *swidget;
    GtkWidget *w;

    w = lookup_widget (GTK_WIDGET (button), "main-category-combobox");
    gtk_widget_set_sensitive (w, TRUE);
    
    w = lookup_widget (GTK_WIDGET (button), "eventbox0");
    swidget = gtk_container_get_children (GTK_CONTAINER (w))->data;
    snappy_card_widget_set_text (GTK_WIDGET (swidget), "", FALSE);
}

static void
editor_activated (GtkWidget *w, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;     
    GtkWidget *editor;     
    
    editor = glade_xml_get_widget (xml, "editor-dialog");
    gtk_widget_show (editor);
}        

static void
categories_activated (GtkWidget *w, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;
    GtkWidget *categories;

    categories = glade_xml_get_widget (xml, "categories-dialog");
    g_print ("here");
    gtk_widget_show (categories);
}

static void
preferences_activated (GtkWidget *w, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;
    GtkWidget *properties;

    properties = glade_xml_get_widget (xml, "preferences-dialog");
    g_print ("showing preferences\n");
    gtk_widget_show (properties);
}

/*
 * When the category selection changes, unload the current category
 * and reload the new selection
 */
static void
category_changed (GtkWidget *combobox, gpointer user_data)
{
    SnappyCatalog *scatalog = (SnappyCatalog *)user_data;
    SnappyCategory *scategory;
    gint index;
    
    index = gtk_combo_box_get_active (GTK_COMBO_BOX (combobox));
    scategory = snappy_catalog_get_category (scatalog, index);
    snappy_category_load (scategory);
}

static void
setup_categories_combobox (gpointer data, gpointer user_data)
{
    SnappyCategory *scategory = (SnappyCategory *)data;
    GtkWidget *w;
    const gchar *name;
    
    w = GTK_WIDGET (user_data);
    g_object_get (scategory, "name", &name, NULL);
    gtk_combo_box_append_text (GTK_COMBO_BOX (w), name);
} 

static void
about_activated (GtkWidget *w, gpointer user_data)
{
    GladeXML *xml = (GladeXML *)user_data;
    GtkWidget *about;

    about = glade_xml_get_widget (xml, "about-dialog");
    gtk_widget_show (about);
}

static void
key_pressed (GtkWidget *w, GdkEventKey *event, gpointer user_data)
{
    SnappyCatalog *scatalog = (SnappyCatalog *)user_data;

    if (0 != scatalog->loaded) {
        if (GDK_space == event->keyval) {
            flip_button_clicked (w, scatalog);
            g_print ("flip");
        }
        else if (GDK_Return == event->keyval) {
            ok_button_clicked (w, scatalog);
            g_print ("return");
        }
        else if (GDK_Escape == event->keyval) {
            g_print ("esc");        
        }
        else if (GDK_Right == event->keyval) {
            skip_button_clicked (w, scatalog);
            g_print ("skip");
        }
    }
}

void 
init_gui (GladeXML *xml, SnappyCatalog *scatalog)
{
    SnappyCategory *scategory;
    SnappyCardWidget *swidget;
    SnappyCard *card;
    GladeXML *xml_dialogs;
    GtkWidget *w;
    GtkWidget *root;
    GdkColor color;
    gint err = 0;

    /* get the root window to hook the children widget to */
    root = glade_xml_get_widget (xml, "window1");

    /* setup keybindings */
    g_signal_connect (root, "key_press_event", G_CALLBACK (key_pressed), scatalog);
    
    /* fullscreen needs stuff hidden and the root window to go full */
    HOOKUP_OBJECT (root, root, "window1");    
    w = glade_xml_get_widget (xml, "hbox5");
    HOOKUP_OBJECT (root, w, "hbox5");
    w = glade_xml_get_widget (xml, "vbox7");
    HOOKUP_OBJECT (root, w, "vbox7");
    w = glade_xml_get_widget (xml, "statusbar1");
    HOOKUP_OBJECT (root, w, "statusbar1");
    w = glade_xml_get_widget (xml, "menubar1");
    HOOKUP_OBJECT (root, w, "menubar1");
    w = glade_xml_get_widget (xml, "eventbox1");
    HOOKUP_OBJECT (root, w, "eventbox1");

    /* I should be getting the default category, instead I will 
     * just grab the 1st
     */
    
    /* add categories to combobox and set selected category */
    w = glade_xml_get_widget (xml, "main-category-combobox");
    g_list_foreach (scatalog->categories, setup_categories_combobox, w);
    gtk_combo_box_set_active (GTK_COMBO_BOX (w), 0);
    HOOKUP_OBJECT (root, w, "main-category-combobox");
    g_signal_connect (w, "changed", G_CALLBACK (category_changed), scatalog);
    
    /* add start button signal handler */
    w = glade_xml_get_widget (xml, "main-start-button");
    g_signal_connect (w, "clicked", G_CALLBACK (start_button_clicked), scatalog);

    /* add stop button signal handler */
    w = glade_xml_get_widget (xml, "main-stop-button");
    g_signal_connect (w, "clicked", G_CALLBACK (stop_button_clicked), scatalog);

    /* add card widget */
    w = glade_xml_get_widget (xml, "eventbox0");
    HOOKUP_OBJECT (root, w, "eventbox0");
    swidget = (SnappyCardWidget *) g_object_new (SNAPPY_CARD_WIDGET_TYPE, NULL);
    gtk_container_add (GTK_CONTAINER (w), GTK_WIDGET (swidget));
    /*-- done adding card --*/

    /* setup labels for the scores */
    w = glade_xml_get_widget (xml, "main-correct-label");
    HOOKUP_OBJECT (root, w, "main-correct-label");
    w = glade_xml_get_widget (xml, "main-skipped-label");
    HOOKUP_OBJECT (root, w, "main-skipped-label");
    /*-- done setting up the labels --*/
    
    /* make buttons insensitive and bind callbacks */
    
    /* add ok button signal handler */
    w = glade_xml_get_widget (xml, "main-ok-button");
    HOOKUP_OBJECT (root, w, "main-ok-button");
    g_signal_connect (w, "clicked", G_CALLBACK (ok_button_clicked), scatalog);
    gtk_widget_set_sensitive (w, FALSE);

    /* add flip button signal handler */
    w = glade_xml_get_widget (xml, "main-flip-button");
    HOOKUP_OBJECT (root, w, "main-flip-button");
    g_signal_connect (w, "clicked", G_CALLBACK (flip_button_clicked), scatalog);
    gtk_widget_set_sensitive (w, FALSE);
    
    /* add skip button signal handler */
    w = glade_xml_get_widget (xml, "main-skip-button");
    HOOKUP_OBJECT (root, w, "main-skip-button");
    g_signal_connect (w, "clicked", G_CALLBACK (skip_button_clicked), scatalog);
    gtk_widget_set_sensitive (w, FALSE);

    /* Hookup fullscreen checkbox */
    w = glade_xml_get_widget (xml, "fullscreen1");
    HOOKUP_OBJECT (root, w, "fullscreen1");

    /* -- end making buttons insensitive and binding callbacks -- */

    /* setup editor */
    xml_dialogs = init_editor (scatalog);
    w = glade_xml_get_widget (xml, "menu-cards-item");
    g_signal_connect (w, "activate", G_CALLBACK (editor_activated), xml_dialogs);

    /* setup categories */
    xml_dialogs = init_categories (scatalog);
    w = glade_xml_get_widget (xml, "menu-categories-item");
    g_signal_connect (w, "activate", G_CALLBACK (categories_activated), xml_dialogs);

    /* setup properties */
    xml_dialogs = init_preferences ();
    w = glade_xml_get_widget (xml, "menu-preferences-item");
    g_signal_connect (w, "activate", G_CALLBACK (preferences_activated), xml_dialogs);

    /* setup about */
    xml_dialogs = init_about ();
    w = glade_xml_get_widget (xml, "menu-about-item");
    g_signal_connect (w, "activate", G_CALLBACK (about_activated), xml_dialogs);

    /* Quit */
    w = glade_xml_get_widget (xml, "quit1");
    g_signal_connect(G_OBJECT (w), "activate", G_CALLBACK (quit), NULL); 

#ifdef OUT 
    /* set window size, tac on standard events */

    g_signal_connect(G_OBJECT(w), "delete_event",
    G_CALLBACK(delete_event_cb), NULL);
    
#endif
    /*-- show everything but the card area --*/
    gtk_widget_show_all (root);
    /*-- end --*/
    
    /* Should i be unref'ing the GladeXML here? */
}
