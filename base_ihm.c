#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#include "base_ihm.h"


/*From ihm.c */
#include "common.h"
#include "messages.h"
#include "rule.h"
#include "print.h"
#include "caract.h"
#include "caract_list.h"
#include "ihm.h"

#define BUFSIZE 2048


//porkas rulez
data_page_rule_p dpr;//Entry de la page rule
data_front_page_p dfp;//Entry de la page rule
data_page_show_rule_p dpsr;
data_window_p dw;//Widgets de la windows
gint pipe_tag; // sert a enregistrer le callback sur le pipe en read

//debug 
int nbpaquetenqueue;

void OnButtonAddRule(GtkWidget *pButton, gpointer data);
void OnButtonAddThisRule(GtkWidget *pButton, gpointer data);
void OnButtonCreateRuleFromCaract(GtkWidget *pButton, gpointer data);
void OnButtonRefreshPageRule(GtkWidget *pButton, gpointer data);
void input_callback(gpointer data,gint source,GdkInputCondition condition );
GtkWidget * create_hbox_from_caract(caract_t * c);
GtkWidget * create_hbox_from_rule(rule_t * rsource);
void OnButtonDrop(GtkWidget *pButton, gpointer data);
void OnButtonDropRule(GtkWidget *pButton, gpointer data);
void OnButtonAccept(GtkWidget *pButton, gpointer data);
void AskDaemonRuleList();


static ihm_t *ihm_p; /* global variable used in signal handlers only */
static void sigquit(int signum);

static void sigquit(int signum){
  printf("SIGINT, SIGTERM or SIGQUIT catched, Bye Bye\n");
  exit(0);
}

void ihm_init(ihm_t * ihm){
  ihm->ihm_to_core_fd = open(FIFO_IHM_TO_CORE, O_WRONLY | O_NONBLOCK | O_NDELAY);
  assert(ihm->ihm_to_core_fd >= 0);

  ihm->rule_tab = NULL;
  ihm->rule_tab_size = 0;
  
  struct rule r_icmp = create_void_rule();
  //creer une regle icmp
  r_icmp=fill_outdev_rule(r_icmp,(u8*)"eth0");
  r_icmp=fill_commande_rule(r_icmp,(u8*)"ping");//juste 192.168.0.3
  //r_icmp=fill_addr_src_rule(r_icmp,3232235523,3232235523);//juste 147.210.18.60 Tanit
  struct icmp_rule i = create_void_icmp_rule();
  i=fill_icmp_type_rule(i,8);//type des unreach a priori
  r_icmp=add_icmp_rule(r_icmp,i);
  r_icmp=fill_verdict_rule(r_icmp,TO_DROP);
  
  message_t m;
  u8* packet;
  m=creer_message(TYPE_RULE,sizeof(struct rule),&r_icmp);
  packet=creer_packet(m);
  
  printf("Envois de Rule taille: %d\n",sizeof(struct rule)+2*sizeof(u16));
  //write(ihm->ihm_to_core_fd, packet, sizeof(struct rule)+2*sizeof(u16));
  
  //pareil on envoi un choix
  choix_t c = creer_choix(1234,TO_ACCEPT);
  m=creer_message_choix(c);
  packet=creer_packet(m);
  printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
  //write(ihm->ihm_to_core_fd, packet,sizeof(struct choix)+2*sizeof(u16));
}




void create_vbox_from_caract_list(GtkVBox * pVBox){
  gint i;

  //on parcoure la liste des caract ihm_p->wai_caract;
  //begin mode ugly ultime
  
  caract_list_p p_temp;
  p_temp = ihm_p->wai_caract;
  while(!caract_list_is_void(p_temp))
    {
      caract_t c = p_temp->c;
      GtkWidget * pHBox = create_hbox_from_caract(&c);
      gtk_box_pack_start(GTK_BOX(pVBox), pHBox, FALSE, FALSE, 0);
      GtkWidget* pSep = gtk_hseparator_new();
      gtk_box_pack_start(GTK_BOX(pVBox), pSep, FALSE, FALSE, 0);
      p_temp=caract_list_next(p_temp);
      //      printf("p_temp = %p\n",p_temp);
    }
  
  
  
  //gtk_box_pack_start(GTK_BOX(pVBox), pHBox, FALSE, FALSE, 0);
  
}

