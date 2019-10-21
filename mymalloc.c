#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

//uses int16_t because the array is only of size 4096, so you only need 2 bytes
struct mymallocmetadata {
  //the memory available to the user
  int16_t memSize;
  //the free space in the array following the memory
  int16_t freeSpaceAfter;
};

#define METADATA struct mymallocmetadata

#define METADATASIZE sizeof(METADATA)

#define MEMSIZE 4096


//keeps track of how much free space there is at the front of the array, before the first malloc'd item
//all other free space in the array is tracked by the struct
//freeSpaceInFront is 4096 when you start, since the array is empty
static int16_t freeSpaceInFront = MEMSIZE;

//starts at a metadata and finds where current free space ends, then returns the metadata stored there
static METADATA* getNextMetadata(METADATA* currMetaData) {
  return (METADATA*) ((char*) currMetaData +
                      METADATASIZE +
                      currMetaData->memSize +
                      currMetaData->freeSpaceAfter);

}
//finds space in memory for new metadata by finding the beginning of the freespace that follows the previous metadata
static METADATA* getNewMetadataAfter(METADATA* currMetaData) {
  return (METADATA*) ((char*) currMetaData + METADATASIZE + currMetaData->memSize);

}
//gets malloc'd memory associated with metadata
static void* getPointer (METADATA* data) {
  return (void*) ((char*) data + METADATASIZE);
}

void* mymalloc(size_t sizeRequested, char *file, int line) {
  if (sizeRequested > MEMSIZE) {
    printf("Could not allocate memory of size %zu. Input exceeds cache size. Error occured in file: %s, on line: %d.\n", sizeRequested, file, line);
    return NULL;
  }
  //the actual size you need is however big the user requested + the size of the metadata
  int16_t sizeNeeded = sizeRequested + METADATASIZE;

  //look for room to insert new thing:
  METADATA* metaDataToInsert  = NULL;
  int16_t freeSpaceAfterNew = 0;
  //if there's enough room at the beginning of the array, put the new item there
  //the free space following the new item is however much free space there was before - the space the new item is taking up
  //now there isn't any free space in the front, since you put something there
  if (freeSpaceInFront >= sizeNeeded) {
    metaDataToInsert = (METADATA*) (&myblock[0]);
    freeSpaceAfterNew = freeSpaceInFront - sizeNeeded;
    freeSpaceInFront = 0;
  }
  //if there isn't enough room at the beginning of the array, look for a space where there is
  else {
    //get a pointer to the first metadata stored in the array
    METADATA* currMetaData =(METADATA*) (myblock + freeSpaceInFront);

    //iterate til you reach the end of the array
    while (currMetaData < (METADATA*) (myblock + MEMSIZE)) {
      //if you found a space with enough room:
      //find the first free space following the current metadata
      //calculate how much freespace follows the new metadata (old free space - new metadata size)
      //the freespace after the metadata you just inserted behind = 0, since you just put the new item there
      if (currMetaData->freeSpaceAfter >= sizeNeeded) {
        metaDataToInsert = getNewMetadataAfter(currMetaData);
        freeSpaceAfterNew = currMetaData->freeSpaceAfter - sizeNeeded;
        currMetaData->freeSpaceAfter = 0;
        break;
      }

      currMetaData = getNextMetadata(currMetaData);
    }
  }

  //if you never found enough room to insert the new data, metaDataToInsert will still be null
  if (metaDataToInsert == NULL) {
    printf("Could not allocate memory of size %zu. Input exceeds cache size. Error occured in file: %s, on line: %d.\n", sizeRequested, file, line);
    return NULL;
  }
  //otherwise, you found a place to insert the new data, so insert it
  metaDataToInsert->memSize = sizeRequested;
  metaDataToInsert->freeSpaceAfter= freeSpaceAfterNew;
  return getPointer(metaDataToInsert);
}

void myfree(void* memToFree, char *file, int line) {
  //if the array is empty, you can't free anything, so return null
  //this check prevents corner case where you go to the end of the blank array and try to free it
  if (freeSpaceInFront == 4096) {
    printf("Pointer %p not freed. Error in file: %s, on line: %d.\n", memToFree, file, line);
  }
  else {
  int count =0;


  //memToFree will be pointing at the part of the block that stores the actual memory item
  //need to get it pointing to the metadata instead, so subtract metadatasize
  //cast it to a char first then turn it into metadata, since you can't do arithmetic on void pointers
  METADATA* dataToDelete = (METADATA*)(((char*)memToFree) - METADATASIZE);

  //flag to let you know if you found memToFree and freed it
  int foundPointer = 0;

  //pointer pointing to the first non-empty spot in the array
  METADATA* prevMetaData =(METADATA*) (myblock + freeSpaceInFront);

  //if what you're looking for is stored in the first empty position of the array
  if (prevMetaData==dataToDelete) {
    freeSpaceInFront+= METADATASIZE +prevMetaData->memSize + prevMetaData->freeSpaceAfter;
    foundPointer=1;
  }
  //if it's not, iterate through keeping a pointer to the current metadata and the previous metadata
  //when you find it: change the previous metadata's freeSpaceAfter to include the space from the thing you just deleted + the free space
  //following the thing you deleted
  else {
    METADATA* currMetaData = getNextMetadata(prevMetaData);
    while (currMetaData < (METADATA*) (myblock + MEMSIZE)) {
      //if you find the pointer:
      //update how much free space is following the previous item:
      //prev item's freespace = however much was there before + the size of the item you freed + the free space following the freed item
      if (currMetaData==dataToDelete) {
        foundPointer=1;
        prevMetaData->freeSpaceAfter += METADATASIZE + currMetaData->memSize + currMetaData->freeSpaceAfter;
        break;

      }
      prevMetaData = getNextMetadata(prevMetaData);
      currMetaData = getNextMetadata(currMetaData);
    }
  }
  //if you never find the pointer, print an error
  if (foundPointer==0) {
    printf("Pointer %p not freed. Error in file: %s, on line: %d.\n", memToFree, file, line);

  }
 }
}
