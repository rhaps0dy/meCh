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
