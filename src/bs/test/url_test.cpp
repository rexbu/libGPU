/**
 * file :	url_test.cpp
 * author :	bushaofeng
 * create :	2014-12-09 17:54
 * func : 
 * history:
 */

#include "bs_url.h"
#include "bs_socket.h"

int main(){
    const char* url1 = "stun.me-yun.com:12081/addr?user=aaa";
    const char* baidu = "www.baidu.com/";
    url_t       url;
    http_response_t response;
    char        res[10240];
    int         size;

    url_parse(&url, url1);
    url_print(&url);

    memset(res, 0, 10240);
    size = http_get(url1, res, 10240);
    printf("stun: %s\n", res);
    http_response_parse(&response, res, size);
    printf("code[%d] body_size[%d] body[%s]", response.response_code, response.body_size, bs_sock_getip((struct sockaddr_in*)response.body));

    return 0;
}
