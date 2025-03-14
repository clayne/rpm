RPMLUA(1)

# NAME

rpmlua - RPM Lua interpreter

# SYNOPSIS

*rpmlua* [{*-e*|*--execute*} "_STATEMENT_"] [{*-i*|*--interactive*}] [_SCRIPT_FILE_] [arg1 ...]

# DESCRIPTION

Run RPM internal Lua interpreter.

Note: indexes start at 1 in Lua, so the program name is at arg[1] instead
of the more customary index zero.

*-i*, *--interactive*
	Run an interactive session after executing optional script or statement.

*--opts*=_OPTSTRING_
	Perform *getopt*(3) option processing on the passed arguments according
	to _OPTSTRING_.

*-e*, *--execute*
	Execute a Lua statement before executing optional script.

# EXAMPLES

Execute test.lua script file:

```
rpmlua test.lua
```

Execute args.lua script file with option processing:

```
rpmlua --opts=ab:c args.lua -- 1 2 3 -c -b5
```

Execute single statement to compare rpm versions:

```
rpmlua -e "print(rpm.ver('1.0') < rpm.ver('2.0'))"
```

Run an interactive session:

```
rpmlua -i
```

# SEE ALSO

*lua*(1), *popt*(3), *getopt*(3), *rpm*(8)

*http://www.rpm.org/*
