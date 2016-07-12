#include <stdlib.h>
#include <gtk/gtk.h>

int main(int argc, char* argv[])
{
	GtkWidget* pWindow;
	GtkWidget* pBox;
	GtkWidget *pScrollbar;
	int i;

	gtk_init(&argc, &argv);

	pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(pWindow), 320, 200);
	gtk_window_set_title(GTK_WINDOW(pWindow), "GtkScrolledWindow");
	g_signal_connect(G_OBJECT(pWindow),"destroy",G_CALLBACK(gtk_main_quit),0);



	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(pWindow),pScrollbar);
	
	pBox=gtk_vbox_new(FALSE,5);
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(pScrollbar), pBox);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	for(i = 1 ; i <= 10 ; ++i)
	{
		GtkWidget *pLabel;
		char sTexte[10];

		g_sprintf(sTexte, "Label %d", i);

		pLabel = gtk_label_new(sTexte);

		gtk_box_pack_start(GTK_BOX(pBox), pLabel, FALSE, FALSE, 5);
	}

	gtk_widget_show_all(pWindow);

	gtk_main();

	return EXIT_SUCCESS;
}
