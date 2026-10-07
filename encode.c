#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include <unistd.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
/// get the size of the bmp or source file

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: Error: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: Error: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: Error: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{  //check user enter the 2nd arg as .bmp file
  char * src_address =strstr(argv[2],".");
    if(argv[2]!=NULL&&src_address!=NULL&&strcmp(src_address,".bmp")==0)
    {
       encInfo->src_image_fname=argv[2];
       
    }
    else
    {     fprintf(stderr,"INFO: Error: source img file should be a .bmp file\n");

        return e_failure;
    }
    //check user enter the third arg as .txt file
    char *secret_address = strstr(argv[3], ".");
    if(argv[3]!=NULL&&secret_address!=NULL&&strcmp(secret_address,".txt")==0)
    {
      encInfo->secret_fname=argv[3];
     

    }
   else
   {     
    fprintf(stderr,"INFO: Error: secrete  file should be a .txt file\n");

    return e_failure;
   }
   //check whether user passs output file name or not
    char *stego_address = strstr(argv[4], ".");
    if(argv[4]!=NULL&&stego_address!=NULL&&strcmp(stego_address,".bmp")==0)
    {
      encInfo->stego_image_fname=argv[4];
      return e_success;

    }
   else
   {     
     encInfo->stego_image_fname="stego.bmp";
     
   }
         
   return e_success;


}
//get the size of secret file
uint get_file_size(FILE *fptr)
{
  fseek(fptr,0,SEEK_END);
  return ftell(fptr);//return the current file position wich is a size of the file

}



Status check_capacity(EncodeInfo *encInfo)
{
  encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
  encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
   
   /* 1)  54 bytes for bmp header,
      2)  2bytes for magic string/secrete key,
      3)  4bytes character in secrete file  extension =.txt,
      4)  4 bytes for size in int of secrete file extension ,
      5)  4 bytes for secrete message size,
      6)  encInfo->size_secret_file for secrete message*/
   if( encInfo->image_capacity>=(54+(2+4+4+4+encInfo->size_secret_file)*8))
   {
    return e_success;
   }
   else
   {
    fprintf(stderr,"INFO: Error:file has insufficient capacity\n");
    return e_failure;
   }
}//copying the BMP header to stego image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  rewind(fptr_src_image);
  char header[54];
  fread(header,sizeof(char),54,fptr_src_image);
  fwrite(header,sizeof(char),54,fptr_dest_image);
  return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned mask = 1 << 7;
  for(int i=0;i<8;i++)
  {
    image_buffer[i]=(image_buffer[i]&0xFE) | ((data & mask)>>(7-i));
    mask=mask>>1;
  }
}
Status encode_data_to_image(const char *data,int size,EncodeInfo *encInfo)
{
  //take character from data and fetch 8 bytes fromsrc image and encode it
  for(int i=0;i<size;i++)
  {
    //read 8 bytes of RGB from src Image
    fread(encInfo->image_data,8,sizeof(char),encInfo->fptr_src_image);
      //encode byte to lsb will do the real encoding of data
      encode_byte_to_lsb(data[i],encInfo->image_data);
      //write the encoded data to back to the desitnation image
     fwrite(encInfo->image_data,8,sizeof(char),encInfo->fptr_stego_image);

  }
  return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
 //every character encoding needs to call encode _data_to_image
 encode_data_to_image(magic_string,strlen(magic_string),encInfo);
}
Status encode_size_to_lsb(char *image_buffer, int size)
{
    unsigned mask = 1 << 31;
  for(int i=0;i<32;i++)
  {
    image_buffer[i]=(image_buffer[i]&0xFE) | ((size & mask)>>(31-i));
    mask=mask>>1;
  }
}
/* Encode the secret file extension size*/
Status encode_secret_file_extn_size(int size,FILE *fptr_src,FILE *fptr_stego)
{
  //to encoded size of 4bytesi nedd 32 bytes rgb
  char rgb[32];
  fread(rgb,32,sizeof(char),fptr_src);
  encode_size_to_lsb(rgb,size);
  fwrite(rgb,32,sizeof(char),fptr_stego);
  return e_success;
}

  

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_ext, EncodeInfo *encInfo)
{
  file_ext=".txt";
    encode_data_to_image(file_ext,strlen(file_ext),encInfo);
    return e_success;
}
/*encoding the size of secret file*/
Status encode_secret_file_size(long int size, EncodeInfo *encInfo)
{
  //to encoded size of 4bytesi nedd 32 bytes rgb
  char rgb[32];
  fread(rgb,32,sizeof(char),encInfo->fptr_src_image);
  encode_size_to_lsb(rgb,size);
  fwrite(rgb,32,sizeof(char),encInfo->fptr_stego_image);
  return e_success;
}
/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
  char ch;
  //bring the secret file pointer to the 0th position
  fseek(encInfo->fptr_secret,0,SEEK_SET);
  for(int i=0;i < encInfo->size_secret_file;i++)
  {
     fread(&ch,1,sizeof(char),encInfo->fptr_secret);
     //read 8 bytes of rgb
     fread(encInfo->image_data,sizeof(char),8,encInfo->fptr_src_image);
     encode_byte_to_lsb(ch,encInfo->image_data);
     fwrite(encInfo->image_data,8,sizeof(char),encInfo->fptr_stego_image);
  }
  for(int i = 50; i <= 100; i++)
{
    printf("\rEncoding secret file data %d%%", i);
    if(i == 100)
    {
        printf("\rINFO: Successfully!! encoded secret file data\n");
        break;

    }
    fflush(stdout);
    usleep(50000);
}
 

  return e_success;
}
///* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
  char ch;
  while(fread(&ch,1,sizeof(char),fptr_src)>0)
  {
     fwrite(&ch,1,sizeof(char),fptr_dest);
  }
  return e_success;
}

