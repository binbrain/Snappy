#ifndef _CARD_WIDGET_H
#define _CARD_WIDGET_H

/*
 * A card widget contains several card objects for the current category.
 * When a new category is loaded, the card list should be reloaded.
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SNAPPY_CARD_WIDGET_TYPE            (snappy_card_widget_get_type ())
#define SNAPPY_CARD_WIDGET(obj) \
		(G_TYPE_CHECK_INSTANCE_CAST        ((obj), SNAPPY_CARD_WIDGET_TYPE, SnappyCardWidget))
#define SNAPPY_CARD_WIDGET_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_CAST           ((klass), SNAPPY_CARD_WIDGET_TYPE, SnappyCardWidgetClass))
#define SNAPPY_IS_CARD_WIDGET(obj) \
		(G_TYPE_CHECK_INSTANCE_TYPE        ((obj), SAPPY_CARD_WIDGET_TYPE))
#define SNAPPY_IS_CARD_WIDGET_CLASS(obj) \
		(G_TYPE_CHECK_CLASS_TYPE           ((klass), SNAPPY_CARD_WIDGET_TYPE))
#define SNAPPY_CARD_WIDGET_GET_CLASS \
		(G_TYPE_INSTANCE_GET_CLASS         ((obj), SNAPPY_CARD_WIDGET_TYPE, SnappyCardWidgetClass))

typedef struct _SnappyCardWidget SnappyCardWidget;
typedef struct _SnappyCardWidgetClass SnappyCardWidgetClass;

struct _SnappyCardWidget 
{
	GtkDrawingArea parent;
    cairo_t *ct;

    gchar *front;
    gchar *back;
    gchar *text;
    gboolean side;

};

struct _SnappyCardWidgetClass
{
    GtkDrawingAreaClass parent_class;

};

GtkWidget *snappy_card_widget_new (void);
const gchar * snappy_card_widget_get_category (GtkWidget *cw);
void snappy_card_widget_set_text (GtkWidget *cw, const gchar *text, gboolean side);
void snappy_card_widget_flip (GtkWidget *cw);

G_END_DECLS

#endif /* _CARD_WIDGET_H */

