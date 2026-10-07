#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include<stdlib.h>
#include <unistd.h>
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
  //check user enter the at least 3 args for decoding

  if(argv[2]==NULL)
  {
    fprintf(stderr,"INFO:  Error: insufficient args for decoding\n");
    return e_failure;
  }
  char * address =strstr(argv[2],".");
  if(argv[2]!=NULL&&address!=NULL&&strcmp(address,".bmp")==0)//check if the second argument is a .bmp file
  {
    decInfo->stego_image_fname=argv[2];
    printf("INFO: Stego image file name is %s\n",decInfo->stego_image_fname);
    return e_success;
  }
  else
  {
    fprintf(stderr,"INFO: Error: stego image file should be a .bmp file\n");
    return e_failure;
  }
  
}
// save the only third argument as output file name in the DecodeInfo structure
Status read_and_save_output_file_name(char *argv[], DecodeInfo *decInfo)
{ 
     if(argv[3]==NULL)//check if the third argument is NULL then save the default output file name as output.txt
  {
    decInfo->output_fname="output.txt";
    return e_success;

  }
    char * address =strstr(argv[3],".");
 
  if(argv[3]!=NULL&&address!=NULL)//check if the third argument is a .txt file
  {
    if(strcmp(address,".txt")==0)
    {
      decInfo->output_fname=argv[3];
      return e_success;
    }
  }
 
  decInfo->output_fname=(char *)malloc(sizeof(char)*20);
   if(argv[3]!=NULL&&address!=NULL)//check if the third argument has a dot in it but not a .txt file
  {   int i=0;
    for(i=0;argv[3][i]!='.';i++)
    {
      decInfo->output_fname[i]=argv[3][i];
    }
    decInfo->output_fname[i]='\0';
    strcat(decInfo->output_fname,".txt");
  }
 else
  {
    decInfo->output_fname=argv[3];//save the third argument as output file name in the DecodeInfo structure
    strcat(decInfo->output_fname,".txt");
  }
}
//open the stego image file
Status open_stego_image_and_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    decInfo->fptr_output = fopen(decInfo->output_fname, "w+");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    if (decInfo->fptr_output == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: ERROR: Unable to open file %s\n", decInfo->output_fname);

    	return e_failure;
    }
    return e_success;   
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
  
  fseek(fptr_stego_image,54,SEEK_SET);
 
  return e_success;
}
/* decode the byte to lsb from the stego image */
Status decode_byte_from_lsb(FILE *fptr_stego_image, char *data)
{    
    *data=0;
    unsigned int mask = 1 << 7;
    for(int i=0;i<8;i++)
    {  char tem;
        fread(&tem,1,sizeof(char),fptr_stego_image);
        if(tem & 0X01)
        {
            *data=*data | mask;
        }
        else
        {
            *data=*data &(~mask);
        }
        mask=mask>>1;
    }
}
/* decode the magic string from the stego image */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    // read Magic string from user
    char user_magic_string[5];
    printf("INFO: Enter the magic string to decode: ");
    scanf("%s",user_magic_string);
   for(int i=0;i<strlen(magic_string);i++)
   {
    char ch;
    decode_byte_from_lsb(decInfo->fptr_stego_image,&ch);
   
     if(ch!=user_magic_string[i])
     {
        fprintf(stderr,"INFO: Error: magic string is not matched\n");
        return e_failure;
     }

   }
    return e_success;
}

