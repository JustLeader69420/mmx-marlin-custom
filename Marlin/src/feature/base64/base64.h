#pragma

enum{
  ENCODE = 0,
  DECODE,
};

char base2num(char str);
int decode(unsigned char *original, unsigned char *ciphertext, int len);
int handleText(unsigned char *original, unsigned char *ciphertext,int len, int enorde);
void getPicSize(unsigned char*str, int*width, int*height, int*size);
int addCiphertext(unsigned char* ciphertext, unsigned char* text, int *sum);
int getGcodePic(char*path, unsigned char*str);

extern int equal_sign_num;
