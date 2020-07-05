extends Camera

const UP = Vector3(0, 1, 0)

onready var player : KinematicBody = get_node("../Player")
onready var movement : State= $Movement

var forward : Vector3
var forward2D : Vector2

export var angle : float = 0
export var offset : Vector2 = Vector2(0, 0)
export var distance : float = 10

func _ready():
	movement.set_root_state($Movement)
	movement.init()
	movement.player = player

func _process(delta):
	if Engine.editor_hint:
		#set rotation
		var rot := get_rotation()
		rot.x = angle
		set_rotation(rot)
		
		return
	
	forward = -get_global_transform().basis.z
	forward2D.x = (forward.z)
	forward2D.y = (forward.x)
	
	# lerp position to target position
	var target_pos : Vector3 = movement.target_position
	set_translation(translation.linear_interpolate(target_pos, movement.POS_LERP * delta * 60))
	
	# slerp rotation to target rotation
	var rot := Quat(transform.basis)
	transform.basis = Basis(rot.slerp(movement.target_rotation, movement.ROT_SLERP))
	# rotation.z = 0	#keep camera facing upright
	set_transform(transform.orthonormalized())
	
	movement.update(delta)
	#set_rotation(get_rotation().linear_interpolate(movement.target_rotation, movement.ROT_LERP * delta * 60))
	#set_rotation(movement.target_rotation)
