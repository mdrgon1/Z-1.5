extends Camera

const UP = Vector3(0, 1, 0)

onready var player = get_node("../Player")
onready var movement = $Movement
onready var focus = $Focus

var forward : Vector3
var forward2D : Vector2

export var distance = 20
export var height = 20

# Called when the node enters the scene tree for the first time.
func _ready():
	movement.enter()
	movement.set_state_root($Movement)
	movement.player = player

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	forward = -get_global_transform().basis.z
	forward2D.x = (forward.z)
	forward2D.y = (forward.x)
	
	set_translation(get_translation().linear_interpolate(movement.target_translation, movement.POS_LERP * delta * 60))
	set_rotation(movement.target_rotation)