//file un hbox a partir d'un caract
GtkWidget * create_hbox_from_caract(caract_t * csource){
  caract_t c = *csource;
    GtkWidget *pLabel1;
    GtkWidget *pButtonAccept = gtk_button_new_with_label("Accept");;
    GtkWidget *pButtonDrop = gtk_button_new_with_label("Drop");
    GtkWidget *pButtonCreateRuleFromCaract = gtk_button_new_with_label("Create Rule");;

    GtkWidget *pHBoxCentral = gtk_hbox_new(FALSE,0);
    GtkWidget *pVBoxDroite = gtk_vbox_new(FALSE,0);
    GtkWidget *pHBoxDroite = gtk_hbox_new(FALSE,0);
    GtkWidget *pVBox = gtk_vbox_new(FALSE,0);
    GtkWidget *pHBox1 = gtk_hbox_new(FALSE,0);
    GtkWidget *pHBox2 = gtk_hbox_new(FALSE,0);
    GtkWidget *pHBox3 = gtk_hbox_new(FALSE,0);

    
    
    //exemple:
    //pLabel1 = gtk_label_new(g_strdup_printf("In:eth0 "));
    //gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0); 

    int * ip = NULL;
    pLabel1 = gtk_label_new(g_strdup_printf("Id:%lu\n",c.id));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
    pLabel1 = gtk_label_new(g_strdup_printf("InDev:%s\n",c.indev));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
    pLabel1 = gtk_label_new(g_strdup_printf("OutDev:%s\n",c.outdev));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
    pLabel1 = gtk_label_new(g_strdup_printf("Commande:%s\n",c.commande));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
    
    ip=(int*)dec2ip_t(c.addr_src);
    pLabel1 = gtk_label_new(g_strdup_printf("AddrSrc:%u = ",c.addr_src));
    gtk_box_pack_start(GTK_BOX(pHBox2), pLabel1, FALSE, FALSE, 0);
    //print_ip_t(ip);
    free(ip);
    ip=(int*)dec2ip_t(c.addr_dst);
    pLabel1 = gtk_label_new(g_strdup_printf("AddrDst:%u = ",c.addr_dst));
    gtk_box_pack_start(GTK_BOX(pHBox2), pLabel1, FALSE, FALSE, 0);
    //print_ip_t(ip);
    free(ip);  
    //pLabel1 = gtk_label_new(g_strdup_printf("\n"));
    
    struct icmp_caract *tl_icmp;
    struct tcp_caract *tl_tcp;
    struct udp_caract *tl_udp;
    
    switch(c.proto){
    case IS_ICMP:
      pLabel1 = gtk_label_new(g_strdup_printf("Protocole: ICMP\n"));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      tl_icmp = (icmp_caract_t*)&(c.trans_lay);
      pLabel1 = gtk_label_new(g_strdup_printf("IcmpType:%u",tl_icmp->type));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      if(tl_icmp->type == 0) {
	pLabel1 = gtk_label_new(g_strdup_printf(" Echo Reply\n"));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      if(tl_icmp->type == 8) {
	pLabel1 = gtk_label_new(g_strdup_printf(" Echo Request\n"));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      if(tl_icmp->type == 3) {
	pLabel1 = gtk_label_new(g_strdup_printf(" Dest Unreach\n"));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      break;
    case IS_TCP:
      pLabel1 = gtk_label_new(g_strdup_printf("Protocole: TCP"));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      tl_tcp = (tcp_caract_t*)&(c.trans_lay);
      pLabel1 = gtk_label_new(g_strdup_printf("PortSrc:%u",tl_tcp->port_src));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      pLabel1 = gtk_label_new(g_strdup_printf("PortDst:%u",tl_tcp->port_dst));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      pLabel1 = gtk_label_new(g_strdup_printf("Flags:%u = ",tl_tcp->flags));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      unsigned char flags;
      //from tcpdump/print-tcp.c
      if ((flags = tl_tcp->flags) & (TH_SYN|TH_FIN|TH_RST|TH_PUSH|
				     TH_ECNECHO|TH_CWR)) {
	if (flags & TH_SYN){
	  pLabel1 = gtk_label_new(g_strdup_printf("S"));
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if (flags & TH_FIN){
	  pLabel1 = gtk_label_new(g_strdup_printf("F"));
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if (flags & TH_RST){
	  pLabel1 = gtk_label_new(g_strdup_printf("R"));
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if (flags & TH_PUSH){
	  pLabel1 = gtk_label_new(g_strdup_printf("P"));
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if (flags & TH_CWR){
	  pLabel1 = gtk_label_new(g_strdup_printf("W"));	/* congestion _W_indow reduced (ECN) */
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if (flags & TH_ECNECHO){
	  pLabel1 = gtk_label_new(g_strdup_printf("E"));	/* ecn _E_cho sent (ECN) */
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
      } else{
	pLabel1 = gtk_label_new(g_strdup_printf("."));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      //      pLabel1 = gtk_label_new(g_strdup_printf("\n"));
      break;
    case IS_UDP:
      pLabel1 = gtk_label_new(g_strdup_printf("Protocole UDP"));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      tl_udp = (udp_caract_t*)&(c.trans_lay);
      pLabel1 = gtk_label_new(g_strdup_printf("PortSrc:%u",tl_udp->port_src));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      pLabel1 = gtk_label_new(g_strdup_printf("PortDst:%u",tl_udp->port_dst));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      break;	   
    default:
      pLabel1 = gtk_label_new(g_strdup_printf("Protocole Unknow"));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
    }
    

    unsigned long *pval = malloc(sizeof(unsigned long));
    //on met la fonction ki pointe vers le id correspondant sur le bouton
    //ca resemble a du code php nan?
    //c'est marrant ca....
    *pval = c.id;
        
    g_signal_connect(G_OBJECT(pButtonDrop), "clicked", G_CALLBACK(OnButtonDrop), pval);
    g_signal_connect(G_OBJECT(pButtonAccept), "clicked", G_CALLBACK(OnButtonAccept), pval);

    gtk_box_pack_start(GTK_BOX(pHBoxDroite), pButtonDrop, FALSE, FALSE, 0); 
    gtk_box_pack_start(GTK_BOX(pHBoxDroite), pButtonAccept, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pVBoxDroite), pHBoxDroite, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pHBoxCentral), pVBox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pHBoxCentral), pVBoxDroite, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(pVBox), pHBox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pVBox), pHBox2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pVBox), pHBox3, FALSE, FALSE, 0);


    g_signal_connect(G_OBJECT(pButtonCreateRuleFromCaract), "clicked", G_CALLBACK(OnButtonCreateRuleFromCaract), pval);
    gtk_box_pack_start(GTK_BOX(pVBoxDroite), pButtonCreateRuleFromCaract, FALSE, FALSE, 0);    

    return pHBoxCentral;
}

void create_vbox_from_rule_tab(GtkVBox * pVBox){
  gint i;

  //on parcoure la liste des caract ihm_p->rule_tab;
  //taille du tableau: ihm_p->rule_tab_size
  
  for(i=0;i<ihm_p->rule_tab_size;i++){
    rule_t r = *(ihm_p->rule_tab + i);
    
    GtkWidget * pHBox = create_hbox_from_rule(&r);
    gtk_box_pack_start(GTK_BOX(pVBox), pHBox, FALSE, FALSE, 0);
    GtkWidget* pSep = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(pVBox), pSep, FALSE, FALSE, 0);
  }
}


//file un hbox a partir d'une rule
GtkWidget * create_hbox_from_rule(rule_t * rsource){
  rule_t r = *rsource;
  GtkWidget *pLabel1;
  GtkWidget *pButtonDrop = gtk_button_new_with_label("Drop");
    
  GtkWidget *pHBoxCentral = gtk_hbox_new(FALSE,0);
  GtkWidget *pVBoxDroite = gtk_vbox_new(FALSE,0);
  GtkWidget *pHBoxDroite = gtk_hbox_new(FALSE,0);
  GtkWidget *pVBox = gtk_vbox_new(FALSE,0);
  GtkWidget *pHBox1 = gtk_hbox_new(FALSE,0);
  GtkWidget *pHBox2 = gtk_hbox_new(FALSE,0);
  GtkWidget *pHBox3 = gtk_hbox_new(FALSE,0);
  
  
  
  //exemple:
  //pLabel1 = gtk_label_new(g_strdup_printf("In:eth0 "));
  //gtk_box_pack_start(GTK_BOX(pHBox), pLabel1, FALSE, FALSE, 0);
  
  int * ip = NULL;
  //pLabel1 = gtk_label_new(g_strdup_printf(""RULE:\n");

  if(r.is_indev_used == TRUE){
    pLabel1 = gtk_label_new(g_strdup_printf("InDev:%s ",r.indev));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
  }
  if(r.is_outdev_used == TRUE){
    pLabel1 = gtk_label_new(g_strdup_printf("OutDev:%s ",r.outdev));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
  }
  if(r.is_commande_used == TRUE){
    pLabel1 = gtk_label_new(g_strdup_printf("Commande:%s",r.commande));
    gtk_box_pack_start(GTK_BOX(pHBox1), pLabel1, FALSE, FALSE, 0);
  }
  if(r.is_addr_src_used == TRUE){
    ip=(int*)dec2ip_t(r.addr_src_beg);
    pLabel1 = gtk_label_new(g_strdup_printf("IpSrc:%u -",r.addr_src_beg));
    gtk_box_pack_start(GTK_BOX(pHBox2), pLabel1, FALSE, FALSE, 0);
    print_ip_t(ip);
    free(ip);
    ip=(int*)dec2ip_t(r.addr_src_end);
    pLabel1 = gtk_label_new(g_strdup_printf("%u",r.addr_src_end));
    gtk_box_pack_start(GTK_BOX(pHBox2), pLabel1, FALSE, FALSE, 0);
    print_ip_t(ip);
    free(ip);  
    //    pLabel1 = gtk_label_new(g_strdup_printf(""));
  }

  if(r.is_addr_dst_used == TRUE){
    ip=(int*)dec2ip_t(r.addr_dst_beg);
    pLabel1 = gtk_label_new(g_strdup_printf("IpDst:%u -",r.addr_dst_beg));
    gtk_box_pack_start(GTK_BOX(pHBox2), pLabel1, FALSE, FALSE, 0);
    print_ip_t(ip);
    free(ip);
    ip=(int*)dec2ip_t(r.addr_dst_end);
    pLabel1 = gtk_label_new(g_strdup_printf("%u",r.addr_dst_end));  
    gtk_box_pack_start(GTK_BOX(pHBox2), pLabel1, FALSE, FALSE, 0);
    print_ip_t(ip);
    free(ip); 
    //    pLabel1 = gtk_label_new(g_strdup_printf(""));
  }

  if(r.is_proto_used == TRUE){
    struct icmp_rule *tl_icmp;
    struct tcp_rule *tl_tcp;
    struct udp_rule *tl_udp;
    
    switch(r.proto){
    case IS_ICMP:
      pLabel1 = gtk_label_new(g_strdup_printf("ICMP: "));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      tl_icmp = (icmp_rule_t*)&(r.next_layer);
      if(tl_icmp->is_type_used == TRUE){
	pLabel1 = gtk_label_new(g_strdup_printf("Type:%u",tl_icmp->type));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	if(tl_icmp->type == 0) {pLabel1 = gtk_label_new(g_strdup_printf(" Echo Reply"));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if(tl_icmp->type == 8) {pLabel1 = gtk_label_new(g_strdup_printf(" Echo Request"));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	if(tl_icmp->type == 3) {pLabel1 = gtk_label_new(g_strdup_printf(" Dest Unreach"));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
      }
      break;
    case IS_TCP:
      pLabel1 = gtk_label_new(g_strdup_printf("TCP: "));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      tl_tcp = (tcp_rule_t*)&(r.next_layer);
      if(tl_tcp->is_port_src_used == TRUE){
	pLabel1 = gtk_label_new(g_strdup_printf("PortSrc:%u -",tl_tcp->port_src_beg));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	pLabel1 = gtk_label_new(g_strdup_printf("%u ",tl_tcp->port_src_end));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      if(tl_tcp->is_port_dst_used == TRUE){
	pLabel1 = gtk_label_new(g_strdup_printf("PortDst:%u -",tl_tcp->port_dst_beg));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	pLabel1 = gtk_label_new(g_strdup_printf("%u ",tl_tcp->port_dst_end));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      if(tl_tcp->is_flags_used == TRUE){
	pLabel1 = gtk_label_new(g_strdup_printf("Flags:%u = ",tl_tcp->flags));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	unsigned char flags;
	//from tcpdump/print-tcp.c
	if ((flags = tl_tcp->flags) & (TH_SYN|TH_FIN|TH_RST|TH_PUSH|
				       TH_ECNECHO|TH_CWR)) {
	  if (flags & TH_SYN){
	    pLabel1 = gtk_label_new(g_strdup_printf("S"));
	    gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	  }
	  if (flags & TH_FIN){
	    pLabel1 = gtk_label_new(g_strdup_printf("F"));
	    gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	  }
	  if (flags & TH_RST){
	    pLabel1 = gtk_label_new(g_strdup_printf("R"));
	    gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	  }
	  if (flags & TH_PUSH){
	    pLabel1 = gtk_label_new(g_strdup_printf("P"));
	    gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	  }
	  if (flags & TH_CWR){
	    pLabel1 = gtk_label_new(g_strdup_printf("W"));	/* congestion _W_indow reduced (ECN) */
	    gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	  }
	  if (flags & TH_ECNECHO){
	    pLabel1 = gtk_label_new(g_strdup_printf("E"));	/* ecn _E_cho sent (ECN) */
	    gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	  }
	} else{
	  pLabel1 = gtk_label_new(g_strdup_printf("."));
	  gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	}
	//	pLabel1 = gtk_label_new(g_strdup_printf(""));

      }//fin flags
      break;
    case IS_UDP:
      pLabel1 = gtk_label_new(g_strdup_printf("UDP: "));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      tl_udp = (udp_rule_t*)&(r.next_layer);
      if(tl_udp->is_port_src_used == TRUE){
	pLabel1 = gtk_label_new(g_strdup_printf("PortSrc:%u -",tl_udp->port_src_beg));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	pLabel1 = gtk_label_new(g_strdup_printf("%u ",tl_udp->port_src_end));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }
      if(tl_udp->is_port_dst_used == TRUE){
	pLabel1 = gtk_label_new(g_strdup_printf("PortDst:%u -",tl_udp->port_dst_beg));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
	pLabel1 = gtk_label_new(g_strdup_printf("%u ",tl_udp->port_dst_end));
	gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      }      
      break;	   
    default:
      pLabel1 = gtk_label_new(g_strdup_printf("Protocole Unknow"));
      gtk_box_pack_start(GTK_BOX(pHBox3), pLabel1, FALSE, FALSE, 0);
      break;
    }
  }

  u32 *pval = malloc(sizeof(u32));
  //on met la fonction ki pointe vers le id correspondant sur le bouton
  //ca resemble a du code php nan?
  //c'est marrant ca....
  *pval = r.id;
  
  g_signal_connect(G_OBJECT(pButtonDrop), "clicked", G_CALLBACK(OnButtonDropRule), pval);
  
  
  gtk_box_pack_start(GTK_BOX(pHBoxDroite), pButtonDrop, FALSE, FALSE, 0); 
  
  gtk_box_pack_start(GTK_BOX(pVBoxDroite), pHBoxDroite, FALSE, FALSE, 0);
  
  gtk_box_pack_start(GTK_BOX(pHBoxCentral), pVBox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pHBoxCentral), pVBoxDroite, FALSE, FALSE, 0);
  
  gtk_box_pack_start(GTK_BOX(pVBox), pHBox1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox), pHBox2, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox), pHBox3, FALSE, FALSE, 0);
  
  
  return pHBoxCentral;
}


/*************** Page Frontale **************/
void create_front_page(GtkNotebook * pNotebook){
  GtkWidget *pVBoxCentral;
  GtkWidget *pVBox1;
  GtkWidget *pVBox2;
  GtkWidget *pSeparatorH;
  GtkWidget *pButtonAddRule;
  GtkWidget *pScrollbar;
  
  //debug
  gint i = 0;
  
  
  GtkWidget *pLabel2;
  GtkWidget *pTabLabel;
  
  pSeparatorH = gtk_hseparator_new();
  pButtonAddRule = gtk_button_new_with_label("AddRule");
  g_signal_connect(G_OBJECT(pButtonAddRule),"clicked",G_CALLBACK(OnButtonAddRule),pNotebook);   
  
  pVBoxCentral = gtk_vbox_new(FALSE, 0);
  pVBox1 = gtk_vbox_new(FALSE, 0);
  pVBox2 = gtk_vbox_new(FALSE, 0);
  
  //on ajoute tout cela dans dfp
  dfp->pVBoxCentral = pVBoxCentral;
  dfp->pVBox1 = pVBox1;
  dfp->pVBox2 = pVBox2;
  
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
  gtk_notebook_insert_page(GTK_NOTEBOOK(pNotebook), pVBoxCentral, pTabLabel,1);
}

void recreate_front_page(){
  GtkWidget * pNotebook = dw->pNotebook;
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
  g_signal_connect(G_OBJECT(pButtonAddRule),"clicked",G_CALLBACK(OnButtonAddRule),pNotebook);   
  
  pVBoxCentral = gtk_vbox_new(FALSE, 0);
  pVBox1 = gtk_vbox_new(FALSE, 0);
  pVBox2 = gtk_vbox_new(FALSE, 0);
  
  //on ajoute tout cela dans dfp
  dfp->pVBoxCentral = pVBoxCentral;
  dfp->pVBox1 = pVBox1;
  dfp->pVBox2 = pVBox2;
  
  /* Creation des differents GtkLabel */
  pTabLabel = gtk_label_new(g_strdup_printf("Nouvelle page %d", i));
  
  pLabel2 = gtk_label_new(g_strdup_printf("Nouvo label Box 2:%d", i));
  
  create_vbox_from_caract_list(GTK_BOX(pVBox1));
  
  gtk_box_pack_start(GTK_BOX(pVBox2), pSeparatorH, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox2), pLabel2, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBox2), pButtonAddRule, FALSE, FALSE, 0);
  
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBox1, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBox2, FALSE, FALSE, 0);



  gint current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(pNotebook));
  printf("Current page = %d\n", current_page);
  // Insertion de la page avec le widget pLabel
  gtk_notebook_remove_page(GTK_NOTEBOOK(pNotebook),0);
  printf("On enleve une page\n");
  gtk_notebook_insert_page(GTK_NOTEBOOK(pNotebook), pVBoxCentral, pTabLabel,0);
  printf("On rajoute une page\n");
  
  printf("On va sur la page %d\n",current_page);


  gtk_widget_show_all(dw->pWindow);
  gtk_notebook_set_current_page (GTK_NOTEBOOK(pNotebook),current_page);
}


void recreate_show_rule_page(){  
  GtkWidget *pVBoxCentral;
  GtkWidget *pVBoxSup;
  GtkWidget * pButtonRefresh = gtk_button_new_with_label("Refresh");
  
  //  data_page_show_rule_p 
  //on ajoute les entry a la data
  dpsr->pButtonRefresh = pButtonRefresh;
  
  GtkWidget *pLabel1 = gtk_label_new(g_strdup_printf("Label Box 1"));
  GtkWidget *pTabLabel = gtk_label_new(g_strdup_printf("Voir regles - new"));
  
  pVBoxCentral = gtk_vbox_new(FALSE, 0);
  pVBoxSup = gtk_vbox_new(FALSE, 0);
  

  create_vbox_from_rule_tab(pVBoxSup);  
  
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBoxSup, FALSE, FALSE, 0);
  
  //on met le bouton refresh dans la partie inferieure
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pButtonRefresh, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(pButtonRefresh),"clicked",G_CALLBACK(OnButtonRefreshPageRule),NULL);  
  
  
  gtk_box_pack_start(GTK_BOX(pVBoxSup), pLabel1, FALSE, FALSE, 0);
  
  
  
  gint current_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(dw->pNotebook));
  printf("Current page = %d\n", current_page);
  // Insertion de la page avec le widget pLabel
  gtk_notebook_remove_page(GTK_NOTEBOOK(dw->pNotebook),2);
  printf("On enleve une page\n");
  gtk_notebook_insert_page(GTK_NOTEBOOK(dw->pNotebook), pVBoxCentral, pTabLabel,2);
  printf("On rajoute une page\n");
  
  printf("On va sur la page %d\n",current_page);
  
  
  gtk_widget_show_all(dw->pWindow);
  gtk_notebook_set_current_page (GTK_NOTEBOOK(dw->pNotebook),current_page);
  // Insertion de la page avec le widget pVBoxCentral
  //  gtk_notebook_insert_page(GTK_NOTEBOOK(dw->pNotebook), pVBoxCentral, pTabLabel,2);
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
  gtk_notebook_insert_page(GTK_NOTEBOOK(pNotebook), pVBoxCentral, pTabLabel,2);
}


