#ifndef TLSCLIENT_HPP
#define TLSCLIENT_HPP

#include <lwip/altcp_tls.h>
#include <lwip/err.h>
#include <lwip/ip_addr.h>

#include <string>

namespace Network
{

class Client
{
  public:
    Client(uint8_t timeout, const uint8_t *cert = NULL, size_t cert_len = 0);
    bool open(const std::string &hostname, const std::string &request);
    int getError() const;
    void setError(int Error);
    void setTimeout(int timeout);
    std::string getRequest() const;
    altcp_pcb *getPcb() const;

  private:
    bool m_Complete;
    int m_Error;
    uint8_t m_Timeout;
    std::string m_Request;
    altcp_pcb *m_Pcb;
    altcp_tls_config *m_Config;
    void connect(const ip_addr_t *ipaddr);
    err_t close();

    static err_t tlsConnected(void *arg, struct altcp_pcb *pcb, err_t err);
    static err_t tlsReceive(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err);
    static void tlsDnsFound(const char *hostname, const ip_addr_t *ipaddr, void *arg);
    static err_t tlsPoll(void *arg, struct altcp_pcb *pcb);
    static void tlsError(void *arg, err_t err);
};

} // namespace Network

#endif /* TLSCLIENT_HPP */
