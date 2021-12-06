#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/HTMLparser.h>
#include <jansson.h>
#include <regex.h>
#include <ctype.h>
#include <libavformat/avio.h>

json_t *murl = 0;

void vdx_add(char *li)
{
    char *p = li;
    regex_t re;
    int ret = 0;


    while (p && *p && (isspace(*p) || *p == '\t' || *p == '\n'))
	p++;
    if (!p || !*p)
	return;
    if (!strstr(p, "setVideo"))
	return;
    if (!murl)
	murl = json_array();
    ret =
	regcomp(&re,
		"^[a-z0-9]+\\.setvideourl([^\\(]{1,5})\\('([^']+)'\\)",
		REG_EXTENDED | REG_ICASE);
    if (ret) {
	puts("Error (Regular Expression Failed To Compile.\n");
	exit(1);
    }

    do {
	regmatch_t ma[9];
	ret = regexec(&re, p, 9, ma, 0);
	if (ret)
	    break;
	int dif = (int) abs(ma[1].rm_so - ma[1].rm_eo);
	char type[dif + 1];
	strncpy(type, p + (ma[1].rm_so), dif);
	type[dif] = 0;
#if 1
	dif = (int) abs(ma[2].rm_so - ma[2].rm_eo);
	char url[dif + 1];
	strncpy(url, p + (ma[2].rm_so), dif);
	strncpy(url, p + (ma[2].rm_so), dif);
	url[dif] = 0;
#endif
	{
	    json_t *ob = json_object();

	    json_object_set(ob, "type", json_string(type));
	    json_object_set(ob, "url", json_string(url));

	    json_array_append(murl, ob);
	}
    } while (0);

    regfree(&re);
}


void my_dump(htmlNodePtr p, int i)
{
    if (!p)
	return;

    if (p->name && !strcmp("script", (char *) p->name)) {
	xmlNodePtr pp = p->children;
	if (pp && pp->content) {
	    char *p = (char *) (pp->content);

	    while (p) {
		char *t = strchr(p, '\n');
		if (!t) {
		    vdx_add(p);
		    break;
		} else if (t - p) {
		    char buf[t - p];
		    strncpy(buf, p, t - p);
		    buf[t - p] = 0;
		    vdx_add(buf);
		}
		p = t + 1;
	    }
	}
    }
    my_dump(p->children, i + 1);
    my_dump(p->next, i);
}

static int my_rd(void *d, char *b, int l)
{
    int ret = 0;
    ret = avio_read((AVIOContext *) d, (unsigned char *) b, l);
    if (ret < 0) {
	return 0;
    } else
	return ret;
}

int my_cl(void *d)
{
    (void) d;
    //False Closer, I will close it after am really done with it.
    return 0;
}

int vdx_load_url(char *url)
{
    AVIOContext *io = 0;
    int ret = 0;

    ret = avio_open(&io, url, AVIO_FLAG_READ);
    if (ret < 0)
	return ret;

    do {
	htmlDocPtr hp = htmlReadIO(my_rd, my_cl, io, url, "UTF-8", 0);

	if (!hp) {
	    ret = 1;
	    break;
	}
	my_dump(hp->children, 0);
	ret = 0;
    } while (0);
    return 0;

    avio_close(io);

    return ret;
}

void err_s_l(void *d, void *g)
{
}

void vdx_download()
{
    int i;
    json_t *element;

    if (!murl)
	return;
    json_array_foreach(murl, i, element)
// Element ????
    {
	json_t *url = json_object_get(element, "url");
	json_t *type = json_object_get(element, "type");

	const char *t = json_string_value(type);
	const char *u = json_string_value(url);
	if (!strcasecmp("high", t)) {
	    //    puts(u);
	    AVIOContext *io = 0;
	    int ret = 0;
	    ret = avio_open(&io, u, AVIO_FLAG_READ);

	    if (ret < 0)
		break;
	    do {
		unsigned char buf[10540];
		while (1) {
		    ret = avio_read(io, buf, sizeof(buf));
		    if (ret < 0) {
			ret = 0;
		    }
		    if (ret == 0)
			break;
		    write(1, buf, ret);
		    fflush(stdout);
		}
	    } while (0);

	    avio_close(io);

	    break;
	}
    }
}

int main(argsc, args, env)
int argsc;
char **args, **env;
{
    char **p = args + 1;
    xmlThrDefSetGenericErrorFunc(0, err_s_l);
    xmlThrDefSetStructuredErrorFunc(0, err_s_l);

    while (p && *p) {
	if (vdx_load_url(*p)) {
	    printf("Failed To Load %s.\n", *p);
	}
	p++;
    }

    vdx_download();
    return 0;
}
