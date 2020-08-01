#include "terrainGen.h"
#include <SurfaceTool.hpp>
#include <Mesh.hpp>
#include <algorithm>
#include <OS.hpp>
#include <Image.hpp>
#include <string.h>

using namespace godot;

void TerrainGen::_register_methods() {
	//register_property<TerrainGen, Ref<Image>>("heightmap", &TerrainGen::set_heightmap, &TerrainGen::get_heightmap, Ref<Image>(NULL));

	register_method("generate_mesh_from_heightmap", &TerrainGen::GenerateMeshFromHeightmap);
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
	m = Ref<ArrayMesh>(ArrayMesh::_new());

	vertexGDArray.resize(3);
	normalGDArray.resize(3);
}

TerrainGen::TerrainGen() {
}

TerrainGen::~TerrainGen() {
	//m->free();
}

void TerrainGen::GenerateMeshFromHeightmap(Ref<Image> heightmap, float height) {
	SetHeightmap(heightmap);
	SetHeight(height);
	GenDensitymap();
	GenerateMesh();
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

//void TerrainGen::GenerateMesh(){
//	{
//		//create write objects for mesh arrays
//		PoolVector3Array::Write vertexWrite = vertexGDArray.write();
//		PoolVector3Array::Write normalWrite = normalGDArray.write();
//
//		vertexWrite[0] = Vector3(0, 0, 0);
//		vertexWrite[1] = Vector3(1, 0, 0);
//		vertexWrite[2] = Vector3(0, 0, 1);
//
//		normalWrite[0] = Vector3(0, 1, 0);
//		normalWrite[1] = Vector3(0, 1, 0);
//		normalWrite[2] = Vector3(0, 1, 0);
//	}
//
//	Array arrays;
//	arrays.resize(Mesh::ARRAY_MAX);
//	arrays[Mesh::ARRAY_VERTEX] = vertexGDArray;
//	arrays[Mesh::ARRAY_NORMAL] = normalGDArray;
//
//	while (m->get_surface_count() != 0) {
//		m->surface_remove(0);
//	}
//
//	m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
//}

void TerrainGen::GenerateMesh() {

	float corner_densities[8];
	int coordinates[3] = {0};
	short int cubeId;
	int edge_index;
	//for (int i = 0; i < NUM_CUBES; i++) {
	//	vertices[i] = Vector3(0, 2, 0);
	//}

	normalGDArray.resize(MAX_NUM_VERTICES);
	vertexGDArray.resize(MAX_NUM_VERTICES);

	{
		//create write objects for mesh arrays
		PoolVector3Array::Write vertexWrite = vertexGDArray.write();
		PoolVector3Array::Write normalWrite = normalGDArray.write();

		int index = 0;	//keep track of the number of vertices and which index of the arrays to write to
		for (int i = 0; i < (CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * (CHUNK_SIZE - 1); i++) {	//x coordinates

			int x = i % (CHUNK_SIZE - 1);
			int y = (i / (CHUNK_SIZE - 1)) % (CHUNK_SIZE - 1);
			int z = i / ((CHUNK_SIZE - 1) * (CHUNK_SIZE - 1));

			//iterate through every cube corner and sample densitymap
			for (int j = 0; j < 8; j++) {
				//assign xyz coordinates
				coordinates[0] = x + VERTEX_TABLE[j][0];
				coordinates[1] = y + VERTEX_TABLE[j][1];
				coordinates[2] = z + VERTEX_TABLE[j][2];

				corner_densities[j] = _densitymap[coordinates[0]][coordinates[1]][coordinates[2]];
			}

			cubeId = GetCubeId(corner_densities);


			//iterate through every set of 3 vertices and write position/normal data to arrays
			for (int vertex_index = 0; vertex_index < MAX_NUM_VERTICES_PER_CUBE; vertex_index += 3) {
				Vertex triangle[3];

				if (TRIANGUALATION_TABLE[cubeId][vertex_index] == -1)
					break;

				//iterate through every vertex in the triangle
				for (int v = 0; v < 3; v++) {

					triangle[v] = Vertex();
					edge_index = TRIANGUALATION_TABLE[cubeId][vertex_index + v];

					if (edge_index != -1) {
						triangle[v] = EdgeVertexPos(edge_index, corner_densities);

						triangle[v].pos.x += x;
						triangle[v].pos.y += y;
						triangle[v].pos.z += z;

						//write position data, account for the change in axis (Y is up and down in Godot, not Z, you asshole)
						vertexWrite[index].x = triangle[v].pos.x;
						vertexWrite[index].y = triangle[v].pos.z;
						vertexWrite[index].z = triangle[v].pos.y;
						//normalWrite[index] = Vector3(float(i) / float(CHUNK_SIZE), float(j) / float(CHUNK_SIZE), float(k) / float(CHUNK_SIZE));
						index++;
					}

				}
				//calculate triangle normals
				GenerateNormals(triangle);

				//gotta retroactively shove these normals into the array
				index -= 3;
				for (int v = 0; v < 3; v++) {
					//normalWrite[index - v] = Vector3(triangle[2 - v]->xNorm, triangle[2 - v]->zNorm, triangle[2 - v]->yNorm);
					normalWrite[index] = triangle[0].norm;
					index++;
				}
				/*delete triangle[0];
				delete triangle[1];
				delete triangle[2];*/
			}
		}

		numVertices = index;
	}

	normalGDArray.resize(numVertices);
	vertexGDArray.resize(numVertices);

	Array arrays;
	arrays.resize(Mesh::ARRAY_MAX);
	arrays[Mesh::ARRAY_VERTEX] = vertexGDArray;
	arrays[Mesh::ARRAY_NORMAL] = normalGDArray;

	while (m->get_surface_count() != 0) {
		m->surface_remove(0);
	}

	m->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
}

inline void TerrainGen::GenerateNormals(Vertex vertices[3]) {
	Vertex* v1 = &vertices[0];
	Vertex* v2 = &vertices[1];
	Vertex* v3 = &vertices[2];

	//create two edges joining the vertices
	Vector3 e1 = v2->pos - v1->pos;
	Vector3 e2 = v3->pos - v1->pos;

	Vector3 norm;

	//calculate the cross product of the two edges
	norm.x = e1.y * e2.z - e1.z * e2.y;
	norm.y = e1.z * e2.x - e1.x * e2.z;
	norm.z = e1.x * e2.y - e1.y * e2.x;

	norm = norm.normalized();

	/*std::cout << v1->pos.x << " " << v1->pos.y << " " << v1->pos.z << ", ";
	std::cout << v2->pos.x << " " << v2->pos.y << " " << v2->pos.z << ", ";
	std::cout << v3->pos.x << " " << v3->pos.y << " " << v3->pos.z << std::endl;*/

	//write vector to vertices array
	vertices[0].norm = Vector3(norm.x, norm.z, norm.y);
	vertices[1].norm = Vector3(norm.x, norm.z, norm.y);
	vertices[2].norm = Vector3(norm.x, norm.z, norm.y);

	//std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;

	//std::cout << vertices[0]->norm.x << " " << vertices[0]->norm.y << " " << vertices[0]->norm.z << std::endl;
	//std::cout << vertices[1]->norm.x << " " << vertices[1]->norm.y << " " << vertices[1]->norm.z << std::endl;
	//std::cout << vertices[2]->norm.x << " " << vertices[2]->norm.y << " " << vertices[2]->norm.z << std::endl << std::endl;
}

void TerrainGen::SetHeight(float newHeight) {
	height = newHeight;
}

Ref<ArrayMesh> TerrainGen::GetMesh() {
	return m;
}

inline int TerrainGen::GetCubeId(float values[8]) {
	short int cubeIndex = 0;
	for (int i = 0; i < 8; i++) {
		if (values[i] > 0) {
			cubeIndex |= 1 << i;
		}
	}

	return cubeIndex;
}

//float Cubifier::DensityFunc(int coordinates[3]) {
inline float TerrainGen::DensityFunc(int x, int y, int z) {
	/*int x = coordinates[0];
	int y = coordinates[1];
	int z = coordinates[2];*/
	float density = z - _heightmap[x][y] * height;
	return density;
}

inline Vertex TerrainGen::EdgeVertexPos(int edge_index, float corner_densities[8]) {
	Vertex v;
	
	//hold the corner indices at the ends of the edge the vertex falls on
	int edge[2];
	edge[0] = EDGE_TABLE[edge_index][0];
	edge[1] = EDGE_TABLE[edge_index][1];

	float v1 = corner_densities[edge[0]];
	float v2 = corner_densities[edge[1]];
	float density_range = abs(v1 - v2);
	float lerp_value = abs(v1) / density_range;

	//lerp between VERTEX_TABLE[edge[0]][i] and VERTEX_TABLE[edge[1]][i]
	v.pos[0] = VERTEX_TABLE[edge[0]][0] + lerp_value * (VERTEX_TABLE[edge[1]][0] - VERTEX_TABLE[edge[0]][0]);
	v.pos[1] = VERTEX_TABLE[edge[0]][1] + lerp_value * (VERTEX_TABLE[edge[1]][1] - VERTEX_TABLE[edge[0]][1]);
	v.pos[2] = VERTEX_TABLE[edge[0]][2] + lerp_value * (VERTEX_TABLE[edge[1]][2] - VERTEX_TABLE[edge[0]][2]);

	return v;
}