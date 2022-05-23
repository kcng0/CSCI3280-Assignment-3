/*
* CSCI3280 Introduction to Multimedia Systems *
* --- Declaration --- *
* I declare that the assignment here submitted is original except for source
* material explicitly acknowledged. I also acknowledge that I am aware of
* University policy and regulations on honesty in academic work, and of the
* disciplinary guidelines and procedures applicable to breaches of such policy
* and regulations, as contained in the website
* http://www.cuhk.edu.hk/policy/academichonesty/ *
* Assignment 3
* Name : Ng Ka Chun
* Student ID : 1155143490
* Email Addr : kcng0@cse.cuhk.edu.hk
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <list> 
#include <iterator> 
#include <vector> 
#include <utility> 
#include <string> 
#include <functional> 
#include <time.h>
#include "bmp.h"
#include <malloc.h>
using namespace std;

#define CODE_SIZE 12
#define TRUE 1
#define FALSE 0
#define dict_size 4096
#define ascii_size 256
#define npos 4294967295
#define temp_txt "temp.txt"


// open hash table with seperate chaining for compression
// initialization : O(n)
// insertion : Best : O(1) , Worst : O(n)
// find : Best : O(1) , Worst : O(n)
class Hash {
	
public:
	// size of hash table
	int size;

	// declare table of linked lists of two values: string and code 
	list< pair<string, unsigned int> > * table;

	// declare hash table of required size
	Hash (int size);

	// insert elements in the hash table : Best : O(1) , Worst : O(n)
	void insert(string data, unsigned int count);

	// find the string with corresponding code : Best : O(1) , Worst : O(n)
	unsigned int find(string data);

	// initialize the hash table with 256 ascii characters inserted : O(n)
	void initialize();
};
	Hash :: Hash(int size){
		this->size = size;
		table = new list< pair<string , unsigned int > >[size];
	}
	void Hash :: insert(string data, unsigned int count) {
		unsigned int key;
		// deal with the probelm that a signed char has value between -128 to 127
		if (data[0] >= 0) 
			key = data[0];
		else key = data[0] + ascii_size;
	
		table[key].push_back(make_pair(data, count));
	}
	unsigned int Hash :: find(string data) {					
		unsigned int key;
		// deal with the probelm that a signed char has value between -128 to 127
		if (data[0] >= 0)
			key = data[0];
		else key = data[0] + ascii_size;

		// search the string and return the respective code, if can't, return npos
		list < pair < string , unsigned int> > :: iterator i;
		for ( i = table[key].begin(); i != table[key].end();i++) {
			if ( (* i).first == data) {
				return (* i).second;
			}
		}
		return -1;
	}
	void Hash :: initialize() {
		
		// free all the memory in the table if it is not empty
	  	for (int i = 0; i < ascii_size; i++) {
		  if (!table[i].empty()) table[i].clear();
	 	 }
		
		// initialize the table with 256 ascii inserted
	  	string c ="";
		for (int i = 0; i < ascii_size; i++){
			c = "";
			c += i;
			table[i].push_back(make_pair (c ,  i));
		}
	}



/* function prototypes */
unsigned int read_code(FILE*, unsigned int); 
void write_code(FILE*, unsigned int, unsigned int); 
void writefileheader(FILE *,char**,int);
void readfileheader(FILE *,char**,int *);
void compress(char * name, FILE*);
void decompress(FILE* , int h , int w);
void extractbmp(int height , int width,  char * name);

// array structure of size 4096 for decompression
// initialization : O(n)
// insertion : Best : O(1) , Worst : O(1) -> direct access
// find : Best : O(1) , Worst : O(1) -> direct access

string dict[dict_size];

// initialization of array dictionary with 256 ascii inserted
void initializeDict(string * dict) {
	int i;
	for (i = 0; i < ascii_size; i++) {
		dict[i] = i;
	}
	for (; i < dict_size; i++) {
		dict[i] = "";
	}
}

