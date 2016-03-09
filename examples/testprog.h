/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <xm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG(...) do {	  \
		fprintf(stderr, __VA_ARGS__); \
		fflush(stderr); \
	} while(0)

#define DEBUG_ERR(...) do {	  \
		perror(__VA_ARGS__); \
		fflush(stderr); \
	} while(0)

#define FATAL(...) do {	  \
		fprintf(stderr, __VA_ARGS__); \
		fflush(stderr); \
		exit(1); \
	} while(0)

#define FATAL_ERR(...) do {	  \
		perror(__VA_ARGS__); \
		fflush(stderr); \
		exit(1); \
	} while(0)

static void create_context_from_file(xm_context_t** ctx, uint32_t rate, const char* filename) {
	FILE* xmf;
	int size;

	xmf = fopen(filename, "rb");
	if(xmf == NULL) {
		DEBUG_ERR("Could not open input file");
		*ctx = NULL;
		return;
	}

	fseek(xmf, 0, SEEK_END);
	size = ftell(xmf);
	rewind(xmf);
	if(size == -1) {
		fclose(xmf);
		DEBUG_ERR("fseek() failed");
		*ctx = NULL;
		return;
	}

	char* data = malloc(size + 1);
	if(fread(data, 1, size, xmf) < size) {
		fclose(xmf);
		DEBUG_ERR("fread() failed");
		*ctx = NULL;
		return;
	}
	
	fclose(xmf);

	switch(xm_create_context_safe(ctx, data, size, rate)) {
		
	case 0:
		break;

	case 1:
		DEBUG("could not create context: module is not sane\n");
		*ctx = NULL;
		break;

	case 2:
		FATAL("could not create context: malloc failed\n");
		break;
		
	default:
		FATAL("could not create context: unknown error\n");
		break;
		
	}
}