/******************* Page d'affichage des règles *****************/
void create_show_rule_page(GtkNotebook * pNotebook){
  GtkWidget *pVBoxCentral;
  GtkWidget *pVBoxSup;
  GtkWidget *  pButtonRefresh = gtk_button_new_with_label("Refresh");
  
  //  data_page_show_rule_p 
  //on ajoute les entry a la data
  dpsr->pButtonRefresh = pButtonRefresh;
  
  GtkWidget *pLabel1 = gtk_label_new(g_strdup_printf("Label Box 1"));
  GtkWidget *pTabLabel = gtk_label_new(g_strdup_printf("Voir regles"));
  
  pVBoxCentral = gtk_vbox_new(FALSE, 0);
  pVBoxSup = gtk_vbox_new(FALSE, 0);
  
  
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pVBoxSup, FALSE, FALSE, 0);
  
  //on met le bouton refresh dans la partie inferieure
  gtk_box_pack_start(GTK_BOX(pVBoxCentral), pButtonRefresh, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(pButtonRefresh),"clicked",G_CALLBACK(OnButtonRefreshPageRule),NULL);  
  gtk_box_pack_start(GTK_BOX(pVBoxSup), pLabel1, FALSE, FALSE, 0);
  
  
  // Insertion de la page avec le widget pVBoxCentral
  gtk_notebook_insert_page(GTK_NOTEBOOK(pNotebook), pVBoxCentral, pTabLabel,2);
}


