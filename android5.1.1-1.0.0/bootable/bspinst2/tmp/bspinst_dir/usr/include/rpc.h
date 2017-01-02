/*
 * Copyright (c) 2008-2011 WonderMedia Technologies, Inc. All Rights Reserved.
 *
 * This PROPRIETARY SOFTWARE is the property of WonderMedia Technologies, Inc.
 * and may contain trade secrets and/or other confidential information of
 * WonderMedia Technologies, Inc. This file shall not be disclosed to any
 * third party, in whole or in part, without prior written consent of
 * WonderMedia.
 *
 * THIS PROPRIETARY SOFTWARE AND ANY RELATED DOCUMENTATION ARE PROVIDED
 * AS IS, WITH ALL FAULTS, AND WITHOUT WARRANTY OF ANY KIND EITHER EXPRESS
 * OR IMPLIED, AND WONDERMEDIA TECHNOLOGIES, INC. DISCLAIMS ALL EXPRESS
 * OR IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * QUIET ENJOYMENT OR NON-INFRINGEMENT.
 */
#ifndef RPC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum rpc_cmd {
	rpc_es_ctrl,
};

#define MAX_RPC_DATA_LEN 128
#define RPC_VERSION "v1.0.0"

struct rpc_cmd_packet {
	struct {
		enum rpc_cmd cmd;
		unsigned int dsize;
	} hdr;
	char data[MAX_RPC_DATA_LEN];
};

int rpc_open(int *fd);
int rpc_close(int fd);
int rpc_ctrl(int fd, char *cmd);
int applet_rpc(int argc, char *argv[]);
int get_whole_file_name(char *f, char *fn, int size);
void sed(char *s, char *s1, char *s2, int bufsize);

#endif

