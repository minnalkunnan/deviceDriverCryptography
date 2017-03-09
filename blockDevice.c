#include "blockDevice.h"
#include "tiny-AES128-C/aes.h"
#define PLAIN_SIZE 256
static int diskLength = 10;
static File **disks;

int main(void) {
   /*int i = 0, notDone = 1, errStatus, mountNum;
   char input;
   disks = (File**)malloc(sizeof(diskLength * sizeof(File*)));
   for (i = 0; i < diskLength; i++) {
      disks[i] = NULL;
   }
   
   char* blockA = (char*)malloc(BLOCKSIZE * sizeof(char));
   char* blockB = (char*)malloc(BLOCKSIZE * sizeof(char));
   char* blockC = (char*)malloc(BLOCKSIZE * sizeof(char));
   
   for (i = 0; i < BLOCKSIZE; i++) {
      blockA[i] = 'a';
      blockB[i] = 'b';
      blockC[i] = 'c';
   } 
   
   mountNum = mountDisk("files/easy.in", 4096);
   printf("easy.in mounted as disk %d\n", mountNum);
   errStatus = readBlock(0, 1, blockC);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Error shouldnt happen\n");
   }
   printf("%s\n", blockC);
   
   errStatus = writeBlock(0, 0, blockB);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Error shouldnt happen\n");
   }
   
   errStatus = writeBlock(0, 1, blockB);
   //errStatus = 0;
   if (errStatus != 0) {
      printf("Yay!\n");
   }
   
   free(blockA);
   free(blockB);
   free(blockC);
   
   for (i = 0; i < diskLength; i++) {
      unmountDisk(i);
   }
   
   //free(disks);*/
   
   //printf("%d\n", ntz("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"));
   
   
}

/*
This function opens a regular file and designates the first nBytes of it as space for the emulated disk. nBytes should be an integral number of the block size. If nBytes > 0 and there is already a file by the given filename, that file’s content may be overwritten. If nBytes is 0, an existing disk is opened, and should not be overwritten. There is no requirement to maintain integrity of any file content beyond nBytes. The return value is -1 on failure or a disk number on success.
*/
int mountDisk(char *filename, int nBytes) {
   int fileInd = 0;
   File *file;
   FILE *fp;
   
   if (nBytes < 0) {
      printf("Negative File Bytes Specified For Mounting");
      return -1;
   } else if (nBytes % BLOCKSIZE != 0) {
      printf("File Bytes Specified For Mounting Not A Multiple Of Blocksize");
      return -1;
   } else {
      fp = fopen(filename, "r+");
   }
   
   while(fileInd < diskLength && disks[fileInd] != NULL) fileInd++;
   
   if (fileInd == diskLength) {
      diskLength *= 2;
      disks = (File**)realloc(disks, diskLength);
   }
   
   file = (File*)malloc(sizeof(File));
   file->fp = fp;
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
   int size;
   
   if (disk >= diskLength || disks[disk] == NULL) {
      printf("Disk %d is not mounted. File not open.\n");
      return -1;
   }
   
   file = disks[disk];
   fp = file->fp;
   
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
   fread(block, BLOCKSIZE, 1, fp);

   //Decryption will happen here

   //fclose(fp);  
   return 0;
}

/* 
writeBlock() takes disk number ‘disk’ and logical block number ‘bNum’ and encrypts and then writes the content of the buffer ‘block’ to that location. ‘block’ must be integral with BLOCKSIZE. Just as in readBlock(), writeBlock() must translate the logical block bNum to the correct byte position in the file. On success, it returns 0. -1 or smaller is returned if disk is not available (i.e. hasn’t been opened) or any other failures. You should define your own error code system.
*/
int writeBlock(int disk, int bNum, void *block) {
   File *file;
   FILE *fp;
   int size;
   
   if (disk >= diskLength || disks[disk] == NULL) {
      printf("Disk %d is not mounted. File not open.\n");
      return -1;
   }
   
   file = disks[disk];
   fp = file->fp;
   
   if (bNum * BLOCKSIZE + BLOCKSIZE > file->nBytes) {
      printf("Cannot write to the file at this location\n");
      return -1;
   }
   
   fseek(fp, bNum * BLOCKSIZE, SEEK_SET);
   
   //Encrypt block, then write
   fwrite(block, BLOCKSIZE, 1, fp);

   

   //fclose(fp);
   return 0;
}

/*int ntz(char *str) {
   int i, j, chr;
   int mask = 1, count = 0;
   
   for (i = 15; i >= 0; i--) {
      chr = str[i];
      for (j = 0; j < 8; j++) {
         if (chr & mask == 1) {
            return count;
         } else {
            chr = chr >> 1;
            count++;
         }
      }
      mask = 1;
   }
   return count;
}*/