// string pixel = "";
char * pixel;
int main(int argc, char **argv)
{   
	clock_t tStart = clock();
    int printusage = 0;
    int	no_of_file;
    char **input_file_names;    
	char *output_file_names;
    FILE *lzw_file;

    if (argc >= 3)
    {
		if ( strcmp(argv[1],"-c") == 0)
		{
			/* compression */
			lzw_file = fopen(argv[2] ,"wb");
        
			/* write the file header */
			input_file_names = argv + 3;
			no_of_file = argc - 3;
			writefileheader(lzw_file,input_file_names,no_of_file);
        	        	
			/* ADD CODES HERE */

			// compress the files one by one
			for (int i = 0; i < no_of_file; i++) {
				compress(input_file_names[i],lzw_file);
				cout << "Compress succesfully!" << endl;
			}
			// flush the buffer
			write_code(lzw_file , dict_size-1 , CODE_SIZE );
			fclose(lzw_file);        	
		} else
		if ( strcmp(argv[1],"-d") == 0)
		{	
			/* decompress */
			lzw_file = fopen(argv[2] ,"rb");
			
			/* read the file header */
			no_of_file = 0;			
			readfileheader(lzw_file,&output_file_names,&no_of_file);
			
			/* ADD CODES HERE */
			char *dfile = strtok(output_file_names, "\n");
			for (int i = 0; i < no_of_file / 3 ; i++) {
				// int height = read_code(lzw_file , CODE_SIZE);
				// int width = read_code(lzw_file,CODE_SIZE);
				printf("dfile: %s\n",dfile);
				char * height = strtok(NULL, "\n");
				char * width = strtok(NULL, "\n");
				int h = stoi(height);
				int w = stoi(width);
				printf("Height : %d , Width : %d\n",h,w);
				cout << "Scanned height and width!" << endl;
				decompress(lzw_file , h , w);
				
                cout << "decompression success!" << endl;

                extractbmp(w, h , dfile);
                cout << "bmp extraction success!" << endl;
				cout << endl;
				dfile = strtok(NULL, "\n");
				free(pixel);
				
			}
		
			fclose(lzw_file);
			free(output_file_names);
		}else
			printusage = 1;
    }else
		printusage = 1;

	if (printusage)
		printf("Usage: %s -<c/d> <lzw filename> <list of files>\n",argv[0]);
	printf("Time taken: %.4fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

	return 0;
}

/*****************************************************************
 *
 * writefileheader() -  write the lzw file header to support multiple files
 *
 ****************************************************************/
void writefileheader(FILE *lzw_file,char** input_file_names,int no_of_files)
{
	int i;
	/* write the file header */
	for ( i = 0 ; i < no_of_files; i++) 
	{
		Bitmap image(input_file_names[i]);
		int h = image.getHeight();
		int w = image.getWidth();
		fprintf(lzw_file,"%s\n",input_file_names[i]);	
		fprintf(lzw_file,"%d\n",h);
		fprintf(lzw_file,"%d\n",w);
	}
	fputc('\n',lzw_file);

}

/*****************************************************************
 *
 * readfileheader() - read the fileheader from the lzw file
 *
 ****************************************************************/
void readfileheader(FILE *lzw_file,char** output_filenames,int * no_of_files)
{
	int noofchar;
	char c,lastc;

	noofchar = 0;
	lastc = 0;
	*no_of_files=0;
	/* find where is the end of double newline */
	while((c = fgetc(lzw_file)) != EOF)
	{
		noofchar++;
		if (c =='\n')
		{
			if (lastc == c )
				/* found double newline */
				break;
			(*no_of_files)++;
		}
		lastc = c;
	}

	if (c == EOF)
	{
		/* problem .... file may have corrupted*/
		*no_of_files = 0;
		return;
	
	}
	/* allocate memeory for the filenames */
	*output_filenames = (char *) malloc(sizeof(char)*noofchar);
	/* roll back to start */
	fseek(lzw_file,0,SEEK_SET);

	fread((*output_filenames),1,(size_t)noofchar,lzw_file);
	
	return;
}

/*****************************************************************
 *
 * read_code() - reads a specific-size code from the code file
 *
 ****************************************************************/
unsigned int read_code(FILE *input, unsigned int code_size)
{
    unsigned int return_value;
    static int input_bit_count = 0;
    static unsigned long input_bit_buffer = 0L;

    /* The code file is treated as an input bit-stream. Each     */
    /*   character read is stored in input_bit_buffer, which     */
    /*   is 32-bit wide.                                         */

    /* input_bit_count stores the no. of bits left in the buffer */

    while (input_bit_count <= 24) {
        input_bit_buffer |= (unsigned long) getc(input) << (24-input_bit_count);
        input_bit_count += 8;
    }
    
    return_value = input_bit_buffer >> (32 - code_size);
    input_bit_buffer <<= code_size;
    input_bit_count -= code_size;
    
    return(return_value);
}


/*****************************************************************
 *
 * write_code() - write a code (of specific length) to the file 
 *
 ****************************************************************/
void write_code(FILE *output, unsigned int code, unsigned int code_size)
{
    static int output_bit_count = 0;
    static unsigned long output_bit_buffer = 0L;

    /* Each output code is first stored in output_bit_buffer,    */
    /*   which is 32-bit wide. Content in output_bit_buffer is   */
    /*   written to the output file in bytes.                    */

    /* output_bit_count stores the no. of bits left              */    

    output_bit_buffer |= (unsigned long) code << (32-code_size-output_bit_count);
    output_bit_count += code_size;

    while (output_bit_count >= 8) {
        putc(output_bit_buffer >> 24, output);
        output_bit_buffer <<= 8;
        output_bit_count -= 8;
    }


    /* only < 8 bits left in the buffer                          */    

}

/*****************************************************************
 *
 * compress() - compress the source file and output the coded text
 *
 ****************************************************************/
void compress(char * name, FILE *output)
{

	/* ADD CODES HERE */
    Bitmap image_data(name);
    cout << name << endl;
	Hash h(ascii_size);
	h.initialize();
	string Prefix = "";
	unsigned int X;
	int end;
	char C;
	int pos = ascii_size;
	int found;
	int temp;
    if (image_data.getData() == NULL) {
		cout << "cannot load data" << endl;
	}
    //     FILE * d = fopen("debug.txt", "w");
    //     fprintf(d, "%s" , name);
    //     fclose(d);
    // }
	int height = image_data.getHeight();
    int width = image_data.getWidth();
    printf("Height : %d Width : %d \n", height, width);
    // write the height and width of the image
    // write_code(output, height, CODE_SIZE);
	// write_code(output, width, CODE_SIZE);
    unsigned char rgb[3]; // R = 0 , G = 1 , B = 2
    for (int i = 0; i < 3; i++) {
       for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                image_data.getColor(c , r , rgb[0] , rgb[1] , rgb[2]);
				
                temp = rgb[i];
				
				if (temp >= ascii_size/2) temp = temp - ascii_size;
				C = (char) temp;
				// printf("rgb : %d r : %d c: %d\n", temp,r,c);
                found = h.find(Prefix + C);
                if (found != npos) {
                    Prefix = Prefix + C;
                } else
                {
                    X = h.find(Prefix);
                    write_code(output, X , CODE_SIZE);
                    h.insert(Prefix + C , pos);
                    Prefix = C;
                    pos++;
                }

                // initialize the hash table when the dictionary size is full
                if (pos == dict_size-1) {
                    h.initialize();
                    pos = ascii_size;
                }

            }
        }
    }
    X = h.find(Prefix);
	write_code(output, X , CODE_SIZE);

    // Add indication of end of file
	write_code(output , dict_size -1 , CODE_SIZE);
	
}

