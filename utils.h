/* returns 1 if one strings begins with the other
 * 0 elsewise */
int strbeg(char *a, char *b);

/* concatenates two strings, honoring the maximum message length */
void strcat_msg(char *msg, char *s);

/* Gets separation of a string in words. Spaces (' ') are the word separator.
 * Returns the number of words, honoring the maximum number of words
 * (maxwds).
 * stores in wds the beginning of every word. wds must have at least length maxwds.
 * stores in spaces the end of every word. spaces must have at least length maxwds-1.
 * maxwds is the maximum number of expected words */
int words(char *str, char **wds, char **spaces, int maxwds);

/* Partitions string processed by the words function according to the number of existing
 * words in it and the number of desired words.
 * spaces are the inter-word spaces returned by the words function.
 * nwds is the number of words the string has
 * targwds is the number of desired words. */
void unwords(char **spaces, int nwds, int targwds);

/* converts a string containing characters [0-9] to a positive integer.
If the string contains characters other than those, it returns -1 */
int proper_atoi(char *a);

#define CHECK_MALLOC(p) \
	if(!p) { \
		fprintf(stderr, "Could not allocate memory on line %d, file %s.\n", __LINE__, __FILE__); \
		exit(1); \
	}
