#include "category.h"

void
snappy_category_append_card (SnappyCategory *scategory, const gchar *front, const gchar *back)
{
    SnappyCard *scard;
    gint id;

    id = cardstore_new_card (scategory->db, front, back, scategory->id);
    scard = g_object_new (SNAPPY_CARD_TYPE
                         ,"id", id
                         ,"front", g_strdup (front)
                         ,"back", g_strdup (back)
                         ,"category", scategory->id
                         ,"database", scategory->db
                         ,NULL
                         );
    scategory->cards = g_list_append (scategory->cards, scard);

}

void
snappy_category_remove_card (SnappyCategory *scategory, gint id)
{
    cardstore_delete_card (scategory->db, id);
}

static void
reset_cards (gpointer data, gpointer user_data)
{
    SnappyCard *scard = (SnappyCard *)data;
    scard->status = NOTSET;
}

void
snappy_category_reset (SnappyCategory *scategory)
{
    scategory->card = 0;
    scategory->loaded = FALSE;
    g_list_foreach (scategory->cards, reset_cards, NULL);    
}
/* 
 * Return the next card based on the gint card index as long as the card
 * isn't already marked as correct. If all the cards are marked as correct,
 * we are done, return NULL
 */
SnappyCard *
snappy_category_get_next_card (SnappyCategory *scategory)
{
    SnappyCard *scardNext = NULL;
    SnappyCard *scard = NULL;
    gint iterator = 0;

    g_print ("starting at card %i\n", scategory->card);
   
    scardNext = g_list_nth_data (scategory->cards, ++scategory->card);
    while (TRUE)
    {   
        if (iterator > g_list_length (scategory->cards)) break;
        if (!scardNext)
        {
            if (scategory->card == g_list_length (scategory->cards)) 
            {
                scategory->card = 0;
                scardNext = g_list_nth_data (scategory->cards, scategory->card);
            }
            else break;
        }
        if (scardNext->status == NOTSET || scardNext->status == SKIPPED)
        {
            g_print ("returning card %i\n", scardNext->id);
            return scardNext;
        }
        scardNext = g_list_nth_data (scategory->cards, ++scategory->card);    
        iterator++;
    }

    g_print ("card not found, returning null\n");
    return NULL;
}

SnappyCard *
snappy_category_get_card (SnappyCategory *scategory, gint id)
{
    SnappyCard *scard = g_list_nth_data (scategory->cards, id);
    g_print ("getting card %p\n", scard);
    return scard;
}

/* I think this should be deprected in favor of directly 
 * editing the card object
 */
void
snappy_category_edit_card (SnappyCategory *scategory
                          ,const gchar *text
                          ,gboolean side
                          ,gint id)
{
    if (scategory->loaded)
        g_print ("\nthis category is loaded\n");
    else
        g_print ("\nnot loaded dude\n");
    SnappyCard *scard = (SnappyCard *) g_list_nth_data (scategory->cards, id);
    const gchar *attr;

    if (side)
        attr = "back";
    else
        attr = "front";
    
    g_object_set (scard, attr, text, NULL);
    
    cardstore_update_card (scategory->db, text, side, id);
    
}
    
/* load the new category and call unload on the old category */
gint
snappy_category_load (SnappyCategory *scategory)
{
    SnappyCard *card;
    SnappyCategoryScore *sscore;
    GList *dbcards = NULL;
    GList *dbscores = NULL;
    const gchar *front;
    const gchar *back;
    gint id, i;

    if (scategory->loaded)
    {
        g_list_free (scategory->cards);
        g_object_set (scategory, "loaded", FALSE, NULL);
    }

    if (!scategory->loaded)
    {
        g_print ("loading category %i\n", scategory->id);

        dbcards = cardstore_get_cards (scategory->db, scategory->id);   
        for (i = 0; i < g_list_length (dbcards); i++)
        {
            id = (gint) g_list_nth_data (dbcards, i);
            front = (gchar *) g_list_nth_data (dbcards, ++i);
            back = (gchar *) g_list_nth_data (dbcards, ++i);

            card = g_object_new (SNAPPY_CARD_TYPE
                    ,"id", id
                    ,"front", front
                    ,"back", back
                    ,"category", scategory->id
                    ,"database", scategory->db
                    ,NULL);
            scategory->cards = g_list_append (scategory->cards, card);
        }

        dbscores = cardstore_get_scores (scategory->db, scategory->id);
        for (i = 0; i < g_list_length (dbscores); i++)
        {
            sscore = (SnappyCategoryScore *) g_malloc (sizeof (SnappyCategoryScore));
            sscore->date = g_strdup (g_list_nth_data (dbscores, i));
            sscore->correct = (gint) g_list_nth_data (dbscores, ++i);
            sscore->skipped = (gint) g_list_nth_data (dbscores, ++i);
            scategory->scores = g_list_append (scategory->scores, sscore);
        }
    }
    else 
    {
        g_print ("category already loaded\n");
    }
    g_object_set (scategory, "loaded", TRUE, NULL);
    
    return 0;
}            

