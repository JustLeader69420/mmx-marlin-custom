#include "base64.h"
#include "../../inc/MarlinConfigPre.h"

#if ENABLED(SHOW_THUMBNAIL)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lodepng.h"
#include "../../sd/cardreader.h"

int equal_sign_num = 0;

// 通过base64的表转换为真实值
char base2num(char str){
  if(str == '+')
    return 62;
  else if(str == '/')
    return 63;
  else if(str == '='){
    equal_sign_num++;
    return 0;
  }
  else if(str>='0' && str<='9')
    return (str - '0' + 52);
  else if(str>='A' && str<='Z')
    return (str - 'A' + 0);
  else if(str>='a' && str<='z')
    return (str - 'a' + 26);
}
// 将密文解码
int decode(unsigned char *original, unsigned char *ciphertext, int len){
  int i,j;
  int k = len%4;
  int num = 0;
  if(ciphertext == NULL) return -1;
  // 存在余数
  if(k > 0){
    // 总数至少补1
    ciphertext[len] = '=';
    len++;
    // 余数为1，则还需再补一次
    if(k > 1){
      ciphertext[len] = '=';
      len++;
      if(k > 2){
        ciphertext[len] = '=';
        len++;
        if(k > 3){
          ciphertext[len] = '=';
          len++;
        }
      }
    }
  }

  j = len/4;equal_sign_num=0;
  /*for(i=0; i<len; i++){
      printf("%d:%d\n", i, base2num(ciphertext[i]));
  }*/
  for(i=0; i<j; i++){
    num = 0;
    // 将base64编码转换为base64字符，并将base64字符转换为ASCII码
    num = ((base2num(ciphertext[i*4+0])&0x3f)<<18) | ((base2num(ciphertext[i*4+1])&0x3f)<<12) | ((base2num(ciphertext[i*4+2])&0x3f)<<6) | (base2num(ciphertext[i*4+3])&0x3f);
    // 将ASCII字符存入original
    original[i*3+0] = (num>>16)&0xff;
    original[i*3+1] = (num>>8)&0xff;
    original[i*3+2] = (num)&0xff;
  }
  // 去除补充的码，'='
  num = j*3-equal_sign_num;
  return num;
}
// 处理文本
int handleText(unsigned char *original, unsigned char *ciphertext,int len, int enorde){
  switch(enorde){
    case ENCODE:
      break;
    case DECODE:
      return decode(original, ciphertext, len);
      break;
    default:break;
  }
}
// 获取png图片的大小
void getPicSize(unsigned char*str, int*width, int*height, int*size){
  char *p;
  const char q[] = " xX*";
  int num = 0;
  
  *width = 0; *height = 0; *size = 0;
  p = strtok((char*)str, q);

  while(NULL != p){
    printf("%s\n", p);
    if(*p >='0' && *p<='9'){
      switch(num){
        case 0:
          *width = atoi(p);
          num++;
          break;
        case 1:
          *height = atoi(p);
          num++;
          break;
        case 2:
          *size = atoi(p);
          num++;
          // return;
          break;
        default:break;
      }
    }
    p = strtok(NULL, q);
  }
  return;
}
// 将收到的信息填入将处理的密文内
int addCiphertext(unsigned char* ciphertext, unsigned char* text, int *sum){
  int i=0;
  unsigned char a;
  while(1){
    a = text[i];
    if(('\r'==a)||('\n'==a) || (0==a)) return (*sum);
    if(('+'==a) || ((a>='/')&&(a<='9')) || ((a>='A')&&(a<='Z')) || ((a>='a')&&(a<='z'))){
        ciphertext[(*sum)++] = a;
    }
    i++;
    //printf("%c\n", a);
  }
  return *sum;
}
// 将rgb转为BGR565
void encodeBGR565(unsigned char*rgb, unsigned char*bgr, unsigned int size){
  unsigned short color;
  unsigned char g,r,b;
  unsigned int i=0;
  for(i=0;i<(size/3);i++){
    r = rgb[i*3+0]>>3;
    g = rgb[i*3+1]>>2;
    b = rgb[i*3+2]>>3;
    color = (b<<11)|(g<<5)|(r<<0);

    bgr[i*2+0] = (unsigned char)color>>8;
    bgr[i*2+1] = (unsigned char)color&0xff;
  }
}
void getCiphertext(unsigned char*ciphertext, int len){
  uint16_t a;
  int i;
  while(1){
    if(i>=len) break;
    a = card.get();
    // 判断获取到的是否为密文
    if(('+'==a) || ((a>='/')&&(a<='9')) || ((a>='A')&&(a<='Z')) || ((a>='a')&&(a<='z'))){
      ciphertext[i++] = (unsigned char)a;
    }
  }
}
int getLine(unsigned char*str, int len){
  uint16_t a;
  int i;
  for(i=0; i<len; i++){
    a = card.get();
    str[i] = (unsigned char)a;
    if('\n'==a){ break; }
    if('\r'==a){ continue; }
  }
  str[i] = 0;
  return i;
}
// 获取png图片
int getGcodePic(char*_path, unsigned char*str){
  long rfile_size = 0;
  unsigned char  read_begin, a;
  unsigned char str_r[128], *str_w, *image;
  int res=0, pic_width=0, pic_height=0, pic_size=0, sum_t=0, sum=0;
  unsigned int error=0, i=0;
  // char *p,str_p[64];
  // FILE *fp = fopen(path, "r");
  
  // if(*_path!='/'){
  //   sprintf(str_p, "/%s", _path);
  //   p = str_p;
  // }
  // else p = _path;
  card.openFileRead(_path);
  if(false == card.isFileOpen()){
    return -1;
  }
  // // 获取文件大小
  // fseek(fp, 0, SEEK_END);
  // rfile_size = ftell(fp);
  // fseek(fp, 0, SEEK_SET);

  read_begin = 0;
  while(1){
    watchdog_refresh();
    if(i>=(card.getFileSize()/10) && i<50*1024 && !read_begin){ card.closefile();return -2; }
    // 当还未读取到缩略图开始时
    if(!read_begin){
      i+=getLine(str_r, 128);
      // i+=128;
      if(NULL == strstr((char*)str_r, "thumbnail begin")){ continue; }
      else getPicSize(str_r, &pic_width, &pic_height, &pic_size);

      read_begin = 1;
      if(pic_size == 0) break;
      str_w = (unsigned char *)malloc(pic_size);
      i = 0;
    }
    else{
      if(pic_size<(i+128)){
        sum_t = pic_size-(i);
      }else{
        sum_t = 128;
      }
      getCiphertext(str_r, sum_t);
      i += sum_t;
      // if(sum_t >= 312)
      {
        res = handleText(&str_w[sum], str_r, sum_t, DECODE);
        if(0 < res){
          // sum_t = 0;
          sum+=res;
          // printf("handle once succeed! sum=%d\n", sum);
        }
        // else printf("handle fail! sum=%d\n", sum);
      }
      if(sum == (pic_size/4*3-equal_sign_num)){
        read_begin = 0;
        break;
      }
    }
  }

  // 当发现缓冲区还未装满
  // if(sum_t > 0){
  //   res = handleText(&str_w[sum], str_t, sum_t, DECODE);
  //   if(0 < res){
  //     sum_t = 0;
  //     sum+=res;
  //     // printf("handle succeed! sum=%d\n", sum);
  //   }
  //   // else printf("handle fail! sum=%d\n", sum);
  // }
  if((sum == (pic_size/4*3-equal_sign_num))&&(sum>0)){
    error = lodepng_decode_memory(&image, (unsigned int *)&pic_width, (unsigned int *)&pic_height, str_w, pic_size, LCT_RGB, 8);
    if(error) { printf("error %u: %s\n", error, lodepng_error_text(error)); card.closefile(); return -3; }
    else {encodeBGR565(image, str, pic_width*pic_height*3);}

    // printf("error is %d,,width is %d ,height is %d\r\n",error,width,height);
    //lodepng_encode24_file(filename_new, image, width, height);
    //fp = fopen("E:\\face\\19_32.raw","w");//生成raw图片是这样生成的吗
    // fp = fopen("E:\\002.raw","wb+");//生成raw图片是这样生成的吗  //一定要用wb打开二进制
    // fwrite(image, width*height*3 , 1, fp);
    // // fwrite(image , 1, width*height*4, fp);
    // //fwrite(image, width*height*4 , 1, fp);
    // fclose(fp);
    free(image);
  }
  card.closefile();
  return 1;
}

#endif
