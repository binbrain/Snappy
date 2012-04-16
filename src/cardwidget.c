#include <gtk/gtk.h>
#include <math.h>

#include "cardwidget.h"

G_DEFINE_TYPE (SnappyCardWidget, snappy_card_widget, GTK_TYPE_DRAWING_AREA);

/* starting points for drawing card */
const guint BOX_START_X = 0;
const guint BOX_START_Y = 0;
const guint BOX_END_X = 560;
const guint BOX_END_Y = 325;

/* artificial box for text containment */
const int BOX_WIDTH = 560;
const int BOX_HEIGHT = 325;

static void
frame_text (cairo_t *ct, const gchar *text)
{
    gchar *delimited_str;
    gint len = 0;
    gint XstartSpot = 0;
    PangoLayout *layout;
    PangoFontDescription *desc;
    int i = 0;

    len = g_string_new (text)->len;
    
    layout = pango_cairo_create_layout (ct);
    pango_layout_set_text (layout, text, -1);
    desc = pango_font_description_from_string ("Sans Bold 34");
    pango_layout_set_font_description (layout, desc);

    pango_font_description_free (desc);
    
    pango_cairo_update_layout (ct, layout);
    
    XstartSpot = (BOX_END_X/3) - (len); 
    cairo_move_to (ct, XstartSpot, 150);
    
    pango_cairo_show_layout (ct, layout);
    
    g_object_unref (layout);
    //cairo_save (ct);
}

void
snappy_card_widget_flip (GtkWidget *cw)
{
    SnappyCardWidget *self = (SnappyCardWidget *)cw;
    GdkRegion *region;
    GtkWidget *card_w;

    card_w = GTK_WIDGET (self);
    
    if (!self->side)
    {
        self->side = TRUE;
        self->text = self->back;
    }
    else
    {
        self->side = FALSE;
        self->text = self->front;
    }
    
    /* redraw the cairo canvas completely by exposing it */
    if (!card_w->window) return;
    region = gdk_drawable_get_clip_region (card_w->window);
    gdk_window_invalidate_region (card_w->window, region, TRUE);
    gdk_window_process_updates (card_w->window, TRUE);
    gdk_region_destroy (region);     
   
}

/*
 * when the text changes the card needs to be redrawn
 */

void
snappy_card_widget_set_text (GtkWidget *cw, const gchar *text, gboolean side)
{
    SnappyCardWidget *self = (SnappyCardWidget *)cw;
    GtkWidget *card_w;
    GdkRegion *region;
    card_w = GTK_WIDGET (self);
    
    if (!side)
    {
        self->front = g_strdup (text);
    }
    else
    {
        self->back = g_strdup (text);
    }
    self->text = self->front;
    // redraw the cairo canvas completely by exposing it
    if (!card_w->window) return;
    region = gdk_drawable_get_clip_region (card_w->window);
    gdk_window_invalidate_region (card_w->window, region, TRUE);
    gdk_window_process_updates (card_w->window, TRUE);
    gdk_region_destroy (region);     
   
}

static void 
draw (GtkWidget *cw, cairo_t *ct)
{
    SnappyCardWidget *self = (SnappyCardWidget *)cw;
    gint i;

    /* Draw Card */    
    cairo_move_to (ct, BOX_START_X, BOX_START_Y);
    cairo_line_to (ct, BOX_END_X, BOX_START_Y);
    cairo_line_to (ct, BOX_END_X, BOX_END_Y);
    cairo_line_to (ct, BOX_START_X, BOX_END_Y);
    cairo_line_to (ct, BOX_START_X, BOX_START_Y);
   
    /* fill card with white-ness */ 
    cairo_set_source_rgb (ct, 1, 1, 1);
    cairo_fill_preserve (ct);
    cairo_stroke (ct);
    
    /* draw notecard-like lines */
    cairo_set_line_width (ct, 1);
    cairo_set_source_rgb (ct, 255, 0, 0);
    cairo_move_to (ct, 0, 30);
    cairo_line_to (ct, BOX_END_X, 30);
    cairo_stroke (ct);
    
    for (i=30; i<320; i=i+20)
    {
        cairo_set_line_width (ct, .2);
        cairo_set_source_rgb (ct, 0, 0, 255);
        cairo_move_to (ct, 0, i);
        cairo_line_to (ct, BOX_END_X, i);
        cairo_stroke (ct);
    }

    /* work on text */
    cairo_set_source_rgb (ct, 0, 0, 0);
    
    frame_text (ct, g_strdup (self->text));
}     

static gboolean
snappy_card_widget_expose (GtkWidget *cw, GdkEventExpose *event)
{
    SnappyCardWidget *self = (SnappyCardWidget *)cw;
    cairo_t *ct = self->ct;

    ct = gdk_cairo_create (cw->window);
    draw (cw, ct);

    /* should I destroy this now or in a finalize function? */    
    cairo_destroy (ct);
    
    return FALSE;
}

static void 
snappy_card_widget_class_init (SnappyCardWidgetClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS (class);
    GtkWidgetClass *widget_class;
    gint err;
    
    widget_class = GTK_WIDGET_CLASS (class);
    widget_class->expose_event = snappy_card_widget_expose;
}

static void
snappy_card_widget_init (SnappyCardWidget *self)
{    
    self->side = FALSE;
    self->front = g_strdup ("");
    self->back = g_strdup ("");
    self->text = self->front;
}

        
GtkWidget*
snappy_card_widget_new (void)
{
    return g_object_new (SNAPPY_CARD_WIDGET_TYPE, NULL);
}