void
snappy_category_unload (SnappyCategory *scategory)
{
    //unload card list
    //free scores
    g_object_set (scategory, "loaded", FALSE);
}

/* 
 * grab the existing score stats and append it to the
 * list of scores for the category, plus, throw it 
 * in the database
 */
void
snappy_category_mark_score (SnappyCategory *scategory)
{
	//gint correct = scategory->score->correct;
	//gint skipped = scategory->score->skipped;
	//cardstore_set_score (scategory->db, correct, skipped, scategory->id);
}
    

enum {
    PROP_INDEX = 1 /* set upon creation, and never again */
    ,PROP_NAME
    ,PROP_DATABASE
    ,PROP_LOADED
};    

static void
snappy_category_get_property (GObject *obj, guint prop_id, GValue *value, GParamSpec *pspec)
{
    SnappyCategory *self = (SnappyCategory *)obj;

    switch (prop_id)
    {
        case PROP_INDEX:
            g_value_set_uint (value, self->id);
	    break;
		
        case PROP_NAME:
            g_value_set_string (value, self->name);
            break;

        case PROP_DATABASE:
            g_value_set_pointer (value, self->db);
            break;

        case PROP_LOADED:
            g_value_set_boolean (value, self->loaded);
            break;
		
        default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
	}
}


static void
snappy_category_set_property (GObject *obj, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    SnappyCategory *self = (SnappyCategory *)obj;

    switch (prop_id)
    {
        case PROP_INDEX:
            self->id = g_value_get_uint (value);
        break;
            
        case PROP_NAME:
            if (self->name) g_free (self->name);
            self->name = g_value_dup_string (value); 
        break;

        case PROP_DATABASE:
            self->db = g_value_get_pointer (value);
        break;

        case PROP_LOADED:
            self->loaded = g_value_get_boolean (value);
        break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
    }
}

static void
snappy_category_finalize (GObject *obj)
{
    SnappyCategory *self = (SnappyCategory *)obj;

    if (self->name) g_free (self->name);
    
}

static void
snappy_category_class_init (gpointer g_class, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
    GParamSpec *psec;

    gobject_class->finalize = snappy_category_finalize;
    gobject_class->set_property = snappy_category_set_property;
    gobject_class->get_property = snappy_category_get_property;

    psec = g_param_spec_uint ("id"        
                             ,"index"
                             ,"category db index"
                             ,0                     
                             ,UINT_MAX               
                             ,0	                      
                             ,G_PARAM_READWRITE
                             );
    g_object_class_install_property (gobject_class, PROP_INDEX, psec);

    psec = g_param_spec_boolean ("loaded"
                                ,"initialized"
                                ,"category cards are loaded into list"
                                ,FALSE
                                ,G_PARAM_READWRITE
                                );
    g_object_class_install_property (gobject_class, PROP_LOADED, psec);
    
    psec = g_param_spec_string ("name"                     
                               ,"category"                
                               ,"name of category"       
                               ,NULL                         
                               ,G_PARAM_READWRITE
                               );
    g_object_class_install_property (gobject_class, PROP_NAME, psec);

    psec = g_param_spec_pointer ("database"
                                ,"database"
                                ,"database handler"
                                ,G_PARAM_READWRITE
                                );
    g_object_class_install_property (gobject_class, PROP_DATABASE, psec);
}

static void
snappy_category_instance_init (GTypeInstance *instance, gpointer g_class)
{
    SnappyCategory *self = (SnappyCategory *)instance;

    self->cards	= NULL;
    self->id = -1;
    self->db = NULL;
    self->loaded = FALSE;
    self->card = 0;
}

GType
snappy_category_get_type (void)
{	
    static GType type = 0;
    if (type == 0) {
        static const GTypeInfo info = {
	    sizeof (SnappyCategoryClass),	
            NULL,   					   
            NULL,   					
            (GClassInitFunc) snappy_category_class_init, 
            NULL,                 		
            NULL,   					
            sizeof (SnappyCategory),		
            0,      					
            (GInstanceInitFunc) snappy_category_instance_init
	};
	type = g_type_register_static (G_TYPE_OBJECT, "SnappyCategoryType", &info, 0);
    }
    return type;
}