int main(int argc, char **argv){
  GtkWidget *pWindow;
  GtkWidget *pNotebook;
  GtkWidget *pScrollbar;

  int fd;
  
  //debug
  nbpaquetenqueue = 0;

  gtk_init(&argc,&argv);
  //reservation memoire des donnees des pages
  dpr = (data_page_rule_p)malloc(sizeof(data_page_rule_t));
  dfp = (data_front_page_p)malloc(sizeof(data_front_page_t));
  dpsr = (data_page_show_rule_p)malloc(sizeof(data_page_show_rule_p));
  dw = (data_window_p)malloc(sizeof(data_window_t));
  
  pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(pWindow), "VSPF");
  gtk_window_set_default_size(GTK_WINDOW(pWindow), 320, 400);
  g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);



  pScrollbar = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(pWindow),pScrollbar);  
  
  // Creation du GtkNotebook
  // Position des onglets : en bas
  // mise dans la fenetre
  pNotebook = gtk_notebook_new();
  
  
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(pScrollbar), pNotebook);
  
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  //on met les donnees de la window dans sa structure
  dw->pWindow = pWindow;
  dw->pNotebook = pNotebook;

  //deplacement
  ihm_t ihm;
  ihm_p = &ihm;
  signal(SIGINT, sigquit);
  signal(SIGTERM, sigquit);
  signal(SIGQUIT, sigquit);

  caract_list_p waiting_caract=caract_list_create();
  printf("Etape -3\n");
  ihm_p->wai_caract=waiting_caract;
  printf("Etape -2\n");
  
  ihm_init(&ihm);
  printf("Initialisation finie\n");

  
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(pNotebook), GTK_POS_BOTTOM);
  //gtk_container_add(GTK_CONTAINER(pWindow), pScrollbar);
  //gtk_container_add(GTK_CONTAINER(pWindow), pNotebook);
  printf("Apres init\n");  
  //on cree la page frontale
  create_front_page(GTK_NOTEBOOK(pNotebook));
  
  
  //on cree la page des règles et ses donnees ds dpr
  create_rule_page_base(GTK_NOTEBOOK(pNotebook));
  
  //on cree la page d'affichage des regles
  create_show_rule_page(GTK_NOTEBOOK(pNotebook));

  
  //Partie provenant de ihm.c
  printf("Ouverture de %s\n",FIFO_CORE_TO_IHM);
  ihm_p->core_to_ihm_fd = open(FIFO_CORE_TO_IHM, O_RDONLY| O_NDELAY);
  assert(ihm_p->core_to_ihm_fd >= 0);
  printf("Fichier %s ouvert\n",FIFO_CORE_TO_IHM);
  //fd = open("./fifo_test", O_RDONLY);
  
  //printf("Bibi %d\n", fd);
  printf("Etape 1\n");
  
  gtk_widget_show_all(pWindow);
  printf("Etape 2\n");

  pipe_tag = gdk_input_add( ihm_p->core_to_ihm_fd,GDK_INPUT_READ,input_callback,NULL);  
  printf("Etape 3\n");
  gtk_main();
  printf("Etape 4\n");
    
  
  return EXIT_SUCCESS;
}

