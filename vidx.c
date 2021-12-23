#include "./xnxx.h"

json_t *murl = 0;

void err_s_l(void *d, xmlError * g)
{
}

static void crap_ignore(void *d, const char *fmt, ...)
{
}

int main(argsc, args, env)
int argsc;
char **args, **env;
{
    char **p = args + 1;
    int is_high = 1;

    /* I just hate the motherfucken logs of crap, kidding (We do not need error but
     * To be honey right ?!!
     */

    xmlSetGenericErrorFunc(0, crap_ignore);
    xmlSetStructuredErrorFunc(0, err_s_l);

    while (p && *p) {
	if (vdx_load_url(*p)) {
	    printf("Failed To Load %s.\n", *p);
	}
	p++;
    }

    vdx_download(is_high);
    return 0;
}
