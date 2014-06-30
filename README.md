meCh
====

IRC bot (mech) in C

Installation
============

```sh
make
```

It should not need external dependencies on any \*NIX-like system,
and it should just work. Report all the issues please.

Creating new modules
====================

struct Module
-------------

Defined in module.h, typedef'd to 'Module'.

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

- name is the name of the module, displayed when the "Help" module is called
  (the "Help" module is the base module, always present)
- help is the help string of the module, you can use $<n> to write 
  ["/msg meCh <invokern>" | "<conf.cmd><invokern>"],
  for example "$0" in "Help" becomes ["/msg meCh help" | ".help"]
- invokers are the command strings upon which the module is called. The last element
  in this array must be a NULL pointer. Modules specifying at least one non-NULL invoker
  must accept at least 3 arguments. A module with only a NULL invoker will always be called
  and has no restriction on the number of accepted arguments
- f is the function called to execute the module. It receives an array of string arguments, as many
  as specified by nargs, and the type of the message that made the module activate.
- nargs is number of arguments (words) the module wants, if the message contains less, the last arguments will be
  empty strings.
- on is a mask of enum irc\_type that indicates the types of messages that activate the module.
- next is used in the linked list of modules, should always be NULL when writing a module
