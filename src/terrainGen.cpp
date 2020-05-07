#include "terrainGen.hpp"
#include <SurfaceTool.hpp>
#include <Mesh.hpp>
#include <algorithm>
#include <OS.hpp>
#include <Image.hpp>

using namespace godot;

void TerrainGen::_register_methods() {
	//register_property<TerrainGen, Ref<Image>>("heightmap", &TerrainGen::set_heightmap, &TerrainGen::get_heightmap, Ref<Image>(NULL));

	register_method("set_heightmap", &TerrainGen::SetHeightmap);
	register_method("get_heightmap", &TerrainGen::GetHeightmap);
	register_method("generate_mesh", &TerrainGen::GenerateMesh);
	register_method("set_height", &TerrainGen::SetHeight);
	register_method("gen_densitymap", &TerrainGen::GenDensitymap);
	register_method("get_densitymap", &TerrainGen::GetDensitymap);
	register_method("get_mesh", &TerrainGen::GetMesh);
	register_method("density_func", &TerrainGen::DensityFunc);
}

void TerrainGen::_init() {
	//std::cout << sizeof(TerrainGen) << std::endl;
}

void TerrainGen::SetHeightmap(Ref<Image> newHeightmap) {

	if (newHeightmap->get_height() != CHUNK_SIZE || newHeightmap->get_width() != CHUNK_SIZE){
		return;
	}
	
	newHeightmap->lock();
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			_heightmap[i][j] = newHeightmap->get_pixel(i, j).get_v();
		}
	}
}

Array TerrainGen::GetHeightmap() {
	Array heightmapGDArr = Array();
	heightmapGDArr.resize(CHUNK_SIZE);

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		Array row = Array();
		row.resize(CHUNK_SIZE);
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			row[j] = _heightmap[i][j];
		}
		heightmapGDArr[i] = row;
	}
	
	return heightmapGDArr;
}

Array TerrainGen::GetDensitymap() {
	Array densitymapGDArr = Array();
	densitymapGDArr.resize(CHUNK_SIZE);

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		Array layer = Array();
		layer .resize(CHUNK_SIZE);
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			Array column = Array();
			column.resize(CHUNK_SIZE);
				for (int k = 0; k < CHUNK_SIZE; ++k) {
					column[k] = _densitymap[i][j][k];
				}
			layer[j] = column;
		}
		densitymapGDArr[i] = layer;
	}

	return densitymapGDArr;
}

void TerrainGen::GenDensitymap() {
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				_densitymap[i][j][k] = DensityFunc(i, j, k);
			}
		}
	}
}

void TerrainGen::GenerateMesh() {

	

	float corner_densities[8];
	int coordinates[3] = {0};
	short int cubeId;
	int edge_index;
	float cube_offset[3];
	//for (int i = 0; i < NUM_CUBES; i++) {
	//	vertices[i] = Vector3(0, 2, 0);
	//}

	//hold vertex positions
	vertexGDArray = PoolVector3Array();
	vertexGDArray.resize(MAX_NUM_VERTICES);
	PoolVector3Array::Write vertexWrite = vertexGDArray.write();

	//hold vertex normals
	normalGDArray = PoolVector3Array();
	normalGDArray.resize(MAX_NUM_VERTICES);
	PoolVector3Array::Write normalWrite = normalGDArray.write();

	int index = 0;	//keep track of the number of vertices and which index of the arrays to write to
	for (int i = 0; i < CHUNK_SIZE - 1; i++) {	//x coordinates
		for (int j = 0; j < CHUNK_SIZE - 1; j++) {	//z coordinates
			for (int k = 0; k < std::min(int(ceil(height)), CHUNK_SIZE - 1); k++) {	//y coordinates
				
				//iterate through every cube corner and sample densitymap
				for (int l = 0; l < 8; l++) {
					//assign xyz coordinates
					coordinates[0] = i + VERTEX_TABLE[l][0];
					coordinates[1] = j + VERTEX_TABLE[l][1];
					coordinates[2] = k + VERTEX_TABLE[l][2];

					corner_densities[l] = _densitymap[coordinates[0]][coordinates[1]][coordinates[2]];
				}

				cubeId = GetCubeId(corner_densities);

				cube_offset[0] = i;
				cube_offset[1] = j;
				cube_offset[2] = k;

				//iterate through every set of 3 vertices and write position/normal data to arrays
				for (int vertex_index = 0; vertex_index < MAX_NUM_VERTICES_PER_CUBE; vertex_index+=3) {
					Vertex* triangle[3];

					if (TRIANGUALATION_TABLE[cubeId][vertex_index] == -1)
						break;

					//iterate through every vertex in the triangle
					for (int v = 0; v < 3; v++) {

						triangle[v] = new Vertex();
						edge_index = TRIANGUALATION_TABLE[cubeId][vertex_index + v];

						if (edge_index != -1) {
							*(triangle[v]) = EdgeVertexPos(edge_index, corner_densities);

							triangle[v]->xPos += i;
							triangle[v]->yPos += j;
							triangle[v]->zPos += k;
							
							//write position data
							vertexWrite[index] = Vector3(triangle[v]->xPos, triangle[v]->zPos, triangle[v]->yPos);
							normalWrite[index] = Vector3(0, 0, 0);
							index++;
						}

					}

					//calculate triangle normals
					GenerateNormals(triangle);


					//gotta retroactively shove these normals into the array
					for (int v = 2; v >= 0; v--) {
						//Godot::print(Vector3(triangle[2 - v]->xNorm, triangle[2 - v]->yNorm, triangle[2 - v]->zNorm));

						normalWrite[index - v] = Vector3(triangle[2 - v]->xNorm, triangle[2 - v]->yNorm, triangle[2 - v]->zNorm);
						delete triangle[2 - v];
						//normalWrite[index - v] = Vector3(0, 0, 0);
					}
				}
			}
		}
	}
	numVertices = index;
}

