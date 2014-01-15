#ifndef __XXTEA_H__
#define __XXTEA_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned __int32 xxtea_long;

unsigned char *xxtea_encrypt(const unsigned char *data, xxtea_long len, unsigned char *key, xxtea_long *ret_len);
unsigned char *xxtea_decrypt(const unsigned char *data, xxtea_long len, unsigned char *key, xxtea_long *ret_len);

#ifdef __cplusplus
}
#endif
#endif/*__XXTEA_H__*/