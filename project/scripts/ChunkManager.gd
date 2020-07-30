extends Node

var generated_chunks : Array
var chunks : Array

func _ready():
	
	generated_chunks.resize(16)

func _process(delta):
	
	# generate chunks, don't add them to tree just yet
	for i in range(generated_chunks.size()):
		var chunk : Chunk = generated_chunks[i]
		if(chunk == null): # this is just to limit chunk generation to once a frame, TODO replace this with actual multithreading or something
			chunk = Chunk.new()
			chunk.heightmap = (load("res://heightmap.png"))
			chunk.height = 5
			chunk.translation.x = i * 15
			chunk._generate_mesh()
			
			generated_chunks[i] = chunk
			add_chunk(i)
			
			break
	

# add a generated chunk to the tree
func add_chunk(chunk_idx : int):
	
	add_child(generated_chunks[chunk_idx])
	chunks.push_back(generated_chunks[chunk_idx])
