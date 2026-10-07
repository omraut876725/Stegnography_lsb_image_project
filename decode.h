#ifndef DECODE_H
#define DECODE_H

#include "types.h"    // Contains user defined types
/* Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 1   
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct _DecodeInfo
{
   
   
   
    
   char *output_fname;
    FILE *fptr_output;
   
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image; 
    uint extension_size;
    uint secret_file_size;
    char image_data[MAX_IMAGE_BUF_SIZE]; 
    char image_buffer[MAX_IMAGE_BUF_SIZE];
    char extension_data[6];
    char temp_buffer;           
  

} DecodeInfo;

//* Decoding function prototype */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
// save the only third argument as output file name in the DecodeInfo structure
Status read_and_save_output_file_name(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/*skip the bmp header of stego image file*/
Status skip_bmp_header(FILE *fptr_stego_image);
/*open the stego image file*/
Status open_stego_image_and_output_file(DecodeInfo *decInfo);
/* decode the magic string from the stego image */
Status decode_magic_string(const char *MAGIC_STRING, DecodeInfo *decInfo);

/* decode the byte to lsb from the stego image */
Status decode_byte_from_lsb(FILE *fptr_stego_image, char *data);
/* decode the secret file extension size from the stego image */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
/* decode the size of secret file from the stego image */
Status decode_byte_from_lsb_int( int *extension_size,char *temp_buffer,FILE *fptr_stego_image);
/* decode the extension of secret file from the stego image */
Status decode_secret_file_extn(DecodeInfo *decInfo);
/* decode the secret file size from the stego image */
Status decode_secret_file_size( int *secret_file_size,char *temp_buffer,FILE *fptr_stego_image);
/* decode the secret file data from the stego image */
Status decode_secret_file_data(DecodeInfo *decInfo);
Status print_decoded_data(DecodeInfo *decInfo);




#endif // DECODE_H