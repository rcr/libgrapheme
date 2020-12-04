/* See LICENSE file for copyright and license details. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "datautil.h"

static struct {
	char         *enumname;
	char         *identifier;
	struct range *table;
	size_t        tablelen;
} properties[] = {
	{
		/* extended pictographic */
		.enumname   = "EMOJI_PROP_EXTPICT",
		.identifier = "Extended_Pictographic",
	},
};

int
process_line(char **field, size_t nfields, char *comment)
{
	size_t i;
	struct range r;

	(void)comment;

	if (nfields < 2) {
		return 1;
	}

	for (i = 0; i < LEN(properties); i++) {
		if (!strcmp(field[1], properties[i].identifier)) {
			if (range_parse(field[0], &r)) {
				return 1;
			}
			range_list_append(&(properties[i].table),
			                  &(properties[i].tablelen), &r);
			break;
		}
	}

	return 0;
}

int
main(void)
{
	size_t i, j;

	printf("/* Automatically generated by data/emo */\n"
	       "#include <stdint.h>\n\n#include \"../src/util.h\"\n\n");

	parse_input(process_line);

	/* output enum */
	printf("enum emoji_prop {\n");
	for (i = 0; i < LEN(properties); i++) {
		printf("\t%s,\n", properties[i].enumname);
	}
	printf("};\n\n");

	/* output table */
	printf("static const struct range_list emoji_prop[] = {\n");
	for (i = 0; i < LEN(properties); i++) {
		printf("\t[%s] = {\n\t\t.data = (struct range[]){\n", properties[i].enumname);
		for (j = 0; j < properties[i].tablelen; j++) {
			printf("\t\t\t{ UINT32_C(0x%06X), UINT32_C(0x%06X) },\n",
			       properties[i].table[j].lower,
			       properties[i].table[j].upper);
		}
		printf("\t\t},\n\t\t.len = %zu,\n\t},\n", properties[i].tablelen);
	}
	printf("};\n");

	return 0;
}
