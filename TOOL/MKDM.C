
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kernel/module.h"

typedef struct {
	char exeid[2];					/* 'MZ' or 'ZM' */
	unsigned last_pagesize;
	unsigned pages;					/* 512 bytes/page */
	unsigned relocations;
	unsigned header_size;			/* paragraph */
	unsigned min_memneed;			/* paragraph */
	unsigned max_memneed;			/* paragraph */
	unsigned stack_offset;
	unsigned sp;
	unsigned sum;
	unsigned ip;
	unsigned code_offset;
	unsigned relocate_table;
	unsigned overlay_number;
} exe_header;

void usage(void);
void make_dlm(char *infile, char *outfile);
void make_ddm(char *infile, char *outfile);
void exeinfo(char *filename);
void readmap_segm(char *mapfile);
void readmap_entry(char *mapfile);
FILE *fileopen(char *filename, char *mode);
void mapname(char *filename);

char MapName[FILENAME_MAX];
unsigned HeaderSize, BodySize, MemNeed, CodeSize, DataSize, DataPos, BssSize;
unsigned InitOffs, TermOffs, OpenOffs, CloseOffs, ReadOffs, WriteOffs;
unsigned IoctlOffs;

int main(int argc, char *argv[])
{
	if (strcmp(argv[1], "-m") == 0)
		make_dlm(argv[2], argv[3]);
	else if (strcmp(argv[1], "-d") == 0)
		make_ddm(argv[2], argv[3]);
	else
		usage();

	return EXIT_SUCCESS;
}

void usage(void)
{
	puts("usage: mkdm [options] [in filename] [out filename]");
	puts("options: -m  make dlm type module");
	puts("         -d  make ddm type module");

	exit(EXIT_FAILURE);
}

void make_dlm(char *infile, char *outfile)
{
	int i, c;
	FILE *fp_in, *fp_out;
	exe_header dummy;
	common_header comhead;

	exeinfo(infile);
	mapname(infile);
	readmap_segm(MapName);

	fp_in = fileopen(infile, "rb");
	fp_out = fileopen(outfile, "wb");

	if (BodySize != CodeSize + DataSize) {
		printf("\nWARNING: CODE + DATA  size mismatch !!!\n");
		printf("    EXE Header: %5u bytes\n", BodySize);
		printf("    MAP File:   %5u bytes\n\n", CodeSize + DataSize);

		BodySize = CodeSize + DataSize;
	}

	comhead.type = 'M';
	comhead.reserved = 0;
	comhead.msize = BodySize;							/* CODE + DATA */
	comhead.memneed = CodeSize + DataSize + BssSize;

	fseek(fp_in, (long)HeaderSize, SEEK_SET);
	fwrite(&comhead, sizeof(comhead), 1, fp_out);

	for (i = 0; i < BodySize; i++) {
		if ((c = getc(fp_in)) == EOF)
			break;
		putc(c, fp_out);
	}
	fclose(fp_out);
	fclose(fp_in);

	printf("DLM file information: %s\n\n", outfile);

	printf("module size:        %5u bytes\n", comhead.msize);
	printf("memory requirement: %5u bytes\n\n", comhead.memneed);
}

void make_ddm(char *infile, char *outfile)
{
	int i, c;
	FILE *fp_in, *fp_out;
	exe_header dummy;
	common_header comhead;
	ddm_header ddmhead;

	exeinfo(infile);
	mapname(infile);
	readmap_segm(MapName);
	readmap_entry(MapName);

	if (BodySize != CodeSize + DataSize) {
		printf("\nWARNING: CODE + DATA  size mismatch !!!\n");
		printf("    EXE Header: %5u bytes\n", BodySize);
		printf("    MAP File:   %5u bytes\n\n", CodeSize + DataSize);

		BodySize = CodeSize + DataSize;
	}

	fp_in = fileopen(infile, "rb");
	fp_out = fileopen(outfile, "wb");

	comhead.type = 'D';
	comhead.reserved = 0;
	comhead.msize = BodySize;							/* CODE + DATA */
	comhead.memneed = CodeSize + DataSize + BssSize;
	ddmhead.init = InitOffs;
	ddmhead.term = TermOffs;
	ddmhead.open = OpenOffs;
	ddmhead.close = CloseOffs;
	ddmhead.read = ReadOffs;
	ddmhead.write = WriteOffs;
	ddmhead.ioctl = IoctlOffs;

	fseek(fp_in, (long)HeaderSize, SEEK_SET);
	fwrite(&comhead, sizeof(comhead), 1, fp_out);
	fwrite(&ddmhead, sizeof(ddmhead), 1, fp_out);

	for (i = 0; i < BodySize; i++) {
		if ((c = getc(fp_in)) == EOF)
			break;
		putc(c, fp_out);
	}
	fclose(fp_out);
	fclose(fp_in);

	printf("DDM file information: %s\n\n", outfile);

	printf("module size:        %5u bytes\n", comhead.msize);
	printf("memory requirement: %5u bytes\n\n", comhead.memneed);

	printf("offset init:        %5u (%04X)\n", ddmhead.init, ddmhead.init);
	printf("offset term:        %5u (%04X)\n", ddmhead.term, ddmhead.term);
	printf("offset open:        %5u (%04X)\n", ddmhead.open, ddmhead.open);
	printf("offset close:       %5u (%04X)\n", ddmhead.close, ddmhead.close);
	printf("offset read:        %5u (%04X)\n", ddmhead.read, ddmhead.read);
	printf("offset write:       %5u (%04X)\n", ddmhead.write, ddmhead.write);
	printf("offset ioctl:       %5u (%04X)\n", ddmhead.ioctl, ddmhead.ioctl);
}

