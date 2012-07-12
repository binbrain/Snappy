#include "cardstore.h"

static int
cardstore_get_categories_callback (void *data, gint cols, gchar **arr, gchar **col_names)
{
    GList *pile;
    const gchar *category;
    const gchar *index;
    gint id;

    pile = data;
    index = (const gchar *) arr[0];
    category = (const gchar *) arr[1];

    if (!g_utf8_validate (category, -1, NULL))
        g_print ("not valid utf8 string");

    id = atoi (index);
    pile = g_list_append (pile, GINT_TO_POINTER (id));
    pile = g_list_append (pile, g_strdup (category)); 

    return 0;
}

static int
cardstore_get_card_callback (void *data, gint cols, gchar **arr, gchar **col_names)
{    
    GList *tmpl;
    const gchar *text;
    
    tmpl = data;
    text = (const gchar *)arr[0];    
    tmpl = g_list_append (tmpl, g_strdup (text));
    
    return 0;
}

/* 
 * Callback should return a non-zero int, otherwise query is aborted
 */
static int
cardstore_get_cards_callback (void *data, gint cols, gchar **arr, gchar **col_names)
{
    GList *pile = NULL;
    guint id;
    const gchar *front;
    const gchar *back;

    pile = data;
    /* get data for printing  */
    id = atoi (arr[0]);
    front = (const gchar *)arr[1];
    back = (const gchar *)arr[2];
    
    //check for all 3 values
    if (!g_utf8_validate ((const gchar *)arr[1], -1, NULL))
        g_print ("not valid\n");

    pile = g_list_append (pile, GINT_TO_POINTER (id));
    pile = g_list_append (pile, g_strdup (front));
    pile = g_list_append (pile, g_strdup (back));
        
    return 0;
}

/* 
 * Callback should return a non-zero int, otherwise query is aborted
 */
static int
cardstore_get_scores_callback (void *data, gint cols, gchar **arr, gchar **col_names)
{
    GList *pile = NULL;
    const gchar *date;
    gint correct, skipped;

    pile = data;
    
    /* get data for printing  */
    date = (const gchar *)arr[0];
    correct = atoi (arr[1]);
    skipped = atoi (arr[2]);
    
    if (!g_utf8_validate ((const gchar *)arr[0], -1, NULL))
        g_print ("not valid\n");

    pile = g_list_append (pile, g_strdup (date));
    pile = g_list_append (pile, GINT_TO_POINTER (correct));
    pile = g_list_append (pile, GINT_TO_POINTER (skipped));
        
    return 0;
}

gboolean
cardstore_get_gconf_path (const gchar *filename)
{
}

/*
 * Check the path and open connection
 * returns false if the connection opens
 */
gboolean
cardstore_open (DBStore *database)
{
    gint status;
    
    database->filename = "cards.db";
    return sqlite3_open (database->filename, &database->conn);
}

gboolean
cardstore_close (DBStore *database)
{
    sqlite3_close(database->conn);
}

GList *
cardstore_get_cards (DBStore *database, gint category)
{
    gchar *errmsg;
    const gchar *sql;
    gint result;
    GList *pile = NULL;
    GList *card = NULL;
    gpointer ppile = NULL;
    gchar *id = (gchar *) g_malloc (25);
    
    pile = g_list_append (pile, g_strdup (""));
    ppile = pile;
    
    g_sprintf (id, "%i", category);
    sql = g_strconcat ("SELECT * FROM card WHERE category = ", id, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,cardstore_get_cards_callback
                          ,(void *)ppile
                          ,&errmsg);
    pile = g_list_next (g_list_first (pile)); 
    g_free (errmsg);

    return pile;
}

GList *
cardstore_get_categories (DBStore *database)
{
    gchar *errmsg;
    const gchar *sql;
    gint result;
    GList *pile = NULL;
    gpointer ppile = NULL;
    
    pile = g_list_append (pile, g_strdup (""));
    ppile = pile;
    sql = "SELECT * FROM category;";
    result = sqlite3_exec (database->conn
                          ,sql
                          ,cardstore_get_categories_callback
                          ,(void *)ppile
                          ,&errmsg);
    pile = g_list_next (g_list_first (pile));
    g_free (errmsg);
    
    return pile;
}

