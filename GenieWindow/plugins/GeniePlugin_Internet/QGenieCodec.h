#ifndef CODEC_H
#define CODEC_H
char* base64_decode(const char *p);
const char* base64_encode(const char *decoded_str);
const char* base64_encode_bin(const char *data, int len);
#endif
