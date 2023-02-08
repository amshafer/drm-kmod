/*
 * Copyright 2022 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef AMDGPU_XCP_H
#define AMDGPU_XCP_H

#include <linux/xarray.h>

#define MAX_XCP 8

#define AMDGPU_XCP_MODE_NONE -1

enum AMDGPU_XCP_IP_BLOCK {
	AMDGPU_XCP_GFXHUB,
	AMDGPU_XCP_GFX,
	AMDGPU_XCP_SDMA,
	AMDGPU_XCP_VCN,
	AMDGPU_XCP_MAX_BLOCKS
};

enum AMDGPU_XCP_STATE {
	AMDGPU_XCP_PREPARE_SUSPEND,
	AMDGPU_XCP_SUSPEND,
	AMDGPU_XCP_PREPARE_RESUME,
	AMDGPU_XCP_RESUME,
};

struct amdgpu_xcp_ip_funcs {
	int (*prepare_suspend)(void *handle, uint32_t inst_mask);
	int (*suspend)(void *handle, uint32_t inst_mask);
	int (*prepare_resume)(void *handle, uint32_t inst_mask);
	int (*resume)(void *handle, uint32_t inst_mask);
};

struct amdgpu_xcp_ip {
	struct amdgpu_xcp_ip_funcs *ip_funcs;
	uint32_t inst_mask;

	enum AMDGPU_XCP_IP_BLOCK ip_id;
	bool valid;
};

struct amdgpu_xcp {
	struct amdgpu_xcp_ip ip[AMDGPU_XCP_MAX_BLOCKS];

	uint8_t id;
	uint8_t mem_node;
	bool valid;
};

struct amdgpu_xcp_mgr {
	struct amdgpu_device *adev;
	struct mutex xcp_lock;
	struct amdgpu_xcp_mgr_funcs *funcs;

	struct amdgpu_xcp xcp[MAX_XCP];
	uint8_t num_xcps;
	int8_t mode;
};

struct amdgpu_xcp_mgr_funcs {
	int (*switch_partition_mode)(struct amdgpu_xcp_mgr *xcp_mgr, int mode,
				     int *num_xcps);
	int (*query_partition_mode)(struct amdgpu_xcp_mgr *xcp_mgr);
	int (*get_ip_details)(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id,
			      enum AMDGPU_XCP_IP_BLOCK ip_id,
			      struct amdgpu_xcp_ip *ip);

	int (*prepare_suspend)(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
	int (*suspend)(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
	int (*prepare_resume)(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
	int (*resume)(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
};

int amdgpu_xcp_prepare_suspend(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
int amdgpu_xcp_suspend(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
int amdgpu_xcp_prepare_resume(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);
int amdgpu_xcp_resume(struct amdgpu_xcp_mgr *xcp_mgr, int xcp_id);

int amdgpu_xcp_mgr_init(struct amdgpu_device *adev, int init_mode,
			int init_xcps, struct amdgpu_xcp_mgr_funcs *xcp_funcs);
int amdgpu_xcp_query_partition_mode(struct amdgpu_xcp_mgr *xcp_mgr);
int amdgpu_xcp_switch_partition_mode(struct amdgpu_xcp_mgr *xcp_mgr, int mode);
int amdgpu_xcp_get_partition(struct amdgpu_xcp_mgr *xcp_mgr,
			     enum AMDGPU_XCP_IP_BLOCK ip, int instance);

#endif