#ifndef _CARD_H
#define _CARD_H

#include <gtk/gtk.h>
#include "cardstore.h"

G_BEGIN_DECLS

/*
 * When a card changes the database should get automatically updated
 */


#define SNAPPY_CARD_TYPE              (snappy_card_get_type ())
#define SNAPPY_CARD(obj)\
        (G_TYPE_CHECK_INSTANCE_CAST   ((obj), SNAPPY_CARD_TYPE, SnappyCard))
#define SNAPPY_CARD_CLASS(klass)\
        (G_TYPE_CHECK_CLASS_CAST      ((klass), SNAPPY_CARD_TYPE, SnappyCardClass))
#define SNAPPY_IS_CARD(obj)\
        (G_TYPE_CHECK_INSTANCE_TYPE   ((obj), SNAPPY_CARD_TYPE))
#define SNAPPY_IS_CARD_CLASS(obj)\
        (G_TYPE_CHECK_CLASS_TYPE      ((klass), SNAPPY_CARD_TYPE))
#define SNAPPY_CARD_GET_CLASS(obj)\
        (G_TYPE_INSTANCE_GET_CLASS    ((obj), SNAPPY_CARD_TYPE, SnappyCardClass))

typedef struct _SnappyCard SnappyCard;
typedef struct _SnappyCardClass SnappyCardClass;

typedef enum {
    NOTSET
    ,SKIPPED
    ,CORRECT
} SnappyCardStatus;

struct _SnappyCard {
    GObject parent;

    DBStore *db;
    
    /* card attributes */
    SnappyCardStatus status;
    guint id;
    gchar *front;
    gchar *back;
    gint category;
    gboolean side;
};

struct _SnappyCardClass {
    GObjectClass parent;
};

void snappy_card_set_text (SnappyCard *scard, const gchar *text, gboolean side);

G_END_DECLS

#endif /* CARD_H */
