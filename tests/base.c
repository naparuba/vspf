#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <unistd.h>

#include <fcntl.h>

#include "base.h"

//porkas rulez
data_page_rule_p dpr;//Entry de la page rule
gint pipe_tag; // sert a enregistrer le callback sur le pipe en read

void OnButtonAddRule(GtkWidget *pButton, gpointer data);
void OnButtonAddThisRule(GtkWidget *pButton, gpointer data);
void OnButtonCreateRuleFromCaract(GtkWidget *pButton, gpointer data);
void input_callback(gpointer data,gint source,GdkInputCondition condition );

void create_vbox_from_caract_list(GtkVBox * pVBox){
  gint i;
  for(i=0 ; i<10 ; i++){
    GtkWidget *pLabel1;
    GtkWidget *pButtonAccept = gtk_button_new_with_label("Accept");;
    GtkWidget *pButtonDrop = gtk_button_new_with_label("Drop");
    GtkWidget *pButtonCreateRuleFromCaract = gtk_button_new_with_label("Create Rule");;
    GtkWidget *pHBox = gtk_hbox_new(FALSE,0);
    
    
    pLabel1 = gtk_label_new(g_strdup_printf("In:eth0 "));
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    pLabel1 = gtk_label_new(g_strdup_printf("Type:UDP "));
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    pLabel1 = gtk_label_new(g_strdup_printf("AddrSrc:147.210.18.1 "));
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    pLabel1 = gtk_label_new(g_strdup_printf("AddrDst:192.168.0.3 "));
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    pLabel1 = gtk_label_new(g_strdup_printf("PortSrc:65535 "));
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    pLabel1 = gtk_label_new(g_strdup_printf("PortDst:53 ")); 
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    pLabel1 = gtk_label_new(g_strdup_printf("Prog:ping ")); 
    gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 
    
    
    gtk_box_pack_start(GTK_BOX(pHBox), pButtonDrop, FALSE, FALSE, 0); 
    gtk_box_pack_start(GTK_BOX(pHBox), pButtonAccept, FALSE, FALSE, 0);
    unsigned long *pval = malloc(sizeof(unsigned long));
    *pval = 12;
    g_signal_connect(G_OBJECT(pButtonCreateRuleFromCaract), "clicked", G_CALLBACK(OnButtonCreateRuleFromCaract), pval);
    gtk_box_pack_start(GTK_BOX(pHBox), pButtonCreateRuleFromCaract, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(pVBox), pHBox, FALSE, FALSE, 0);
  }
}


/*************** Page Frontale **************/
void create_front_page(GtkNotebook * pNotebook){
  GtkWidget *pVBoxCentral;
  GtkWidget *pVBox1;
  GtkWidget *pVBox2;
  GtkWidget *pSeparatorH;
  GtkWidget *pButtonAddRule;

  //debug
  gint i = 0;
      

  GtkWidget *pLabel2;
  GtkWidget *pTabLabel;

  pSeparatorH = gtk_hseparator_new();
  pButtonAddRule = gtk_button_new_with_label("AddRule");
  g_signal_connect(G_OBJECT(pButtonAddRule), "clicked", G_CALLBACK(OnButtonAddRule), pNotebook);      

  pVBoxCentral = gtk_vbox_new(FALSE, 0);
  pVBox1 = gtk_vbox_new(FALSE, 0);
  pVBox2 = gtk_vbox_new(FALSE, 0);
      
  /* Creation des differents GtkLabel */
  pTabLabel = gtk_label_new(g_strdup_printf("Page %d", i));

  
  pLabel2 = gtk_label_new(g_strdup_printf("Label Box 2:%d", i));
      
  create_vbox_from_caract_list(GTK_BOX(pVBox1));


  gtk_box_pack_start(GTK_BOX(pVBox2), pSeparatorH, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox2), pLabel2, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox2), pButtonAddRule, FALSE, FALSE, 0);
      
      
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBox1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBox2, FALSE, FALSE, 0);

            
  // Insertion de la page avec le widget pLabel
  gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBoxCentral, pTabLabel);
}