void OnButtonAddRule(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  GtkWidget *pChild;
  gint iPageNum;
  const gchar *sTabLabel;

  /* Recuperation de la page active */
  //iPageNum = gtk_notebook_get_current_page(GTK_NOTEBOOK(data));
  iPageNum = gtk_notebook_get_current_page(dw->pNotebook);

  //si on est deja sur la page rule, on ne fait rien
  if(iPageNum != 1){
    gchar *sDialogText;
    /* Recuperation du widget enfant */
    pChild = gtk_notebook_get_nth_page(dw->pNotebook, iPageNum);
    
    /* Recuperation du label de l'onglet */
    sTabLabel = gtk_notebook_get_tab_label_text(dw->pNotebook, pChild);
    
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
    
    gtk_notebook_set_current_page (dw->pNotebook, (gint)0);
    
    g_free(sDialogText);
  }
}

void AskDaemonRuleList(){
  message_t m;
  u8* packet;
  m = creer_message(TYPE_GET_LISTE_RULE,0,NULL);
  packet = creer_packet(m);
  
  printf("Envois de Rule taille: %d\n",sizeof(message_t));
  write(ihm_p->ihm_to_core_fd, packet, sizeof(message_t));
  sync();
  close(ihm_p->ihm_to_core_fd);
  ihm_p->ihm_to_core_fd = open(FIFO_IHM_TO_CORE, O_WRONLY|O_NONBLOCK| O_NDELAY);
}