gint
cardstore_new_card (DBStore *database, const gchar *front, const gchar *back, gint category)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;
    gchar id[4096];

    g_snprintf (id, sizeof (id), "%i", category);
    sql = g_strconcat ("INSERT INTO card VALUES (NULL, '", front, "','", back, "','", id, "');", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg);

   if (SQLITE_CONSTRAINT == result)
        g_print ("constraint voilation");

    g_free (errmsg);
    
   return sqlite3_last_insert_rowid (database->conn);
} 

/* 
 * Return the a card side
 */
const gchar *
cardstore_get_card (DBStore *database, gint index, gboolean side)
{
    gchar *errmsg;
    const gchar *sql;
    const gchar *col;
    const gchar *text;
    GList *tmpl = NULL;
    gpointer p = NULL;
    gchar id[4096];
    gint result;

    if (side) 
        col = " back "; 
    else 
        col = " front ";

    tmpl = g_list_append (tmpl, "");
    p = tmpl;    
    g_snprintf (id, sizeof (id), "%i", index);
    sql = g_strconcat ("SELECT", col, "FROM card WHERE id=", id, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,cardstore_get_card_callback
                          ,(void *)p
                          ,&errmsg);
    text = g_list_nth (tmpl, 1)->data;
    
    return g_strdup (text);
}

gint
cardstore_new_category (DBStore *database, const gchar *name)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;

    sql = g_strconcat ("INSERT INTO category VALUES (NULL, '", name, "');", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg);

   if (SQLITE_CONSTRAINT == result)
        g_print ("constraint voilation");
    g_free (errmsg);
    
   return sqlite3_last_insert_rowid (database->conn);
}

void
cardstore_rename_category (DBStore *database, gint index, const gchar *name)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;
    gchar *id = (gchar *) g_malloc (sizeof (gchar *));
    
    g_snprintf (id, sizeof (id), "%i", index);
    sql = g_strconcat ("UPDATE card SET name =", g_strdup (name), "' WHERE id=", id, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg
                          );
}
    
void
cardstore_delete_category (DBStore *database, gint index)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;
    
    sql = g_strconcat ("DELETE FROM category WHERE id=", index, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg
                          );
}

/* not used right now */
const gchar *
cardstore_get_category (DBStore *database, gint index) 
{
    const gchar *sql;
    gpointer name;
    gchar *errmsg;
    gint result;

    sql = g_strconcat ("SELECT name FROM category WHERE id=", index, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,(void *)name
                          ,&errmsg
                          );

    return g_strdup (name);
}

void
cardstore_delete_card (DBStore *database, gint index)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;
    
    sql = g_strconcat ("DELETE FROM card WHERE id=", index, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg
                          );
}

void
cardstore_update_card (DBStore *database
                      ,const gchar *text
                      ,gboolean side
                      ,gint card)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;
    const gchar *col;
    gchar *id = (gchar *) g_malloc (sizeof (gchar *));
    
    if (side) 
        col = " back "; 
    else 
        col = " front ";
    
    g_snprintf (id, sizeof (id), "%i", card);
    sql = g_strconcat ("UPDATE card SET ", col, "='", text, "' WHERE id=", id, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg
                          );
}    

GList *
cardstore_get_scores (DBStore *database, gint category)
{
    gchar *errmsg;
    const gchar *sql;
    gint result;
    GList *pile = NULL;
    GList *card = NULL;
    gpointer ppile = NULL;
    gchar *id = (gchar *) g_malloc (sizeof (gchar *));
    
    pile = g_list_append (pile, g_strdup (""));
    ppile = pile;
    
    g_sprintf (id, "%i", category);
    sql = g_strconcat ("SELECT * FROM score WHERE category = ", id, ";", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,cardstore_get_scores_callback
                          ,(void *)ppile
                          ,&errmsg);
    pile = g_list_next (g_list_first (pile)); 
    g_free (errmsg);

    return pile;
}

gint
cardstore_set_score (DBStore *database, gint correct, gint skipped, gint category)
{
    const gchar *sql;
    gchar *errmsg;
    gint result;

    sql = g_strconcat ("INSERT INTO score VALUES (datetime ('now'), '"
                      ,correct, "','", skipped, "','", category, "','"
                      ,");", NULL);
    result = sqlite3_exec (database->conn
                          ,sql
                          ,NULL
                          ,NULL
                          ,&errmsg);
    if (SQLITE_CONSTRAINT == result)
        g_print ("constraint voilation");
    g_free (errmsg);
    
    return sqlite3_last_insert_rowid (database->conn);
}
