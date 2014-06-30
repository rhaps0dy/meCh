#meCh

IRC bot (mech) in C

#Installation

```sh
make
```

It should not need external dependencies on any \*NIX-like system,
and it should just work. Report all the issues please.

#Creating new modules

##The `Module` data structure

Defined in `module.h`, `typedef`d to `Module`.

This is the basic component of any module. It serves as an entry point
for calling the module, it specifies when and how should that module be
called.

```c
struct Module {
	char *name;
	char *help;
	char **invokers;
	void (*f)(char**, enum irc_type);
	int nargs;
	int on;
	Module *next;
};
```

- `name`: the name of the module, displayed when the "Help" module is called
  (the "Help" module is the base module, always present)
- `help`: the help string of the module, you can use `$<n` to write 
  `["/msg meCh <invokern>" | "<conf.cmd><invokern>"]`,
  for example a `$0` in `help` becomes `["/msg meCh help" | ".help"]`
- `invokers`: the command strings upon which the module is called. The last element
  in this array must be a `NULL` pointer. Modules specifying at least one non-`NULL` invoker
  must accept at least 3 arguments. A module with only a `NULL` invoker will always be called
  and has no restriction on the number of accepted arguments
- `f`: the function called to execute the module. It receives an array of string arguments, as many
  as specified by nargs, and the type of the message that made the module activate.
- `nargs`: number of arguments (words) the module wants, if the message contains less, the last arguments will be
  empty strings.
- `on`: a bitmask of `enum irc_type`s that indicates the types of messages that activate the module.
- `next`: used in the linked list of modules, should always be `NULL` when writing a module

##An example `Module`

```c
void example_function(char **args, enum irc_type type);
static char *inv_example[2] = {"exm", NULL};
static char *always[1] = {NULL};
static Module example = {
	"Example",
	"$0 to do an example.",
	inv_example,
	example_function,
	3,
	T_MSG|T_CHAN,
	NULL
};
```

In this example, the module will be called provided the following conditions:
- The read line is a PRIVMSG to the bot or channel (`example.on = T_MSG|T_CHAN;`)
- The first word (space-separated token) is "exm" if the message is a private message to
  the bot, or the first word is ".exm" if the message is a message to the channel. The "." in
  ".exm" is configured with the conf.cmd value in the global configuration.

If `example.invokers` was `always`, the module would always be called by the handler, if the received message
fitted in the `example.on` mask.

Also, the help about this module will send via private message `Example: ["/msg meCh exm" | ".exm"] to do an example.`
The function `example_function` will be called to execute the module, and `next` should always be `NULL` for clarity
and avoiding potential bugs, though its initial value does not matter.

###About `nargs`

The arguments passed to the `args` argument of the module function are, in order: `nick`, `words`, ..., `rest of message`.
If `nargs <= 2` the words are omitted.
If `nargs == 1` the rest of message is not really omitted, but the module should only use the nick, which is `args[0]`.

Examples:
`<rhaps0dy> .exm This is a test phrase`
- `nargs = 5`: `args = {"rhaps0dy", ".exm", "This", "is", "a test phrase"}`
- `nargs = 4`: `args = {"rhaps0dy", ".exm", "This", "is a test phrase"}`
- `nargs = 3`: `args = {"rhaps0dy", ".exm", "This is a test phrase"}`
- `nargs = 2`: `args = {"rhaps0dy", ".exm This is a test phrase"}`
- `nargs = 1`: `args = {"rhaps0dy"}`

This is why if you want invokers, `nargs` must be greater or equal than 3.

`<rhaps0dy>                  .exm   This      is a         test    phrase`
- `nargs = 5`: `args = {"rhaps0dy", ".exm", "This", "is", "a         test    phrase"}`
- `nargs = 4`: `args = {"rhaps0dy", ".exm", "This", "is a         test    phrase"}`
- `nargs = 3`: `args = {"rhaps0dy", ".exm", "This      is a         test    phrase"}`
- `nargs = 2`: `args = {"rhaps0dy", ".exm   This      is a         test    phrase"}`
- `nargs = 1`: `args = {"rhaps0dy"}`

The tokenizer also strips multiple spaces when separating arguments.

#What do I put in the module function?

Typically in that function one processes the input, changes some state (stored as a static variable either in
the module function or its C file, and then use the `irc_reply`, `irc_msg`, `irc_say` or `irc_cmd` functions
defined in `irc.h` to provide a response to the user.

For a simple module without state, look at the "utopia" module in `mod/utopia.c`.
 
For a more complex module that keeps state and does different things depending on how it's invoked, look at the "tell" module in `mod/tell.c`
 
For a composite module that has separate `Module` structs for keeping state and providing replise, look at the "seen", "lastseen" and "on" modules
in `mod/seen.c`, `mod/lastseen.c`, `mod/lastseen.h` and `mod/on.c`
