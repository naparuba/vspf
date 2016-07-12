#ifndef BASE_H
#define BASE_H

typedef struct data_page_rule{
  GtkWidget *pEntryInDev;
  GtkWidget *pEntryOutDev;
  GtkWidget *pEntryIpSrc;
  GtkWidget *pEntryIpDst;
  GtkWidget *pEntryUdpPortSrc;
  GtkWidget *pEntryUdpPortDst;
  GtkWidget *pEntryTcpPortSrc;
  GtkWidget *pEntryTcpPortDst;
} data_page_rule_t,* data_page_rule_p;

typedef struct data_page_show_rule{
  GtkWidget *  pButtonRefresh;
} data_page_show_rule_t,* data_page_show_rule_p;


typedef struct data_front_page{
  GtkWidget *pVBoxCentral;
  GtkWidget *pVBox1;
  GtkWidget *pVBox2;
} data_front_page_t,* data_front_page_p;

typedef struct data_window{
  GtkWidget *pWindow;
  GtkWidget *pNotebook;

} data_window_t,* data_window_p;


#endif
