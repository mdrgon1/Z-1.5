extends Node

onready var movement = $Movement

# Called when the node enters the scene tree for the first time.
func _ready():
	movement.init()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	movement.update(delta)
