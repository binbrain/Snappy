#include "catalog.h"

void
snappy_catalog_append_category (SnappyCatalog *scatalog, const gchar *name)
{
    SnappyCategory *scategory;
    gint id;
    
    id = cardstore_new_category (&scatalog->db, name);
    scategory = g_object_new (SNAPPY_CATEGORY_TYPE
                             ,"id", id
                             ,"name", name
                             ,"database", &scatalog->db
                             ,NULL);
    scatalog->categories = g_list_append (scatalog->categories, scategory);
}

void
snappy_catalog_rename_category (SnappyCatalog *scatalog, gint id, const gchar *name)
{
    SnappyCategory *scategory;
    cardstore_rename_category (&scatalog->db, id, g_strdup (name));
}

void
snappy_catalog_remove_category (SnappyCatalog *scatalog, gint id)
{
    cardstore_delete_category (&scatalog->db, id);
}

/* debugging method for dumping out all categories and all
 * the contents of those categories
 */
void
snappy_catalog_dump_categories (SnappyCatalog *scatalog)
{
    SnappyCategory *scategory;
    SnappyCategoryScore *sscore;
    SnappyCard *scard;
    gchar *name;
    gint i, j, id;
    
    for (i = 0; i < g_list_length (scatalog->categories); i++)
    {
        scategory = g_list_nth_data (scatalog->categories, i);
        g_object_get (scategory, "id", &id, "name", &name, NULL);
        g_print ("%i %s\n", id, name);

        for (j = 0; j < g_list_length (scategory->scores); j++)
        {
            sscore = (SnappyCategoryScore *) g_list_nth_data (scategory->scores, j);
            g_print ("score for this category is %i %i\n", sscore->correct, sscore->skipped);
        }
        for (j = 0; j < g_list_length (scategory->cards); j++)
        {
            scard = (SnappyCard *) g_list_nth_data (scategory->cards, j);
            g_print ("card %s %s\n", scard->front, scard->back);
        }
    }
}

void
snappy_catalog_set_loaded (SnappyCatalog *scatalog, gint i)
{
	scatalog->loaded = i;
}

SnappyCategory *
snappy_catalog_get_category (SnappyCatalog *scatalog, gint id)
{
    return g_list_nth_data (scatalog->categories, id);
}

/* 
 * Set the first category to the default, load the cards for 
 * that category 
 */
static void
init_category_objects (GList *dbCategoryList, SnappyCatalog *scatalog)
{
    SnappyCategory *scategory;
    const gchar *name;
    guint id;
    gint i;
    
    for (i = 0; i < g_list_length (dbCategoryList); i++)
    {
        id = (guint) g_list_nth_data (dbCategoryList, i);
        name = g_strdup ((gchar *) g_list_nth_data (dbCategoryList, ++i));

        scategory = g_object_new (SNAPPY_CATEGORY_TYPE
                                 ,"id", id
                                 ,"name", name
                                 ,"database", &scatalog->db
                                 ,NULL);
        scatalog->categories = g_list_append (scatalog->categories, scategory);
    }
}

void
snappy_catalog_init (SnappyCatalog *scatalog)
{
    gint err;
    GList *dbCategoryList = NULL;
    
    /* init */
    scatalog->categories = NULL;
    scatalog->categories = g_list_append (scatalog->categories, g_strdup (""));
    
    /* should be setting this to gconf value */
    scatalog->loaded = 0;
   
    err = cardstore_open (&scatalog->db);	 
    g_print ("err %i", err);
    if (err)
    {   
        g_print ("database connection error\n");
	exit (1);
    }

    dbCategoryList = cardstore_get_categories (&scatalog->db);
    if (g_list_length (dbCategoryList) > 0) 
    {
        init_category_objects (dbCategoryList, scatalog);
        scatalog->categories = g_list_next (g_list_first (scatalog->categories));
   
        /*load cards for the 1st category */
        snappy_category_load (g_list_nth_data (scatalog->categories, 0));
    }
}
