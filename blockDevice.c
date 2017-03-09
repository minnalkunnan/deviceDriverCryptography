#include "blockDevice.h"

static int diskLength = 10;
static int nonceNum = 0;
static File **disks;

int main(void) {
   int i = 0, notDone = 1, errStatus, mountNum;
   char input;
   disks = (File**)malloc(sizeof(diskLength * sizeof(File*)));
   for (i = 0; i < diskLength; i++) {
      disks[i] = NULL;
   }
   
   uint8_t nonce[16] = { '\0', };
   uint8_t key[16] = {'d', 'e', 'a', 'l', ' ', 'w', 'i', 't', 'h', ' ', 'i', 't', ' ', 'b', 'r', 'o'};
   
   uint8_t* blockA = (uint8_t*)malloc(BLOCKSIZE * sizeof(uint8_t));
   uint8_t* blockB = (uint8_t*)malloc(BLOCKSIZE * sizeof(uint8_t));
   uint8_t* blockC = (uint8_t*)malloc(BLOCKSIZE * sizeof(uint8_t));
   
   for (i = 0; i < BLOCKSIZE; i++) {
      blockA[i] = 'a';
      blockB[i] = 'b';
      blockC[i] = 'c';
   } 
   
   mountNum = mountDisk("files/easy.in", 4096);
   printf("easy.in mounted as disk %d\n", mountNum);
   /*errStatus = readBlock(0, 1, blockC);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Error shouldnt happen\n");
   }
   printf("%s\n", blockC);*/
   
   errStatus = writeBlock(0, 0, blockB);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Error shouldnt happen\n");
   }
   
   errStatus = readBlock(0, 0, blockC);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Error shouldnt happen\n");
   }
   printf("%s\n", blockC);
   
   /*errStatus = writeBlock(0, 1, blockB);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Yay!\n");
   }*/
   
   free(blockA);
   free(blockB);
   free(blockC);
   
   for (i = 0; i < diskLength; i++) {
      unmountDisk(i);
   }
   
   //free(disks);
   
   //printf("%d\n", ntz("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"));
   //uint8_t key[16];
   /*int i;
   uint8_t* largePT = (uint8_t*)malloc(BLOCKSIZE * sizeof(uint8_t));
   for (i = 0; i < BLOCKSIZE; i++) {
      largePT[i] = 'a';
   }
   uint8_t* largeCT = (uint8_t*)malloc((BLOCKSIZE + 8) * sizeof(uint8_t));
   for (i = 0; i < BLOCKSIZE + 8; i++) {
      largeCT[i] = 'a';
   }
   uint8_t pt[16] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
   uint8_t ct[16];
   uint8_t nonce[16] = { '\0', };
   uint8_t key[16] = {'d', 'e', 'a', 'l', ' ', 'w', 'i', 't', 'h', ' ', 'i', 't', ' ', 'b', 'r', 'o'};
   
   AES128_ECB_encrypt(pt, key, ct);
   
   for (i = 0; i < 16; i++) {
      printf("%d ", ct[i]);
   }
   printf("\n");
   
   AES128_ECB_decrypt(ct, key, pt);
   
   for (i = 0; i < 16; i++) {
      printf("%d ", pt[i]);
   }
   printf("\n");
   
   printf("%d\n", ntz(1));
   printf("%d\n", ntz(128));
   
   
   ocb_encrypt(largePT, nonce, largeCT, key);
   
   printf("CT");
   for (i = 0; i < BLOCKSIZE + 8; i++) {
      if (i % 16 == 0) printf("\n");
      printf("%d ", largeCT[i]);
   }
   printf("\n");
   
   ocb_decrypt(largeCT, nonce, largePT, key);
   
   printf("PT");
   for (i = 0; i < BLOCKSIZE; i++) {
      if (i % 16 == 0) printf("\n");
      printf("%d ", largePT[i]);
   }
   printf("\n");
   
   free(largePT);
   free(largeCT);*/
}

