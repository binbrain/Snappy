#ifndef _CARD_STORE_H
#define _CARD_STORE_H

#include <sqlite3.h>
#include <gtk/gtk.h>

typedef struct _DBStore DBStore;

struct _DBStore {
    sqlite3 *conn;
    const gchar *filename;
};

void              dump                        (gpointer data, gpointer user_data);

/* connection methods */

gboolean          cardstore_get_gconf_path    (const gchar *filename);
gboolean          cardstore_open              (DBStore *database);
gboolean          cardstore_close             (DBStore *database);

/* category methods */

gint              cardstore_new_category      (DBStore *database, const gchar *category);
void              cardstore_delete_category   (DBStore *database, gint index);
void              cardstore_rename_category   (DBStore *database
                                              ,gint index
                                              ,const gchar *category);

const gchar *     cardstore_get_category      (DBStore *database, gint index);
GList *           cardstore_get_categories    (DBStore *database);


/* card methods */
GList *           cardstore_get_cards         (DBStore *database, gint category);

const gchar *     cardstore_get_card          (DBStore *database
                                              ,gint index
                                              ,gboolean side);
void              cardstore_update_card       (DBStore *database
                                              ,const gchar *text
                                              ,gboolean side
                                              ,gint card);
void              cardstore_delete_card       (DBStore *database, gint index);
gint              cardstore_new_card          (DBStore *database
                                              ,const gchar *front
                                              ,const gchar *back
                                              ,gint category);

GList *           cardstore_get_scores        (DBStore *database, gint category);
gint			  cardstore_set_score		  (DBStore *database, gint correct, gint skipped, gint category);

#endif /* _CARD_STORE_H */