/******************* Page des règles *****************/
void create_rule_page_base(GtkNotebook * pNotebook){
  GtkWidget *pVBoxCentral;
  GtkWidget *pVBox1;
  GtkWidget *pVBox2;
  GtkWidget *pSeparatorH;

  GtkWidget *pEntryInDev = gtk_entry_new();
  GtkWidget *pEntryOutDev = gtk_entry_new();
  GtkWidget *pEntryIpSrc = gtk_entry_new();
  GtkWidget *pEntryIpDst = gtk_entry_new();
  
  GtkWidget *pEntryUdpPortSrc = gtk_entry_new();
  GtkWidget *pEntryUdpPortDst = gtk_entry_new();
  
  GtkWidget *pEntryTcpPortSrc = gtk_entry_new();
  GtkWidget *pEntryTcpPortDst = gtk_entry_new();
  
  GtkWidget *  pButtonAddThisRule = gtk_button_new_with_label("Add this rule");

  //  data_page_rule_p 

  //on ajoute les entry a la data
  dpr->pEntryInDev = pEntryInDev;
  dpr->pEntryOutDev = pEntryOutDev;
  dpr->pEntryIpSrc = pEntryIpSrc;
  dpr->pEntryIpDst = pEntryIpDst;
  dpr->pEntryUdpPortSrc = pEntryUdpPortSrc;
  dpr->pEntryUdpPortDst = pEntryUdpPortDst;  
  dpr->pEntryTcpPortSrc = pEntryTcpPortSrc;
  dpr->pEntryTcpPortDst = pEntryTcpPortDst;

  //debug
  gint i = 1;
  
  GtkWidget *pLabel1;
  GtkWidget *pLabel2;
  GtkWidget *pTabLabel;
  
  pSeparatorH = gtk_hseparator_new();
  
  pVBoxCentral = gtk_vbox_new(FALSE, 0);
  pVBox1 = gtk_vbox_new(FALSE, 0);
  pVBox2 = gtk_vbox_new(FALSE, 0);

  //on met le bouton add dans le vbox 2
  gtk_box_pack_start(GTK_BOX(pVBox2), pButtonAddThisRule, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(pButtonAddThisRule),"clicked",G_CALLBACK(OnButtonAddThisRule),NULL);  

  /* Creation des differents GtkLabel */
  pTabLabel = gtk_label_new(g_strdup_printf("Page %d", i));
  
  pLabel1 = gtk_label_new(g_strdup_printf("Label Box 1:%d", i));
  pLabel2 = gtk_label_new(g_strdup_printf("Label Box 2:%d", i));
  
  gtk_box_pack_start(GTK_BOX(pVBox1), pLabel1, FALSE, FALSE, 0);
  
  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("InDev:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryInDev, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("OutDev:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryOutDev, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("IpSrc:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryIpSrc, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("IpDst:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryIpDst, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("UdpPortSrc:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryUdpPortSrc, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("UdpPortDst:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryUdpPortDst, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("TcpPortSrc:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryTcpPortSrc, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox1), gtk_label_new(g_strdup_printf("TcpPortDst:")),FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(pVBox1), pEntryTcpPortDst, FALSE, FALSE, 0);


  gtk_box_pack_start(GTK_BOX(pVBox1), pSeparatorH, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(pVBox2), pLabel2, FALSE, FALSE, 0);
        
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBox1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBox2, FALSE, FALSE, 0);

            
  // Insertion de la page avec le widget pLabel
  gtk_notebook_append_page(GTK_NOTEBOOK(pNotebook), pVBoxCentral, pTabLabel);
}





int main(int argc, char **argv){
  GtkWidget *pWindow;
  GtkWidget *pNotebook;
  
  int fd;
  
  gtk_init(&argc,&argv);
  //reservation memoire des donnees des pages
  dpr = (data_page_rule_p)malloc(sizeof(data_page_rule_t));

  pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(pWindow), "GtkNotebook");
  gtk_window_set_default_size(GTK_WINDOW(pWindow), 320, 200);
  g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  
  // Creation du GtkNotebook
  // Position des onglets : en bas
  // mise dans la fenetre
  pNotebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(pNotebook), GTK_POS_BOTTOM);
  gtk_container_add(GTK_CONTAINER(pWindow), pNotebook);

  //on cree la page frontale
  create_front_page(GTK_NOTEBOOK(pNotebook));


  //on cree la page des règles et ses donnees ds dpr
  create_rule_page_base(GTK_NOTEBOOK(pNotebook));
  

  
  fd = open("./fifo_test", O_RDONLY|O_NONBLOCK);

  
  printf("Bibi %d\n", fd);
  
  
  gtk_widget_show_all(pWindow);

  pipe_tag = gdk_input_add( fd,GDK_INPUT_READ,input_callback,NULL);
  
  gtk_main();


  
  
  return EXIT_SUCCESS;
}

void OnButtonAddRule(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  GtkWidget *pChild;
  gint iPageNum;
  const gchar *sTabLabel;

  /* Recuperation de la page active */
  iPageNum = gtk_notebook_get_current_page(GTK_NOTEBOOK(data));

  //si on est deja sur la page rule, on ne fait rien
  if(iPageNum != 1){
    gchar *sDialogText;
    /* Recuperation du widget enfant */
    pChild = gtk_notebook_get_nth_page(GTK_NOTEBOOK(data), iPageNum);
    
    /* Recuperation du label de l'onglet */
    sTabLabel = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(data), pChild);
    
    /* Creation du label de la boite de dialogue */
    sDialogText = g_strdup_printf("C'est la page %d\n"
				  "Le label de l'onglet est \"%s\"\n",
				  iPageNum,
				  sTabLabel);
    
    pDialog = gtk_message_dialog_new (NULL,
				      GTK_DIALOG_MODAL,
				      GTK_MESSAGE_INFO,
				      GTK_BUTTONS_OK,
				      sDialogText);
    
    gtk_dialog_run(GTK_DIALOG(pDialog));
    
    gtk_widget_destroy(pDialog);
    
    gtk_notebook_set_current_page (GTK_NOTEBOOK(data), (gint)1);
    
    g_free(sDialogText);
  }
}