void TerrainGen::GenerateNormals(Vertex* vertices[3]) {
	Vertex* v1 = vertices[0];
	Vertex* v2 = vertices[1];
	Vertex* v3 = vertices[2];

	Vertex e1 = *v2 - *v1;
	Vertex e2 = *v3 - *v1;

	v1->xNorm = e1.yPos * e2.zPos - e1.zPos * e2.yPos;
	v1->yNorm = e1.zPos * e2.xPos - e1.xPos * e2.zPos;
	v1->zNorm = e1.xPos * e2.yPos - e1.yPos * e2.xPos;

	v2->xNorm = v1->xNorm;
	v2->yNorm = v1->yNorm;
	v2->zNorm = v1->zNorm;

	v3->xNorm = v1->xNorm;
	v3->yNorm = v1->yNorm;
	v3->zNorm = v1->zNorm;

	float mag = powf(v1->xNorm * v1->xNorm + v1->yNorm * v1->yNorm + v1->zNorm * v1->zNorm, 0.5);
	
	//normalize the normal vectors
	for (int i = 0; i < 3; i++) {
		vertices[i]->xNorm /= mag;
		vertices[i]->yNorm /= mag;
		vertices[i]->zNorm /= mag;
	}

	//for (int i = 0; i < 3; i++) {
	//	std::cout << '[' << vertices[i]->xPos << ", " << vertices[i]->yPos << ", " << vertices[i]->zPos << "], ";
	//	std::cout << '[' << vertices[i]->xNorm << ", " << vertices[i]->yNorm << ", " << vertices[i]->zNorm << "] ";
	//	std::cout << "\n";
	//}
	//std::cout << '[' << e1.xPos << ", " << e1.yPos << ", " << e1.zPos << "] \n";
	//std::cout << '[' << e2.xPos << ", " << e2.yPos << ", " << e2.zPos << "] \n\n";
}

void TerrainGen::SetHeight(float newHeight) {
	height = newHeight;
}

Ref<ArrayMesh> TerrainGen::GetMesh() {

	vertexGDArray.resize(numVertices);
	normalGDArray.resize(numVertices);

	meshGDArray.resize(Mesh::ARRAY_MAX);
	/*
	vertexGDArray.resize(0);
	normalGDArray.resize(0);

	Vertex vertex;
	for (int i = 0; i < numVertices; i++) {
		vertex = vertices[i];
		vertexGDArray.push_back(Vector3(vertex.xPos, vertex.zPos, vertex.yPos));
		normalGDArray.push_back(Vector3(0, 0, 0));
	}
	*/
	meshGDArray[Mesh::ARRAY_VERTEX] = vertexGDArray;
	meshGDArray[Mesh::ARRAY_NORMAL] = normalGDArray;

	Ref<ArrayMesh> m = ArrayMesh::_new();

	m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, meshGDArray);

	return m;

	//mesh_gdarr[Mesh::ARRAY_VERTEX] = vertex_gdarr;
	//mesh_gdarr[Mesh::ARRAY_TEX_UV] = vertex_gdarr;
	//mesh_gdarr[Mesh::ARRAY_NORMAL] = vertex_gdarr;

	//Ref<ArrayMesh> mesh;
	//mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, mesh_gdarr);

	//return mesh;

	//Ref<SurfaceTool> st = SurfaceTool::_new();
	//st->begin(Mesh::PRIMITIVE_TRIANGLES);

	//const Vector3 normal = Vector3(0, 0, 1);
	////Vertex vertex;
	//for (int i = 0; i < numVertices; i++) {
	//	vertex = vertices[i];
	//	st->add_normal(normal);
	//	st->add_vertex(Vector3(vertex.x, vertex.z, vertex.y));
	//}

	//st->index();
	//st->generate_normals();

	//return st->commit();
}

int TerrainGen::GetCubeId(float values[8]) {
	short int cubeIndex = 0;
	for (int i = 0; i < 8; i++) {
		if (values[i] > 0) {
			cubeIndex |= 1 << i;
		}
	}

	return cubeIndex;
}

//float Cubifier::DensityFunc(int coordinates[3]) {
float TerrainGen::DensityFunc(int x, int y, int z) {
	/*int x = coordinates[0];
	int y = coordinates[1];
	int z = coordinates[2];*/
	float density = z - _heightmap[x][y] * height;
	return density;
}

Vertex TerrainGen::EdgeVertexPos(int edge_index, float corner_densities[8]) {
	Vertex pos;
	
	int edge[2];
	edge[0] = EDGE_TABLE[edge_index][0];
	edge[1] = EDGE_TABLE[edge_index][1];

	float v1 = corner_densities[edge[0]];
	float v2 = corner_densities[edge[1]];
	float density_range = abs(v1 - v2);
	float lerp_value = abs(v1) / density_range;

	float posArray[3];
	for (int i = 0; i < 3; i++) {
		//lerp between VERTEX_TABLE[edge[0]][i] and VERTEX_TABLE[edge[1]][i], apparently not already a function
		posArray[i] = VERTEX_TABLE[edge[0]][i] + lerp_value * (VERTEX_TABLE[edge[1]][i] - VERTEX_TABLE[edge[0]][i]);
	}

	pos.xPos = posArray[0];
	pos.yPos = posArray[1];
	pos.zPos = posArray[2];

	return pos;
}