void OnButtonRefreshPageRule(GtkWidget *pButton, gpointer data){
  AskDaemonRuleList();
}

void OnButtonAddThisRule(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  gchar *sDialogText;
  struct rule r = create_void_rule();
  //creer une regle icmp
  
  if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryInDev))))
    r = fill_indev_rule(r,(u8*)gtk_entry_get_text(GTK_ENTRY(dpr->pEntryInDev)));
  //    printf("InDev:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryInDev)));
  if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryOutDev))))
    r = fill_outdev_rule(r,(u8*)gtk_entry_get_text(GTK_ENTRY(dpr->pEntryOutDev)));
  //    printf("OutDev:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryOutDev)));
  //  if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpSrc))))
  //  r = fill_addr_src_rule(r,(u8*)gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpSrc)));
  //    printf("IpSrc:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpSrc)));
  //if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpDst))))
  //  r = fill_addr_dst_rule(r,(u8*)gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpDst)));
  //    printf("IpDst:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryIpDst)));
  //if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortSrc))))
  //  printf("UdpPortSrc:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortSrc)));
  //if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortDst))))
  //  printf("UdpPortDst:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryUdpPortDst)));
  //if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortSrc))))
  //  printf("TcpPortSrc:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortDst)));
  //if(strcmp("", gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortDst))))
  //  printf("TcpPortDst:%s\n",gtk_entry_get_text(GTK_ENTRY(dpr->pEntryTcpPortDst)));

  //au cas ou :)
  r = fill_addr_src_rule(r,1111111111,1111111111);
  
  r = fill_verdict_rule(r,TO_ACCEPT);
  
  print_rule(r);
  
  message_t m;
  u8* packet;
  m = creer_message(TYPE_RULE,sizeof(struct rule),&r);
  packet = creer_packet(m);
  
  printf("Envois de Rule taille: %d\n",sizeof(struct rule)+2*sizeof(u16));
  write(ihm_p->ihm_to_core_fd, packet, sizeof(struct rule)+2*sizeof(u16));
  close(ihm_p->ihm_to_core_fd);
  ihm_p->ihm_to_core_fd = open(FIFO_IHM_TO_CORE, O_WRONLY|O_NONBLOCK| O_NDELAY);  

  // Creation du label de la boite de dialogue 
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
  caract_t c;
  
  /* Creation du label de la boite de dialogue */
  sDialogText = g_strdup_printf("Id: %lu \n", caract_id);
  
  c = caract_list_get_from_id(ihm_p->wai_caract,caract_id);
  
  print_caract(c);
  
  struct icmp_caract *tl_icmp;
  struct tcp_caract *tl_tcp;
  struct udp_caract *tl_udp;
  
  switch(c.proto){
  case IS_ICMP:
    tl_icmp = (icmp_caract_t*)&(c.trans_lay);
    //printf("\tIcmpType:%u",tl_icmp->type);
    break;
  case IS_TCP:
    tl_tcp = (tcp_caract_t*)&(c.trans_lay);
    //printf("\tPortSrc:%u\n",tl_tcp->port_src);
    //printf("\tPortDst:%u\n",tl_tcp->port_dst);
    //printf("\tFlags:%u = ",tl_tcp->flags);
    break;
  case IS_UDP:
    tl_udp = (udp_caract_t*)&(c.trans_lay);
    //printf("\tPortSrc:%u\n",tl_udp->port_src);
    //printf("\tPortDst:%u\n",tl_udp->port_dst);
    break;	   
  default:
    break;
    //printf("\tProtocole Unknow\n");
  }

  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryInDev),c.indev);
  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryOutDev),c.outdev);
  //  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryIpSrc),c.addr_src);
  //gtk_entry_set_text(GTK_ENTRY(dpr->pEntryIpDst),c.addr_src);
  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryUdpPortSrc),"prout");
  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryUdpPortDst),"prout");
  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryTcpPortSrc),"prout");
  gtk_entry_set_text(GTK_ENTRY(dpr->pEntryTcpPortDst),"prout");
  
  g_free(sDialogText);

  
  //on set sur la page des rules avec ce qu'il faut de deja rentre
  //TODO
  
}