/*
This function opens a regular file and designates the first nBytes of it as space for the emulated disk. nBytes should be an integral number of the block size. If nBytes > 0 and there is already a file by the given filename, that file’s content may be overwritten. If nBytes is 0, an existing disk is opened, and should not be overwritten. There is no requirement to maintain integrity of any file content beyond nBytes. The return value is -1 on failure or a disk number on success.
*/
int mountDisk(char *filename, int nBytes) {
   int fileInd = 0, fileLen = 0, i;
   File *file;
   FILE *fp;
   FILE *fpD;
   char *dataname;
   char *data = "data";
   
   while (filename[fileLen] != '.') {
      fileLen++;
   }
   fileLen++;
   dataname = (char*)malloc(sizeof(char) * (fileLen + 4));
   for (i = 0; i < fileLen + 4; i++) {
      if (i < fileLen) {
         dataname[i] = filename[i];
      } else {
         dataname[i] = data[i-fileLen];
      }
   }
   //printf("%s\n", dataname);
   if (nBytes < 0) {
      printf("Negative File Bytes Specified For Mounting");
      return -1;
   } else if (nBytes % BLOCKSIZE != 0) {
      printf("File Bytes Specified For Mounting Not A Multiple Of Blocksize");
      return -1;
   } else {
      fp = fopen(filename, "r+");
      fpD = fopen(dataname, "r+");
   }
   free(dataname);
   while(fileInd < diskLength && disks[fileInd] != NULL) fileInd++;
   
   if (fileInd == diskLength) {
      diskLength *= 2;
      disks = (File**)realloc(disks, diskLength);
   }
   
   file = (File*)malloc(sizeof(File));
   file->fp = fp;
   file->fpData = fpD;
   file->nBytes = nBytes;
   
   disks[fileInd] = file;
   return fileInd;
}

/*
This function unmounts the open disk (identified by ‘disk’).
*/
int unmountDisk(int disk) {
   if (disk < 0 || disk > diskLength) {
      printf("Bad Disk Index For Unmount\n");
      disk = -1;
   } else if (disks[disk] == NULL) {
      printf("No Disk To Unmount At Index [%d]\n", disk);
      disk = -1;
   } else {
      fclose(disks[disk]->fp);
      fclose(disks[disk]->fpData);
      free(disks[disk]);
      disks[disk] = NULL;
   }
   return disk;
}

/*
readBlock() reads an entire block of BLOCKSIZE bytes from the open disk (identified by ‘disk’) and copies the result into a local buffer, block (which must be at least of BLOCKSIZE bytes). The bNum is a logical block number, which must be translated into a byte offset within the disk. The translation from logical to physical block is straightforward: bNum=0 is the very first byte of the file. bNum=1 is BLOCKSIZE bytes into the disk, bNum=n is n*BLOCKSIZE bytes into the disk. On success, it returns 0. -1 or smaller is returned if disk is not available (hasn’t been opened) or any other failures. readBlock will also perform the decryption operation. You should define your own error code system. 
*/
int readBlock(int disk, int bNum, void *block) {
   File *file;
   FILE *fp;
   FILE *fpData;
   int size;
   uint8_t key[16] = {'d', 'e', 'a', 'l', ' ', 'w', 'i', 't', 'h', ' ', 'i', 't', ' ', 'b', 'r', 'o'};
   uint8_t tag[12] = { '\0', };
   uint8_t blockE[BLOCKSIZE + 8] = { '\0', };
   uint8_t nonce[16] = { '\0', };
   int i;
   if (disk >= diskLength || disks[disk] == NULL) {
      printf("Disk %d is not mounted. File not open.\n");
      return -1;
   }
   
   file = disks[disk];
   fp = file->fp;
   fpData = file->fpData;
   
   fseek(fp, 0L, SEEK_END);
   size = ftell(fp);
   if (bNum * BLOCKSIZE + BLOCKSIZE > size ) {
   printf("%d\n", size);
      printf("Disk contains no memory at this location\n");
      return -1;
   }
   rewind(fp);
   
   fseek(fp, bNum * BLOCKSIZE, SEEK_SET);
   printf("LOC: %d\n", bNum * BLOCKSIZE);
   fread(blockE, BLOCKSIZE, 1, fp);

   fseek(fpData, bNum * 12, SEEK_SET);
   printf("LOC: %d\n", bNum * 12);
   fread(tag, 12, 1, fpData);
   
   //Decryption will happen here

   //Complete Ciphertext
   for (i = 0; i < 8; i++) {
      blockE[BLOCKSIZE + i] = tag[i];
   }
   for (i = 0; i < 4; i++) {
      nonce[i + 8] = tag[i + 8];      
   }
   
   ocb_decrypt(blockE, nonce, block, key);

   //fclose(fp);  
   return 0;
}

