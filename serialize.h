/*requires
<stdio.h>
*/
#define SERIALIZE_REGISTER(NAME) serialize_register(NAME##_serialize, NAME##_deserialize, #NAME)

typedef void (*SerializeFunc)(FILE *f);

void serialize_register(SerializeFunc serialize, SerializeFunc deserialize, const char *name);
