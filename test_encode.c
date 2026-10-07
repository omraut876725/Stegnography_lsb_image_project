/*
 * ================================================================
 *                    STEGANOGRAPHY PROJECT
 * ================================================================
 *
 * Project Name : Steganography LSB (Least Significant Bit) Implementation
 * Student Name : Om Raut
 * Batch ID     : 26019f_009
 * Date         : 2-Oct-2026
 *
 * Description:
 * This project implements image steganography to hide and extract
 * a secret file inside a BMP image using Least Significant Bit (LSB)
 * manipulation.
 *
 * Encoding:
 * During encoding, the program takes a source BMP image and a secret
 * file as input. It embeds the secret information into the Least
 * Significant Bits of the image data.
 *
 * The encoder stores a predefined magic string, secret file extension,
 * secret file size, and the actual secret file data in the image by
 * modifying its LSBs. The modified image is saved as a stego image
 * containing the hidden file.
 *
 * Decoding:
 * During decoding, the program reads the stego image and extracts
 * the hidden information from the Least Significant Bits of the
 * image data.
 *
 * It first extracts and validates the magic string, followed by the
 * secret file extension, secret file size, and secret file data.
 * The extracted data is reconstructed and written into an output
 * file using the original secret file extension.
 *
 * Concepts Used:
 * - File Handling
 * - Structures
 * - Pointers
 * - Dynamic Memory Allocation
 * - Bitwise Operations
 * - Command-Line Arguments
 * - LSB-based Data Hiding and Extraction
 *
 * In short, this project hides a secret file inside a BMP image
 * during encoding and extracts the hidden file from the stego image
 * during decoding.
 *
 * ================================================================
 */

#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "encode.h"
#include<string.h>
int main(int argc,char *argv[])
{
  if(check_operation_type(argv)==e_encode)
  {
    printf("INFO: Encoder operation is selected\n");
    EncodeInfo encfo;
    if(read_and_validate_encode_args(argv,&encfo)==e_success)
    {
      printf("INFO: Encoder read and validation argc is Successfully\n");
      do_encoding(&encfo);

    }
    else
    {
      printf("INFO:  Error in reading and validating encode argc\n");
    }

  }
  else if(check_operation_type(argv)==e_decode)
  {
    printf("INFO: decoder operation is selected\n");
         DecodeInfo decinfo;
   // DecodeInfo decinfo;
   if(read_and_validate_decode_args(argv,&decinfo)==e_success)
    {
      printf("INFO: Decoder read and validation argc is Successfully\n");
     // read_and_save_output_file_name(argv,&decinfo);
      read_and_save_output_file_name(argv,&decinfo);
      printf("INFO: Source image file name is %s\n",decinfo.output_fname);
      do_decoding(&decinfo);

    }
    else
    {
      printf("INFO: Error in reading and validating decode argc\n");
    }
  }
  else
  {
    printf("INFO: unsupported  operation \n");
    printf("*************Usage*******************\n");
    printf("To Encode: ./a.out <-e> <source_Img file> <secret file> <stego Img>\n");
    printf("To Decode: ./a.out <-d> <stego Img> <output txt file> \n");
    printf("*************************************\n");

  }

}
/* operation checking function*/
 OperationType check_operation_type(char *argv[])
{
 if(argv[1]!=NULL&&strcmp(argv[1],"-e")==0)
 {
  return e_encode;
 }
 else if(argv[1]!=NULL&&strcmp(argv[1],"-d")==0)
 {
  return e_decode;
 }
else
{
  return e_unsupported;
}
}