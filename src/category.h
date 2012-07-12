#ifndef _CATEGORY_H
#define _CATEGORY_H

#include <gtk/gtk.h>
#include "card.h"
#include "cardstore.h"

G_BEGIN_DECLS

/*
 * When a card changes the database should get automatically updated
 */

#define SNAPPY_CATEGORY_TYPE          (snappy_category_get_type ())
#define SNAPPY_CATEGORY(obj)\
        (G_TYPE_CHECK_INSTANCE_CAST   ((obj), SNAPPY_CATEGORY_TYPE, SnappyCategory))
#define SNAPPY_CATEGORY_CLASS(klass)\
        (G_TYPE_CHECK_CLASS_CAST      ((klass), SNAPPY_CATEGORY_TYPE, SnappyCategoryClass))
#define SNAPPY_IS_CATEGORY(obj)\
        (G_TYPE_CHECK_INSTANCE_TYPE   ((obj), SNAPPY_CATEGORY_TYPE))
#define SNAPPY_IS_CATEGORY_CLASS(klass)\
        (G_TYPE_CHECK_CLASS_TYPE      ((klass), SNAPPY_CATEGORY_TYPE))
#define SNAPPY_CATEGORY_GET_CLASS(obj)\
        (G_TYPE_INSTANCE_GET_CLASS    ((obj), SNAPPY_CATEGORY_TYPE, SnappyCategoryClass))

typedef struct _SnappyCategory SnappyCategory;
typedef struct _SnappyCategoryClass SnappyCategoryClass;
typedef struct _SnappyCategoryScore SnappyCategoryScore;

struct _SnappyCategoryScore {
    const gchar *date;
    gint correct;
    gint skipped;
    glong time;
};

struct _SnappyCategory {
    GObject parent;

    guint id;
    gboolean loaded;
    gchar *name;
    GList *cards;
    GList *scores;
    DBStore *db;
    SnappyCategoryScore score;

    /* currently loaded card */
    gint card;
};

struct _SnappyCategoryClass {
    GObjectClass parent;
};


GType snappy_category_get_type (void);

SnappyCard * snappy_category_get_next_card  (SnappyCategory *scategory);
SnappyCard * snappy_category_get_card       (SnappyCategory *scategory, gint id);
gint snappy_category_load                   (SnappyCategory *scategory);
void snappy_category_unload                 (SnappyCategory *scategory);
void snappy_category_reset                  (SnappyCategory *scategory);

void snappy_category_append_card            (SnappyCategory *scategory
                                            ,const gchar *front
                                            ,const gchar *back);
void snappy_category_remove_card            (SnappyCategory *scategory, gint id);
void snappy_category_edit_card              (SnappyCategory *scategory
                                            ,const gchar *text
                                            ,gboolean side
                                            ,gint id);
void snappy_category_mark_score             (SnappyCategory *scategory);

G_END_DECLS

#endif /* CATEGORY_H */