/* 
writeBlock() takes disk number ‘disk’ and logical block number ‘bNum’ and encrypts and then writes the content of the buffer ‘block’ to that location. ‘block’ must be integral with BLOCKSIZE. Just as in readBlock(), writeBlock() must translate the logical block bNum to the correct byte position in the file. On success, it returns 0. -1 or smaller is returned if disk is not available (i.e. hasn’t been opened) or any other failures. You should define your own error code system.
*/
int writeBlock(int disk, int bNum, void *block) {
   File *file;
   FILE *fp;
   FILE *fpData;
   int size, i;
      
   uint8_t key[16] = {'d', 'e', 'a', 'l', ' ', 'w', 'i', 't', 'h', ' ', 'i', 't', ' ', 'b', 'r', 'o'};
   uint8_t nonce[16] = { '\0', };
   uint8_t blockE[BLOCKSIZE + 8] = { '\0', };
   uint8_t tag[12] = { '\0', };
   
   if (disk >= diskLength || disks[disk] == NULL) {
      printf("Disk %d is not mounted. File not open.\n");
      return -1;
   }
   
   file = disks[disk];
   fp = file->fp;
   fpData = file->fpData;
   
   if (bNum * BLOCKSIZE + BLOCKSIZE > file->nBytes) {
      printf("Cannot write to the file at this location\n");
      return -1;
   }
   
   fseek(fp, bNum * BLOCKSIZE, SEEK_SET);
   
   //Encrypt block, then write
   ocb_encrypt(block, nonce, blockE, key);
   
   fwrite(blockE, BLOCKSIZE, 1, fp);

   //Write encryption data
   for (i = 0; i < 8; i++) {
      tag[i] = blockE[BLOCKSIZE + i];
   }
   tag[8] = nonceNum >> 24;
   tag[9] = nonceNum >> 16;
   tag[10] = nonceNum >> 8;
   tag[11] = nonceNum;
   
   fseek(fpData, bNum * 12, SEEK_SET);
   fwrite(tag, 12, 1, fpData);
   
   nonceNum++;
   //fclose(fp);
   return 0;
}

int ntz(int ind) {
   int i, j, chr;
   int mask = 1, count = 0;
   
   for (i = 31; i >= 0; i--) {
      //chr = str[i];
      if (ind & mask == 1) {
         return count;
      } else {
         ind = ind >> 1;
         count++;
      }
   }
   return count;
}

void memDot(uint8_t* mem, int trailing, uint8_t* to) {
   uint8_t prevNum = 0;
   uint8_t firstBit = mem[0] & 128;
   uint8_t temp;
   uint8_t mask = 1;
   int i;
   
   //Get Mask
   mask = mask << trailing;
   mask -= 1;
   
   //printf("MASK: %d\n", mask);
   
   //Shift
   for (i = 15; i >= 0; i--) {
      temp = mem[i] << trailing;
      if (prevNum) {
         temp |= prevNum;
      } else {
         temp &= (255 ^ mask);
      }
      prevNum = (mem[i] >> (8 - trailing)) & mask;
      to[i] = temp;
   }
   
   //First Bit Check
   if (firstBit) {
      to[15] = to[15] ^ 135;
   }
}

void invMemDot(uint8_t* mem, uint8_t* to) {
   uint8_t prevBit = 0;
   uint8_t lastBit = mem[15] & 1;
   uint8_t temp;
   int i;
   
   //Shift
   for (i = 0; i < 16; i++) {
      temp = mem[i] >> 1;
      if (prevBit) {
         temp |= 128;
      } else {
         temp &= 127;
      }
      prevBit = mem[i] & 1;
      to[i] = temp;
   }
   
   //Last Bit Check
   if (lastBit) {
      to[15] = to[15] ^ ((135 >> 1) & 127);
      to[0] = to[0] ^ 128;
   }
}

