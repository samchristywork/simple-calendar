#include <gtk/gtk.h>

gboolean handle_return(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  if (event->keyval == GDK_KEY_Return) {
    gtk_dialog_response(GTK_DIALOG(data), GTK_RESPONSE_OK);
  }

  return FALSE;
}

char *ask_for_string(char *title) {
  GtkWidget *dialog =
      gtk_dialog_new_with_buttons(title, NULL, 0, "Ok", GTK_RESPONSE_OK,
                                  "Cancel", GTK_RESPONSE_CANCEL, NULL);

  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *entry = gtk_entry_new();

  gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
  gtk_container_add(GTK_CONTAINER(content_area), entry);
  gtk_widget_show_all(dialog);

  g_signal_connect(entry, "key_press_event", G_CALLBACK(handle_return), dialog);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    return strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  }

  gtk_widget_destroy(dialog);
  return NULL;
}
