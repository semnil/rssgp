#pragma warning(disable: 4996)

#include "xml.h"

struct _spconvt{
	char			name[16];
	unsigned char	glen;
	char			pdata[15];
};

class Html : public Xml
{
public:
	Html();
	Html(char *str);
	~Html();
	static void decode(char* filepath);

	static struct _spconvt *spconvt;
	static int spconv_max;
};
