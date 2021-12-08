/* Copyright 2021 Eddington Chamunorwa <eddingtonchamunorwa34@gmail.com>
 * 
 * This software is a free give away of some of my programs written as an experiment.
 * To Test not to "Watch porn" But to be able to download videos from porn sites with
 * emaculately profound ease. It is licensed under the GNU Public license.
 */

#ifndef SEX_IN
#define SEX_IN

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/HTMLparser.h>
#include <jansson.h>
#include <regex.h>
#include <ctype.h>
#include <libavformat/avio.h>

extern json_t *murl;
void vdx_add(char *);
int vdx_load_url(char *);
void my_dump (htmlNodePtr);
void vdx_download(int is_high);

#endif /* SEX_IN */
