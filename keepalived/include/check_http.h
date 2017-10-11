/*
 * Soft:        Keepalived is a failover program for the LVS project
 *              <www.linuxvirtualserver.org>. It monitor & manipulate
 *              a loadbalanced server pool using multi-layer checks.
 *
 * Part:        check_http.c include file.
 *
 * Authors:     Alexandre Cassen, <acassen@linux-vs.org>
 *              Jan Holmberg, <jan@artech.net>
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *              See the GNU General Public License for more details.
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Copyright (C) 2001-2012 Alexandre Cassen, <acassen@gmail.com>
 */

#ifndef _CHECK_HTTP_H
#define _CHECK_HTTP_H

/* system includes */
#include <stdio.h>
#include <openssl/md5.h>
#include <openssl/ssl.h>
#include <limits.h>

/* local includes */
#include "check_data.h"
#include "ipwrapper.h"
#include "scheduler.h"
#include "layer4.h"
#include "list.h"

/* Checker argument structure  */
/* ssl specific thread arguments defs */
typedef struct _request {
	char				*buffer;
	char				*extracted;
	int				error;
	int				status_code;
	int				len;
	int				dynamic_weight;
	SSL				*ssl;
	BIO				*bio;
	MD5_CTX				context;
} request_t;

/* http specific thread arguments defs */
typedef struct _http {
	int				retry_it;	/* current number of get retry */
	int				url_it;		/* current url checked index */
	request_t			*req;		/* GET buffer and SSL args */
} http_t ;

typedef struct _url {
	char				*path;
	char				*digest;
	int				status_code;
	int				dynamic_weight_enable;		/* enable dynamic weight body parsing */
	int				dynamic_weight_coefficient;	/* dynamic_weight value */
	int				allow_zero_dynamic_weight;	/* by default zero value equal wrong answer */
} url_t;

typedef struct _http_checker {
	int				proto;
	int				nb_get_retry;
	long				delay_before_retry;
	list				url;
	http_t				*arg;
} http_checker_t;

/* global defs */
#define MD5_BUFFER_LENGTH 32
#define GET_BUFFER_LENGTH 2048
#define MAX_BUFFER_LENGTH 4096
#define PROTO_HTTP	0x01
#define PROTO_SSL	0x02

/* GET processing command */
#define REQUEST_TEMPLATE "GET %s HTTP/1.0\r\n" \
                         "User-Agent: KeepAliveClient\r\n" \
                         "Host: %s%s\r\n\r\n"

#define REQUEST_TEMPLATE_IPV6 "GET %s HTTP/1.0\r\n" \
                         "User-Agent: KeepAliveClient\r\n" \
                         "Host: [%s]%s\r\n\r\n"

/* macro utility */
#define HTTP_ARG(X) ((X)->arg)
#define HTTP_REQ(X) ((X)->req)
#define FMT_HTTP_RS(C) FMT_CHK(C)

/* check_http functions */
#define CHECK_HTTP_MIN(a, b)	((a) < (b) ? (a) : (b))
#define CHECK_HTTP_MAX(a, b)	((a) > (b) ? (a) : (b))

/* Define prototypes */
extern void install_http_check_keyword(void);
extern int calc_next_weight_value(thread_t *);
extern int epilog(thread_t *, int, int, int);
extern int timeout_epilog(thread_t *, char *);
extern url_t *fetch_next_url(http_checker_t *);
extern int http_process_response(request_t *, int, int, int);
extern int http_handle_response(thread_t *, unsigned char digest[16]
				, int);
#endif