void OnButtonAddThisRule(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  
  gchar *sDialogText;
    
  /* Creation du label de la boite de dialogue */
  sDialogText = g_strdup_printf("InDev %s\nOutDev%s\nIpSrc%s\nIpDst%s\nUdpPortSrc%s\nUdpPortDst%s\nTcpPortSrc%s\nTcpPortDst%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryInDev)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryOutDev)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpSrc)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpDst)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortSrc)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortDst)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortSrc)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortDst)));
  
  pDialog = gtk_message_dialog_new (NULL,
				    GTK_DIALOG_MODAL,
				    GTK_MESSAGE_INFO,
				    GTK_BUTTONS_OK,
				    sDialogText);
  gtk_dialog_run(GTK_DIALOG(pDialog));
  gtk_widget_destroy(pDialog);
  g_free(sDialogText);
  
}

void OnButtonCreateRuleFromCaract(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  gchar *sDialogText;
  unsigned long * pcaract_id = (unsigned long*)data;
  unsigned long caract_id = *pcaract_id;
  
  /* Creation du label de la boite de dialogue */
  sDialogText = g_strdup_printf("Id: %lu \n", caract_id);
  /*
,gtk_entry_get_text(GTK_ENTRY(dpr->pEntryInDev)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryOutDev)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpSrc)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpDst)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortSrc)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortDst)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortSrc)),
				gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortDst)));*/
  
  pDialog = gtk_message_dialog_new (NULL,
				    GTK_DIALOG_MODAL,
				    GTK_MESSAGE_INFO,
				    GTK_BUTTONS_OK,
				    sDialogText);
  gtk_dialog_run(GTK_DIALOG(pDialog));
  gtk_widget_destroy(pDialog);
  g_free(sDialogText);

  
  //on set sur la page des rules avec ce qu'il faut de deja rentre
  //TODO
  
}

//attention feinte: on read, on lcose et re read bien sur
// on enleve l'ancien message et on remet le nouvo
void input_callback(gpointer data,gint source,GdkInputCondition condition ){
  //GtkWidget *pDialog;
  //gchar *sDialogText;
  gint fd;
  gint nbytes;

  printf("On a appele le callback de fifo\n");
  
  char buf[1024];
  
  nbytes=read(source,buf,1024);
  close(source);
  
  //on vire l'ancien callback
  gdk_input_remove(pipe_tag);
  
  // on remet le meme...
  fd = open("./fifo_test", O_RDONLY|O_NONBLOCK);
  pipe_tag = gdk_input_add( fd,GDK_INPUT_READ,input_callback,NULL);  
  

  /* Creation du label de la boite de dialogue */
  //sDialogText = g_strdup_printf("Data:%s\n", buf);
  //pDialog = gtk_message_dialog_new (NULL,
  //				    GTK_DIALOG_MODAL,
  //				    GTK_MESSAGE_INFO,
  //				    GTK_BUTTONS_OK,
  //				    sDialogText);
  //gtk_dialog_run(GTK_DIALOG(pDialog));
  //gtk_widget_destroy(pDialog);
  //g_free(sDialogText);
  
  printf("On a recu:%d\n", nbytes);
  
  
}
