#include "word.h"

#include <stdlib.h>

/* Utility functions. An interface between UNIX and XSM data types. */

/* Determine the type of data in the word. */
int
word_get_unix_type (xsm_word* word)
{
	char *data = word->val;
	unsigned int index;

	index = 0;

	if (data[0] == '+' || data[0] == '-')
		index++;

	for (; data[index] != '\0'; index++)
	{
		if (data[index] < '0' || data[index] > '9')
			return XSM_TYPE_STRING;
	}

	return XSM_TYPE_INTEGER;
}

/* Retrieve the integer value in the given word. */
int
word_get_integer (xsm_word* word)
{
	return atoi(word->val);
}

char*
word_get_string (xsm_word* word)
{
	return word->val;
}

int
word_store_integer (xsm_word* word, int integer)
{
	char *data;

	data = word->val;

	sprintf (data, "%d", integer);

	return XSM_SUCCESS;
}

int
word_store_string (xsm_word *word, const char *str)
{
	char *data;

	data = word->val;

	return strncpy (data, str, XSM_WORD_SIZE);
}

void
word_copy (xsm_word *dest, xsm_word *src)
{
	memcpy (dest, src, sizeof(xsm_word));
}
