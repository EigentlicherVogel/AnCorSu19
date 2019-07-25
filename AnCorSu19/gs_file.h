#pragma once

#include <stdio.h>

class gs_file
{
public:
	gs_file(void);
public:
	virtual ~gs_file(void);

public:
	int ktape;
	int kfile;
	int kthead;
	int kfhead;
	long ksize;
	char fname[80];
	int Read(FILE *f);
};