//all rest if encoding function is here
Status do_encoding(EncodeInfo *encInfo)
{
  //open all required file
  if(open_files(encInfo)==e_success)
  {
    printf("INFO: All files are opened successfully\n");
    //check capacity of image for hiding  info
    if(check_capacity(encInfo)==e_success)
    {
      printf("INFO: Capacity checked passed\n");
      //copy the header to stego image
      if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
      {
        printf("INFO: BMP header copied successfully\n");
        //encode the magric string
        if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
        {
         printf("INFO: Magic string encoded successfully\n");
         //encode the secret file extenxion size
          if(encode_secret_file_extn_size(strlen(".txt"),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
          {
            printf("INFO: Secret file extension size encoded successfully\n");
            //encode the secret file extension
            if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
            {
              printf("INFO: Successfully encoded secret file extension\n");
              //encode the secret file/data size 
              if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
              {
                //printf("successfully!! encoding the size of the secret file\n");
                //encode the secret file data
                if(encode_secret_file_data(encInfo)==e_success)
                {
                 // printf("successfully!! encoded the secret file data\n");
                  //copy reamining image data
                  if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
                  {
                    printf("INFO: successfully! copying the reamining data\n");

                  }
                  else
                  {
                    printf("INFO: Error: failure of coping the reamining data\n");
                  }
                }
                else
                {
                  printf("INFO: Error:  failure to encoded the secret file data\n");
                  return e_failure;
                }
              }
              else
              {
                printf("INFO: Error: failure of encoding the size of the secret file\n");
              }
            }
            else
            {
              printf("INFO: Error: failure to encode the secret file extension\n");
              return e_failure;
            }
          }
          else
          {
            printf("INFO: Error: Failed to encode the extension size\n");
            return e_failure;
          }

        }
        else
        {
          printf("INFO: Error: Encoding magic string failure\n");
          return e_failure;
        }
      }
      else
      {
        printf("INFO: Error: in coping the BMP header \n");
        return e_failure;
      }

    }
    else
    {
      printf("INFO: Error: img file has not sufficient capacity\n");
    }

  }
  else 
  {
    printf("INFO: Error: Error in opening files\n");
    return e_failure;
  }
  return e_success;
}