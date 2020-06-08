extends State

func enter(args : Array):
	if(args.size() != 1):
		print_debug("pinpoint state only takes one argument")