/*****************************************************************
 *
 * decompress() - decompress a compressed file to the orig. file
 *
 ****************************************************************/
void decompress(FILE *input ,int h, int w )
{	

	/* ADD CODES HERE */
	int count = 0;
	initializeDict(dict);
	unsigned int PW = read_code(input, CODE_SIZE);
	char C = dict[PW][0];
	unsigned int CW;
	int end;
	pixel = (char * )malloc(sizeof(char) * h*w*3);
	pixel[count] = C;
	count++ ;
	string S,P;
	
	int pos = ascii_size;
	// FILE * tfile = fopen("temp.txt", "w");
	// read the code of file whenever there is no indication of the end of file
	while ((end = read_code(input,CODE_SIZE) ) != dict_size-1) {
		
		CW = end;
		if (CW < pos) {
			C = dict[CW][0];
			S = dict[CW];
		} else {
			C = dict[PW][0];
			S = dict[PW] + C;
		}
		for (int i = 0; i < S.length(); i++){
			pixel[i+count] = S.at(i);
		}
		count += S.length();
		P = dict[PW];
		dict[pos] = P + C;
		pos++;
		PW = CW;
		if (pos == dict_size-1) {
			initializeDict(dict);
			pos = ascii_size;
		}
	}
	// fclose(tfile);
}

void extractbmp(int width, int height , char * name)
{   
    Bitmap image_data(width, height);
    int h = image_data.getHeight();
    int count = 0;
    int w = image_data.getWidth();
	char temp;
	FILE * tfile = fopen("temp.txt", "r");
    short * cr = (short *)malloc(sizeof(short) * h * w);
	short * cg = (short *)malloc(sizeof(short) * h * w);
	short * cb = (short *)malloc(sizeof(short) * h * w);
    int rgb[3];
	int tint;
    // for (int i = 0; i < 3; i++) {
        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
			// fscanf(tfile , "%d", &temp);
			
            temp = pixel[count];
            // if (i == 0)
			*(cr+r*w+c) = temp;
            // else if (i == 1)
			temp = pixel[count + h*w];
                *(cg+r*w+c) = temp;
				// else 
			temp = pixel[count + h*w*2];
				*(cb+r*w+c) = temp;
                // printf("rgb : %d r : %d c: %d\n", rgb,r,c);
			image_data.setColor(c,r,*(cr+r*w+c),*(cg+r*w+c),*(cb+r*w+c));
			count++;
        }
        fclose(tfile);
    }
    // }
	// for (int r = 0; r < h; r++) {
    //         for (int c = 0; c < w; c++) {
	// 			image_data.setColor(c,r,*(cr+r*w+c),*(cg+r*w+c),*(cb+r*w+c));
	// 		}
	// }
	free(cr);
	free(cb);
	free(cg);
    image_data.save(name);
}
