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




#endif
