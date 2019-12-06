

// resizes a BMP file n times

#include <stdio.h>
#include <stdlib.h>
#include<ctype.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: resize n infile outfile\n");
        return 1;
    }

    //converts the string n into integ
    int n=atoi(argv[1]);
    if(n<=0||n>100)
    {
        fprintf(stderr, "Usage: resize n infile outfile\n");
        return 2;

    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }




    BITMAPFILEHEADER bf;

    BITMAPFILEHEADER bf_resize;



    //read file header of input file
    fread(&bf,sizeof(BITMAPFILEHEADER),1,inptr);
     bf_resize=bf;

    BITMAPINFOHEADER bi;

    BITMAPINFOHEADER bi_resize;


    //read file info header of input file
    fread(&bi,sizeof(BITMAPINFOHEADER),1,inptr);
     //assign values to the output file infoheader from the infile's
    bi_resize=bi;

    //ensures infile is 24 bit uncompressed BMP
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }

    //calculates width and height of new BMP
    bi_resize.biWidth=bi.biWidth*n;
    bi_resize.biHeight=bi.biHeight*n;

    //calculates padding
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    int resize_padding =  (4 - (bi_resize.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;




    //calculates bfsize and biSizeImage

    bf_resize.bfSize=54+((sizeof(RGBTRIPLE)*bi_resize.biWidth)+resize_padding)*abs(bi_resize.biHeight);

    bi_resize.biSizeImage=bf_resize.bfSize-54;


    //write the file header in output file
    fwrite(&bf_resize,sizeof(BITMAPFILEHEADER),1,outptr);

    //write the infoheader in output file
    fwrite(&bi_resize,sizeof(BITMAPINFOHEADER),1,outptr);


    //iterates over every row of input BMP
    for(int i=0,h=abs(bi.biHeight);i<h;i++)
    {

       //iterates to print the resized line of BMP n times
        for(int j=0;j<n;j++)
        {

            //iterates over every pixel in the row i
            for(int k=0;k<bi.biWidth;k++)
            {
                //temporory variable
                RGBTRIPLE triple;

                //reads the rgb value of the pixel k from row i
                fread(&triple,sizeof(RGBTRIPLE),1,inptr);

                //iterates n times to make n copies of pixel k
                for(int l=0;l<n;l++)
                {

                    fwrite(&triple,sizeof(RGBTRIPLE),1,outptr);

                }


            }

            for(int m=0;m<resize_padding;m++)
            {

                fputc(0x00,outptr);


            }


            if (j < (n - 1))
            {

                fseek(inptr, -(bi.biWidth * sizeof(RGBTRIPLE)), SEEK_CUR);


            }
        }
        fseek(inptr, padding, SEEK_CUR);
    }




    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
