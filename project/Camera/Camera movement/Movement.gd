extends State

const ROT_LERP = 0.01
const POS_LERP = .1

var player : Spatial

onready var target_translation =  owner.get_translation()
onready var target_rotation = owner.get_rotation()

func to_player():
	return owner.get_translation() - player.get_translation()

func focus_to_player():
	return owner.player.get_translation() - owner.to_global(owner.focus.get_translation())
