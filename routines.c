#include "./xnxx.h"

int my_cl(void *d)
{
    (void) d;
    //False Closer, I will close it after am really done with it.
    return 0;
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
	my_dump(hp->children);
	ret = 0;
    } while (0);
    return 0;

    avio_close(io);

    return ret;
}

static int vdx_download1(char *url, int i)
{
    AVIOContext *io = 0;
    int ret = 0;
    ret = avio_open(&io, url, AVIO_FLAG_READ);

    if (ret < 0)
	return 1;
    do {
	unsigned char buf[10540];
	while (1) {
	    ret = avio_read(io, buf, sizeof(buf));
	    if (ret < 0) {
		ret = 0;
	    }
	    if (ret == 0)
		break;
	    if (i == 0)
		fwrite(buf, 1, ret, stdout), fflush(stdout);
	}
    } while (0);

    avio_close(io);

    return 0;
}

void vdx_download(int it)
{
    int i;
    int di = 0;
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
	if ((it ? 1 : 0) == !strcasecmp("high", t)) {
#if 1
	    vdx_download1(u, di++);
	}
#endif
    }
}

static void pr_attr(xmlAttrPtr p)
{
    while (p) {
	if (!strcmp("href", p->name)) {
	    printf("%s=%s\n", p->name, p->children->content);
	    break;
	}
	p = p->next;
    }
}

void my_dump(htmlNodePtr p)
{
    while (p) {
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
	} else if (p->name && !strcmp("a", p->name)) {
	    puts(p->name);
	    pr_attr(p->properties);
	}
	my_dump(p->children);
	p = (p->next);
    }
}