void ocb_encrypt(uint8_t* pt, uint8_t* nonce, uint8_t* ct, uint8_t* key) {
   uint8_t temp[16] = { '\0', };
   uint8_t temp2[16] = { '\0', };
   uint8_t L[16] = { '\0', };
   uint8_t R[16] = { '\0', };
   uint8_t X[16] = { '\0', };
   uint8_t Y[16] = { '\0', };
   uint8_t Checksum[16] = { '\0', };
   uint8_t T[8] = { '\0', };
   
   int i, j, partitions;
   
   partitions = BLOCKSIZE / 16;
   //printf("Parts: %d\n", partitions);
   
   uint8_t M[partitions][16];
   uint8_t Z[partitions][16];
   
   // Make M
   for (i = 0; i < partitions; i++) {
      for (j = 0; j < 16; j++) {
         M[i][j] = pt[(i * 16) + j];
      }
   }
   
   /*memDot(M[0], 1, temp);
   for (i = 0; i < 16; i++) {
      printf("%d ", temp[i]);
   }
   printf("\n");
   memDot(M[0], 3, temp);
   for (i = 0; i < 16; i++) {
      printf("%d ", temp[i]);
   }
   printf("\n");
   invMemDot(M[0], temp);
   for (i = 0; i < 16; i++) {
      printf("%d ", temp[i]);
   }
   printf("\n");*/
   /*printf("MESSAGE\n");
   for (i = 0; i < partitions; i++) {
      for (j = 0; j < 16; j++) {
         printf("%d ", M[i][j]);
      }
      printf("\n");
   }
   printf("MESSAGE DONE\n");*/
   
   // Make L
   AES128_ECB_encrypt(temp, key, L);
   
   // Make R
   for (i = 0; i < 16; i++) {
      temp[i] = L[i] ^ nonce[i];
   }
   AES128_ECB_encrypt(temp, key, R);
   
   // Make Z
   for (i = 0; i < 16; i++) {
      Z[0][i] = L[i] ^ R[i];
   }
   for (i = 1; i < partitions; i++) {
      //printf("NTZ: %d\n", ntz(i));
      memDot(L, ntz(i), temp);
      for (j = 0; j < 16; j++) {
         Z[i][j] = Z[i-1][j] ^ temp[j];
      }
   }
   
   // Fill in ciphertext
   for (i = 0; i < partitions - 1; i++) {
      for (j = 0; j < 16; j++) {
         temp[j] = M[i][j] ^ Z[i][j];
      }
      AES128_ECB_encrypt(temp, key, temp2);
      for (j = 0; j < 16; j++) {
         ct[(i * 16) + j] = temp2[j] ^ Z[i][j];
      }
   }
   
   // Make X
   invMemDot(L, temp);
   // xor with length force set to 256
   temp[14] = temp[14] ^ 1;
   for (j = 0; j < 16; j++) {
      X[j] = temp[j] ^ Z[partitions - 1][j];
   }
   
   // Make Y
   AES128_ECB_encrypt(X, key, Y);
   
   // Fill in last block of ciphertext
   for (j = 0; j < 16; j++) {
      ct[((partitions - 1) * 16) + j] = Y[j] ^ M[partitions - 1][j];
   }
   
   // Do checksum
   for (j = 0; j < 16; j++) {
      for (i = 0; i < partitions - 1; i++) {
         Checksum[j] = Checksum[j] ^ M[i][j];
      }
      Checksum[j] = Checksum[j] ^ ct[((partitions - 1) * 16) + j];
      Checksum[j] = Checksum[j] ^ Y[j];
   }
   
   // Fill in T
   for (i = 0; i < 16; i++) {
      temp[i] = Checksum[i] ^ Z[partitions - 1][i];
   }
   AES128_ECB_encrypt(temp, key, temp2);
   for (i = 0; i < 8; i++) {
      T[i] = temp2[i];
   }
   
   // Attach tag to ciphertext
   for (i = 0; i < 8; i++) {
      ct[(partitions * 16) + i] = T[i];
   }
   
   /*for (i = 0; i < 16; i++) {
      printf("%d ", temp[i]);
   }
   printf("\n");
   
   for (i = 0; i < 16; i++) {
      printf("%d ", L[i]);
   }
   printf("\n");
   
   for (i = 0; i < 16; i++) {
      printf("%d ", R[i]);
   }
   printf("\n");*/
}