void OnButtonDrop(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  gchar *sDialogText;
  unsigned long * pcaract_id = (unsigned long*)data;
  unsigned long caract_id = *pcaract_id;
  choix_t c = creer_choix(caract_id,TO_DROP);

  printf("On Drop le paquet id:%lu \n",caract_id);
  ihm_p->wai_caract = caract_list_apply(ihm_p->ihm_to_core_fd,ihm_p->wai_caract,c);  
  recreate_front_page();
}

void OnButtonAccept(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  gchar *sDialogText;
  unsigned long * pcaract_id = (unsigned long*)data;
  unsigned long caract_id = *pcaract_id;
  choix_t c = creer_choix(caract_id,TO_ACCEPT);
  
  printf("On Accept le paquet id:%lu \n",caract_id);
  ihm_p->wai_caract = caract_list_apply(ihm_p->ihm_to_core_fd,ihm_p->wai_caract,c);  
  recreate_front_page();
}


void OnButtonDropRule(GtkWidget *pButton, gpointer data){
  GtkWidget *pDialog;
  gchar *sDialogText;
  u32 * prule_id = (u32*)data;
  u32 rule_id = *prule_id;
  //choix_t c = creer_choix(caract_id,TO_DROP);
  printf("On DROP la rule %d\n",rule_id);
  //printf("On Drop le paquet id:%lu \n",caract_id);
  //ihm_p->wai_caract = apply_choix_caract(ihm_p->ihm_to_core_fd,ihm_p->wai_caract,c);  
  
  
  message_t m;
  u8* packet;
  m=creer_message(TYPE_REMOVE_RULE,sizeof(u32),&rule_id);
  packet=creer_packet(m);
  
  printf("Envois de supression de taille: %d\n",sizeof(u32)+2*sizeof(u16));
  write(ihm_p->ihm_to_core_fd, packet, sizeof(u32)+2*sizeof(u16));
  sync();
  close(ihm_p->ihm_to_core_fd);
  ihm_p->ihm_to_core_fd = open(FIFO_IHM_TO_CORE, O_WRONLY|O_NONBLOCK| O_NDELAY);

  printf("On a envoye la demande de supression\n");
  AskDaemonRuleList();
  //  printf("On fait une demande
  //recreate_show_rule_page();
}

