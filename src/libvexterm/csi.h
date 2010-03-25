#ifndef _CSI_H_
#define _CSI_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct{
	char prefix;
	GArray * nums;
	gboolean first_digit;
	char suffix1;
	char suffix2;
} Csi;

int get_value_or_default_from_csi(Csi * csi, int def);

#endif /* _CSI_H_ */