void ocb_decrypt(uint8_t* ct, uint8_t* nonce, uint8_t* pt, uint8_t* key) {
   uint8_t temp[16] = { '\0', };
   uint8_t temp2[16] = { '\0', };
   uint8_t L[16] = { '\0', };
   uint8_t R[16] = { '\0', };
   uint8_t X[16] = { '\0', };
   uint8_t Y[16] = { '\0', };
   uint8_t Checksum[16] = { '\0', };
   uint8_t T[8] = { '\0', };
   uint8_t T2[8] = { '\0', };
   
   int i, j, partitions;
   
   partitions = BLOCKSIZE / 16;
   //printf("Parts: %d\n", partitions);
   
   uint8_t C[partitions][16];
   uint8_t Z[partitions][16];
   
   // Make C
   for (i = 0; i < partitions; i++) {
      for (j = 0; j < 16; j++) {
         C[i][j] = ct[(i * 16) + j];
      }
   }
   // Make T
   for (i = 0; i < 8; i++) {
      T[i] = ct[(partitions * 16) + i];
   }
   
   // Make L
   AES128_ECB_encrypt(temp, key, L);
   
   // Make R
   for (i = 0; i < 16; i++) {
      temp[i] = L[i] ^ nonce[i];
   }
   AES128_ECB_encrypt(temp, key, R);
   
   // Make Z
   for (i = 0; i < 16; i++) {
      Z[0][i] = L[i] ^ R[i];
   }
   for (i = 1; i < partitions; i++) {
      //printf("NTZ: %d\n", ntz(i));
      memDot(L, ntz(i), temp);
      for (j = 0; j < 16; j++) {
         Z[i][j] = Z[i-1][j] ^ temp[j];
      }
   }
   
   // Fill in plaintext
   for (i = 0; i < partitions - 1; i++) {
      for (j = 0; j < 16; j++) {
         temp[j] = C[i][j] ^ Z[i][j];
      }
      AES128_ECB_decrypt(temp, key, temp2);
      for (j = 0; j < 16; j++) {
         pt[(i * 16) + j] = temp2[j] ^ Z[i][j];
      }
   }
   
   // Make X
   invMemDot(L, temp);
   // xor with length force set to 256
   temp[14] = temp[14] ^ 1;
   for (j = 0; j < 16; j++) {
      X[j] = temp[j] ^ Z[partitions - 1][j];
   }
   
   // Make Y
   AES128_ECB_encrypt(X, key, Y);
   
   // Fill in last block of ciphertext
   for (j = 0; j < 16; j++) {
      pt[((partitions - 1) * 16) + j] = Y[j] ^ C[partitions - 1][j];
   }
   
   // Do checksum
   for (j = 0; j < 16; j++) {
      for (i = 0; i < partitions - 1; i++) {
         Checksum[j] = Checksum[j] ^ pt[(i * 16) + j];
      }
      Checksum[j] = Checksum[j] ^ C[partitions - 1][j];
      Checksum[j] = Checksum[j] ^ Y[j];
   }
   
   // Fill in T2
   for (i = 0; i < 16; i++) {
      temp[i] = Checksum[i] ^ Z[partitions - 1][i];
   }
   AES128_ECB_encrypt(temp, key, temp2);
   for (i = 0; i < 8; i++) {
      T2[i] = temp2[i];
   }
   
   j = 1;
   // Attach tag to ciphertext
   for (i = 0; i < 8; i++) {
      if (T[i] != T2[i]) {
         j = 0;
      }
   }
   
   if (!j) {
      for (i = 0; i < partitions; i++) {
         for (j = 0; j < 16; j++) {
            pt[(i * 16) + j] = '\0';
         }
      }
   }
}
