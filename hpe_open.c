/* hpe_open (c) 2014 Jacek Lipkowski <sq5bpf@lipkowski.org>
 *
 * Uniden hpe file opener
 *
 * The .hpe (homepatrol) files are simple text files, compressed with gzip
 * and xor-ed with 0x0c for obfuscation. 
 *
 * compile with:
 * gcc -O2 -lz hpe_open.c -o hpe_open
 *
 * for windows use (compiles fine with mingw gcc):
 * gcc -O2 -lz hpe_open.c -o hpe_open -DWIN32
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <zlib.h>
#include <string.h>


#define VER "0.2"

unsigned char secret=0xc; /* shhh... don't tell anyone :) */

int xorfile(char *filein,char *fileout)
{
	int fd1,fd2;
	unsigned char c;
#ifdef WIN32
	fd1=open(filein,O_RDONLY|O_BINARY);
	fd2=open(fileout,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,0666);
#else
	fd1=open(filein,O_RDONLY);
	fd2=open(fileout,O_WRONLY|O_CREAT|O_TRUNC,0666);
#endif
	if (fd1<3) { fprintf (stderr, "open %s: %s\n",filein,strerror (errno)); exit(1); }
	if (fd2<3) { fprintf (stderr, "open %s: %s\n",fileout,strerror (errno)); exit(1); }
	while(read(fd1,&c,1)==1) {
		c=c^secret;
		if (write(fd2,&c,1)!=1) { 
			fprintf (stderr, "write %s: %s\n",fileout,strerror (errno)); exit(1); }
	}
	close(fd1);
	close(fd2);
}

int gcompress(char *filein,char *fileout)
{
	gzFile *gf;
	int rfd;

#define LEN 1024
	unsigned char buf[LEN];
	int r;

	gf=gzopen(fileout,"wb");
	if (!gf) {
		fprintf (stderr, "decompress %s: %s.\n",fileout,strerror (errno));
		exit(1);
	}
#ifdef WIN32
	rfd=open(filein,O_RDONLY|O_BINARY);
#else
	rfd=open(filein,O_RDONLY);
#endif
	if (rfd<3) { fprintf (stderr, "open %s: %s\n",filein,strerror (errno)); exit(1); }

	while(1) {

		r = read (rfd, (char *)&buf, LEN);
		if (r>0) { 
			gzwrite(gf,(char *)&buf,r);
		} else {
			break;
		}
	}
	gzclose(gf);
	close(rfd);

}

int degcompress(char *filein,char *fileout)
{
	gzFile *gf;
	int wfd;

#define LEN 1024
	unsigned char buf[LEN];
	int r;

	gf=gzopen(filein,"rb");
	if (!gf) {
		fprintf (stderr, "decompress %s: %s.\n",filein,strerror (errno));
	}
#ifdef WIN32
	wfd=open(fileout,O_CREAT|O_WRONLY|O_TRUNC|O_BINARY,0666);
#else
	wfd=open(fileout,O_CREAT|O_WRONLY|O_TRUNC,0666);
#endif
	if (wfd<3) { fprintf (stderr, "open %s: %s\n",fileout,strerror (errno)); exit(1); }

	while(1) {
		r = gzread (gf, (char *)&buf, LEN);
		if (r>0) { 
			write(wfd,(char *)&buf,r);
		} else {
			if (gzeof(gf)) break;
			fprintf(stderr,"gzip error\n");
			break;
		}
	}
	gzclose(gf);
	close(wfd);

}

int helpme()
{
	fprintf(stderr,"Usage:\n");
	fprintf(stderr,"hpe_open -c file   ;compress and obfuscate file\n");
	fprintf(stderr,"hpe_open -d file.hpe  ;deobfuscate and decompress file\n");
}

int banner()
{
	fprintf(stderr,"hpe_open %s HPE file compressor/decompressor\n(c) 2014 Jacek Lipkowski SQ5BPF\n",VER);
}

int main(int argc,char **argv) {
	char file[128];
	char file2[128];
	banner();

	if (argc!=3) { helpme(); exit(1); }
	if (strcmp(argv[1],"-d")==0) {
		snprintf((char *)&file,sizeof(file),"%s-dexor",argv[2]);
		snprintf((char *)&file2,sizeof(file2),"%s.gz",file);
		xorfile(argv[2],(char *)&file2);
		degcompress((char *)&file2,(char *)&file);
		fprintf(stderr,"written %s and %s\n",(char *)&file2,(char *)&file);

	} else

		if (strcmp(argv[1],"-c")==0) {
			snprintf((char *)&file,sizeof(file),"%s.hpe",argv[2]);
			snprintf((char *)&file2,sizeof(file2),"%s.gz",argv[2]);
			gcompress(argv[2],(char *)&file2);

			xorfile((char *)&file2,(char *)&file);
			fprintf(stderr,"written %s and %s\n",(char *)&file2,(char *)&file);
		} else 
			helpme();

}
