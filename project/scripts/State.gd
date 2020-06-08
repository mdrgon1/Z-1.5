extends Node
class_name State

onready var substates_map = { }
onready var num_children = get_child_count()

var current_state
var default_state
var root_state

func _ready():
	for child in get_children():
		substates_map[child.get_name()] = child
	
	if(num_children != 0):
		default_state = get_children()[0]

func update_state(new_state):
	if new_state in substates_map:
		substates_map[new_state].exit()
		current_state = substates_map[new_state]
		current_state.enter()

func _process(delta):
	if(num_children != 0):
		update_state(current_state.update(delta))

func set_state_root(new_root : State):
	root_state = new_root
	if num_children != 0:
		for state in substates_map:
			substates_map[state].set_state_root(new_root)

func enter():
	current_state = default_state

func update(delta):
	pass

func exit():
	pass
