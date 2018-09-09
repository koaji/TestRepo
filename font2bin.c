#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

  char lineBuf[4096] = "\0";
  int  codeBuf[4][2] = {{0},{0}};
  int  carry = 0;
  int i = 0;
  int index = 0;
  int charCount = 0;
  
  FILE *fpCharSet;
  char charSetFilePath[4096] = "./hankaku.txt";
  FILE *fpCharBin;
  char charBinFilePath[4096] = "./fontBin.s";

  /* setting file path */
  if(argc >= 3){
    strcpy(charSetFilePath,argv[1]);
    strcpy(charBinFilePath,argv[2]);
  }

  printf("Input File Path  : %s\n",charSetFilePath);
  printf("Output File Path : %s\n",charBinFilePath);
  printf("\n");

  /* file check */
  fpCharSet = fopen(charSetFilePath, "r");
  if (fpCharSet == NULL){
    fprintf(stderr,"cannot open %s\n",charSetFilePath);
    return -1;
  }
  
  fpCharBin = fopen(charBinFilePath, "w");
  if ( fpCharBin == NULL ) {
    fprintf(stderr,"cannot create %s\n",charBinFilePath);
    return -1;
  }

  printf("Initialize Success.\n");

  /* write header information */
  printf("Writting header information...\n");
  fprintf(fpCharBin,".file \"%s\"\n",charBinFilePath);
  fprintf(fpCharBin,".global charset\n");
  fprintf(fpCharBin,".section .data\n");
  fprintf(fpCharBin,"charset:\n");

  /* Convert process */
  printf("Converting...\n");
  while(fscanf(fpCharSet,"%s", lineBuf) != EOF){

    if(lineBuf[0] != '.' && lineBuf[0] != '*'){
      fprintf(fpCharBin,"# %s \n",lineBuf);                /* Comment out part which is not character data.*/
      continue;
    }

    /* Convert with 4bit separetor */
    carry = 8;                                             /* Weight of digit in binary number */
    for(i = 0; i < 8; i++){                                /* Convert 8 bits  */
      if(lineBuf[i] == '.'){
	codeBuf[index][i/4] += 0;
      }else if(lineBuf[i] == '*'){
	codeBuf[index][i/4] += carry;
      }
      carry /= 2;
      if(i == 3){                                          /* Digit weight reset */
	carry = 8;
      }
    }
    
    index++;                                               /* convertion times */
    if(index == 4){                                        /* assembly code .int stores 32bits. 1 row 8bits * 4 = 32bit */
      fprintf(fpCharBin,".int    0x");                     /* Write Directive */
      for(i = 3; i >= 0; i--){                             /* Write in reverse order when using Little Endian CPU*/
	fprintf(fpCharBin,"%x%x",codeBuf[i][0],codeBuf[i][1]);
	codeBuf[i][0] = 0;                                 /* buffer Initialize*/
	codeBuf[i][1] = 0;
      }
      fprintf(fpCharBin,"\n");                             /* New Line*/
      index = 0;
      charCount++;
    }
  }

  printf("Successfully converted for %d characters.\n", charCount / 4);
  fclose(fpCharSet);
  fclose(fpCharBin);

  return 0;
}
  

