/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

#include <gtk/gtk.h>


#include <glib/gi18n.h>


GList *images = NULL;

GtkWidget *window;

static gboolean update_image ( gpointer ud )
{
    GtkImage *image = (GtkImage*)ud;
    if ( images ) {
        int length = g_list_length(images);
        int e = g_random_int_range ( 0, length);
        GList *i = g_list_nth(images, e);
        if (i ){
            printf("Load: %s\n", (char*)i->data);
            int w, h;
            gtk_window_get_size ( GTK_WINDOW ( window), &w, &h);
            GdkPixbuf *pb = gdk_pixbuf_new_from_file_at_size ( (char *)i->data,w,h,NULL );
            if ( pb )
            {
                gtk_image_set_from_pixbuf ( image, pb ); 
                g_object_unref(pb);
            }
        }
    }

    return G_SOURCE_CONTINUE;
}

static GtkWidget* create_window (void)
{
	GtkWidget *image;
	GtkWidget *box;

    {
        GtkCssProvider *provider;
        GdkDisplay *display;
        GdkScreen *screen;

        provider = gtk_css_provider_new ();
        display = gdk_display_get_default ();
        screen = gdk_display_get_default_screen (display);
        gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

        const gchar *myCssFile = "style.css";
        GError *error = 0;

        gtk_css_provider_load_from_file(provider, g_file_new_for_path(myCssFile), &error);
        g_object_unref (provider);
        if ( error != NULL )
        {
            printf("msg: %s\n", error->message);
        }
    } 
	/* Set up the UI */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "image-viewer-c");

	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
	image = gtk_image_new ();

	gtk_box_pack_start (GTK_BOX (box), image, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (window), box);

	/* Connect signals */

	
	/* Exit when the window is closed */
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    g_timeout_add_seconds( 5, (GSourceFunc)update_image, image);

    
    update_image ( (gpointer)image);	
	return window;
}
static void
dir_changed (GFileMonitor* monitor,
	     GFile *child,
	     GFile *other_file,
	     GFileMonitorEvent event_type,
	     gpointer user_data)
{
    printf("Changed\n");
    switch (event_type) {
        default:
        case G_FILE_MONITOR_EVENT_CHANGED:
            /* ignore */
            break;
        case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
        case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
            break;
        case G_FILE_MONITOR_EVENT_DELETED:
            printf("deleted\n");
            break;
        case G_FILE_MONITOR_EVENT_CREATED:
            {
                char *path = g_file_get_path ( child );
                printf("Added: %s\n", path);
                images = g_list_append ( images, path );
                break;
            }
        case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
            /* TODO: Do something */
            break;
        case G_FILE_MONITOR_EVENT_UNMOUNTED:
            /* TODO: Do something */
            break;
    }

}


int main (int argc, char *argv[])
{
 	GtkWidget *window;

	gtk_init (&argc, &argv);


    GFile *dir = g_file_new_for_commandline_arg(argv[1]);
    GDir *d = g_dir_open(argv[1],0,NULL);
    if ( d ) {
        for ( const char *n = g_dir_read_name (d); n ; n = g_dir_read_name(d)){
            images = g_list_append( images, g_build_filename(argv[1],n, NULL) ); 
        }
        g_dir_close(d);
    }

	window = create_window ();
    gtk_window_fullscreen ( GTK_WINDOW(window) );
	gtk_widget_show_all (window);

    GFileMonitor *m = g_file_monitor_directory ( dir, G_FILE_MONITOR_NONE,NULL, NULL );
    g_signal_connect ( m , "changed", (GCallback)dir_changed, NULL);
	gtk_main ();
	return 0;
}

