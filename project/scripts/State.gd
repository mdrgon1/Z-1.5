extends Node
class_name State

onready var substates_map := { }
onready var num_children := get_child_count()

var current_state : State
var default_state : State
var root_state : State

func _ready():
	for child in get_children():
		substates_map[child.get_name()] = child
	
	if(num_children != 0):
		default_state = get_children()[0]

# new_state may either be just a string, or an array containing both the new state
# and the arguments to pass to the new state
func update_state(new_state):
	if(new_state == null):
		return
	
	var args = []
	
	if typeof(new_state) == TYPE_ARRAY:
		args = new_state.slice(1, new_state.size() - 1)	#isolate the arguments out of new_state
		new_state = new_state[0]
	
	#check that new_state has the correct data in it
	if(typeof(new_state) != TYPE_STRING):
		print_debug("incorrect format for new_state\ncurrent state: ", current_state, current_state.name, "\nnew state: ", new_state, args)
	
	if new_state in substates_map:
		substates_map[new_state].exit()
		current_state = substates_map[new_state]
		current_state.enter(args)

# runs the entire state machine/tree, should only be explicitly called on the top level state
func update(delta):
	run(delta)
	if(num_children != 0):
		update_state(current_state.run(delta))

func set_root_state(new_root : State):
	root_state = new_root
	if num_children != 0:
		for state in substates_map:
			substates_map[state].set_root_state(new_root)

# calls the enter functions for each state
func init():
	enter([])
	if(num_children != 0):
		current_state.init()

func enter(_args : Array):
	current_state = default_state

func run(delta):
	pass

func exit():
	pass
