#ifndef SCE_SIF_H
#define SCE_SIF_H

typedef struct {
	unsigned int	data;
	unsigned int	addr;
	unsigned int	size;
	unsigned int	mode;
} sceSifDmaData;

int sceSifDmaStat(unsigned int id);
unsigned int sceSifSetDma(sceSifDmaData *sdd, int len);

#endif // SCE_SIF_H
