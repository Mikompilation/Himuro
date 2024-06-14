#ifndef SCE_SIFRPC_H
#define SCE_SIFRPC_H

typedef struct _sif_rpc_data {
	void *paddr;
	unsigned int pid;
	int tid;
	unsigned int mode;
} sceSifRpcData;

typedef void (* sceSifEndFunc)(void *);

typedef struct _sif_client_data {
	struct _sif_rpc_data rpcd;
	unsigned int command;
	void *buff;
	void *gp;
	sceSifEndFunc func;
	void *para;
	struct _sif_serve_data *serve;
} sceSifClientData;

void sceSifInitRpc(unsigned int mode);
int sceSifBindRpc(sceSifClientData *client, unsigned int rpc_number, unsigned int mode);
int sceSifCheckStatRpc(sceSifRpcData *cd);
int sceSifCallRpc(sceSifClientData *client, unsigned int rpc_number, unsigned int mode, void * send, int ssize, void * receive, int rsize, sceSifEndFunc end_function, void *end_param);


#endif // SCE_SIFRPC_H