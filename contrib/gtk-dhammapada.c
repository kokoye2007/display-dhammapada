/* gtk-dhammapada.c:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Copyright (C) 2003 Josh Buhl
 *
 * Alot of this code was originally in the fish.c file from the Gnome
 * Fish Applet. -- Josh Buhl
 *
 * Authors:
 *      Josh Buhl <jbuhl@users.sourceforge.net>
 *      George Lebl  <jirka@5z.com> (Gnome Fish Applet)
 *
 * version: 0.1
 * date: 2003.04.13
 */

#include <string.h>
#include <gtk/gtk.h>

#define WINDOW_TITLE "The Wisdom of Buddha"

GtkWidget *dhammapada_dialog;
GtkWidget *dhammapada_view;
GtkWidget *dhammapada_buffer;


static void display_dhammapada_dialog ();

static gboolean
delete_event (GtkWidget  *widget)
{
	gtk_main_quit();
}

static void
handle_response (GtkWidget  *widget,
		 int         id)
{
	switch(id)
	{
	case 1 : display_dhammapada_dialog(widget);
		break;
	case GTK_RESPONSE_CLOSE : delete_event(widget);
		break;
	}

	return;
		
}

static char *
locate_dhammapada_command ()
{
	char *retval = NULL;

	if (!retval)
		retval = g_find_program_in_path ("display-dhammapada");

	if (!retval && g_file_test ("/usr/bin/display-dhammapada",
				    G_FILE_TEST_EXISTS))
		retval = g_strdup ("/usr/bin/display-dhammapada");

	if (!retval && g_file_test ("/usr/local/bin/display-dhammapada",
				    G_FILE_TEST_EXISTS))
		retval = g_strdup ("/usr/local/bin/display-dhammapada");

 	if (!retval)
	{
		GtkWidget *dialog;
		char *message;		
		message = "Unable to find command display-dhammapada in path";
		g_message(message);
	
		dialog = gtk_message_dialog_new (
			NULL,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,
			message, NULL);

		g_signal_connect (dialog, "response",
				  G_CALLBACK (gtk_widget_destroy),
				  NULL);

		gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
		gtk_widget_show (dialog);

	}

	return retval;
}

static void
insert_dhammapada_text (const char *text)
{
	GtkTextIter iter;

	gtk_text_buffer_get_iter_at_offset (dhammapada_buffer, &iter, -1);

	gtk_text_buffer_insert (dhammapada_buffer, &iter,
				text, -1);
}

static void
clear_dhammapada_text ()
{
	GtkTextIter begin, end;

	gtk_text_buffer_get_iter_at_offset (dhammapada_buffer, &begin, 0);
	gtk_text_buffer_get_iter_at_offset (dhammapada_buffer, &end, -1);
	gtk_text_buffer_delete (dhammapada_buffer, &begin, &end);

	/* insert an empty line */
	insert_dhammapada_text ("\n");
}

static void
create_dhammapada_dialog()
{
	GtkWidget *scrolled;
	GdkScreen *screen;
	int        screen_width;
	int        screen_height;

	dhammapada_dialog =
		gtk_dialog_new_with_buttons (
			"", NULL, 0,
			"Next", 1,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
			NULL);

	g_signal_connect (dhammapada_dialog, "delete_event",
			  G_CALLBACK (delete_event), dhammapada_dialog);

	g_signal_connect (dhammapada_dialog, "response",
			  G_CALLBACK (handle_response), dhammapada_dialog);
 
	screen = gtk_widget_get_screen (GTK_WIDGET (dhammapada_dialog));

	screen_width  = gdk_screen_get_width (screen);
	screen_height = gdk_screen_get_height (screen);

	gtk_window_set_screen (GTK_WINDOW (dhammapada_dialog), screen);

	gtk_window_set_default_size (GTK_WINDOW (dhammapada_dialog),
				     MIN (580, screen_width  * 0.9),
				     MIN (280, screen_height * 0.9));

	dhammapada_view = gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (dhammapada_view), FALSE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (dhammapada_view), FALSE);
	gtk_text_view_set_left_margin (GTK_TEXT_VIEW (dhammapada_view), 10);
	gtk_text_view_set_right_margin (GTK_TEXT_VIEW (dhammapada_view), 10);
	dhammapada_buffer =
		gtk_text_view_get_buffer (GTK_TEXT_VIEW (dhammapada_view));

	scrolled = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled),
					     GTK_SHADOW_IN);

	gtk_container_add (GTK_CONTAINER (scrolled), dhammapada_view);


	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dhammapada_dialog)->vbox),
			    scrolled, TRUE, TRUE, 0);

	gtk_window_set_title (GTK_WINDOW (dhammapada_dialog), WINDOW_TITLE);

	gtk_widget_show_all (dhammapada_dialog);

}

static void 
display_dhammapada_dialog ()
{
	GError *error = NULL;
	char   *dhammapada_command;
	char   *output = NULL;
	char   *utf8_output;

	dhammapada_command = locate_dhammapada_command ();
	if (!dhammapada_command)
		return;
	if (!dhammapada_dialog)
		return;

	gtk_window_set_screen (GTK_WINDOW (dhammapada_dialog),
			       gtk_widget_get_screen (GTK_WIDGET (dhammapada_dialog)));
	gtk_window_present (GTK_WINDOW (dhammapada_dialog));

	clear_dhammapada_text();

	g_spawn_command_line_sync (dhammapada_command, &output, NULL, NULL, &error);
	if (error) {
		char *message;

		message = g_strdup_printf ("Unable to execute '%s'\n\nDetails: %s",
					   dhammapada_command, error->message);
		g_message(message);
		g_free (message);
		g_error_free (error);
	}
	
	g_free (dhammapada_command);

	/* The output is not guarantied to be in UTF-8 format, most
	 * likely it's just in ASCII-7 or in the user locale
	 */
	if (!g_utf8_validate (output, -1, NULL))
		utf8_output = g_locale_to_utf8 (output, -1, NULL, NULL, NULL);
	else
		utf8_output = g_strdup (output);

	if (utf8_output)
		insert_dhammapada_text (utf8_output);
	else
		insert_dhammapada_text ("display-dhammapada did not generate any output.");

	g_free (output);
	g_free (utf8_output);
}

int main(int argc, char *argv[] )
{
    
    gtk_init (&argc, &argv);

    create_dhammapada_dialog();

    display_dhammapada_dialog();

    gtk_main ();

    return 0;
}
