#include <stdlib.h>
#include <gtk/gtk.h>

int main(int argc, char **argv)
{
    GtkWidget *pWindow;
    GtkWidget *pTable;
    GtkWidget *pButton[4];

    gtk_init(&argc, &argv);

    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(pWindow), 320, 200);
    gtk_window_set_title(GTK_WINDOW(pWindow), "Les GtkTable");
    g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* Creation et insertion de la table 3 lignes 2 colonnes */
    pTable=gtk_table_new(3,2,TRUE);
    gtk_container_add(GTK_CONTAINER(pWindow), GTK_WIDGET(pTable));

    /* Creation des boutons */
    pButton[0]= gtk_button_new_with_label("Bouton 1");
    pButton[1]= gtk_button_new_with_label("Bouton 2");
    pButton[2]= gtk_button_new_with_label("Bouton 3");
    pButton[3]= gtk_button_new_with_label("Bouton 4");

    /* Insertion des boutons */
    gtk_table_attach(GTK_TABLE(pTable), pButton[0],
        0, 2, 0, 1,
        GTK_EXPAND | GTK_FILL, GTK_EXPAND,
        0, 0);
    gtk_table_attach_defaults(GTK_TABLE(pTable), pButton[1],
        0, 1, 1, 2);
    gtk_table_attach(GTK_TABLE(pTable), pButton[2],
        1, 2, 1, 2,
        GTK_EXPAND, GTK_EXPAND | GTK_FILL,
        0, 0);
    gtk_table_attach_defaults(GTK_TABLE(pTable), pButton[3],
        0, 2, 2, 3);

    gtk_widget_show_all(pWindow);

    gtk_main();

    return EXIT_SUCCESS;
}