Status decode_byte_from_lsb_int( int *extension_size,char *temp_buffer,FILE *fptr_stego_image)
{  unsigned int mask=1<<31;
      *extension_size=0;
    for(int i=0;i<32;i++)
    {
        *temp_buffer=0;
        fread(temp_buffer,1,sizeof(char),fptr_stego_image);
        if((*temp_buffer & 0X01)!=0)
        {
            *extension_size=*extension_size | mask;
        }
        else
        {
            *extension_size=*extension_size &(~mask);
        }
     
        mask=mask>>1;

    }
    return e_success;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
  if(decode_byte_from_lsb_int(&decInfo->extension_size,&decInfo->temp_buffer,decInfo->fptr_stego_image)==e_success)
  {
    return e_success;
  }
  else
  {
    fprintf(stderr,"INFO: Error: unable to decode the secret file extension size\n");
    return e_failure;
  }
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{  int i=0;
  for(i=0;i<decInfo->extension_size;i++)
  {
    decode_byte_from_lsb(decInfo->fptr_stego_image,&decInfo->temp_buffer);
    decInfo->extension_data[i]=decInfo->temp_buffer;

  }
  decInfo->extension_data[i]='\0';
  return e_success;
}
/* decode the secret file size from the stego image */
 Status decode_secret_file_size( int *secret_file_size,char *temp_buffer,FILE *fptr_stego_image)
 {
    if(decode_byte_from_lsb_int(secret_file_size,temp_buffer,fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        fprintf(stderr,"INFO: Error: unable to decode the secret file size\n");
        return e_failure;
    }
 }
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    for(int i=0;i<decInfo->secret_file_size;i++)
    {
        decode_byte_from_lsb(decInfo->fptr_stego_image,&decInfo->temp_buffer);
        fwrite(&decInfo->temp_buffer,1,sizeof(char),decInfo->fptr_output);
    }
     for(int i = 50; i <= 100; i++)
{
    printf("\rINFO: Decoding secret file data %d%%", i);
    if(i == 100)
    {
        printf("\rINFO: Secret file data decoded successfully\n");
        break;
    }
    fflush(stdout);
    usleep(50000);
}

 return e_success;   
 }
 Status print_decoded_data(DecodeInfo *decInfo)
 {  rewind(decInfo->fptr_output);
    
    printf("Decoded data is:\n");
    for(int i=0;i<decInfo->secret_file_size;i++)
    {
       putchar(fgetc(decInfo->fptr_output));
    }
    printf("\n");
    return e_success;
 }
Status do_decoding(DecodeInfo *decInfo)
{
  //open stego image file
  if(open_stego_image_and_output_file(decInfo)==e_success)
  {
   printf("INFO: Stego image file opened successfully\n");
    //skip the bmp header
    if(skip_bmp_header(decInfo->fptr_stego_image)==e_success)
    {
        printf("INFO: BMP header skipped successfully\n");
        //decode the magic string from the stego image
        if(decode_magic_string(MAGIC_STRING,decInfo)==e_success)
        {
            printf("INFO: Magic string decoded successfully\n"); 
            //decode the secret file extension size from the stego image
            if(decode_secret_file_extn_size(decInfo)==e_success)
            {
                printf("INFO: Secret file extension size decoded successfully\n");
                printf("INFO: Secret file extension size is %d\n",decInfo->extension_size);
                //decode the extension of secret file from the stego image
                if(decode_secret_file_extn(decInfo)==e_success)
                {
                    printf("INFO: Secret file extension decoded successfully\n");
                    if(decode_secret_file_size(&decInfo->secret_file_size,&decInfo->temp_buffer,decInfo->fptr_stego_image)==e_success)
                    {
                       
                        printf("INFO: Secret file size is %d\n",decInfo->secret_file_size);
                        if(decode_secret_file_data(decInfo)==e_success)
                        {
                            //printf("Secret file data decoded successfully\n");
                            print_decoded_data(decInfo);
                            
                            return e_success;
                        }
                        else
                        {
                            fprintf(stderr,"INFO: Error: unable to decode the secret file data\n");
                            return e_failure;
                        }
                       
                        
                    }
                    else
                    {
                        fprintf(stderr,"INFO: Error: unable to decode the secret file size\n");
                        return e_failure;
                    }
                   
                    
                    return e_success;
                }
                else
                {
                    fprintf(stderr,"INFO: Error: unable to decode the secret file extension\n");
                    return e_failure;
                }

            }
            else
            {
                fprintf(stderr,"INFO: Error: unable to decode the secret file extension size\n");
                return e_failure;
            } 
        }
        else
        {
            fprintf(stderr,"INFO: Error: magic string is not matched\n");
            return e_failure;
        }
    }
    else
    {
      fprintf(stderr,"INFO: Error: unable to skip the bmp header\n");
      return e_failure;
    }
  }
  else
  {
    fprintf(stderr,"INFO: Error: stego image file is not opened\n");
    return e_failure;
  }
}