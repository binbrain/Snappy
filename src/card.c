#include "card.h"

/*
 * set the text on a side and save it to the database
 */
void
snappy_card_set_text (SnappyCard *scard, const gchar *text, gboolean side)
{
    if (!side) 
    {
        g_object_set (scard, "front", g_strdup (text), NULL);
        cardstore_update_card (scard->db, g_strdup (text), 0, scard->id);
    } 
    else
    {
        g_object_set (scard, "back", g_strdup (text), NULL);
        cardstore_update_card (scard->db, g_strdup (text), 1, scard->id);
    }
}

enum {
    PROP_ID = 1
    ,PROP_FRONT
    ,PROP_BACK
    ,PROP_CATEGORY
    ,PROP_DATABASE
};    

static void
snappy_card_get_property (GObject *obj, guint prop_id, GValue *value, GParamSpec *pspec)
{
    SnappyCard *self = (SnappyCard *) obj;

    switch (prop_id)
    {
        case PROP_ID:
            g_value_set_uint (value, self->id);
        break;
        case PROP_FRONT:
            g_value_set_string (value, self->front);
            break;
        case PROP_BACK:
            g_value_set_string (value, self->back);
            break;
        case PROP_CATEGORY:
            g_value_set_int (value, self->category);
            break;
        case PROP_DATABASE:
            g_value_set_pointer (value, self->db);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
    }
}


static void
snappy_card_set_property (GObject *obj, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    SnappyCard *self = (SnappyCard *)obj;

    switch (prop_id)
    {
        case PROP_ID:
            self->id = g_value_get_uint (value);
        break;
        case PROP_FRONT:
            g_free (self->front);
            self->front = g_value_dup_string (value); 
        break;
        case PROP_BACK:
            g_free (self->back);
            self->back = g_value_dup_string (value); 
        break;
        case PROP_CATEGORY:
            self->category = g_value_get_int (value);
        break;
        case PROP_DATABASE:
            self->db = g_value_get_pointer (value);
        break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
    }
}

static void
snappy_card_finalize (GObject *obj)
{
    SnappyCard *self = (SnappyCard *)obj;

    if (self->front) g_free (self->front);
    if (self->back) g_free (self->back);
}

static void
snappy_card_class_init (gpointer gobject, gpointer g_class_data)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (gobject);
    GParamSpec *psec;

    gobject_class->finalize = snappy_card_finalize;
    gobject_class->set_property = snappy_card_set_property;
    gobject_class->get_property = snappy_card_get_property;

    psec = g_param_spec_uint ("id"                         /* identifier */
                             ,"card id"                    /* nickname */
                             ,"primary key in card table"  /* description */
                             ,0                            /* minimum */
                             ,UINT_MAX                     /* maximum */
                             ,0                            /* default */
                             ,G_PARAM_READWRITE            /* flags */
                             );
    g_object_class_install_property (gobject_class, PROP_ID, psec);
    
    psec = g_param_spec_string ("front"                      /* identifier */
                               ,"front"                      /* nickname */
                               ,"front of flash card"        /* description */
                               ,NULL                         
                               ,G_PARAM_READWRITE            /* flags */
                               );
    g_object_class_install_property (gobject_class, PROP_FRONT, psec);
    
    psec = g_param_spec_string ("back"                       /* identifier */
                               ,"back"                       /* nickname */
                               ,"back of flash card"         /* description */
                               ,NULL                         
                               ,G_PARAM_READWRITE            /* flags */
                               );
    g_object_class_install_property (gobject_class, PROP_BACK, psec);

    psec = g_param_spec_int ("category"
                            ,"category"
                            ,"category id"
                            ,-1
                            ,INT_MAX
                            ,0
                            ,G_PARAM_READWRITE
                            );
    g_object_class_install_property (gobject_class, PROP_CATEGORY, psec);

    psec = g_param_spec_pointer ("database"
                                ,"db"
                                ,"database handle"
                                ,G_PARAM_READWRITE
                                );
    g_object_class_install_property (gobject_class, PROP_DATABASE, psec);
}

static void
snappy_card_instance_init (GTypeInstance *instance, gpointer g_class)
{
    SnappyCard *self = (SnappyCard *)instance;
    
    self->id = -1;
    self->front = g_strdup ("");
    self->back = g_strdup ("");
    self->category = -1;
}

GType
snappy_card_get_type (void)
{   
    static GType type = 0;
    if (type == 0) {
        static const GTypeInfo info = {
            sizeof (SnappyCardClass),   /* class stucture size */
            NULL,                       /* base_init */
            NULL,                       /* base_finalize */
            snappy_card_class_init,     /* class_init */
            NULL,                       /* class_finalize */
            NULL,                       /* class_data */
            sizeof (SnappyCard),        /* instance structure size */
            0,                          /* n_preallocs */
            snappy_card_instance_init,  /* instance_init */
       };
       type = g_type_register_static (G_TYPE_OBJECT, "SnappyCardType", &info, 0);
    }
    return type;
}