void exeinfo(char *filename)
{
	FILE *fp;
	exe_header exehead;

	fp = fileopen(filename, "rb");
	fread(&exehead, sizeof(exehead), 1, fp);
	fclose(fp);

	HeaderSize = exehead.header_size * 16;
	if (exehead.last_pagesize == 0)
		BodySize = exehead.pages * 512 - HeaderSize;
	else {
		BodySize = (exehead.pages - 1) * 512
			+ exehead.last_pagesize - HeaderSize;
	}
	MemNeed = exehead.min_memneed * 16;

	printf("EXE file information: %s\n\n", filename);

	printf("CODE + DATA:        %5u bytes\n", BodySize);
	printf("  pages             %5u pages (512 bytes/page)\n", exehead.pages);
	printf("  last page size    %5u bytes\n", exehead.last_pagesize);
	printf("  header size       %5u paragraphs\n", exehead.header_size);
	printf("Memory Requirement: %5u bytes\n\n", MemNeed);
	printf("Relocations:        %5u ...", exehead.relocations);
	if (exehead.relocations == 0)
		puts("OK");
	else {
		puts("ERROR");
		exit(EXIT_FAILURE);
	}
	putchar('\n');
}

void readmap_segm(char *mapfile)
{
	FILE *fp;
	char buf[128], name[64];
	unsigned start, end, size;

	fp = fileopen(mapfile, "rb");

	printf("MAP file information: %s\n\n", mapfile);

	while (fgets(buf, sizeof(buf), fp) != 0) {
		if (sscanf(buf, " %XH %XH %XH %s", &start, &end, &size, name) != 4)
			continue;
		printf("segment '%-8s'  start %5u - end %5u : %5u bytes\n",
			name, start, end, size);

		if (strcmp(name, "_TEXT") == 0)
			CodeSize = size;
		if (strcmp(name, "_DATA") == 0) {
			DataPos = start;
			DataSize = size;
		}
		if (strcmp(name, "_BSS") == 0)
			BssSize = size;
	}
	fclose(fp);
	putchar('\n');
}

void readmap_entry(char *mapfile)
{
	FILE *fp;
	char buf[128], name[64];
	unsigned segm, offs, xoffs;

	fp = fileopen(mapfile, "rb");

	while (fgets(buf, sizeof(buf), fp) != 0) {
		if (strstr(buf, "Publics by Value") != 0)
			break;
	}

	while (fgets(buf, sizeof(buf), fp) != 0) {
		if (sscanf(buf, " %X:%X %s", &segm, &offs, name) != 3)
			continue;
		xoffs = segm * 16 + offs;
		printf("  entry '%-16.16s'  offset %5u (%04X)\n", name, xoffs, xoffs);

		if (strcmp(name, "_dev_init") == 0)
			InitOffs = xoffs;
		if (strcmp(name, "_dev_term") == 0)
			TermOffs = xoffs;
		if (strcmp(name, "_dev_open") == 0)
			OpenOffs = xoffs;
		if (strcmp(name, "_dev_close") == 0)
			CloseOffs = xoffs;
		if (strcmp(name, "_dev_read") == 0)
			ReadOffs = xoffs;
		if (strcmp(name, "_dev_write") == 0)
			WriteOffs = xoffs;
		if (strcmp(name, "_dev_ioctl") == 0)
			IoctlOffs = xoffs;
	}
	fclose(fp);
	putchar('\n');
}

FILE *fileopen(char *filename, char *mode)
{
	FILE *fp;

	if ((fp = fopen(filename, mode)) == 0) {
		printf("%s: can't open.\n", filename);
		exit(EXIT_FAILURE);
	}
	return fp;
}

void mapname(char *filename)
{
	char *s;

	strcpy(MapName, filename);
	if ((s = strrchr(MapName, '.')) == 0)
		strcat(MapName, ".map");
	else
		strcpy(s, ".map");

}

