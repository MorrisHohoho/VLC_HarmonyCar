
#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include <stdio.h>

#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "cJSON.h"

void wifi_stop_softap(void);


#define APP_INIT_VAP_NUM    2
#define APP_INIT_USR_NUM    2

static struct netif *g_lwip_netif = NULL;
extern bool led_flg;


char recvline[1024];



void udp_thread(void)
{
    int ret;
    struct sockaddr_in servaddr;
    cJSON *recvjson;

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(50001);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while(1)
    {

        /*Your code shuold implementate here*/
        struct sockaddr_in addrClient;
        int sizeClientAddr = sizeof(struct sockaddr_in);

        memset(recvline, sizeof(recvline), 0);
        ret = recvfrom(sockfd, recvline, 1024, 0, (struct sockaddr*)&addrClient,(socklen_t*)&sizeClientAddr);
        
        if(ret>0)
        {
            char *pClientIP =inet_ntoa(addrClient.sin_addr);
 
            printf("%s-%d(%d) says:%s\n",pClientIP,ntohs(addrClient.sin_port),addrClient.sin_port, recvline);

            //进行json解析
            recvjson = cJSON_Parse(recvline);

            printf("led status : %s\r\n", cJSON_GetObjectItem(recvjson, "led")->valuestring);

            if(strcmp(cJSON_GetObjectItem(recvjson, "led")->valuestring, "on") == 0)
            {
                printf("led on\r\n");
                led_flg = true;
            }
            else if(strcmp(cJSON_GetObjectItem(recvjson, "led")->valuestring, "off") == 0)
            {
                printf("led off\r\n");
                led_flg = false;
            }
            else
            {
                printf("led error\r\n");
            }
            cJSON_Delete(recvjson);
            memset(recvline, sizeof(recvline), 0);
		}
		sendto(sockfd, recvline, ret, 0, (struct sockaddr *)&addrClient, sizeClientAddr);
        /*End!!!!*/
    }
}



/* clear netif's ip, gateway and netmask */
void softap_reset_addr(struct netif *pst_lwip_netif)
{
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    if (pst_lwip_netif == NULL) {
        printf("hisi_reset_addr::Null param of netdev\r\n");
        return;
    }

    IP4_ADDR(&st_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&st_gw, 0, 0, 0, 0);
    IP4_ADDR(&st_netmask, 0, 0, 0, 0);

    netifapi_netif_set_addr(pst_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);
}

int wifi_start_softap(void)
{
    int ret;
    errno_t rc;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    int len = sizeof(ifname);
    hi_wifi_softap_config hapd_conf = {0};
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    

    rc = memcpy_s(hapd_conf.ssid, HI_WIFI_MAX_SSID_LEN + 1, "MyWIFI", 6);  /* input your SSID for example: Hispark-WiFi-IoT */
    if (rc != EOK) {
        printf("failed to memcpy_s ssid\r");
        return -1;
    }

    rc = memcpy_s(hapd_conf.key, HI_WIFI_MAX_KEY_LEN+1, "12345678", 8); /* input your password for example: 12345678 */
    if (rc != EOK) {
       printf("failed to memcpy_s key\r");
        return -1;
    }

    hapd_conf.authmode =  HI_WIFI_SECURITY_WPAPSK_WPA2PSK_MIX;
    hapd_conf.channel_num = 12;

    ret = hi_wifi_softap_start(&hapd_conf, ifname, &len);
    if (ret != HISI_OK) {
        printf("failed to started hi_wifi_softap_start\n");
        return -1;
    }

    /* acquire netif for IP operation */
    g_lwip_netif = netifapi_netif_find(ifname);
    if (g_lwip_netif == NULL) {
        printf("%s: get netif failed\n", __FUNCTION__);
        return -1;
    }

    IP4_ADDR(&st_gw, 111, 111, 111, 1);       /* input your gateway for example:192.168.10.1 */
    IP4_ADDR(&st_ipaddr, 111, 111, 111, 1);   /* input your ip for example:192.168.10.1*/
    IP4_ADDR(&st_netmask, 255, 255, 255, 0);   /*input your netmask for example:255,255,255,0*/  
    netifapi_netif_set_addr(g_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);

    netifapi_dhcps_start(g_lwip_netif, 0, 0);

    udp_thread();

    return 0;
}

void wifi_stop_softap(void)
{
    int ret;

    netifapi_dhcps_stop(g_lwip_netif);
    softap_reset_addr(g_lwip_netif);

    ret = hi_wifi_softap_stop();
    if (ret != HISI_OK) {
        printf("failed to stop softap\n");
    }

    ret = hi_wifi_deinit();
    if (ret != HISI_OK) {
        printf("failed to deinit wifi\n");
    }

    g_lwip_netif = NULL;
}

