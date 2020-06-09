extends State

const ROT_SLERP = 0.03
const POS_LERP = 0.1

var player : Spatial

onready var target_translation =  owner.get_translation()
onready var target_rotation = owner.get_rotation()

func to_player():
	return owner.get_translation() - player.get_translation()

#returns vector from the focus to the player
func focus_to_player():
	return owner.player.get_translation() - owner.to_global(owner.focus.get_translation())

#returns vector from the focus to a target
func focus_to(target : Spatial):
	return target.get_translation() - owner.to_global(owner.focus.get_translation())
