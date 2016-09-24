#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tuple.h"

static int
valid_date(char *val)
{
	char s[5];
	int d, m;

	/* Syntax check */
	if (!(isdigit(val[0]) &&
	    isdigit(val[1]) &&
	    val[2] == '-' &&
	    isdigit(val[3]) &&
	    isdigit(val[4]) &&
	    val[5] == '-' &&
	    isdigit(val[6]) &&
	    isdigit(val[7]) &&
	    isdigit(val[8]) &&
	    isdigit(val[9]))) {
#if _DEBUG
		printf("valid_date: bad syntax\n");
#endif
		return 0;
	}
	/* Month range check */
	memset(s, 0, 5);
	memcpy(s, val, 2);
	m = atoi(s);
	if (m < 1 || m > 12) {
#if _DEBUG
		printf("valid_date: month out of range\n");
#endif
		return 0;
	}
	/* Day of month range check */
	memset(s, 0, 5);
	memcpy(s, val + 3, 2);
	d = atoi(s);
	switch (m) {
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		if (d < 1 || d > 31) {
#if _DEBUG
			printf("valid_date: day of month out of range\n");
#endif
			return 0;
		}
		break;

	case 4: case 6: case 9: case 11:
		if (d < 1 || d > 30) {
#if _DEBUG
			printf("valid_date: day of month out of range\n");
#endif
			return 0;
		}
		break;

	case 2:
		/* XXX Need to do better with leap year */
		if (d < 28 || d > 29) {
#if _DEBUG
			printf("valid_date: day of month out of range\n");
#endif
			return 0;
		}
		break;
	}

	/* Passed all checks */
	return 1;
}

int
tuple_set(tuple_t t, char *name, char *val)
{
	int offset;
	base_types_t bt;
	
	assert (t != NULL);
	assert (name != NULL);
	assert (val != NULL);

	offset = tuple_get_offset(t, name);
#if _DEBUG
	printf("name [%s] offset %d\n", name, offset);
#endif
	if (offset < 0) {
		printf("offset of [%s] not found\n", name);
		return (-1);
	}
	bt = schema_find_type_by_name(t->s, name);
	if (bt == BASE_TYPES_MAX) {
		printf("type of [%s] not found\n", name);
		return (-1);
	}
	switch (bt) {
	case CHARACTER:
		tuple_set_char(t->buf + offset, val[0]);
		break;

	case VARCHAR:
		tuple_set_varchar(t->buf + offset, val);
		break;

	case BOOLEAN:
		if (strcasecmp(val, "true") == 0)
			tuple_set_bool(t->buf + offset, 1);
		else if (strcasecmp(val, "false") == 0)
			tuple_set_bool(t->buf + offset, 0);

		break;
	case INTEGER:
		{
			int i = atoi(val);
			tuple_set_int(t->buf + offset, i);
		}
		break;
	case FLOAT:
		{
			float fval = atof(val);
			tuple_set_float(t->buf + offset, fval);
		}
		break;
	case DOUBLE:
		{
			double dval = atof(val);
			tuple_set_double(t->buf + offset, dval);
		}
		break;
	case DATE:
		if (valid_date(val))
			tuple_set_date(t->buf + offset, val);
		break;
	case TIME:
		/* XXX Need to validate time string */
		tuple_set_time(t->buf + offset, val);
		break;
	case BASE_TYPES_MAX:
		break;
	}
#if _DEBUG
	bufdump(t->buf, t->len);
#endif
	return 0;
}