//attention feinte: on read, on lcose et re read bien sur
// on enleve l'ancien message et on remet le nouvo
void input_callback(gpointer data,gint source,GdkInputCondition condition ){
  //GtkWidget *pDialog;
  //gchar *sDialogText;

  message_t *m;
  caract_t *r;

  gint nbytes;
  int tmp;
  int nb_packet = 10;

  printf("On a appele le callback de fifo\n");
  
  char buf[BUFSIZE];

  printf("On lit sur %s\n",FIFO_CORE_TO_IHM);
  nbytes=read(source,buf,BUFSIZE);
  printf("On a lu sur le pipe\n");
  close(source);
  printf("On ferme le fichier\n");
  //on vire l'ancien callback
  gdk_input_remove(pipe_tag);
  printf("On a retire le callback\n");
  
  // on remet le meme...
  ihm_p->core_to_ihm_fd = open(FIFO_CORE_TO_IHM, O_RDONLY| O_NDELAY);  
  printf("On a reouvert le pipe\n");
  //  fd = open("./fifo_test", O_RDONLY|O_NONBLOCK);
  pipe_tag = gdk_input_add( ihm_p->core_to_ihm_fd,GDK_INPUT_READ,input_callback,NULL);  
  printf("On a remi le callback\n");
  printf("On a recu:%d\n", nbytes);
  
  
  
  //Partie recopiee:
  //on a le buffer, on peut faire le traitement
  
  
  //  nbytes = read(ihm_p->core_to_ihm_fd, buf, BUFSIZE);  
  if(nbytes < 0){
    printf("Problemes dans la lecture de %s car %i\n",FIFO_CORE_TO_IHM,nbytes);
  }else{
    tmp = (nbytes<BUFSIZE?nbytes:BUFSIZE); 
    //on met un 0 ds le dernier caract
    //    buf[tmp] = '\0';
    printf("On a receptionne un message de taille %d\n",tmp);   
    m=(message_t*)buf;
    printf("Get Type:%d Len:%d\n",m->type,m->len);
    if(m->type == TYPE_CARACT){
      printf("On a receptionne un caract\n");
      r = (struct caract*)(&(m->payload));
      caract_t r2 = *r;
      print_caract(r2);
      ihm_p->wai_caract = caract_list_add(ihm_p->wai_caract,r2);
    }
    if(m->type == TYPE_LISTE_RULE){
      //on recoi la liste des rules
      printf("On recoi la liste des rules\n");
      struct rule * rule_tab = (struct rule *)(&(m->payload));
      int nbRule = (m->len - 2*sizeof(u16))/sizeof(struct rule);
      if((m->len - 2*sizeof(u16)) > 0){
	struct rule * rule_tab_ihm = (struct rule *)malloc((m->len - 2*sizeof(u16))*sizeof(struct rule));
	printf("Taille: %d\n",(m->len - 2*sizeof(u16)));
	printf("Nombre Rule: %d\n",nbRule);
	memcpy(rule_tab_ihm,rule_tab,(m->len - 2*sizeof(u16)));
	printf("Apres memcpy\n");
	if(ihm_p->rule_tab != NULL)
	  free(ihm_p->rule_tab);
	printf("apres free\n");
	ihm_p->rule_tab = rule_tab_ihm;
	ihm_p->rule_tab_size = nbRule;
	
	printf("On a %d rules\n",nbRule);
	//maintenant on parcoure les rule :)
	int i=0;
	for(i=0;i<nbRule;i++)
	  print_rule( *(rule_tab_ihm+i) );
	recreate_show_rule_page();
      }
    }
    /*
      printf("On renvoi TO_ACCEPT au packet:\n");
      message_t m_choix;
      u8* packet_choix;
      //pareil on envoi un choix
      choix_t c = creer_choix(r2.id,TO_ACCEPT);
      m_choix=creer_message_choix(c);
      packet_choix=creer_packet(m_choix);
      printf("Envois de Choix taille: %d\n",sizeof(struct choix)+2*sizeof(u16));
      print_choix(c);
      write(ihm_to_core_fd, packet_choix,sizeof(struct choix)+2*sizeof(u16));
    */
      
    //printf("Policy a jour:\n");
    //close(ihm_p->core_to_ihm_fd); plus besoin, fait au dessus
  } 


  //test
  recreate_front_page();
  
}
