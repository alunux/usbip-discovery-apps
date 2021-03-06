/*
 * Copyright (C) 2017 La Ode Muh. Fadlun Akbar <fadlun.net@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/wireless.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "detect_iface.h"

static int
check_wireless(const char* ifname)
{
    int sock = -1;
    char protocol[IFNAMSIZ] = { 0 };
    struct iwreq pwrq;

    memset(&pwrq, 0, sizeof(pwrq));
    strncpy(pwrq.ifr_name, ifname, IFNAMSIZ - 1);
    pwrq.ifr_name[IFNAMSIZ - 1] = '\0';

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 0;
    }

    if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
        strncpy(protocol, pwrq.u.name, IFNAMSIZ - 1);
        protocol[IFNAMSIZ - 1] = '\0';
        close(sock);
        return 1;
    }

    close(sock);
    return 0;
}

char*
find_wifi_interface(void)
{
    struct ifaddrs* iface_addr;
    struct ifaddrs* iter;
    char* ret_iface = NULL;
    size_t ifa_name_len;

    if (getifaddrs(&iface_addr) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    for (iter = iface_addr; iter != NULL; iter = iter->ifa_next) {
        if (iter->ifa_addr == NULL || iter->ifa_addr->sa_family != AF_PACKET) {
            continue;
        }
        if (check_wireless(iter->ifa_name)) {
            ifa_name_len = strlen(iter->ifa_name) + 1;
            ret_iface = malloc(ifa_name_len * sizeof(char));
            strncpy(ret_iface, iter->ifa_name, ifa_name_len - 1);
            ret_iface[ifa_name_len - 1] = '\0';
            break;
        }
    }

    freeifaddrs(iface_addr);
    return ret_iface;
}

const char*
get_iface_addr(void)
{
    struct ifreq ifr;
    int fd;
    char* iface_name;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    
    iface_name = find_wifi_interface();
    if (iface_name != NULL) {
        strncpy(ifr.ifr_name, "virbr0", IFNAMSIZ - 1);
        free(iface_name);
        iface_name = NULL;
    }

    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    return inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
}